#include <array>
#include <cstdlib>
#include "util.hpp"
#include "tvsim2bw/tvsim2bw.hpp"
#include "utils/pparser.hpp"
#include "utils/str.hpp"

void imgui_desat(auto &tvsim) {
  int sel {int(tvsim.desat_type)};
  Cstr iteams {
    "average\0"
    "BT2001\0"
    "BT709\0"
    "BT601\0"
    "HSL\0"
    "Euclidian distance\0"
    "Red\0"
    "Green\0"
    "Blue\0"
  };
  if (ImGui::Combo("desaturation", &sel, iteams))
    tvsim.desat_type = desaturation_e(sel);
} // imgui_desat

void imgui_proc(auto &tvsim) {
  ImGui::Begin("config");
  imgui_desat(tvsim);
  ImGui::End();
}

SDL_MAIN {
  auto [window, renderer] = init_sdl();
  init_imgui(window, renderer);
  Tvsim2bw tvsim;
  seze::Image src, dst;
  auto parser = seze::pparser({
    {
      {"-i", "--input"},
      "input image",
      [&src](CN(Str) name) { load(src, name); },
      true
    }
  });
  parser(argc, argv);

  SDL_Texture *tex =
    SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
      SDL_TEXTUREACCESS_TARGET, src.X, src.Y);
  dst.init(src.X, src.Y, src.type);

  while ( !tvsim2bw::is_end) {
    proc_sdl_event(window);
    start_frame(renderer);
    imgui_proc(tvsim);
    tvsim(src, dst);
    draw_image(dst, tex, renderer);
    draw_sdl_imgui(renderer);
  }
  
  finalize_sdl_imgui(window, renderer, tex);
  return EXIT_SUCCESS;
} // main
