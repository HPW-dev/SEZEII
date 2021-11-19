#pragma once
#include "../image/color.hpp"
// FFmpeg
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}
#include <map>

namespace seze {

struct ReaderCtx {
  double framerate = 0;
  AVCodec* codec = nullptr;
  AVFrame* av_frame = nullptr;
  AVStream* stream = nullptr;
  AVPacket* av_packet = nullptr;
  AVCodecContext* codec_context = nullptr;
  AVFormatContext* format_context = nullptr;
  int in_vid_x = 0, in_vid_y = 0;
};

struct FramebufferInfo {
  int x = 0, y = 0, stride = 0;
  color_t type = color_t::none;
};

//!@class global ffmpeg initializer
class FFctor {
public:
  FFctor();
};
extern FFctor ffctor; // in ffcommon.cpp

AVPixelFormat convert(color_t type);
color_t convert(AVPixelFormat type);

} // seze ns
