#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "pparser.hpp"

namespace seze {

pparser::pparser(pparser::v_param_t&& in) noexcept
: v_param{in} {}

void pparser::operator ()(int argc, char** argv) const {
  if (argc < 2 && !skip_empty) {
    print_info();
    std::exit(EXIT_FAILURE);
  }
  auto tokens {get_tokens(argc, argv)};
  for (auto param: v_param) {
    auto opt {get_options(tokens, param.keys)};
    if (opt)
      param.action(*opt);
    if (!opt && param.needed) {
      Str msg {"need param for "};
      for (auto key: param.keys)
        msg += key + ' ';
      throw std::invalid_argument(msg);
    }
  } // for v_param
} // op (argv)

void pparser::operator ()(CN(Str) opts) const {
  if (opts.empty() && !skip_empty) {
    print_info();
    std::exit(EXIT_FAILURE);
  }
  auto tokens {get_tokens(opts)};
  for (auto param: v_param) {
    auto opt {get_options(tokens, param.keys)};
    if (opt)
      param.action(*opt);
    if (!opt && param.needed) {
      print_info();
      
      Str msg {"need param for "};
      for (auto key: param.keys)
        msg += key + ' ';
      throw std::invalid_argument(msg);
    }
  } // for v_param
} // op (str)

std::optional<Str> pparser::get_option(vector_t<Str> tokens, CN(Str) option) const {
  auto itr {std::find(tokens.begin(), tokens.end(), option)};
  if (itr != tokens.end()) { // найден ключ
    if (++itr != tokens.end()) // найдена команда после ключа
      return *itr;
    return "";
  }
  return std::nullopt;
}

std::optional<Str> pparser::get_options(vector_t<Str> tokens,
CN(vector_t<Str>) options) const {
  for (auto va: options) {
    auto ret {get_option(tokens, va)};
    if (ret)
      return ret;
  }
  return std::nullopt;
}

vector_t<Str> pparser::get_tokens(int argc, char** argv) const {
  vector_t<Str> tokens;
  for (int i {1}; i < argc; ++i)
    tokens.push_back(argv[i]);
  return tokens;
}

vector_t<Str> pparser::get_tokens(CN(Str) opts) const {
  std::stringstream ss(opts);
  std::string str;
  vector_t<Str> tokens;
  while (getline(ss, str, ' '))
    tokens.push_back(str);
  return tokens;
}

void pparser::print_info() const {
  std::cout << "Usage:\n";
  for (auto param: v_param) {
    for (auto key: param.keys)
      std::cout << key << ' ';
    std::cout << std::setw(12) << "\t: " << param.desc << "\n";
    std::cout.flush();
  }
}

} // seze ns
