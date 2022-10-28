#pragma once
#include <vector>
#include <thread>
#include <SDL2/SDL.h>
#include "util.hpp"
#include "global.hpp"
#include "video/reader.hpp"
#include "video/writer.hpp"
#include "plugins/plugin.hpp"
#include "utils/log.hpp"
#include "utils/time.hpp"

inline void print_progress() {
  using namespace std::literals::chrono_literals;
  static TimePoint tm {};
  constexpr real time_protion (3);
  auto diff {time_diff_sec(get_time(), tm)};
  if (diff >= time_protion) {
    LOG("progress: "
      << int(double(seze::current_frame) / seze::total_frames * 100.0)
      << "%\n");
    tm = get_time();
  }
}

inline auto load_plugin() {
  auto plugin {new_plugin(seze::pname)};
  auto pligin_info {plugin->init(seze::popts)};
  print_plugin_info(pligin_info);
  correct_plugin_settings(pligin_info);
  print_converting_info();
  return std::tuple(plugin, pligin_info);
}

inline auto init_render(CN(auto) pligin_info) {
  auto reader = make_shared_p<seze::Reader>(seze::iname,
    pligin_info.in_x, pligin_info.in_y,
    pligin_info.color_type);
  return std::tuple(reader, reader->get_ctx());
}

inline auto init_thread_buffers(CN(auto) reader) {
  std::vector<shared_p<seze::Image>> vp_framebuffer(seze::num_threads);
  auto framebuffer_info {reader.get_framebuffer_info()};
  seze::framebuffer_x = framebuffer_info.x;
  seze::framebuffer_y = framebuffer_info.y;
  // make buffers for threads
  for (auto &fb: vp_framebuffer)
    fb = make_shared_p<seze::Image>(seze::framebuffer_x,
      seze::framebuffer_y, framebuffer_info.type);
  return std::tuple(vp_framebuffer, framebuffer_info);
}

inline void fix_output_resolution(CN(auto) reader_ctx) {
  if (seze::width == 0)
    seze::width = reader_ctx.in_vid_x;
  if (seze::height == 0)
    seze::height = reader_ctx.in_vid_y;
}

inline auto init_sdl() {
  SDL_Window* window {nullptr};
  SDL_Surface* surface {nullptr};
  SDL_Event event;
  if ( !seze::nrend) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("SEZE II",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
      seze::framebuffer_x, seze::framebuffer_y, 0);
    surface = SDL_GetWindowSurface(window);
    SDL_memset(surface->pixels, 0, surface->h * surface->pitch);
  }
  return std::tuple(window, surface, event);
}

inline void read_to_bufs(auto &reader, auto &vp_framebuffer) {
  for (auto &fb: vp_framebuffer) {
    if (seze::is_end) {
      fb.reset();
      continue;
    }
    if ( !reader->read_to(fb.get())) {
      fb.reset();
      seze::is_end = true;
    }
  }
} // read_to_bufs

inline void proc_bufs(auto &plugin, auto &vp_framebuffer) {
  std::vector<std::thread> v_thread(seze::num_threads);
  for (uint i {0}; auto &fb: vp_framebuffer) {
    if (fb) {
      v_thread[i] = std::thread([&]() {
        plugin->core(fb->get_data(), fb->X, fb->Y,
          fb->stride, fb->type);
        ++seze::current_frame;
      });
    }
    ++i;
  } // enable plugin
  // stop threads
  for (auto &th: v_thread)
    if (th.joinable())
      th.join();
} // proc_bufs

inline void write_to_file(auto &writer, auto &vp_framebuffer) {
  for (auto &fb: vp_framebuffer)
    if (fb)
      writer << fb.get();
}

inline void draw_bufs(auto &window, auto &surface, auto &vp_framebuffer) {
  for (auto &fb: vp_framebuffer) {
    if (fb) {
      SDL_LockSurface(surface);
      image_to_surface(*fb, surface);
      SDL_UnlockSurface(surface);
      SDL_UpdateWindowSurface(window);
    }
  }
}

auto get_total_frames(CN(auto) reader_ctx) {
  // я не знаю как правильно узнать сколько кадров в видео ;]
  return reader_ctx.duration / (reader_ctx.framerate * 1000.0);
}

void proc_sdl(auto event) {
  if (SDL_PollEvent(&event))
    switch (event.type) {
      case SDL_QUIT: {
        seze::is_end = true;
        break;
      }
      case SDL_KEYDOWN: {
        if (event.key.keysym.sym == SDLK_SPACE)
          seze::is_pause = !seze::is_pause;
      }
      default: {}
    } // switch event.type
}

void free_sdl(auto surface, auto window) {
  if ( !seze::nrend) {
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
}
