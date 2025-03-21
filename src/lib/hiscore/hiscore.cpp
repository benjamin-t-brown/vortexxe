#include "hiscore.h"
#include "lib/sdl2wrapper/AssetLoader.h"
#include "lib/sdl2wrapper/Logger.h"
#include <sstream>

namespace hiscore {
bool isLoaded = false;
std::string hiscorePath = "hiscore.txt";
std::vector<HiscoreRow> hiscores;

std::vector<HiscoreRow> parseHiscoreText(const std::string& hiscoreText) {
  std::vector<HiscoreRow> hiscores;
  std::stringstream ss(hiscoreText);
  std::string line;
  while (std::getline(ss, line)) {
    std::stringstream lineStream(line);
    std::string name;
    int score;
    lineStream >> name >> score;
    sdl2w::Logger().get() << "Parsed hiscore: " << name << " " << score
                          << sdl2w::Logger::endl;
    hiscores.push_back({name, score});
  }
  return hiscores;
}

std::string serializeHiscoreRow(const HiscoreRow& row) {
  std::stringstream ss;
  ss << row.name << " " << row.score;
  return ss.str();
}

std::vector<HiscoreRow> getHighScores() {
  if (!isLoaded) {
    try {
      const std::string hiscoreText = sdl2w::loadFileAsString(hiscorePath);
      hiscores = parseHiscoreText(hiscoreText);
    } catch (std::exception& e) {
      // if file does not exist, create it
      saveHighScores(hiscores);
    }
    isLoaded = true;
  }
  return hiscores;
}

void saveHighScores(const std::vector<HiscoreRow>& hiscoresA) {
  hiscores = hiscoresA;

  // TODO local storage
#ifdef __EMSCRIPTEN__
  return;
#endif
  std::string content = "";
  for (auto& score : hiscoresA) {
    content += serializeHiscoreRow(score) + "\n";
  }
  sdl2w::saveFileAsString(hiscorePath, content);
}
} // namespace hiscore