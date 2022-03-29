#include "bw.hpp"

TVsim_BW::TVsim_BW(int mx, int my) {
  display = new seze::Image(mx, my, component_type);
  buffer = new seze::Image(mx, my, component_type);
  check_config();
} // TVsim_BW c-tor

TVsim_BW::~TVsim_BW() {
  delete display;
  delete buffer;
}

void TVsim_BW::process(seze::Image& dst) {
  if (dst.type == seze_RGB24)
    RGB24_to_gray(*buffer, dst);
  else if (dst.type == seze_gray)
    gray_to_fgray(*buffer, dst);
  else
    error("TVsim_BW.process: unsupported color format");
  auto stream = encode(*buffer);
  if (tv::enable_AM)
    modulate(stream, tv::AM_ratio_in);
  apply_noise(stream, tv::noise_level, tv::power,
    tv::signal_shift);
  tv_signal_filter(stream, tv::filter_power);
  if (tv::enable_AM)
    demodulate(stream, tv::AM_ratio_out, tv::AM_shift);
  decode_to(stream, *buffer);
  if (dst.type == seze_RGB24)
    gray_to_RGB24(dst, *buffer);
  else
    fgray_to_gray(dst, *buffer);
  is_odd_string = !is_odd_string; // next interlace str
} // process

//! signal generation states
enum class State: int {
  start = 0,
  first_str,
  second_str,
  first_bound,
  second_bound,
  data,
  hsync,
  vsync,
  end,
  error,
};

Stream TVsim_BW::encode(CN(seze::Image) src) const {
  Stream ret;
  State state = State::start;
  int src_y;
  // state event loop:
  while (state != State::end) {
    switch (state) {
      case State::start: {
        src_y = 0;
        state = State::first_str;
        break;
      }
      case State::first_str: {
        FOR (str, tv::first_strings) {
          // blank data
          FOR (x, src.X + tv::first_bound + tv::second_bound)
            ret.data.push_back(tv::empty_level);
          // hsync
          FOR (x, tv::hsync_size)
            ret.data.push_back(tv::sync_level);
        }
        state = State::first_bound;
        break;
      }
      case State::second_str: {
        FOR (str, tv::second_strings) {
          // blank data
          FOR (x, src.X + tv::first_bound + tv::second_bound)
            ret.data.push_back(tv::empty_level);
          // hsync
          FOR (x, tv::hsync_size)
            ret.data.push_back(tv::sync_level);
        }
        state = State::vsync;
        break;
      }
      case State::first_bound: {
        FOR (x, tv::first_bound)
          ret.data.push_back(tv::empty_level);
        state = State::data;
        break;
      }
      case State::second_bound: {
        FOR (x, tv::second_bound)
          ret.data.push_back(tv::empty_level);
        state = State::hsync;
        break;
      }
      case State::data: {
        FOR (x, src.X) {
          component luma;
          if (tv::interlace) {
            if (is_odd_string)
              luma = src.get<component>(x, src_y + 1);
            else
              luma = src.fast_get<component>(x, src_y);
          } else
            luma = src.fast_get<component>(x, src_y);
          // [0..1] to [black..white]
          luma *= tv::white_level - tv::black_level;
          luma += tv::black_level;
          ret.data.push_back(luma);
        }
        state = State::second_bound;
        break;
      }
      case State::hsync: {
        FOR (x, tv::hsync_size)
          ret.data.push_back(tv::sync_level);
        src_y += tv::interlace ? 2 : 1;
        if (src_y >= src.Y)
          state = State::second_str;
        else
          state = State::first_bound;
        break;
      }
      case State::vsync: {
        FOR (x, tv::vsync_size)
          ret.data.push_back(tv::sync_level);
        state = State::end;
        break;
      }
      case State::end: break;
      case State::error:
      default: error("TVsim_BW.encode: state error");
    } // switch
  } // while
  ret.size = ret.data.size();
  return ret;
} // encode

void TVsim_BW::decode_to(CN(Stream) src, seze::Image& dst) {
  // color fading
  FOR (i, display->size) {
    auto& c = display->fast_get<component>(i);
    c -= tv::fading;
  }
  // interlace str
  int odd_y;
  if (tv::interlace)
    odd_y = is_odd_string ? 1 : 0;
  else
    odd_y = 0;
  // signal decode
  FOR (i, src.size) {
    auto val = src.data[i];
    // beam bounds:
    beam_x = std::clamp<component>(beam_x, -tv::hbound,
      display->X + tv::hbound);
    beam_y = std::clamp<component>(beam_y, -tv::hbound,
      display->Y + tv::hbound);
    // data
    if (val >= tv::black_level) {
      beam_x += tv::beam_speed;
      is_data = true;
      auto Y = tv_ringing(src, i, tv::ringing_ratio,
        tv::ringing_length, tv::ringing_power);
      auto luma = (Y - tv::black_level)
        / (tv::white_level - tv::black_level);
      auto p = display->get_ptr<component>(beam_x + tv::hbound, beam_y + odd_y);
      if (p) {
        *p = std::max(luma, *p);
      }
      sync_count = 0;
    }
    // blank string
    if (val > tv::blank_level and val < tv::black_level) {
      if (is_data)
        beam_x += tv::beam_speed;
      sync_count = 0;
    }
    // sync
    if (val <= tv::blank_level) {
      beam_x -= tv::beam_reverse_speed;
      ++sync_count;
      if (is_data)
        beam_y += tv::interlace ? 2 : 1;
      is_data = false;
      if (sync_count > tv::vsync_detect) 
        beam_y -= tv::beam_reverse_speed;
    }
  } // for: stream data
  display->fast_copy_to(dst);
} // decode_to
