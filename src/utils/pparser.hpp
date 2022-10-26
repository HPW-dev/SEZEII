#pragma once
#include <functional>
#include "types.hpp"
#include "macro.hpp"

namespace seze {

//! parsing cmd line args
class pparser {
  nocopy(pparser);

public:
  struct Param {
    v_str_t keys {};
    Str desc {};
    std::function<void (CN(Str))> action {};
    bool needed {false};
  };
  using v_param_t = vector_t<Param>;

  //! @param in vector of: {{"-i", "--input", ...}, "description", lambda}
  explicit pparser(v_param_t&& in) noexcept;
  void operator ()(int argc, char** argv) const;
  ~pparser() = default;
  void print_info() const;

private:
  v_param_t v_param {};

  Str get_option(vector_t<Str> tokens, CN(Str) option) const;
  Str get_options(vector_t<Str> tokens, CN(vector_t<Str>) options) const;
  vector_t<Str> get_tokens(int argc, char** argv) const;
};

} // seze ns
