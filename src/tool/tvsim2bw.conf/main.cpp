#include <cstdlib>
#include "util.hpp"
#include "tvsim2bw/tvsim2bw.hpp"
#include "utils/pparser.hpp"

void imgui_proc(auto &tvsim) {
  /*ImGui::Begin("preview");
  ImGui::Text("pointer = %p", my_image_texture);
  ImGui::Text("size = %d x %d", my_image_width, my_image_height);
  ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
  ImGui::End();*/
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
    start_frame_imgui();
    imgui_proc(tvsim);
    tvsim(src, dst);
    draw_image(dst, tex, renderer);
    draw_sdl_imgui(renderer);
  }
  
  finalize_sdl_imgui(window, renderer, tex);
  return EXIT_SUCCESS;
} // main
