#pragma once
#include "utils/str.hpp"
#include "image/image.hpp"
#include "utils/macro.hpp"
#include "ffcommon.hpp"

namespace seze {

//!@class video decoder
class Reader {
  FramebufferInfo framebuffer_info = {};
  double framerate = 0;
  AVCodec* codec = nullptr;
  AVFrame* av_frame = nullptr;
  AVFrame* av_frame_output = nullptr;
  AVStream* in_stream = nullptr;
  AVPacket* av_packet = nullptr;
  SwsContext* swsctx = nullptr;
  SwsContext* swsctx4seze = nullptr;
  AVCodecContext* codec_context = nullptr;
  AVFormatContext* format_context = nullptr;
  AVCodecParameters* codec_parms = nullptr;
  AVPixelFormat dst_pix_fmt = AV_PIX_FMT_NONE;
  uint vstream = -1;
public:
  Reader(CN(Str) iname, int width=0, int height=0,
    color_t col_type=seze_RGB24);
  ~Reader();
  CN(FramebufferInfo) get_framebuffer_info() const;
  CP(AVStream) get_stream() const;
  double get_framerate() const;
  CP(AVCodecContext) get_codec_ctx() const;
  //! if nocopy2dst is tru, copy to internal framebuffer
  bool read_to(Image* dst);
  ReaderCtx get_ctx() const;
private:
  int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext,
    AVFrame *pFrame, seze::Image* dst);
}; // Reader

} // seze ns
