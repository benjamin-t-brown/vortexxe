#include "Localization.h"
#include "lib/sdl2wrapper/AssetLoader.h"
#include "lib/sdl2wrapper/Logger.h"
#include <unordered_map>

#ifndef MIYOOA30
#include "lib/minijsoncpp/minijsoncpp.h"
#endif

namespace sdl2w {

std::string L10n::language = "en";
std::unordered_map<std::string, std::unordered_map<LOCSTR_TYPE, std::string>>
    L10n::locStrings;
std::vector<std::string> L10n::supportedLanguages = {"en", "la"};

void L10n::init() {
  Logger().get() << "Localization is " << L10N_STATUS << Logger::endl;
  if (!isEnabled()) {
    return;
  }

  for (const auto& lang : supportedLanguages) {
    const std::string path = "assets/translation." + lang + ".json";
    try {
      const std::string languageFile = loadFileAsString(path);
      loadLanguage(lang, languageFile);
    } catch (std::exception& e) {
      Logger().get(ERROR) << "Failed to load language file '" << path
                          << "': " << e.what() << Logger::endl;
    }
  }
}

void L10n::loadLanguage(const std::string& lang, const std::string& langFile) {
  if (!isEnabled()) {
    return;
  }

  // TODO MiyooA30's gcc does not support minjson
#ifndef MIYOOA30
  auto parseResult = minjson::parse(langFile);
  if (parseResult.status != minjson::ParsingResultStatus::Success) {
    throw std::runtime_error("Could not parse json.");
  }
  locStrings[lang] = std::unordered_map<LOCSTR_TYPE, std::string>();
  for (const auto& [key, val] : parseResult.value.asObject()) {
    auto hashKey = LOCSTR(key);
    locStrings[lang][hashKey] = val.asString();
  }
#else
  Logger().get(ERROR) << "Localization not supported on this platform."
                      << Logger::endl;
#endif
}

bool L10n::isEnabled() { return L10N_STATUS == std::string("enabled"); }

void L10n::setLanguage(const std::string& lang) {
  if (!isEnabled()) {
    return;
  }

  auto it = locStrings.find(lang);
  if (it != locStrings.end()) {
    language = lang;
  } else {
    Logger().get(ERROR) << "Language '" << lang << "' not supported."
                        << Logger::endl;
  }
}

const std::unordered_map<LOCSTR_TYPE, std::string>& L10n::getStrings() {
  return locStrings[language];
}

std::string L10n::trans(const LOCSTR_TYPE& id) {
#ifdef SDL2WRAPPER_ENABLE_LOCALIZATION
  auto strings = getStrings();
  auto it = strings.find(id);
  if (it != strings.end() && it->second.size()) {
    return it->second;
  }
  return "?MISSING?";
#else
  return std::string(id);
#endif
}

} // namespace sdl2w