#include "../plugin-info.hpp"
#include "../../utils/cmd-parser.hpp"
#include "../../utils/error.hpp"
#include "../../utils/random.hpp"
#include "../../image/rgb24.hpp"
#include <functional>
#include <map>
#include <sstream>

using namespace seze;

extern "C" {
  PluginInfo init(CN(std::string) options);
  void core(byte* dst, int x, int y, int stride, color_t color_type);
  void finalize();
}

enum class op_t {and_, or_, xor_, add, sub, mul, div, min, max};
using ft_operation = std::function<byte(byte, byte)>;

byte r_value = 0x03;
byte g_value = 0x00;
byte b_value = 0x02;
op_t operation_type = op_t::mul;
ft_operation user_operation = {};
bool user_rnd = false; ///< random operation value every frame

static ft_operation make_op(op_t type) {
  switch (type) {
    case op_t::and_: return [](byte a, byte b)->byte { return a & b; };
    case op_t::or_: return [](byte a, byte b)->byte { return a | b; };
    case op_t::add: return [](byte a, byte b)->byte { return a + b; };
    case op_t::sub: return [](byte a, byte b)->byte { return a - b; };
    case op_t::mul: return [](byte a, byte b)->byte { return a * b; };
    case op_t::div: return [](byte a, byte b)->byte { return a / ( !b ? 1 : b); };
    case op_t::min: return [](byte a, byte b)->byte { return std::min(a, b); };
    case op_t::max: return [](byte a, byte b)->byte { return std::max(a, b); };
    case op_t::xor_:
    default: return [](byte a, byte b)->byte { return a ^ b; };
  }
}

static op_t decode_op_t(CN(std::string) on_name) {
  std::map<std::string, op_t> table = {
    {"or", op_t::or_},
    {"xor", op_t::xor_},
    {"add", op_t::add},
    {"and", op_t::and_},
    {"sub", op_t::sub},
    {"div", op_t::div},
    {"mul", op_t::mul},
    {"min", op_t::min},
    {"max", op_t::max},
  };
  return table.at(on_name);
}

//! decode hex string (RRGGBB) to r/g/b_value
static void parse_value(CN(std::string) value_str) {
  std::stringstream ss;
  ss << std::hex << value_str;
  uint x;   
  ss >> x;
  b_value = x & 0xFF;
  x >>= 8;
  g_value = x & 0xFF;
  x >>= 8;
  r_value = x & 0xFF;
}

PluginInfo init(CN(std::string) options) {
  PluginInfo ret;
  ret.color_type = color_t::RGB24;
  ret.title = "color operations";
  ret.info = "Usage:\n"
    "--op\toperarion name\n"
    "--value\toperarion value (6 hex numbers RRGGBB)\n"
    "--rnd\tenable random value every frame\n"
    "operarion names: xor, and, add, or, sub, mul, div, min, max.\n"
    "example: --op xor --value 00FF7C\n";
// parse opts:
  CmdParser parser(options);
// --rnd
  if (parser.opt_exists("--rnd")) {
    seze::randomize_seed();
    user_rnd = true;
  }
  ret.multithread = user_rnd ? false : true;
// --op
  if (auto op_name = parser.get_option("--op"); !op_name.empty())
    operation_type = decode_op_t(op_name);
  user_operation = make_op(operation_type);
// --value
  if (auto value_str = parser.get_option("--value");
  !value_str.empty())
    parse_value(value_str);
  return ret;
}

void core(byte* dst, int mx, int my, int stride,
color_t color_type) {
  iferror( !dst, "core: dst is null");
  auto size = mx * my;
  RGB24* data = rcast(RGB24*, dst);
  if (user_rnd) {
    r_value = irand(0, 256);
    g_value = irand(0, 256);
    b_value = irand(0, 256);
  }
  std::transform(data, data + size, data,
    [](CN(RGB24) pix)->RGB24 {
      return RGB24(
        user_operation(pix.R, r_value),
        user_operation(pix.G, g_value),
        user_operation(pix.B, b_value)
      ); 
    });
} // core

void finalize() {}
