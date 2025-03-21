#pragma once

#include "lib/sdl2wrapper/Logger.h"
#include <sstream>
#include <stdarg.h>

using sdl2w::Logger;
using sdl2w::LogType;

namespace logger {

inline void args(va_list lst, const char* c, std::stringstream& ss) {
  while (*c != '\0') {
    if (*c != '%') {
      ss << *c;
      // NOLINTNEXTLINE
      c++;
      continue;
    }
    // NOLINTNEXTLINE
    c++;
    if (*c == '%') {
      ss << '%';
      // NOLINTNEXTLINE
      c++;
      continue;
    }
    switch (*c) {
    case 's':
      ss << va_arg(lst, char*);
      break;
    case 'i':
    case 'c':
      ss << va_arg(lst, int);
      break;
    case 'd':
      ss << va_arg(lst, double);
      break;
    }
    // NOLINTNEXTLINE
    c++;
  }
}

inline void info(const char* c, ...) {
  std::stringstream ss;
  va_list lst;
  va_start(lst, c);
  args(lst, c, ss);
  va_end(lst);
  Logger().get(LogType::INFO) << ss.str() << Logger::endl;
}

inline void error(const char* c, ...) {
  std::stringstream ss;
  va_list lst;
  va_start(lst, c);
  args(lst, c, ss);
  va_end(lst);
  Logger().get(LogType::ERROR) << ss.str() << Logger::endl;
}

inline void debug(const char* c, ...) {
  std::stringstream ss;
  va_list lst;
  va_start(lst, c);
  args(lst, c, ss);
  va_end(lst);
  Logger().get(LogType::DEBUG) << ss.str() << Logger::endl;
}

inline void warn(const char* c, ...) {
  std::stringstream ss;
  va_list lst;
  va_start(lst, c);
  args(lst, c, ss);
  va_end(lst);
  Logger().get(LogType::WARN) << ss.str() << Logger::endl;
}
} // namespace logger