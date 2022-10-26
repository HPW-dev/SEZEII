#include <iostream>
#include <vector>
#include <thread>
#include <SDL2/SDL.h>
#include "util.hpp"
#include "global.hpp"
#include "video/reader.hpp"
#include "video/writer.hpp"
#include "plugins/plugin.hpp"

#ifdef WINDOWS
int SDL_main(int argc, char* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
  parse_args(argc, argv);
// load plugin
  auto plugin = new_plugin_by_ext(seze::pname);
  auto pligin_info = plugin->init(seze::popts);
  print_plugin_info(pligin_info);
  enable_plugin_settings(pligin_info);
  print_converting_info();
// init video reader
  seze::Reader reader(seze::iname, pligin_info.in_x, pligin_info.in_y,
    pligin_info.color_type);
  auto reader_ctx = reader.get_ctx();
// init thred buffers
  std::vector<seze::Image*> vp_framebuffer(seze::num_threads);
  auto framebuffer_info = reader.get_framebuffer_info();
  seze::framebuffer_x = framebuffer_info.x;
  seze::framebuffer_y = framebuffer_info.y;
  // use input video res if out res not defined
  if (seze::width == 0)
    seze::width = reader_ctx.in_vid_x;
  if (seze::height == 0)
    seze::height = reader_ctx.in_vid_y;
// make buffers for threads
  FOR (i, seze::num_threads)
    vp_framebuffer[i] = new seze::Image(seze::framebuffer_x,
      seze::framebuffer_y, framebuffer_info.type);
// init video writer
  seze::Writer* writer;
  if (seze::nout)
    writer = nullptr;
  else
    writer = new seze::Writer(seze::oname, framebuffer_info, seze::width, seze::height,
      reader_ctx, seze::simple_encoder);
// init SDL2
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
// video reading/processing/writing loop
  bool is_end = false;
  std::vector<std::thread> v_thread(seze::num_threads);
  while ( !is_end) {
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
      break;
    // read 2 buffers
    FOR (i, seze::num_threads) {
      auto& p = vp_framebuffer[i];
      if (is_end) {
        zero_delete(p);
        continue;
      }
      if ( !reader.read_to(p)) {
        zero_delete(p);
        is_end = true;
      }
    } // read 2 frame
    // multithread processing
    FOR (i, seze::num_threads) {
      auto& p = vp_framebuffer[i];
      if (p) {
        v_thread[i] = std::thread([&]() {
          plugin->core(p->get_data(), p->X, p->Y,
            p->stride, p->type);
        });
      }
    } // enable plugin
    // stop threads
    FOR (i, seze::num_threads)
      if (v_thread[i].joinable())
        v_thread[i].join();
    // render frame:
    if ( !seze::nrend) {
      FOR (i, seze::num_threads) {
        auto& p = vp_framebuffer[i];
        if (p) {
          SDL_LockSurface(surface);
          image_to_surface(*p, surface);
          SDL_UnlockSurface(surface);
          SDL_UpdateWindowSurface(window);
        }
      }
    } // if !nrend
    // write frame 2 file
    if ( !seze::nout) {
      FOR (i, seze::num_threads) {
        auto& p = vp_framebuffer[i];
        if (p)
          *writer << p;
      }
    } // write
  } // loop
// free SEZE
  plugin->finalize();
  for (auto& p: vp_framebuffer)
    delete p;
  delete writer;
// free SDL
  if ( !seze::nrend) {
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
  return EXIT_SUCCESS;
} // main
