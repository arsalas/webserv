#ifndef STATUSCODE_HPP
# define STATUSCODE_HPP

# include <map>
# include <string>

class StatusCode {
 public:
  StatusCode();
  std::string &operator[](int status_code);

 private:
  std::map<int, std::string> _error_codes;
};

#endif