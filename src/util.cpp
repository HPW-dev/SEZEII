#include "util.hpp"
#include <iostream>
#include <map>
#include <thread>
#include <cassert>
#include <filesystem>
#include <SDL2/SDL.h>
#include <stdexcept>
#include "image/image.hpp"
#include "image/rgb24.hpp"
#include "image/RGB555-RGB565.hpp"
#include "image/color.hpp"
#include "utils/pparser.hpp"
#include "utils/log.hpp"
#include "global.hpp"
#include "version.hpp"
#include "video/reader.hpp"

void convert_rgb24(CN(seze::Image) src, SDL_Surface *dst) {
  auto pDst {rcast(byte*, dst->pixels)};
  auto pSrc {src.get_cdata()};
  auto pEnd {&pSrc[src.bytes]};
  while (pSrc != pEnd) {
    pDst[2] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[0] = pSrc[2];
    pDst += 4; // BGRA
    pSrc += 3; // RGB
  }
}

void convert_gray(CN(seze::Image) src, SDL_Surface *dst) {
  auto pDst {rcast(byte*, dst->pixels)};
  auto pSrc {src.get_cdata()};
  auto pEnd {&pSrc[src.bytes]};
  while (pSrc != pEnd) {
    pDst[0] = *pSrc;
    pDst[1] = *pSrc;
    pDst[2] = *pSrc;
    pDst += 4; // BGRA
    pSrc += 1; // luma
  }
}

void convert_rgb565(CN(seze::Image) src, SDL_Surface *dst) {
  auto pDst {rcast(byte*, dst->pixels)};
  auto pSrc {src.get_cdata()};
  auto pEnd {&pSrc[src.bytes]};
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
}

void convert_rgb555(CN(seze::Image) src, SDL_Surface *dst) {
  auto pDst = rcast(byte*, dst->pixels);
  auto pSrc = src.get_data();
  auto pEnd = &pSrc[src.bytes];
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
}

//! copy seze-image data 2 SDL surface
void image_to_surface(CN(seze::Image) src, SDL_Surface *dst) {
  switch (src.type) {
    case seze_RGB24: convert_rgb24(src, dst); break;
    case seze_gray: convert_gray(src, dst); break;
    case seze_RGB565: convert_rgb565(src, dst); break;
    case seze_RGB555: convert_rgb555(src, dst); break;
    default:
      throw std::invalid_argument("image_to_surface: need color convesion impl");
  }
} // image_to_surface

void opts_check() {
  if (seze::num_threads == 0) {
    seze::num_threads = std::thread::hardware_concurrency();
    if (seze::num_threads)
      seze::num_threads = 4;
  }
  if (seze::oname.empty())
    assert(seze::nout);
  assert(seze::height > -1);
  assert(seze::width > -1);
} // opts_check

void parse_args(int argc, char** argv) {
  std::cout << "SEZE II. Plugin based video converter." << std::endl
    << "Version: " << sezeii::ver << std::endl
    << "Compiler: " << sezeii::gcc_ver << std::endl
    << "Copyright (c): " << sezeii::copyright << std::endl
    << "Github page: " << sezeii::github_page << std::endl;
  seze::pparser parser({
    {
      {"-i", "--input"},
      "input file path",
      [](CN(Str) arg) { seze::iname = arg; },
      true
    },
    {
      {"-o", "--output"},
      "input file path",
      [](CN(Str) arg) { seze::oname = arg; }
    },
    {
      {"-x", "--width"},
      "output video width",
      [](CN(Str) arg) { seze::width = std::stoi(arg); }
    },
    {
      {"-y", "--height"},
      "output video height",
      [](CN(Str) arg) { seze::height = std::stoi(arg); }
    },
    {
      {"-j", "--jobs"},
      "number of threads",
      [](CN(Str) arg) { seze::num_threads = std::stoi(arg); }
    },
    {
      {"-p", "--plug"},
      "path to plugin file",
      [](CN(Str) arg) { seze::pname = arg; },
      true
    },
    {
      {"-op", "--opts"},
      "options for plugin (--opts '-a -b')",
      [](CN(Str) arg) { seze::popts = arg; }
    },
    {
      {"-n", "--nout"},
      "disable video output",
      [](CN(Str) arg) { seze::nout = true; }
    },
    {
      {"--norend"},
      "disable video rendering",
      [](CN(Str) arg) { seze::nrend = true; }
    },
    {
      {"--noraw"},
      "no lossless video encoding",
      [](CN(Str) arg) { seze::simple_encoder = true; }
    },
    {
      {"-h", "--help"},
      "print this help",
      [&parser](CN(Str) arg) { 
        parser.print_info();
        std::cout << "Usage example: seze -i \"in.avi\" --p "
          << "\"plugins\\free\\invert.dll\" -o \"out.mp4\"" << std::endl;
        std::exit(EXIT_SUCCESS);
      }
    }
  }); // parser init

  parser(argc, argv);
  opts_check();
} // parse args

void correct_plugin_settings(PluginInfo &src) {
  if ( !(src.flags & PLGNINF_MULTITHREAD))
    seze::num_threads = 1;
  if (src.in_x != 0)
    seze::framebuffer_x = src.in_x;
  if (src.in_y != 0)
    seze::framebuffer_y = src.in_y;
}

void print_plugin_info(CN(PluginInfo) x) {
  LOG("\nPlugin info:"
    << "\nopts: \"" << seze::popts << "\""
    << "\ntitle: \"" << x.title << "\""
    << "\nmultithread mode: "
    << ((x.flags & PLGNINF_MULTITHREAD) ? "yes" : "no")
    << "\nversion: " << x.version
    << "\ninfo: " << (x.info ? x.info : "none"));
}

void print_converting_info() {
  LOG("\nConverting info:\n"
    "threads: " << seze::num_threads << "\n"
    "lossless: " << (seze::simple_encoder ? "no" : "yes") << "\n"
    "input: \"" << seze::iname << "\"\n"
    "output: \"" << seze::oname << "\"\n"
    "plugin: \"" << seze::pname << "\"\n"
    "output resolution: " <<
    seze::width << "x" << seze::height << " (0 is auto)\n\n");
}

shared_p<seze::Plugin> new_plugin(CN(Str) fname) {
  namespace fs = std::filesystem;
  Str ext = fs::path(fname).extension().string();
  std::map<Str, seze::plugin_format_t> table {
    {".dll", seze::plugin_format_t::seze},
    {".so", seze::plugin_format_t::seze},
    {".rpi", seze::plugin_format_t::RPI},
  };
  try {
    seze::plugin_format = table.at(ext);
  } catch (...) {
    seze::plugin_format = seze::plugin_format_t::unknown;
  }
  switch (seze::plugin_format) {
    case seze::plugin_format_t::seze: return make_shared_p<seze::PluginShared>(fname);
    case seze::plugin_format_t::RPI: throw std::invalid_argument("new_plugin_by_ext: RPI plugins not supported");
    case seze::plugin_format_t::unknown:
    default: throw std::invalid_argument("new_plugin_by_ext: unknown plugin format");
  }
  return nullptr;
} // new_plugin_by_ext
