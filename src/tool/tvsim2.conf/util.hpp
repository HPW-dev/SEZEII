#pragma once
#include <tuple>
#include <cassert>
#include <SDL2/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "global.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

namespace seze {
  class Image;
}

void load(seze::Image &dst, CN(Str) fname);

#ifdef WINDOWS
#define SDL_MAIN int SDL_main(int argc, char* argv[])
#else
#define SDL_MAIN int main(int argc, char* argv[])
#endif

inline auto init_sdl(CN(Str) title) {
#if !SDL_VERSION_ATLEAST(2,0,17)
  #error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif
  assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER
    | SDL_INIT_GAMECONTROLLER) == 0);
  auto window = SDL_CreateWindow(title.c_str(),
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    tvsim2bw::wnd_w, tvsim2bw::wnd_h,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  auto renderer = SDL_CreateRenderer(window, -1,
    SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  assert(renderer);
  return std::tuple(window, renderer);
}

inline void init_imgui(auto window, auto renderer) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto &io {ImGui::GetIO()};
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer_Init(renderer);
}

inline void proc_sdl_event(auto window) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type) {
      case SDL_QUIT: { tvsim2bw::is_end = true; break; }
      case SDL_KEYDOWN: {
        if (event.key.keysym.sym == SDLK_ESCAPE)
          tvsim2bw::is_end = true;
        break;
      }
      case SDL_WINDOWEVENT: {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE
        && event.window.windowID == SDL_GetWindowID(window))
          tvsim2bw::is_end = true;
        break;
      }
      default: break;
    }
  } // switch sdl event type
}

inline void start_frame(auto renderer) {
  ImGui_ImplSDLRenderer_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
  SDL_RenderClear(renderer);
}

inline void draw_sdl_imgui(auto renderer) {
  ImGui::Render();
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
  SDL_RenderPresent(renderer);
}
  
inline void finalize_sdl_imgui(auto window, auto renderer, auto tex) {
  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(tex);
  SDL_Quit();
}

inline void draw_image(seze::Image &image, auto tex, auto renderer) {
  SDL_UpdateTexture(tex, nullptr, (CP(void))image.get_cdata(), image.stride);
  SDL_Rect texture_rect;
  texture_rect.x = 0;  //the x coordinate
  texture_rect.y = 0; // the y coordinate
  texture_rect.w = image.X; //the width of the texture
  texture_rect.h = image.Y;
  SDL_RenderCopy(renderer, tex, nullptr, &texture_rect);
}
