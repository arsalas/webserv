#ifndef INPUTARGS_HPP
#define INPUTARGS_HPP

#include <string>
#include <vector>
#include <map>

#include "Logger.hpp"
#include "Utils.hpp"

class InputArgs
{
private:
  vector<std::string> _argv;
  LogLevel _logLevel;
  std::string _path;
  std::map<std::string, bool> _options;

public:
  InputArgs();
  InputArgs(int argc, char **argv);
  ~InputArgs();

  std::string &getPath();

  void parse();
  LogLevel log();
  std::string helpText();
  bool help();
  bool test();
  bool location();
};

#endif