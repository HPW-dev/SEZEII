#include <sstream>
#include <fstream>
#include "palette.hpp"
#include "utils/macro.hpp"
#include "utils/error.hpp"

void init_rgb1b(Palette &pal) {
  FOR (ir, 2)
    FOR (ig, 2)
      FOR (ib, 2)
        pal.push_back(seze::RGB24(ir*255, ig*255, ib*255));
}

static uint32_t hex_to_u32(CN(Str) str) {
  std::stringstream ss;
  ss << std::hex << str;
  uint32_t ret;   
  ss >> ret;
  return ret;
}

void load_pdn_pal(Palette &pal, CN(Str) fname) {
  iferror (fname.empty(), "load_pnd_pal: file name is empty");
  std::fstream file(fname, std::ios_base::in);
  iferror ( !file || file.bad(), "load_pnd_pal: file reading error");
  Str line;
  while ( !file.eof()) {
    std::getline(file, line);
    if (line.empty())
      continue;
    // игнорить коменты
    if (line[0] == ';')
      continue;
    auto u32 {hex_to_u32(line)};
    // AABBGGRR
    seze::RGB24 color(
      (u32 >> 16) & 0xFF,
      (u32 >> 8) & 0xFF,
      u32 & 0xFF
    );
    pal.push_back(color);
  }
  iferror(pal.empty(), "load_pnd_pal: output pal is empty");
} // pal
