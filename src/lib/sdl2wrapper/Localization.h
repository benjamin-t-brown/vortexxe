/*
This method of localization is taken from
https://gamedev.stackexchange.com/questions/168362/handling-localization-strings-in-a-performant-game-engine-c

It uses a hash map to store the strings, and a constexpr hash function to
generate the keys.

You would use this as so:

drawText(L10n::trans(LOCSTR("HelloWorld")), 0, 0);
**/

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace sdl2w {

inline size_t hash(const std::string& str) {
  std::hash<std::string> hasher;
  return hasher(str);
}

#ifdef SDL2WRAPPER_ENABLE_LOCALIZATION
#define LOCSTR(token) sdl2w::hash(token)
#define LOCSTR_TYPE size_t
#define L10N_STATUS "enabled"
#else
#define LOCSTR(token) token
#define LOCSTR_TYPE std::string
#define L10N_STATUS "disabled"
#endif

class L10n {
  static std::string language;
  static std::unordered_map<std::string,
                            std::unordered_map<LOCSTR_TYPE, std::string>>
      locStrings;
  static std::vector<std::string> supportedLanguages;

public:
  static void init();
  static void loadLanguage(const std::string& lang,
                           const std::string& langFile);
  static void setLanguage(const std::string& lang);
  static const std::unordered_map<LOCSTR_TYPE, std::string>& getStrings();
  static std::string trans(const LOCSTR_TYPE& id);
};

} // namespace sdl2w
