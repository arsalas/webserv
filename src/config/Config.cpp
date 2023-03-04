#include "Config.hpp"
#include <algorithm> 
#include <cctype>
#include <locale>

Config::Config(std::string &path) : path_(path) {
  workers_ = 0;
  fd_ = open(path_.c_str(), O_RDONLY);
  if (fd_ < 0)
    throw webserv_exception("could not open configuration file : %", 0, path_);
}

Config::~Config() {
  clear();
}

void Config::clear() {
  if (fd_ > 0) {
    close(fd_);
    fd_ = 0;
  }
  tokens_.clear();
  file_content_.clear();
}


std::string Config::ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string Config::rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(" \t");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string Config::trim(const std::string &s)
{
  return (rtrim(ltrim(s)));
}


/**
 * @brief 
 * myfile es el path que me envian y tengo en mi estructura Config
 * Abro el file y recojo linea a linea con un getline
 * Primero hacemos un trim de lo que hay delante
 * 
 */
void Config::tokenize()
{
  std::string line, tmp;
  std::stack<bool> brackets;

  int line_idx = 1;

  std::ifstream myfile(path_.c_str());

  while (std::getline(myfile, line))
  {
    file_content_ += line + "\n";
    // si no nos encontramos con una línea con solo espacios o tabs
    // while ('}' != std::string::npos)
    // {
      std::cout << line << std::endl;
      // hacemos un trim
      tmp = rtrim(line);
      // si nos encontramos con un comentario, no cuenta
      if (tmp[0] != '#')
      {
        // tenemos una variable (brackets) para saber si abrimos o cerramos {}
        if (tmp == "{")
          brackets.push(true);
        else if (tmp == "}")
        {
          // si hemos abierto y cerrado sin poner nada dentro, es un error
          if (brackets.empty())
            throw webserv_exception("extra closing '}' on line %", 0, ft::to_string(line_idx));
          brackets.pop();
        }
        if (isValidDirective(tmp) && line[line.length()] != ';')
          throw webserv_exception("missing ';' on line %", 0, ft::to_string(line_idx));
        if (tmp.find(';', tmp.length() - 1) != std::string::npos)
        {
          tmp.erase(tmp.length() - 1, 1);
          tokens_.push_back(tmp);
          tokens_.push_back(";");
        }
        else
          tokens_.push_back(tmp);
      }
    line_idx++;
  }
}
// void Config::tokenize() {
//   std::string line, tmp;
//   std::string::size_type first, last;
//   std::stack<bool> brackets;

//   int line_idx = 1;

//   std::ifstream myfile( path_.c_str());

//   while (std::getline(myfile, line)) {
//     file_content_ += line + "\n";
//     last = 0;
//     while ((first = line.find_first_not_of(" \t", last)) != std::string::npos) {
//       if (line[first] == '#')
//         break;
//       last = line.find_first_of(" \t", first);
//       tmp = line.substr(first, last - first);
//       if (tmp == "{")
//         brackets.push(true);
//       else if (tmp == "}") {
//         if (brackets.empty())
//           throw webserv_exception("extra closing '}' on line %", 0, ft::to_string(line_idx));
//         brackets.pop();
//       }
//       if (isValidDirective(tmp) && line[line.find_last_not_of(" \t", line.length())] != ';')
//         throw webserv_exception("missing ';' on line %", 0, ft::to_string(line_idx));
//       if (tmp.find(';', tmp.length() - 1) != std::string::npos) {
//         tmp.erase(tmp.length() - 1, 1);
//         tokens_.push_back(tmp);
//         tokens_.push_back(";");
//       }
//       else
//         tokens_.push_back(tmp);
//     }
//     line_idx++;
//   }
// }

void Config::parse() {
  tokenize();
  int i = 1;
  std::vector<std::string>::iterator it;

  for (it = tokens_.begin(); it != tokens_.end(); ++it) {
    if (*it == "server") {
      ServerConfig serv;

      serv.id_ = i++;
      serv.server(++it);
      servers_.push_back(serv);
    }
    else if (*it == "workers") {
      workers_ = ft::stoi(*(++it));
      if (workers_ > 16 || workers_ < 0)
        throw std::runtime_error("workers must be between [0-16]range");
      if (*++it != ";")
        throw std::runtime_error("missing ';' in 'workers'");
    }
    else
      throw webserv_exception("invalid directive % in main block", 0, *it);
  }
  if (servers_.empty())
    throw webserv_exception("missing server block");
}

std::string &Config::getPath() {
  return path_;
}

std::vector<ServerConfig> &Config::getServers() {
  return servers_;
}

int Config::getWorkers() {
  return workers_;
}

std::string &Config::getFileContent() {
  return file_content_;
}
