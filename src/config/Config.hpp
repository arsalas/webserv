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
  void	      openBrackets(std::stack<bool> &brackets, std::string &tmp);
  void	      closeBrackets(std::stack<bool> &brackets, std::string &tmp, int line_idx);
  void        endOfLine(std::string &tmp);
  void        pushToken(std::string &tmp);

  void	      serverToken(int i, std::vector<std::string>::iterator &it);
  void	      workersToken(std::vector<std::string>::iterator &it);


 private:
  std::string _path;
  int fd_;
  std::ifstream fdStream_;

  int workers_;

  std::string file_content_;
  std::vector<std::string> _tokens;
  std::vector<ServerConfig> _servers;
};

#endif
