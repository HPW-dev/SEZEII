#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "pparser.hpp"

namespace seze {

pparser::pparser(pparser::v_param_t&& in) noexcept
: v_param{in} {}

void pparser::operator ()(int argc, char** argv) const {
  if (argc < 2) {
    print_info();
    std::exit(EXIT_FAILURE);
  }
  auto tokens {get_tokens(argc, argv)};
  for (auto param: v_param) {
    auto opt {get_options(tokens, param.keys)};
    if (opt.empty()) {
      if (param.needed) {
        Str msg {"need param for "};
        for (auto key: param.keys)
          msg += key + ' ';
        throw std::invalid_argument(msg);
      }
    } else
      param.action(opt);
  }
}

Str pparser::get_option(vector_t<Str> tokens, CN(Str) option) const {
  auto itr {std::find(tokens.begin(), tokens.end(), option)};
  if (itr != tokens.end() && ++itr != tokens.end())
    return *itr;
  return {};
}

Str pparser::get_options(vector_t<Str> tokens,
CN(vector_t<Str>) options) const {
  for (auto va: options) {
    auto ret {get_option(tokens, va)};
    if ( !ret.empty())
      return ret;
  }
  return {};
}

vector_t<Str> pparser::get_tokens(int argc, char** argv) const {
  vector_t<Str> tokens;
  for (int i {1}; i < argc; ++i)
    tokens.push_back(argv[i]);
  return tokens;
}

void pparser::print_info() const {
  std::cout << "usage:\n";
  for (auto param: v_param) {
    for (auto key: param.keys)
      std::cout << key << ' ';
    std::cout << std::setw(12) << "\t: " << param.desc << "\n";
    std::cout.flush();
  }
}

} // seze ns
