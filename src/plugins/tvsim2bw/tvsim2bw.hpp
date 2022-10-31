#pragma once
#include "utils/macro.hpp"
#include "utils/point.hpp"
#include "types.hpp"

namespace seze {
  class Image;
}

class Tvsim2bw final {
  nocopy(Tvsim2bw);

  shared_p<seze::Image> bw_img {};
  shared_p<seze::Image> bw_img_scaled {};
  Point<real> beam {0, 0}; ///< расположение луча кинескопа
  uint sync_cnt {0}; ///< отсчёт времени при синхроимпульсе
  bool is_pix {false}; ///< сейчас рисуется пиксель
  bool is_odd_str {false}; ///< для черезстрочки
  vector_t<real> stream {}; ///< содержит tv сигнал

  void downscale();
  void upscale();
  void desaturate_(CN(seze::Image) src);
  void convert_to_dst(seze::Image &dst);
  void encode_stream(CN(seze::Image) src);
  void decode_stream(seze::Image &dst);
  //! определяет сколько нужно памяти под tv stream
  void resize_stream(CN(seze::Image) src);

public:
  desaturation_e desat_type {desaturation_e::average};
  scale_e scale_type_in {scale_e::bilinear};
  scale_e scale_type_out {scale_e::bilinear};
  Point<int> scale_wh {320, 240};
  bool use_scale {true}; ///< при 0 юзает RAW разрешение
  uint hfront {35}; ///< отступ перед строкой
  uint hback {21}; ///< отступ после строки
  uint hsync_sz {65}; ///< длинна строчного импульса
  uint vfront {120}; ///< отступ перед кадром
  uint vback {80}; ///< отступ после кадра
  uint vsync_sz {6'571};  ///< длинна кадрового импульса
  uint vsync_needed_cnt {1'000}; ///< через сколько сихнхроимпульсов считается vsync
  real beam_spd {1}; ///< скорость движения луча вперёд
  real beam_spd_back {10}; ///< скорость движения луча назад

  Tvsim2bw();
  ~Tvsim2bw() = default;
  //! main processing func
  void operator ()(CN(seze::Image) src, seze::Image &dst);
}; // Tvsim2bw
