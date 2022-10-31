#include <array>
#include <cstdlib>
#include "util.hpp"
#include "tvsim2bw/tvsim2bw.hpp"
#include "utils/pparser.hpp"
#include "utils/str.hpp"
#include "utils/time.hpp"

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

void imgui_draw_fps() {
  using namespace std::chrono_literals;
  static auto st {get_time()};
  static uint fps {0};
  static uint fps_for_print {0};
  if (get_time() - st >= 1s) {
    st = get_time();
    fps_for_print = fps;
    fps = 0;
  }
  ++fps;
  ImGui::Text(("fps: " + std::to_string(fps_for_print)).c_str());
}

void imgui_scale_in_out(auto &tvsim) {
  int sel_1 {int(tvsim.scale_type_in)};
  int sel_2 {int(tvsim.scale_type_out)};
  Cstr iteams {
    "nearest\0"
    "bilinear\0"
    "bicubi\0"
  };
  if (ImGui::Combo("scale in", &sel_1, iteams))
    tvsim.scale_type_in = scale_e(sel_1);
  if (ImGui::Combo("scale out", &sel_2, iteams))
    tvsim.scale_type_out = scale_e(sel_2);
} // imgui_scale_in_out

void imgui_scale_wh(auto &tvsim) {
  int v_int[2] {tvsim.scale_wh.x, tvsim.scale_wh.y};
  if (ImGui::DragInt2("scale w/h", v_int, 1.0f, 1, 1280))
    tvsim.scale_wh = {v_int[0], v_int[1]};
}

void imgui_scale_using(auto &tvsim)
  { ImGui::Checkbox("use scaling", &tvsim.use_scale); }

void imgui_proc(auto &tvsim) {
  ImGui::Begin("config");
  imgui_draw_fps();
  imgui_desat(tvsim);
  imgui_scale_wh(tvsim);
  imgui_scale_in_out(tvsim);
  imgui_scale_using(tvsim);
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
