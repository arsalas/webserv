#include "Request.hpp"

Request::Request() {
  body_offset_ = 0;
  chunk_size_ = 0;
  status_ = FIRST_LINE;
  _protocol = "HTTP/1.1";
  gettimeofday(&start_timer_, NULL);
}

Request::~Request() {}

int Request::parse(std::string &buffer) {
  size_t ret = 0;

  gettimeofday(&last_timer_, NULL);

  buffer_ += buffer;
  buffer.clear();

  if (status_ == FIRST_LINE)
    ret = _methodline();
  if (status_ == HEADERS)
    ret = headers();
  if (status_ == PREBODY)
    ret = prebody();
  if (status_ == BODY)
    ret = body();
  if (status_ == CHUNK)
    ret = chunk();
  if (status_ == COMPLETE || ret == 1) {
    status_ = COMPLETE;
    return ret;
  }
  else if (status_ == ERROR || ret > 1) {
    status_ = ERROR;
    return ret;
  }
  return ret;
}

bool check_validity(std::string &uri) {
  int count = 0;

  std::string tmp = uri;

  while (tmp.find('/') != std::string::npos) {
    tmp = tmp.substr(tmp.find('/') + 1);
    if (tmp.empty())
      break;
    std::string tmp2 = tmp.substr(0, tmp.find('/'));

    if (tmp2.find("..", 0) != std::string::npos)
      count--;
    else
      count++;
  }
  return count >= 0;
}

int Request::_methodline() {
  if (buffer_.find("\r\n") != std::string::npos) {
    std::string tmp = buffer_.substr(0, buffer_.find(' '));

    if (isValidMethod(tmp)) {
      _method = tmp;
      buffer_.erase(0, _method.length() + 1);
    } else
      return 501;

    if (buffer_.find(' ') == 0)
      return 400;

    tmp = buffer_.substr(0, buffer_.find(' '));

    if (tmp[0] != '/')
      return 400;

    if (!check_validity(tmp))
      return 403;

    if (tmp.length() < 100000) {
      _target = tmp;
      buffer_.erase(0, _target.length() + 1);
    } else
      return 414;

    if (_target.find('?') != std::string::npos) {
      query_string_ = _target.substr(_target.find('?') + 1);
      _target.erase(_target.find('?'));
    }

    if (buffer_.find(' ') == 0)
      return 400;

    size_t end = buffer_.find("\r\n");
    tmp = buffer_.substr(0, end);

    if (tmp == "HTTP/1.1") {
      _protocol = tmp;
      buffer_.erase(0, end + 2);
    } else
      return 505;

    status_ = HEADERS;
  }

  return 0;
}

int Request::headers() {
  size_t end, last;
  std::string header;
  std::string value;

  while ((end = buffer_.find("\r\n")) != std::string::npos) {
    if (buffer_.find("\r\n") == 0) {
      buffer_.erase(0, end + 2);
      status_ = PREBODY;
      break;
    }
    if ((last = buffer_.find(':', 0)) != std::string::npos) {
      if (last == 0 || buffer_[last - 1] == ' ')
        return 400;
      header = buffer_.substr(0, last);
      value = buffer_.substr(last + 1, end - last - 1);
      if (header == "Host" && _headers.count(header))
        return 400;
      if (header.length() > 1000 || value.length() > 4000)
        return 400;
      _headers[header] = ft::trim_left(ft::trim_right(value, ' '), ' ');
      if (_headers[header].empty())
        _headers.erase(header);
    }
    else
      return 400;
    buffer_.erase(0, end + 2);
  }
  return 0;
}

int Request::prebody() {
  body_offset_ = 0;

  if (_headers.find("Host") == _headers.end() || _headers["Host"].empty())
    return 400;

  if (_headers["Host"].find("@") != std::string::npos)
    return 400;

  if (_headers.find("Transfer-Encoding") != _headers.end() && _headers["Transfer-Encoding"] == "chunked") {
    status_ = CHUNK;
    chunk_status_ = CHUNK_SIZE;
  } else if (_headers.find("Content-Length") != _headers.end()) {
    if (_headers["Content-Length"].find_first_not_of("0123456789") != std::string::npos)
      return 400;
    try {
      length_ = ft::stoi(_headers["Content-Length"]);
      if (length_ < 0)
        throw std::invalid_argument("negative content-length");
    }
    catch (std::exception &e) {
      return 400;
    }
    status_ = BODY;
  }
  else {
    return 1;
  }
  if (_method != "POST" && _method != "PUT")
    return 1;
  return 0;
}

int Request::chunk_trailer() {
  size_t end, last;
  std::string header;
  std::string value;

  while ((end = buffer_.find("\r\n")) != std::string::npos) {
    if (buffer_.find("\r\n") == 0) {
      buffer_.erase(0, end + 2);
      break;
    }
    if ((last = buffer_.find(':', 0)) != std::string::npos) {
      header = buffer_.substr(0, last);
      value = buffer_.substr(last + 1, end - last - 1);
      _headers[header] = ft::trim_left(value, ' ');
    }
    else
      return 400;
    buffer_.erase(0, end + 2);
  }
  return 1;
}

int Request::chunk() {
  size_t end;

  while ((end = buffer_.find("\r\n")) != std::string::npos) {
    if (chunk_status_ == CHUNK_SIZE) {
      std::string hex = buffer_.substr(0, end);
      chunk_size_ = ft::to_hex(hex);
      buffer_.erase(0, end + 2);
      chunk_status_ = CHUNK_BODY;
    } else if (chunk_status_ == CHUNK_BODY) {
      if (chunk_size_ == 0) {
        if (!buffer_.empty())
          return chunk_trailer();
        return 1;
      }
      _req_body += buffer_.substr(0, end);
      buffer_.erase(0, end + 2);
      chunk_size_ = 0;
      chunk_status_ = CHUNK_SIZE;
    }
  }
  return 0;
}

int Request::body() {
  if (buffer_.length() >= length_) {
    _req_body.insert(body_offset_, buffer_, 0, length_);
    body_offset_ += buffer_.length();
    buffer_.clear();

    if (_req_body.length() == length_)
      return 1;
    else
      return 400;
  }
  return 0;
}

bool Request::timeout() {
  if (status_ != COMPLETE) {
    status_ = ERROR;
    return true;
  }
  return false;
}

int Request::getStatus() {
  return status_;
}

time_t Request::get_start_timer_in_sec() {
  return start_timer_.tv_sec;
}

time_t Request::get_last_timer_in_sec() {
  return last_timer_.tv_sec;
}