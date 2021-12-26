#include "video/reader.hpp"
#include "video/writer.hpp"
#include "image/rgb24.hpp"
#include "image/RGB555-RGB565.hpp"
#include "main-config.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <thread>

//! copy seze-image data 2 SDL surface
void image_to_surface(CP(seze::Image) src, SDL_Surface* dst) {
  if (src->type() == seze_RGB24) {
    auto pDst = rcast(byte*, dst->pixels);
    auto pSrc = src->get_cdata();
    auto pEnd = &pSrc[src->bytes()];
    while (pSrc != pEnd) {
      pDst[0] = pSrc[2];
      pDst[1] = pSrc[1];
      pDst[2] = pSrc[0];
      pDst += 4; // BGRA
      pSrc += 3; // RGB
    }
  } else if (src->type() == seze_gray) {
    auto pDst = rcast(byte*, dst->pixels);
    auto pSrc = src->get_cdata();
    auto pEnd = &pSrc[src->bytes()];
    while (pSrc != pEnd) {
      pDst[0] = *pSrc;
      pDst[1] = *pSrc;
      pDst[2] = *pSrc;
      pDst += 4; // BGRA
      pSrc += 1; // luma
    }
  } else if (src->type() == seze_RGB565) {
    auto pDst = rcast(byte*, dst->pixels);
    auto pSrc = src->get_cdata();
    auto pEnd = &pSrc[src->bytes()];
    while (pSrc != pEnd) {
      seze::RGB565 col(*rcast(CP(uint16_t), pSrc));
      byte r, g, b;
      col.set_to(r, g, b);
      pDst[0] = r;
      pDst[1] = g;
      pDst[2] = b;
      pDst += 4; // BGRA
      pSrc += 2; // WORD
    }
  } else if (src->type() == seze_RGB555) {
    auto pDst = rcast(byte*, dst->pixels);
    auto pSrc = src->get_cdata();
    auto pEnd = &pSrc[src->bytes()];
    while (pSrc != pEnd) {
      seze::RGB555 col(*rcast(CP(uint16_t), pSrc));
      byte r, g, b;
      col.set_to(r, g, b);
      pDst[0] = r;
      pDst[1] = g;
      pDst[2] = b;
      pDst += 4; // BGRA
      pSrc += 2; // WORD
    }
  } else
    error("image_to_surface: need color convesion impl");
} // image_to_surface

#ifdef WINDOWS
int SDL_main(int argc, char* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
// parse main args
  if ( !parse_args(argc, argv))
    return EXIT_FAILURE;
// load plugin
  auto plugin = new_plugin_by_ext(pname);
  auto pligin_info = plugin->init(popts);
  print_plugin_info(pligin_info);
  enable_plugin_settings(pligin_info);
  print_converting_info();
// init video reader
  seze::Reader reader(iname, pligin_info.in_x, pligin_info.in_y,
    pligin_info.color_type);
  auto reader_ctx = reader.get_ctx();
// init thred buffers
  std::vector<seze::Image*> vp_framebuffer(num_threads);
  auto framebuffer_info = reader.get_framebuffer_info();
  framebuffer_x = framebuffer_info.x;
  framebuffer_y = framebuffer_info.y;
  // use input video res if out res not defined
  if (width == 0)
    width = reader_ctx.in_vid_x;
  if (height == 0)
    height = reader_ctx.in_vid_y;
// make buffers for threads
  FOR (i, num_threads)
    vp_framebuffer[i] = new seze::Image(framebuffer_x,
      framebuffer_y, framebuffer_info.type);
// init video writer
  seze::Writer* writer;
  if (nout)
    writer = nullptr;
  else
    writer = new seze::Writer(oname, framebuffer_info, width, height,
      reader_ctx, simple_encoder);
// init SDL2
  SDL_Window* window = nullptr;
  SDL_Surface* surface = nullptr;
  SDL_Event event;
  if ( !nrend) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("SEZE II",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
      framebuffer_x, framebuffer_y, 0);
    surface = SDL_GetWindowSurface(window);
    SDL_memset(surface->pixels, 0, surface->h * surface->pitch);
  }
// video reading/processing/writing loop
  bool is_end = false;
  std::vector<std::thread> v_thread(num_threads);
  while ( !is_end) {
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
      break;
    // read 2 buffers
    FOR (i, num_threads) {
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
    FOR (i, num_threads) {
      auto& p = vp_framebuffer[i];
      if (p) {
        v_thread[i] = std::thread([&]() {
          plugin->core(p->get_data(), p->get_x(), p->get_y(),
            p->get_stride(), p->type());
        });
      }
    } // enable plugin
    // stop threads
    FOR (i, num_threads)
      if (v_thread[i].joinable())
        v_thread[i].join();
    // render frame:
    if ( !nrend) {
      FOR (i, num_threads) {
        auto& p = vp_framebuffer[i];
        if (p) {
          SDL_LockSurface(surface);
          image_to_surface(p, surface);
          SDL_UnlockSurface(surface);
          SDL_UpdateWindowSurface(window);
        }
      }
    } // if !nrend
    // write frame 2 file
    if ( !nout) {
      FOR (i, num_threads) {
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
  if ( !nrend) {
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
  return EXIT_SUCCESS;
} // main
