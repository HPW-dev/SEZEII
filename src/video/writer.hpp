#pragma once
#include "utils/str.hpp"
#include "image/image.hpp"
#include "ffcommon.hpp"

namespace seze {

class Writer {
  ReaderCtx reader_ctx = {};
  int64_t pts = 0;
  CP(AVCodec) codec = {};
  AVFrame* av_frame = {};
  AVStream* stream = {};
  AVStream* video_avs = {};
  SwsContext* swsctx = {};
  CP(AVOutputFormat) format = {};
  AVCodecContext* codec_context = {};
  AVFormatContext* format_context = {};
  AVPixelFormat in_pix_fmt = AV_PIX_FMT_NONE;
  AVPixelFormat out_pix_fmt = AV_PIX_FMT_NONE;
  int frame_x = 0, frame_y = 0;
  int in_stride = 0;
  bool raw_mode = false; ///< for fast pixel copy
public:
  Writer(CN(Str) oname, CN(FramebufferInfo) framebuffer_info_,
    int out_x, int out_y, CN(ReaderCtx) reader_ctx_,
    bool simple_encoder=false);
  ~Writer();
  void operator << (Image* image);
private:
  void encode();
  void write_frame();
}; // Writer

} // seze ns
