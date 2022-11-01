#pragma once
#include "utils/macro.hpp"
#include "utils/point.hpp"
#include "types.hpp"

namespace seze {
  class Image;
}

struct tvsim_conf {
  Point<int> scale_wh {320, 240};
  desaturation_e desat_type {desaturation_e::average};
  scale_e scale_type_in {scale_e::bilinear};
  scale_e scale_type_out {scale_e::bilinear};
  int hfront {35}; ///< отступ перед строкой
  int hback {21}; ///< отступ после строки
  int hsync_sz {300}; ///< длинна строчного импульса
  int vfront {120}; ///< отступ перед кадром
  int vback {80}; ///< отступ после кадра
  int vsync_sz {6'571};  ///< длинна кадрового импульса
  int vsync_needed_cnt {1'000}; ///< через сколько сихнхроимпульсов считается vsync
  real beam_spd_x {1}; ///< скорость движения луча вправо
  real beam_spd_y {1}; ///< скорость движения луча вниз
  real beam_spd_back {10}; ///< скорость движения луча назад
  real white_lvl {1}; ///< уровень белого
  real black_lvl {0}; ///< уровень чёрного
  real beam_off_signal {-0.1}; ///< сигнал гашения
  real sync_lvl {-0.43f}; ///< уровень синхроимпульсов
  real sync_signal {-0.5f}; ///< уровень синхроимпульсов
  real fading {0.1};
  real amp {1.0}; ///< усиление сигнала
  real ringing_ratio {0.27f};
  int ringing_len {13};
  real ringing_power {1.3f};
  filter_e filter_type {filter_e::average_fast};
  int filter_power {3};
  real noise_level {0.066f};
  bool fix_opts {true}; ///< корректирование настроек
  bool interlacing {true};
  bool use_fading {true};
  bool use_scale {true}; ///< при 0 юзает RAW разрешение
  bool use_ringing {false};
}; // tvsim_conf

class Tvsim2bw final {
  nocopy(Tvsim2bw);

  shared_p<seze::Image> bw_img {};
  shared_p<seze::Image> bw_img_scaled {};
  shared_p<seze::Image> display {}; ///< для симуляции затухания кинескопа
  Point<real> beam {0, 0}; ///< расположение луча кинескопа
  uint sync_cnt {0}; ///< отсчёт времени при синхроимпульсе
  bool is_pix {false}; ///< сейчас рисуется пиксель
  bool is_odd_str {false}; ///< для черезстрочки
  vector_t<real> stream {}; ///< содержит tv сигнал
  size_t str_sz {0};

  void downscale();
  void upscale();
  void desaturate_(CN(seze::Image) src);
  void convert_to_dst(seze::Image &dst);
  void encode_stream(CN(seze::Image) src);
  void decode_stream(seze::Image &dst);
  //! определяет сколько нужно памяти под tv stream
  void resize_stream(CN(seze::Image) src);
  real encode_pix(luma_t src) const;
  luma_t decode_pix(real src) const;
  void update();
  void display_simul(seze::Image &dst);
  void ringing(std::vector<luma_t> &stream,
    real ratio, int len, real power);

public:
  tvsim_conf conf {};

  Tvsim2bw();
  ~Tvsim2bw() = default;
  //! main processing func
  void operator ()(CN(seze::Image) src, seze::Image &dst);
}; // Tvsim2bw
