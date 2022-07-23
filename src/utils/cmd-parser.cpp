#include "cmd-parser.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>

CmdParser::CmdParser(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) // ignore first arg
    tokens.push_back(std::string(argv[i]));
}

CmdParser::CmdParser(CN(std::string) cmds) {
  // TODO add tokenization of "..." params
  std::stringstream ss(cmds);
  std::string str;
  while (getline(ss, str, ' '))
    tokens.push_back(str);
}

std::string CmdParser::get_option(CN(std::string) option) const {
  auto itr = std::find(tokens.begin(), tokens.end(), option);
  if (itr != tokens.end() && ++itr != tokens.end())
    return *itr;
  return {};
}

std::string CmdParser::get_options(
CN(std::vector<std::string>) options) const {
  for (CN(auto) va: options) {
    auto ret = get_option(va);
    if ( !ret.empty())
      return ret;
  }
  return {};
}

bool CmdParser::opt_exists(CN(std::string) option) const {
  return std::find(tokens.begin(), tokens.end(),
    option) != tokens.end();
}
