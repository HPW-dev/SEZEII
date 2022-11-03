#pragma once
#include "macro.hpp"
#include <string>
#include <vector>

/** commad parser (old ver)
 * @author iain 2009, HPW-dev 2021 */
class CmdParser {
  std::vector<std::string> tokens = {};
public:
  CmdParser(int argc, char *argv[]);
  CmdParser(CN(std::string) cmds);
  //! example: params = get_option("--input")
  std::string get_option(CN(std::string) option) const;
  std::string get_options(CN(std::vector<std::string>) options) const;
  //! example: if (opt_exists("--help")) {...}
  bool opt_exists(CN(std::string) option) const;
}; // CmdParser
