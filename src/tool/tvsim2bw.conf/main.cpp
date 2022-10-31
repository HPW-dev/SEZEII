#include <array>
#include <cstdlib>
#include "util.hpp"
#include "tvsim2bw/tvsim2bw.hpp"
#include "utils/pparser.hpp"
#include "utils/str.hpp"
#include "utils/time.hpp"

void imgui_desat(auto &conf) {
  int sel {int(conf.desat_type)};
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
    conf.desat_type = desaturation_e(sel);
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

void imgui_scale_in_out(auto &conf) {
  int sel_1 {int(conf.scale_type_in)};
  int sel_2 {int(conf.scale_type_out)};
  Cstr iteams {
    "nearest\0"
    "bilinear\0"
    "bicubic\0"
    "bilinear_fast\0"
    "bicubic_fast\0"
  };
  if (ImGui::Combo("scale in", &sel_1, iteams))
    conf.scale_type_in = scale_e(sel_1);
  if (ImGui::Combo("scale out", &sel_2, iteams))
    conf.scale_type_out = scale_e(sel_2);
} // imgui_scale_in_out

void imgui_scale_wh(auto &conf) {
  int v_int[2] {conf.scale_wh.x, conf.scale_wh.y};
  if (ImGui::DragInt2("scale w/h", v_int, 1.0f, 1, 1280))
    conf.scale_wh = {v_int[0], v_int[1]};
}

void imgui_scale_using(auto &conf)
  { ImGui::Checkbox("use scaling", &conf.use_scale); }

void imgui_tvsim_opts(auto &conf) {
  ImGui::Checkbox("interlacing", &conf.interlacing);
  ImGui::Checkbox("use fading", &conf.use_fading);
  ImGui::DragFloat("fading", &conf.fading, 0.0001f, 0, 1, "%.4f");
  ImGui::DragFloat("amplify", &conf.amp, 0.01f, 0, 4.0f);
  ImGui::DragInt("H front", &conf.hfront,   1.0f, 0, 1'000);
  ImGui::DragInt("H back",  &conf.hback,    1.0f, 0, 1'000);
  ImGui::DragInt("H sync",  &conf.hsync_sz, 1.0f, 0, 1'000);
  ImGui::DragInt("V front", &conf.vfront,   6.5f, 0, 5'000);
  ImGui::DragInt("V back",  &conf.vback,    6.5f, 0, 5'000);
  ImGui::DragInt("V sync",  &conf.vsync_sz, 6.5f, 0, 7'000);
  ImGui::DragInt("V sync detect count", &conf.vsync_needed_cnt, 6.5f, 0, 7'000);
  ImGui::DragFloat("beam speed x", &conf.beam_spd_x, 0.01f, 0, 10.0f);
  ImGui::DragFloat("beam speed y", &conf.beam_spd_y, 0.01f, 0, 10.0f);
  ImGui::DragFloat("beam reverse speed", &conf.beam_spd_back, 1.0f, 0, 100.0f);
  ImGui::DragFloat("white level", &conf.white_lvl, 0.01, -2.0f, 2.0f);
  ImGui::DragFloat("black level", &conf.black_lvl, 0.01, -2.0f, 2.0f);
  ImGui::DragFloat("beam off signal value",
    &conf.beam_off_signal, 0.01f, -2.0f, 2.0f);
  ImGui::DragFloat("sync level", &conf.sync_lvl, 0.01f, -2.0f, 2.0f);
  ImGui::DragFloat("sync signal value", &conf.sync_signal, 0.01f, -2.0f, 2.0f);
  ImGui::Checkbox("autofix options", &conf.fix_opts);
} // imgui_tvsim_opts

void imgui_filters(auto &conf) {
  int sel {int(conf.filter_type)};
  Cstr iteams {
    "none\0"
    "average\0"
    "average_fast\0"
    "median\0"
  };
  if (ImGui::Combo("filter", &sel, iteams))
    conf.filter_type = filter_e(sel);
  ImGui::DragInt("filter power", &conf.filter_power, 1, 0, 256);
} // imgui_filters

void imgui_proc(auto &conf) {
  ImGui::Begin("config");
  imgui_draw_fps();
  imgui_desat(conf);
  imgui_scale_wh(conf);
  imgui_scale_in_out(conf);
  imgui_scale_using(conf);
  imgui_tvsim_opts(conf);
  imgui_filters(conf);
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

  tvsim_conf conf;
  while ( !tvsim2bw::is_end) {
    proc_sdl_event(window);
    start_frame(renderer);
    memcpy(&conf, &tvsim.conf, sizeof(conf));
    imgui_proc(conf);
    memcpy(&tvsim.conf, &conf, sizeof(conf));
    tvsim(src, dst);
    draw_image(dst, tex, renderer);
    draw_sdl_imgui(renderer);
  }
  
  finalize_sdl_imgui(window, renderer, tex);
  return EXIT_SUCCESS;
} // main
