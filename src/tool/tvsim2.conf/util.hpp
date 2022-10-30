#pragma once
#include <tuple>
#include <cassert>
#include <SDL2/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "global.hpp"

#ifdef WINDOWS
#define SDL_MAIN int SDL_main(int argc, char* argv[])
#else
#define SDL_MAIN int main(int argc, char* argv[])
#endif

auto init_sdl() {
#if !SDL_VERSION_ATLEAST(2,0,17)
  #error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif
  assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER
    | SDL_INIT_GAMECONTROLLER) == 0);
  auto window = SDL_CreateWindow("TVsim 2 configuration util",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    tvsim2::wnd_w, tvsim2::wnd_h,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  auto renderer = SDL_CreateRenderer(window, -1,
    SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  assert(renderer);
  return std::tuple(window, renderer);
}

void init_imgui(auto window, auto renderer) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto &io {ImGui::GetIO()};
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer_Init(renderer);
}

void proc_sdl_event(auto window) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type) {
      case SDL_QUIT:
        tvsim2::is_end = true;
        break;
      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_CLOSE
        && event.window.windowID == SDL_GetWindowID(window))
          tvsim2::is_end = true;
        break;
      default: break;
    }
  } // switch sdl event type
}

void start_frame_imgui() {
  ImGui_ImplSDLRenderer_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}

void draw_sdl_imgui(auto renderer) {
  ImGui::Render();
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
  SDL_RenderPresent(renderer);
}
  
void finalize_sdl_imgui(auto window, auto renderer) {
  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
