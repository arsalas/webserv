#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string>
# include <vector>
# include <stack>
#include <fstream>
# include <fcntl.h>

# include "Server.hpp"
# include "ServerConfig.hpp"
# include "StringUtils.hpp"
# include "Utils.hpp"

class Config {
 public:
  Config(std::string &path);
  ~Config();

  void tokenize();
  void parse();
  void clear();

  std::vector<ServerConfig> &getServers();
  std::string &getPath();

  int getWorkers();

  std::string &getFileContent();

  std::string trim(const std::string &s);
  std::string rtrim(const std::string &s);
  std::string ltrim(const std::string &s);
  void        endOfLine(std::string &tmp);


 private:
  std::string path_;
  int fd_;
  std::ifstream fdStream_;

  int workers_;

  std::string file_content_;
  std::vector<std::string> _tokens;
  std::vector<ServerConfig> _servers;
};

#endif
