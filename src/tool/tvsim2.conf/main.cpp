#include <cstdlib>
#include "util.hpp"
#include "tvsim2/tvsim2.hpp"

SDL_MAIN {
  auto [window, renderer] = init_sdl();
  init_imgui(window, renderer);

  while ( !tvsim2::is_end) {
    proc_sdl_event(window);
    start_frame_imgui();
    // imgui begin end TODO
    draw_sdl_imgui(renderer);
  }
  
  finalize_sdl_imgui(window, renderer);
  return EXIT_SUCCESS;
} // main
