#include "tv-yuv.hpp"

TVsim_YUV::TVsim_YUV(int mx, int my) {
  display = new seze::Image(mx, my, seze::color_t::RGB24);
  buffer_y = new seze::Image(mx, my, component_type);
  buffer_u = new seze::Image(mx, my, component_type);
  buffer_v = new seze::Image(mx, my, component_type);
  check_config();
} // TVsim_YUV c-tor

TVsim_YUV::~TVsim_YUV() {
  delete display;
  delete buffer_y;
  delete buffer_u;
  delete buffer_v;
}

void TVsim_YUV::RGB24_to_buffers(CN(seze::Image) src) {
  FOR (i, src.size()) {
    auto rgb = src.fast_get<seze::RGB24>(i);
    auto yuv = RGB24_to_yuv(rgb);
    buffer_y->fast_set<component>(i, yuv.Y);
    buffer_u->fast_set<component>(i, yuv.U);
    buffer_v->fast_set<component>(i, yuv.V);
  }
}

void TVsim_YUV::process(seze::Image& dst) {
  iferror(dst.type() != seze::color_t::RGB24,
    "TVsim_YUV: dst pix type != RGB24");
  RGB24_to_buffers(dst);
  auto stream_y = encode(*buffer_y);
  is_color = true;
  auto stream_u = encode(*buffer_u);
  auto stream_v = encode(*buffer_v);
  is_color = false;
  if (tv::enable_AM) {
    modulate(stream_y, tv::AM_ratio_in);
    modulate(stream_u, tv::AM_ratio_in_u);
    modulate(stream_v, tv::AM_ratio_in_v);
  }
  apply_noise(stream_y, tv::noise_level, tv::power, tv::signal_shift);
  apply_noise(stream_u, tv::noise_level_u, tv::power_u, tv::signal_shift_u);
  apply_noise(stream_v, tv::noise_level_v, tv::power_v, tv::signal_shift_v);
  tv_signal_filter(stream_y, tv::filter_power);
  tv_signal_filter(stream_u, tv::filter_power_u);
  tv_signal_filter(stream_v, tv::filter_power_v);
  if (tv::enable_AM) {
    demodulate(stream_y, tv::AM_ratio_out, tv::AM_shift);
    demodulate(stream_u, tv::AM_ratio_out_u, tv::AM_shift_u);
    demodulate(stream_v, tv::AM_ratio_out_v, tv::AM_shift_v);
  }
  decode_to_display(stream_y, stream_u, stream_v);
  display->fast_copy_to(dst);
  // TODO disp to dst
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

Stream TVsim_YUV::encode(CN(seze::Image) src) const {
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
          FOR (x, src.get_x() + tv::first_bound + tv::second_bound)
            ret.data.push_back(tv::empty_level);
          // hsync
          FOR (x, tv::hsync_size) {
            auto level = is_color ? tv::empty_level : tv::sync_level;
            ret.data.push_back(level);
          }
        }
        state = State::first_bound;
        break;
      }
      case State::second_str: {
        FOR (str, tv::second_strings) {
          // blank data
          FOR (x, src.get_x() + tv::first_bound + tv::second_bound)
            ret.data.push_back(tv::empty_level);
          // hsync
          FOR (x, tv::hsync_size) {
            auto level = is_color ? tv::empty_level : tv::sync_level;
            ret.data.push_back(level);
          }
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
        FOR (x, src.get_x()) {
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
        FOR (x, tv::hsync_size) {
          auto level = is_color ? tv::empty_level : tv::sync_level;
          ret.data.push_back(level);
        }
        src_y += tv::interlace ? 2 : 1;
        if (src_y >= src.get_y())
          state = State::second_str;
        else
          state = State::first_bound;
        break;
      }
      case State::vsync: {
        FOR (x, tv::vsync_size) {
          auto level = is_color ? tv::empty_level : tv::sync_level;
          ret.data.push_back(level);
        }
        state = State::end;
        break;
      }
      case State::end: break;
      case State::error:
      default: error("TVsim_YUV.encode: state error");
    } // switch
  } // while
  ret.size = ret.data.size();
  return ret;
} // encode

void TVsim_YUV::decode_to_display(CN(Stream) src_y, CN(Stream) src_u,
CN(Stream) src_v) {
  // color fading
  auto display_data_ptr = display->get_data();
  FOR (i, display->bytes()) {
    auto& c = display_data_ptr[i];
    c = std::clamp<int>(c - tv::fading * 255, 0, 255);
  }
  // interlace str
  int odd_y;
  if (tv::interlace)
    odd_y = is_odd_string ? 1 : 0;
  else
    odd_y = 0;
  // signal decode Y
  FOR (i, src_y.size) {
    // beam bounds:
    beam_x = std::clamp<component>(beam_x, -tv::hbound,
      display->get_x() + tv::hbound);
    beam_y = std::clamp<component>(beam_y, -tv::hbound,
      display->get_y() + tv::hbound);
    // data
    if (src_y.data[i] >= tv::black_level) {
      beam_x += tv::beam_speed;
      is_data = true;
      auto Y = tv_ringing(src_y, i, tv::ringing_ratio,
        tv::ringing_length, tv::ringing_power);
      auto U = src_u.data[i];
      auto V = src_v.data[i];
      auto yf = (Y - tv::black_level) / (tv::white_level - tv::black_level);
      auto uf = (U - tv::black_level) / (tv::white_level - tv::black_level);
      auto vf = (V - tv::black_level) / (tv::white_level - tv::black_level);
      //LOG("YUV: " << yf << "," << uf << "," << vf << std::endl);
      auto rgb = yuv_to_RGB24(component_yuv(yf, uf, vf));
      auto p = display->get_ptr<seze::RGB24>(beam_x + tv::hbound,
        beam_y + odd_y);
      if (p) {
        p->R = std::max(rgb.R, p->R);
        p->G = std::max(rgb.G, p->G);
        p->B = std::max(rgb.B, p->B);
      }
      sync_count = 0;
    }
    // blank string
    if (src_y.data[i] > tv::blank_level
    and src_y.data[i] < tv::black_level) {
      if (is_data)
        beam_x += tv::beam_speed;
      sync_count = 0;
    }
    // sync
    if (src_y.data[i] <= tv::blank_level) {
      beam_x -= tv::beam_reverse_speed;
      ++sync_count;
      if (is_data)
        beam_y += tv::interlace ? 2 : 1;
      is_data = false;
      if (sync_count > tv::vsync_detect) 
        beam_y -= tv::beam_reverse_speed;
    }
  } // for: stream data
} // decode_to

