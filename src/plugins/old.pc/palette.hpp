#pragma once
#include <vector>
#include "image/rgb24.hpp"
#include "utils/str.hpp"

using Palette = std::vector<seze::RGB24>;

void init_rgb1b(Palette &pal);

//! loading paint dot net palette
void load_pdn_pal(Palette &pal, CN(Str) fname);
