#pragma once
#include "tvsim2-base.hpp"

class Tvsim2bw: public Tvsim2_base {
  nocopy(Tvsim2bw);

protected:
  shared_p<seze::Image> bw_img {};
  shared_p<seze::Image> bw_img_scaled {};
  shared_p<seze::Image> display {}; ///< для симуляции затухания кинескопа
  Point<real> beam {0, 0}; ///< расположение луча кинескопа
  uint sync_cnt {0}; ///< отсчёт времени при синхроимпульсе
  bool is_pix {false}; ///< сейчас рисуется пиксель
  bool is_odd_str {false}; ///< для черезстрочки
  v_luma_t stream {}; ///< содержит tv сигнал
  size_t str_sz {0};
  v_luma_t buf_a {}; ///< буффер для AM модуляции
  v_luma_t buf_b {}; ///< буффер для AM модуляции

  virtual void downscale();
  virtual void upscale();
  void desaturate_(CN(seze::Image) src);
  void convert_to_dst(seze::Image &dst);
  void encode_stream(CN(seze::Image) src);
  void decode_stream(seze::Image &dst);
  //! определяет сколько нужно памяти под tv stream
  size_t resize_stream(CN(seze::Image) src);
  real encode_pix(luma_t src) const;
  luma_t decode_pix(real src) const;
  void update();
  void display_simul(seze::Image &dst);
  void am_modulate(v_luma_t &dst, real noise_level, int filter_power);
  void draw_debug(seze::Image &dst) const;
  virtual void amplify(real amp=1.0);

public:
  Tvsim2bw();
  ~Tvsim2bw() = default;
  //! main processing func
  void operator ()(CN(seze::Image) src, seze::Image &dst) override;
}; // Tvsim2bw
