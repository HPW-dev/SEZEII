#pragma once
#include "tvsim.hpp"
#include "../color-type.hpp"
#include "utils.hpp"

class TVsim_BW: public TVsim {
  seze::Image* display = 0; ///< 4 interlace fading
  seze::Image* buffer = 0;
  component beam_x = 0, beam_y = 0;
  int sync_count = 0;
  bool is_data = false;
  bool is_odd_string = false; ///< 4 interlace
public:
  TVsim_BW(int mx, int my);
  ~TVsim_BW();
  void process(seze::Image& dst) override;
private:
  Stream encode(CN(seze::Image) src) const;
  void decode_to(CN(Stream) src, seze::Image& dst);
};
