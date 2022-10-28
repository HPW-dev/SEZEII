#pragma once
#include <atomic>
#include "plugins/plugin-shared.hpp"
#include "utils/types.hpp"

namespace seze {

inline Str iname {}; // input video file
inline Str oname {}; // output video file
inline Str pname {}; // plugin file
inline Str popts {}; // plugin options
inline uint num_threads {0};
inline int width {0}, height {0}; // if not 0, custom output video size
inline int framebuffer_x {0}, framebuffer_y {0};
inline plugin_format_t plugin_format {plugin_format_t::unknown};
inline bool nout {false}; // if 1, disable video output
inline bool nrend {false}; // if 1, disable rendering output framebuffer
inline bool simple_encoder {false}; // if 1, enable h264 crf 23 nv12
inline std::atomic_bool is_end {false};
inline std::atomic_bool is_pause {false};
inline std::atomic_int64_t current_frame {0};
inline std::atomic_int64_t total_frames {0};

} // seze ns
