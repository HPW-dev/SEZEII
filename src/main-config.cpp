#include "main-config.hpp"
extern "C" {
#include <getopt.h>
}

Str iname = {};
Str oname = {};
Str pname = {};
Str popts = {};
uint num_threads = 4;
int width = 0, height = 0;
int framebuffer_x = 0, framebuffer_y = 0;
seze::plugin_format_t plugin_format = seze::plugin_format_t::unknown;
bool nout = false;
bool nrend = false;
bool simple_encoder = false;

seze::Plugin* new_plugin_by_ext(CN(Str) fname) {
  namespace fs = std::filesystem;
  Str ext = fs::path(fname).extension().string();
  std::map<Str, seze::plugin_format_t> table = {
    {".dll", seze::plugin_format_t::seze},
    {".so", seze::plugin_format_t::seze},
    {".rpi", seze::plugin_format_t::RPI},
  };
  try {
    plugin_format = table.at(ext);
  } catch (...) {
    plugin_format = seze::plugin_format_t::unknown;
  }
  switch (plugin_format) {
    case seze::plugin_format_t::seze: return new seze::PluginShared(fname);
    case seze::plugin_format_t::RPI: error("new_plugin_by_ext: RPI plugins not supported");
    case seze::plugin_format_t::unknown:
    default: error("new_plugin_by_ext: unknown plugin format");
  }
  return nullptr;
} // new_plugin_by_ext

void print_help() {
  LOG(
    "\nSEZE II. Plugin based video converter.\n"
    "Github page: github.com/HPW-dev\n"
    "Copyright (c) HPW-dev (hpwdev0@gmail.com), 2021-2022\n"
    "\nCommands:\n"
    "-i \"path to src video\"\t\tinput file path\n"
    "-o \"path to dst video\"\t\toutput file path\n"
    "-x <num> \t\t\toutput video width\n"
    "-y <num> \t\t\toutput video wheight\n"
    "-j <num threads>\t\tnumber of threads (default -j 4)\n"
    "--plugin \"path\\to\\plugin\"\tpath to plugin file\n"
    "--opts '-a -b ... -c'\t\toptions for plugin\n"
    "--nout\t\t\t\tdisable video output\n"
    "--norend\t\t\tdisable video rendering\n"
    "--noraw\t\t\t\tno lossless video encoding\n"
    "-h, --help\t\t\tprint help\n"
    "\nUsage example:\nseze -i \"in.avi\" --plugin "
    "\"plugins\\free\\invert.dll\" -o \"out.mp4\"\n");
} // print_help

void opts_check() {
  if (num_threads == 0) {
    LOG("threads = 0! Selected 4 by default" << std::endl);
    num_threads = 4;
  }
  iferror(iname.empty(), "input file name is empty");
  iferror( !nout && oname.empty(), "output file name is empty");
  iferror(pname.empty(), "plugin file path is empty");
  iferror(height < 0, "video height < 0");
  iferror(width < 0, "video width < 0");
}

bool parse_args(int argc, char* argv[]) {
  if (argc < 2) {
    LOG("Pls, use commands from list.");
    print_help();
    return false;
  }
  while (1) {
    enum class long_opt: int {
      plugin = 100000,
      plugin_opts, no_output, no_render, no_raw
    };
    static option long_options[] = {
      {"plugin", required_argument, 0, int(long_opt::plugin)},
      {"opts", required_argument, 0, int(long_opt::plugin_opts)},
      {"nout", no_argument, 0, int(long_opt::no_output)},
      {"norend", no_argument, 0, int(long_opt::no_render)},
      {"noraw", no_argument, 0, int(long_opt::no_raw)},
      {0, 0, 0, 0},
    };
    int option_index = 0;
    int rez = getopt_long(argc, argv, "hj:i:o:x:y:",
      long_options, &option_index);
    if (rez == -1)
      break; // end parsing
		switch (rez) {
      case 'j': num_threads = std::stoi(optarg); break;
      case 'x': width = std::stoi(optarg); break;
      case 'y': height = std::stoi(optarg); break;
      case int(long_opt::plugin): pname = optarg; break;
      case int(long_opt::plugin_opts): popts = optarg; break;
      case int(long_opt::no_output): nout = true; break;
      case int(long_opt::no_render): nrend = true; break;
      case int(long_opt::no_raw): simple_encoder = true; break;
      case 'i': iname = optarg; break;
      case 'o': oname = optarg; break;
      case 'h':
      case '?':
      default: print_help(); break;
    };
	}; // while getopt
  opts_check();
  return true;
} // parse_args

void print_converting_info() {
  LOG("\nConverting info:\n"
    "threads = " << num_threads << "\n"
    "lossless = " << (simple_encoder ? "no" : "yes") << "\n"
    "input = \"" << iname << "\"\n"
    "output = \"" << oname << "\"\n"
    "plugin = \"" << pname << "\"\n"
    "output resolution = " <<
    width << "x" << height << " (0 is auto)\n\n");
}

void print_plugin_info(CN(PluginInfo) x) {
  LOG("\nPlugin info:"
    << "\nopts = \"" << popts << "\""
    << "\ntitle = " << x.title
    << "\nmultithread mode = "
    << ((x.flags & PLGNINF_MULTITHREAD) ? "yes" : "no")
    << "\nversion = " << x.version
    << "\ninfo = " << (x.info ? x.info : "none"));
}

void enable_plugin_settings(CN(PluginInfo) src) {
  if ( !(src.flags & PLGNINF_MULTITHREAD))
    num_threads = 1;
  if (src.in_x != 0)
    framebuffer_x = src.in_x;
  if (src.in_y != 0)
    framebuffer_y = src.in_y;
}
