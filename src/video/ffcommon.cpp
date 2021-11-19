#include "ffcommon.hpp"
#include "../utils/error.hpp"

namespace seze {

FFctor::FFctor() {
#ifdef FFLOG
  av_log_set_level(AV_LOG_DEBUG);
#endif
}
namespace seze { FFctor ffctor; }

AVPixelFormat convert(color_t type) {
  static const std::map<color_t, AVPixelFormat> table = {
    {color_t::RGB24, AV_PIX_FMT_RGB24},
    {color_t::BGR24, AV_PIX_FMT_BGR24},
    {color_t::RGB555, AV_PIX_FMT_RGB555LE},
    {color_t::RGB565, AV_PIX_FMT_RGB565LE},
    {color_t::YUV24, AV_PIX_FMT_YUV444P},
    {color_t::RAW, AV_PIX_FMT_NONE},
    {color_t::gray, AV_PIX_FMT_GRAY8},
  };
  try {
    return table.at(type);
  } catch(...) {
    error("convert color_t to avformat: mapping error");
  }
}

color_t convert(AVPixelFormat type) {
  static const std::map<AVPixelFormat, color_t> table = {
    {AV_PIX_FMT_RGB24, color_t::RGB24},
    {AV_PIX_FMT_BGR24, color_t::BGR24},
    {AV_PIX_FMT_RGB555LE, color_t::RGB555},
    {AV_PIX_FMT_RGB565LE, color_t::RGB565},
    {AV_PIX_FMT_YUV444P, color_t::YUV24},
    {AV_PIX_FMT_NONE, color_t::RAW},
    {AV_PIX_FMT_GRAY8, color_t::gray},
  };
  try {
    return table.at(type);
  } catch(...) {
    error("convert avformat to color_t: mapping error");
  }
}

} // seze ns
