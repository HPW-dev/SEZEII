#pragma once
#include "types.hpp"
#include "utils/point.hpp"
#include "utils/macro.hpp"

struct tvsim_conf {
  Point<int> scale_wh {320, 240};
  desaturation_e desat_type {desaturation_e::average};
  scale_e scale_type_in {scale_e::bilinear_fast};
  scale_e scale_type_out {scale_e::bilinear_fast};
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
  real pre_amp {1.0}; ///< предусиление сигнала
  real amp {1.0}; ///< усиление сигнала
  filter_e filter_type {filter_e::average_fast};
  int filter_power {3};
  real noise_level {0.066f};
  real am_freg {1};
  real am_depth {1};
  real am_tune {1};
  bool fix_opts {true}; ///< корректирование настроек
  bool interlacing {true};
  bool use_fading {true};
  bool use_scale {true}; ///< при 0 юзает RAW разрешение
  bool use_am {false}; ///< использовать AM модуляцию
  bool debug {false}; ///< включает отображение осцилографа
  bool debug_black_bg {false}; ///< чёрный фон осцила
}; // tvsim_conf

Str conf_to_opts(CN(tvsim_conf) conf);
void opts_to_conf(CN(Str) str, tvsim_conf &conf);
