#include "writer.hpp"
#include "utils/error.hpp"
#include "utils/log.hpp"

namespace seze {

Writer::Writer(CN(Str) oname, CN(FramebufferInfo) framebuffer_info,
int out_x, int out_y, CN(ReaderCtx) reader_ctx_, bool simple_encoder) {
  frame_x = framebuffer_info.x;
  frame_y = framebuffer_info.y;
  if (out_x < 1)
    out_x = frame_x;
  if (out_y < 1)
    out_y = frame_y;
  LOG("Writer: output video resolution " << out_x << "x" <<
    out_y << std::endl);
  reader_ctx = reader_ctx_;
  in_stride = framebuffer_info.stride;
  in_pix_fmt = convert(framebuffer_info.type);
// find encoder
  //TODO add codec selector
  if (simple_encoder)
    codec = avcodec_find_encoder_by_name("libx264");
  else
    codec = avcodec_find_encoder_by_name("libx264rgb");
  iferror( !codec, "Writer: Could not create codec");
// guess format
  format = av_guess_format(NULL, oname.c_str(), NULL);
  iferror( !format, "Writer: Could not create format");
  const_cast<AVOutputFormat*>(format)->video_codec = codec->id;
// encoding settings:
  codec_context = avcodec_alloc_context3(codec);
  iferror( !codec_context, "Writer: Could not allocate context for the codec");
  codec_context->codec = codec;
  codec_context->codec_id = codec->id;
  // TODO pixel fmt switcher
  if (simple_encoder)
    out_pix_fmt = AV_PIX_FMT_NV12;
  else {
    if (in_pix_fmt != AV_PIX_FMT_RGB24)
      out_pix_fmt = AV_PIX_FMT_RGB24;
    else
      out_pix_fmt = in_pix_fmt;
  }
  raw_mode = (out_pix_fmt == in_pix_fmt);
  if (out_x != framebuffer_info.x or out_y != framebuffer_info.y)
    raw_mode = false;
  LOG("Wirter: RAW-copy mode " <<
    (raw_mode ? "enabled" : "disabled") << std::endl);
  codec_context->pix_fmt = out_pix_fmt;
  codec_context->width = out_x;
  codec_context->height = out_y;
  codec_context->time_base = av_inv_q(reader_ctx.stream->r_frame_rate);
  codec_context->gop_size = int(reader_ctx.framerate / 2.0);
  codec_context->delay = 0; 
  codec_context->max_b_frames = 0; 
  codec_context->thread_count = 4;
  codec_context->refs = 3;
  if (auto ret = avformat_alloc_output_context2(&format_context,
    const_cast<AVOutputFormat*>(format),
    NULL, NULL); ret < 0 || !format_context)
    error("Writer: Could not allocate format context");
  format_context->oformat = const_cast<AVOutputFormat*>(format);
  format_context->video_codec_id = codec->id;
// init stream
  stream = avformat_new_stream(format_context, codec);
  iferror( !stream, "Writer: Could not create new stream");
  stream->time_base = codec_context->time_base;
  if (auto ret = avcodec_parameters_from_context(stream->codecpar,
  codec_context); ret < 0)
    error("Writer: Could not convert AVCodecContext to AVParameters");
  //TODO add opt 4 compression
  if (simple_encoder)
    av_opt_set_int(codec_context->priv_data, "crf", 23, 0); // lossles mode
  else
    av_opt_set_int(codec_context->priv_data, "crf", 0, 0); // lossles mode
  av_opt_set(codec_context->priv_data, "tune", "zerolatency", 0);
  av_opt_set(codec_context->priv_data, "preset", "ultrafast", 0);
  if (format_context->oformat->flags & AVFMT_GLOBALHEADER)
    codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  av_dump_format(format_context, 0, oname.c_str(), 1);
// opening file:
  if (auto ret = avcodec_open2(codec_context, codec, NULL); ret < 0)
    error("Writer: avcodec_open2 error");
  avcodec_parameters_from_context(stream->codecpar, codec_context);
  if (auto ret = avio_open(&format_context->pb, oname.c_str(),
  AVIO_FLAG_WRITE); ret < 0 || !format_context->pb)
    error("Writer: Could not open output file");
  if (auto ret = avformat_write_header(format_context, NULL);
  ret < 0)
    error("Writer: Could not write header");
// init frame
  if (av_frame = av_frame_alloc(); !av_frame)
    error("Writer: Could not allocate frame");
  av_frame->format = out_pix_fmt;
  av_frame->width = out_x;
  av_frame->height = out_y;
  if (av_frame_get_buffer(av_frame, ALIGN) < 0)
    error("Writer: Failed to allocate picture");
// init scaler:
  // TODO add opt 4 select scale alg
  if ( !raw_mode) {
    swsctx = sws_getCachedContext(
      nullptr, framebuffer_info.x, framebuffer_info.y,
      in_pix_fmt, codec_context->width,
      codec_context->height, out_pix_fmt,
      SWS_BICUBIC, nullptr, nullptr, nullptr);
    iferror( !swsctx, "Writer: can't not create scale context");
  }
} // Writer c-tor

Writer::~Writer() {
  if (format_context) {
    LOG("Writer: closing output")
    av_write_trailer(format_context);
    if ( !(format_context->flags & AVFMT_NOFILE))
		  avio_closep(&format_context->pb); 
  }
  avformat_free_context(format_context);
  avcodec_close(codec_context);
  avcodec_free_context(&codec_context);
  sws_freeContext(swsctx);
  av_frame_free(&av_frame);
} // Writer d-tor

void Writer::operator << (Image* image) {
#ifdef DEBUG
  iferror( !image, "Writer <<: !image");
  iferror(frame_x != image->X, "Writer <<: frame_x != image->get_X()");
  iferror(frame_y != image->Y, "Writer <<: frame_y != image->get_Y()");
#endif
  if (raw_mode) {
    if (av_image_fill_arrays(av_frame->data, av_frame->linesize,
    rcast(CP(uint8_t), image->get_cdata()),
    in_pix_fmt, frame_x, frame_y, 1) < 0) // use aligment 1 byte only
      error("Writer.<<: av_image_fill_arrays error");
  } else {
    // https://stackoverflow.com/a/46046953
    uint8_t* src_data[1] = { rcast(uint8_t*, image->get_data()) };
    int stride[1] { in_stride };
    sws_scale(swsctx, src_data, stride, 0, frame_y, av_frame->data,
      av_frame->linesize);
  }
  av_frame->pts = av_rescale_q(pts++, codec_context->time_base,
    reader_ctx.stream->time_base);
  AVPacket pkt;
  av_init_packet(&pkt);
  if (auto ret = avcodec_send_frame(codec_context, av_frame);
  ret < 0 && ret != AVERROR_EOF)
    error("Writer.encode: avcodec_send_frame error");
  if (auto ret = avcodec_receive_packet(codec_context, &pkt);
  ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
  {}
  else if (ret == 0) {
    av_packet_rescale_ts(&pkt, reader_ctx.stream->time_base,
      stream->time_base);
    pkt.stream_index = stream->index;
    pkt.duration = stream->time_base.den / stream->time_base.num /
      reader_ctx.stream->avg_frame_rate.num *
      reader_ctx.stream->avg_frame_rate.den;
    if (av_interleaved_write_frame(format_context, &pkt) < 0)
      error("Writer.<<: Could not write the received packet");
  } else 
    error("Writer.<<: avcodec_receive_packet error");
  av_packet_unref(&pkt);
} // <<

} // seze ns
