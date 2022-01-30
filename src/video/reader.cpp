#include "reader.hpp"
#include "utils/error.hpp"
#include "utils/log.hpp"

namespace seze {

Reader::Reader(CN(Str) iname, int width, int height, color_t col_type) {
// open file:
  if (auto ret = avformat_open_input(&format_context, iname.c_str(),
  NULL, NULL); ret < 0)
    error("Reader: Could not open input file \"" + iname + "\"");  
// find vstream & codec info
  if (auto ret = avformat_find_stream_info(format_context, NULL);
  ret < 0)
    error("Reader: Failed to retrieve input stream information");
  const auto streams = format_context->nb_streams;
  FOR(i, format_context->nb_streams) {
    in_stream = format_context->streams[i];
    AVCodecParameters* in_codecpar = in_stream->codecpar;
    if (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      vstream = i;
      codec = avcodec_find_decoder(in_codecpar->codec_id);
      codec_parms = in_codecpar;
      LOG("Reader: Input video resolution = " << codec_parms->width << "x"
        << codec_parms->height << std::endl);
      LOG("Reader: Input video Codec = " << codec->name << std::endl);
      framerate = av_q2d(in_stream->r_frame_rate);
      LOG("Reader: Input video fps = " << framerate << std::endl);
      break;
    }
  }
  if (vstream == -1) {
    error("Reader: video stream is not finded in file");
  } else {
    LOG("Reader: video stream id = " << vstream << std::endl);
  }
// init codec context:
  if (codec_context = avcodec_alloc_context3(codec); !codec_context)
    error("Reader: failed to allocated memory for AVCodecContext");
  iferror(avcodec_parameters_to_context(codec_context, codec_parms) < 0,
    "Reader: failed to copy codec params to codec context");
  iferror(avcodec_open2(codec_context, codec, NULL) < 0,
    "Reader: failed to open codec through avcodec_open2");
// init packet & frame:
  if (av_frame = av_frame_alloc(); !av_frame)
    LOG("Reader: failed to allocated memory for AVFrame");
  if (av_frame_output = av_frame_alloc(); !av_frame_output)
    LOG("Reader: failed to allocated memory for av_frame_output");
  if (av_packet = av_packet_alloc(); !av_packet)
    LOG("Reader: failed to allocated memory for AVPacket");
// init scaler:
  if (width == 0)
    width = codec_context->width;
  if (height == 0)
    height = codec_context->height;
  dst_pix_fmt = convert(col_type);
  iferror(av_image_alloc(av_frame_output->data, av_frame_output->linesize,
    width, height, dst_pix_fmt, ALIGN) < 0, "Reader: av_image_alloc error");
  av_frame_output->format = dst_pix_fmt;
  av_frame_output->width = width;
  av_frame_output->height = height;
  // TODO add opt 4 select interpolation alg
  swsctx = sws_getContext(
    codec_context->width, codec_context->height,
    codec_context->pix_fmt, av_frame_output->width,
    av_frame_output->height, dst_pix_fmt,
    SWS_BICUBIC, nullptr, nullptr, nullptr);
  iferror( !swsctx, "Reader: fail to sws_getCachedContext");
  swsctx4seze = sws_getContext(width, height, dst_pix_fmt,
    width, height, dst_pix_fmt, SWS_POINT, nullptr, nullptr, nullptr);
  iferror( !swsctx4seze, "Reader: fail to sws_getCachedContext (swsctx4seze)");
  framebuffer_info.x = width;
  framebuffer_info.y = height;
  framebuffer_info.type = col_type;
  framebuffer_info.stride = get_size(col_type) * width;
  LOG("Reader: framebuffer_info settings = " << width << 'x'
    << height << ", " << av_get_pix_fmt_name(dst_pix_fmt) 
    << " stride=" << framebuffer_info.stride << std::endl);
} // Reader c-tor

Reader::~Reader() {
  avformat_close_input(&format_context);
  avcodec_free_context(&codec_context);
  av_frame_free(&av_frame);
  av_frame_free(&av_frame_output);
  av_packet_free(&av_packet);
  sws_freeContext(swsctx);
  sws_freeContext(swsctx4seze);
}

CN(FramebufferInfo) Reader::get_framebuffer_info() const
  { return framebuffer_info; }

double Reader::get_framerate() const { return framerate; }
CP(AVStream) Reader::get_stream() const { return in_stream; }
CP(AVCodecContext) Reader::get_codec_ctx() const { return codec_context; }

bool Reader::read_to(Image* dst) {
#ifdef DEBUG
  iferror( !dst, "Writer <<: !image");
  iferror(framebuffer_info.x != dst->X,
    "Writer <<: framebuffer_info.x != dst.x");
  iferror(framebuffer_info.y != dst->Y,
    "Writer <<: framebuffer_info.y != dst.y");
#endif
  uint timeout = 100;
  while (timeout != 0) { // find vstream
    // fill the Packet with data from the Stream
    if (av_read_frame(format_context, av_packet) < 0) {
      LOG("read_to: end of input file\n");
      return false;
    }
    if (av_packet->stream_index == vstream) {
      //TODO opt for print disable/enable
      //LOG("read_to: AVPacket->pts = " << av_packet->pts);
      if (auto response = decode_packet(av_packet, codec_context,
      av_frame, dst); response < 0)
        return false;
      break;
    }
    av_packet_unref(av_packet);
    --timeout;
  } // while
  return true;
} // read_to

int Reader::decode_packet(AVPacket *pPacket,
AVCodecContext *pCodecContext, AVFrame *pFrame, Image* dst) {
  int response = avcodec_send_packet(pCodecContext, pPacket);
  if (response < 0) {
    LOG("decode_packet: Error while sending a packet to the decoder\n");
    return response;
  }
  while (response >= 0) {
    // Return decoded output data (into a frame) from a decoder
    // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
    response = avcodec_receive_frame(pCodecContext, pFrame);
    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
      break;
    } else if (response < 0) {
      LOG("decode_packet: Error while receiving a frame from the decoder\n");
      return response;
    }
    if (response >= 0) {
      sws_scale(swsctx, pFrame->data, pFrame->linesize, 0,
        codec_context->height, av_frame_output->data,
        av_frame_output->linesize);
    // перенос из av_frame в данные seze_image
      uint8_t* framebuffer_data[1] =
        { rcast(uint8_t*, dst->get_data()) };
      int framebuffer_stride[1] = { dst->stride };
      av_image_copy_to_buffer(dst->get_data(), dst->bytes,
        av_frame_output->data, av_frame_output->linesize, dst_pix_fmt,
          dst->X, dst->Y, 1); // use aligment 1 byte only
    }
  } // while response
  return 0;
} // decode_packet

ReaderCtx Reader::get_ctx() const {
  ReaderCtx ret;
  ret.in_vid_x = codec_context->width;
  ret.in_vid_y = codec_context->height;
  ret.av_frame = av_frame;
  ret.av_packet = av_packet;
  ret.codec = codec;
  ret.codec_context = codec_context;
  ret.format_context = format_context;
  ret.framerate = framerate;
  ret.stream = in_stream;
  return ret;
}

} // seze ns
