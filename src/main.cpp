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

std::atomic_bool is_end {false};
std::atomic_int64_t current_frame {0};
std::atomic_int64_t total_frames {0};

void print_progress() {
  using namespace std::literals::chrono_literals;
  static TimePoint tm {};
  constexpr real time_protion (3);
  auto diff {time_diff_sec(get_time(), tm)};
  if (diff >= time_protion) {
    LOG("progress: " << int(double(current_frame) / total_frames * 100.0)
      << "%\n");
    tm = get_time();
  }
}

auto load_plugin() {
  auto plugin {new_plugin(seze::pname)};
  auto pligin_info {plugin->init(seze::popts)};
  print_plugin_info(pligin_info);
  correct_plugin_settings(pligin_info);
  print_converting_info();
  return std::tuple(plugin, pligin_info);
}

auto init_render(CN(auto) pligin_info) {
  auto reader = make_shared_p<seze::Reader>(seze::iname,
    pligin_info.in_x, pligin_info.in_y,
    pligin_info.color_type);
  return std::tuple(reader, reader->get_ctx());
}

auto init_thread_buffers(CN(auto) reader) {
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

void fix_output_resolution(CN(auto) reader_ctx) {
  if (seze::width == 0)
    seze::width = reader_ctx.in_vid_x;
  if (seze::height == 0)
    seze::height = reader_ctx.in_vid_y;
}

auto init_sdl() {
  SDL_Window* window = nullptr;
  SDL_Surface* surface = nullptr;
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

SDL_MAIN {
  parse_args(argc, argv);
  auto [plugin, pligin_info] {load_plugin()};
  auto [reader, reader_ctx] {init_render(pligin_info)};
  auto [vp_framebuffer, framebuffer_info] {init_thread_buffers(*reader)};
  fix_output_resolution(reader_ctx);
  shared_p<seze::Writer> writer {
    seze::nout
    ? nullptr
    : make_shared_p<seze::Writer>(seze::oname,
      framebuffer_info, seze::width, seze::height,
      reader_ctx, seze::simple_encoder)
  };
  // я не знаю как узнать сколько кадров в видео ;]
  total_frames = reader_ctx.duration / (reader_ctx.framerate * 1000.0);
  auto [window, surface, event] {init_sdl()};
// video reading/processing/writing loop
  std::vector<std::thread> v_thread(seze::num_threads);
  while ( !is_end) {
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
      break;
    // read 2 buffers
    for (auto &fb: vp_framebuffer) {
      if (is_end) {
        fb.reset();
        continue;
      }
      if ( !reader->read_to(fb.get())) {
        fb.reset();
        is_end = true;
      }
    } // read 2 frame
    // multithread processing
    for (uint i {0}; auto &fb: vp_framebuffer) {
      if (fb) {
        v_thread[i] = std::thread([&]() {
          plugin->core(fb->get_data(), fb->X, fb->Y,
            fb->stride, fb->type);
          ++current_frame;
        });
      }
      ++i;
    } // enable plugin
    // stop threads
    for (auto &th: v_thread)
      if (th.joinable())
        th.join();
    // render frame:
    if ( !seze::nrend) {
      for (auto &fb: vp_framebuffer) {
        if (fb) {
          SDL_LockSurface(surface);
          image_to_surface(*fb, surface);
          SDL_UnlockSurface(surface);
          SDL_UpdateWindowSurface(window);
        }
      }
    } // if !nrend
    // write frame 2 file
    if ( !seze::nout) {
      for (auto &fb: vp_framebuffer)
        if (fb)
          *writer << fb.get();
    } // write

    print_progress();
  } // loop

  plugin->finalize();
  if ( !seze::nrend) {
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
  return EXIT_SUCCESS;
} // main
