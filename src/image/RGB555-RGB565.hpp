#pragma once
#include "utils/macro.hpp"

namespace seze {

extern const byte table5[]; 
extern const byte table6[];

struct RGB565 {
public:
  uint16_t RGB = 0; ///< 5:6:5
  RGB565(byte r=0, byte g=0, byte b=0);
  RGB565(uint16_t data);
  //! copy components to vals
  void set_to(byte& r, byte& g, byte& b) const;
}; // RGB565

struct RGB555 {
public:
  uint16_t RGB = 0; ///< 5:5:5
  RGB555(byte r=0, byte g=0, byte b=0);
  RGB555(uint16_t data);
  //! copy components to vals
  void set_to(byte& r, byte& g, byte& b) const;
}; // RGB555

/** https://stackoverflow.com/a/9069480

RGB565 -> RGB888
R8 = ( R5 * 527 + 23 ) >> 6;
G8 = ( G6 * 259 + 33 ) >> 6;
B8 = ( B5 * 527 + 23 ) >> 6;

RGB888 -> RGB565
R5 = ( R8 * 249 + 1014 ) >> 11;
G6 = ( G8 * 253 +  505 ) >> 10;
B5 = ( B8 * 249 + 1014 ) >> 11;
*/

} // seze ns