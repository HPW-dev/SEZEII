#include "tvsim.hpp"
#include "../color-type.h"
#include "utils.hpp"

class TVsim_YUV: public TVsim {
  seze::Image* display = 0; ///< 4 interlace fading
  seze::Image* buffer_y = 0;
  seze::Image* buffer_u = 0;
  seze::Image* buffer_v = 0;
  component beam_x = 0, beam_y = 0;
  int sync_count = 0;
  bool is_data = false;
  bool is_color = false;
  bool is_odd_string = false; ///< for interlace
  component mul_bw_diff = 0; ///< for optimization
public:
  TVsim_YUV(int mx, int my);
  ~TVsim_YUV();
  void process(seze::Image& dst) override;
private:
  Stream encode(CN(seze::Image) src) const;
  void decode_to_display(CN(Stream) src_y, CN(Stream) src_u,
    CN(Stream) src_v);
  void RGB24_to_buffers(CN(seze::Image) src);
};
