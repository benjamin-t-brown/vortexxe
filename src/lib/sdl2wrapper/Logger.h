#pragma once

#include <fstream>
#include <sstream>
#include <string>

#ifdef __vita__
#include "debugScreen.h"
#endif

namespace sdl2w {

enum LogType { DEBUG, INFO, WARN, ERROR };

class Logger {
public:
  static const std::string endl;
  static bool disabled;
  static bool colorEnabled;
  static bool logToFile;
  static std::fstream logFile;

  Logger(){};
  virtual ~Logger();
  std::ostringstream& get(LogType level = INFO);
  std::ostringstream os;
  std::string getLabel(LogType type);
  static void setLogToFile(bool logToFile);

  int printf(const char* format, ...);
};

} // namespace sdl2w
