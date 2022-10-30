#include <cstdlib>
#include "util.hpp"
#include "tvsim2bw/tvsim2bw.hpp"

void imgui_proc(auto &tvsim) {

}

SDL_MAIN {
  auto [window, renderer] = init_sdl();
  init_imgui(window, renderer);
  Tvsim2bw tvsim;
  seze::Image src, dst;

  while ( !tvsim2bw::is_end) {
    proc_sdl_event(window);
    start_frame_imgui();
    imgui_proc(tvsim);
    tvsim(src, dst);
    draw_image(dst, renderer);
    draw_sdl_imgui(renderer);
  }
  
  finalize_sdl_imgui(window, renderer);
  return EXIT_SUCCESS;
} // main
