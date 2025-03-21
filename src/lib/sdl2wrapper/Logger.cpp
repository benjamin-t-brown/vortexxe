#include "Logger.h"
#include <regex>
#include <stdarg.h>
#include <stdio.h>

#ifdef __vita__
#include "debugScreen.h"
#endif

namespace sdl2w {

static std::string removeANSIEscapeCodes(const std::string& str) {
  static const std::regex ansiRegex("\x1B\\[[0-9;]*m");
  return std::regex_replace(str, ansiRegex, "");
}

const std::string Logger::endl = std::string("\n");
bool Logger::disabled = false;
bool Logger::colorEnabled = true;
bool Logger::logToFile = false;
std::fstream Logger::logFile;

std::ostringstream& Logger::get(LogType level) {
  std::string label = getLabel(level);
  os << label;
  return os;
}
std::string Logger::getLabel(LogType type) {
  std::string label;
  switch (type) {
  case DEBUG:
    label = "{ DEBUG } ";
    if (Logger::colorEnabled) {
      label = "\033[36m" + label + "\033[0m";
    }
    break;
  case INFO:
    label = "{ INFO  } ";
    if (Logger::colorEnabled) {
      label = "\033[32m" + label + "\033[0m";
    }
    break;
  case WARN:
    label = "{ WARN  } ";
    if (Logger::colorEnabled) {
      label = "\033[33m" + label + "\033[0m";
    }
    break;
  case ERROR:
    label = "{ ERROR } ";
    if (Logger::colorEnabled) {
      label = "\033[31m" + label + "\033[0m";
    }
    break;
  }
  return label;
}
Logger::~Logger() {
  if (!Logger::disabled) {
    fprintf(stdout, "%s", os.str().c_str());
    fflush(stdout);
    if (Logger::logToFile && Logger::logFile.is_open()) {
      Logger::logFile << removeANSIEscapeCodes(os.str());
      Logger::logFile.flush();
    }
  }
}

void Logger::setLogToFile(bool logToFileA) {
  if (logToFileA) {
    Logger::logFile.open("output.log", std::ios::out | std::ios::trunc);
  } else if (!logToFileA && Logger::logFile.is_open()) {
    Logger::logFile.close();
  }
  Logger::logToFile = logToFileA;
}

int Logger::printf(const char* c, ...) {
  if (Logger::disabled) {
    return 0;
  }
#ifdef __vita__
  va_list lst;
  va_start(lst, c);
  psvDebugScreenPrintf(c, lst);
  va_end(lst);
#else
  va_list lst;
  va_start(lst, c);
  while (*c != '\0') {
    if (*c != '%') {
      putchar(*c);
      // NOLINTNEXTLINE
      c++;
      continue;
    }
    // NOLINTNEXTLINE
    c++;
    switch (*c) {
    case 's':
      fputs(va_arg(lst, char*), stdout);
      break;
    case 'c':
      putchar(va_arg(lst, int));
      break;
    }
    // NOLINTNEXTLINE
    c++;
  }
  va_end(lst);
  return 0;
#endif
}
} // namespace sdl2w
