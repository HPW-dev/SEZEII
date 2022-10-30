#include <filesystem>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "util.hpp"

void load(seze::Image &dst, CN(Str) fname) {
  // открыть файл
  assert( !fname.empty());
  std::ifstream file(fname, std::ios_base::binary);
  assert(file);
  auto file_size {std::filesystem::file_size(fname)};
  v_byte_t data(file_size);
  file.read(reinterpret_cast<char*>(data.data()), file_size);
  assert(!data.empty());
  // распарсить пикчу
  int x, y;
  int comp; // сколько цветовых каналов
  auto decoded = stbi_load_from_memory(rcast(CP(stbi_uc), data.data()),
    data.size(), &x, &y, &comp, STBI_rgb);
  assert(decoded);
  // перенос данных на растр:
  dst.init(x, y, color_t::seze_RGB24);
  int rgb_index {0};
  for (int i {0}; i < dst.size; ++i) {
    auto &col {dst.fast_get<seze::RGB24>(i)};
    col.R = decoded[rgb_index + 0];
    col.G = decoded[rgb_index + 1];
    col.B = decoded[rgb_index + 2];
    rgb_index += 3;
  }
  stbi_image_free(decoded);
}
