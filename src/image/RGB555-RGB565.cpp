#include "RGB555-RGB565.hpp"

namespace seze {

const byte table5[] = {
  0, 8, 16, 25, 33, 41, 49, 58, 66, 74, 82, 90, 99, 107, 115, 123, 132,
  140, 148, 156, 165, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255};
 
const byte table6[] = {
  0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 45, 49, 53, 57, 61, 65, 69,
  73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 130, 134, 138,
  142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190, 194, 198,
  202, 206, 210, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255};

RGB565::RGB565(byte r, byte g, byte b) {
  r = r * 31.0f / 255.0f + 0.5f;
  g = g * 63.0f / 255.0f + 0.5f;
  b = b * 31.0f / 255.0f + 0.5f;
  RGB = r;
  RGB <<= 5;
  RGB |= g;
  RGB <<= 6;
  RGB |= b;
}
void RGB565::set_to(byte& r, byte& g, byte& b) const {
  auto _RGB = RGB; 
  auto _b = _RGB & 0x1F;
  b = table5[_b];
  _RGB >>= 5;
  auto _g = _RGB & 0x3F;
  g = table6[_g];
  _RGB >>= 6;
  auto _r = _RGB & 0x1F;
  r = table5[_r];
}
RGB565::RGB565(uint16_t data): RGB(data) {}

RGB555::RGB555(byte r, byte g, byte b) {
  r = r * 31.0f / 255.0f + 0.5f;
  g = g * 31.0f / 255.0f + 0.5f;
  b = b * 31.0f / 255.0f + 0.5f;
  RGB = (uint16_t(r) << 11) | (uint16_t(g) << 5) | uint16_t(b);
}
void RGB555::set_to(byte& r, byte& g, byte& b) const {
  r = (RGB & 0xF800) >> 11;
  g = (RGB & 0x7E0) >> 5;
  b = RGB & 0x1F; 
}
RGB555::RGB555(uint16_t data): RGB(data) {}

} // seze ns
