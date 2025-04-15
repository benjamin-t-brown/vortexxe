#include "AssetLoader.h"
#include "Animation.h"
#include "Logger.h"
#include "Sprite.h"
#include "Store.h"
#include <fstream>
#include <sstream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// maximum length of a line in the asset file
#define TEXT_LEN 1024 * 32

#ifdef __vita__
#define PREFIX std::string("app0:")
#else
#define PREFIX std::string("")
#endif

namespace sdl2w {
std::string slice(const std::string& str, int a, int b) {
  if (a == b) {
    return std::string("");
  }

  if (b <= 0) {
    b = str.size() + b;
  } else if ((unsigned int)b > str.size()) {
    b = str.size();
  }

  if (a < 0) {
    a = str.size() + a;
  } else if ((unsigned int)a > str.size()) {
    a = str.size();
  }

  if (b <= a || a < 0) {
    return std::string("");
  }

  return str.substr(a, b - a);
}

std::string trim(const std::string& str) {
  if (!str.size()) {
    return std::string(str);
  }

  std::string ret(str);
  int firstNonspace = 0;
  int lastNonspace = str.size() - 1;

  for (unsigned int i = 0; i < str.size(); i++) {
    if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t') {
      firstNonspace = i;
      break;
    }
  }

  for (int i = (int)str.size() - 1; i >= 0; i--) {
    if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t') {
      lastNonspace = i;
      break;
    }
  }
  return slice(str, firstNonspace, lastNonspace + 1);
}

void split(const std::string& str,
           const std::string& token,
           std::vector<std::string>& out) {
  if (str.size() == 0) {
    unsigned int len = str.size();
    for (unsigned int i = 0; i < len; ++i) {
      std::string s = std::to_string(str.at(i));
      out.emplace_back(s);
    }
    return;
  }

  bool searching = true;
  std::size_t firstIndex = 0;
  std::size_t secondIndex = 0;
  while (searching) {
    secondIndex = str.find(token, firstIndex);
    if (secondIndex == std::string::npos || secondIndex < firstIndex) {
      searching = false;
    } else {
      out.push_back(slice(str, firstIndex, secondIndex));
      firstIndex = secondIndex + 1;
    }
  }
  if (secondIndex == 0) {
    out.push_back(str);
  } else {
    out.push_back(slice(str, firstIndex, str.size()));
  }
}

bool strEndsWith(const std::string& fullString, const std::string& ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(),
                                    ending.length(),
                                    ending));
  } else {
    return false;
  }
}

int getLineFromStream(std::istream& is) {
  int lineCount = 1;
  is.clear();
  auto originalPos = is.tellg();
  if (originalPos < 0) {
    return -1;
  }
  is.seekg(0);
  char c;
  while ((is.tellg() < originalPos) && is.get(c)) {
    if (c == '\n') {
      ++lineCount;
    }
  }
  return lineCount;
}

void loadSpriteAssetsFromFile(const std::string& path) {
  Logger().get(DEBUG) << "Loading sprite assets from file " << (PREFIX + path)
                      << Logger::endl;
  std::ifstream file(PREFIX + path);
  if (!file.is_open()) {
    Logger().get(ERROR) << "Failed to open file: " << (PREFIX + path)
                        << Logger::endl;
    return;
  }
  std::string lastPicture;
  int lastSpriteInd = 0;
  std::string line;
  try {
    while (std::getline(file, line)) {
      line = trim(line);
      if (line.empty())
        continue;
      std::vector<std::string> arr;
      split(line, ",", arr);
      if (arr.empty())
        continue;
      if (arr[0] == "Picture") {
        lastPicture = arr[1];
        lastSpriteInd = 0;
        Store::createTexture(arr[1], arr[2]);
      } else if (arr[0] == "SpriteList") {
        Sprite& image = Store::getSprite(lastPicture);
        std::string name = arr[1];
        int n = std::stoi(arr[2]) + lastSpriteInd;
        int w = std::stoi(arr[3]);
        int h = std::stoi(arr[4]);
        int num_x = image.cw / w;
        int ctr = 0;
        for (int i = lastSpriteInd; i < n; i++) {
          std::string sprName = name + "_" + std::to_string(ctr);
          Store::createSprite(
              sprName, lastPicture, (i % num_x) * w, (i / num_x) * h, w, h);
          ctr++;
        }
        lastSpriteInd = n;
      } else if (arr[0] == "Sprite") {
        std::string name = arr[1];
        int x = std::stoi(arr[2]);
        int y = std::stoi(arr[3]);
        int w = std::stoi(arr[4]);
        int h = std::stoi(arr[5]);
        Store::createSprite(name, lastPicture, x, y, w, h);
      }
    }
    file.close();
  } catch (std::exception& e) {
    Logger().get(ERROR) << "Failed to parse sprite list: " << e.what()
                        << Logger::endl;
  }
}

void loadAnimationAssetsFromFile(const std::string& path) {
  Logger().get(DEBUG) << "Loading anim assets from file " << (PREFIX + path)
                      << Logger::endl;
  try {
    std::string animName = "";
    std::string line;

    std::ifstream file(PREFIX + path);

    int lineOffset = 0;

    while (std::getline(file, line)) {
      if (line[0] == '#') {
        lineOffset = 1;
      } else if (line.size() > 1) {
        if (lineOffset == 1) {
          animName = std::string(line);
          lineOffset = 2;
        } else if (lineOffset == 2) {
          const std::string loop = std::string(line);
          Store::createAnimationDefinition(animName,
                                           (loop == "loop" ? true : false));
          lineOffset = 3;
        } else {
          AnimationDefinition& anim = Store::getAnimationDefinition(animName);
          std::stringstream ss;
          ss << line;
          std::string strName;
          std::string strFrames;
          std::getline(ss, strName, ' ');
          std::getline(ss, strFrames, ' ');
          int frames = 0;

          try {
            frames = std::stoi(strFrames);
          } catch (std::exception& e) {
            Logger().get(ERROR)
                << "Failed to load anim sprite for: " << animName
                << Logger::endl;
            Logger().get(ERROR) << " FROM: '" << line << "'" << Logger::endl;
          }

          bool flip = false;
          if (strEndsWith(strName, "_f")) {
            flip = true;
            strName = strName.substr(0, strName.size() - 2);
          }

          anim.addSprite(strName, frames, flip);
        }
      }
    }
    file.close();
  } catch (std::exception& e) {
    Logger().get(ERROR) << "Failed to parse anim list: " << e.what()
                        << Logger::endl;
  }
}

void loadSoundAssetsFromFile(const std::string& path) {
  Logger().get(DEBUG) << "Loading sound assets from file " << (PREFIX + path)
                      << Logger::endl;
  std::ifstream file(PREFIX + path);
  if (!file.is_open()) {
    Logger().get(ERROR) << "Failed to open file: " << (PREFIX + path)
                        << Logger::endl;
    return;
  }
  std::string line;
  try {
    while (std::getline(file, line)) {
      line = trim(line);
      if (line.size()) {
        std::vector<std::string> arr;
        split(line, ",", arr);
        if (arr.size() >= 3) {
          if (arr[0] == "Sound") {
            Store::createSound(arr[1], arr[2]);
          } else if (arr[0] == "Music") {
            Store::createMusic(arr[1], arr[2]);
          }
        }
      }
    }
    file.close();
  } catch (std::exception& e) {
    Logger().get(ERROR) << "Failed to parse sound/music list: " << e.what()
                        << Logger::endl;
  }
}

void loadAssetsFromFile(const std::string& type, const std::string& path) {
  if (type == "sprite") {
    loadSpriteAssetsFromFile(path);
  } else if (type == "animation") {
    loadAnimationAssetsFromFile(path);
  } else if (type == "sound") {
    loadSoundAssetsFromFile(path);
  }
}

std::string loadFileAsString(const std::string& path) {
#ifdef __EMSCRIPTEN__
  Logger().get(DEBUG) << "Loading file " << ("/sdl2wdata/" + path)
                      << Logger::endl;
  std::ifstream file("/sdl2wdata/" + path);
#else
  Logger().get(DEBUG) << "Loading file " << (PREFIX + path) << Logger::endl;
  std::ifstream file(PREFIX + path);
#endif

  if (!file) {
#ifdef __EMSCRIPTEN__
    return "";
#else
    throw std::runtime_error("Error opening file: " + path);
#endif
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void saveFileAsString(const std::string& path, const std::string& content) {
#ifdef __EMSCRIPTEN__
  Logger().get(DEBUG) << "Saving file " << ("/sdl2wdata/" + path)
                      << Logger::endl;
  std::ofstream file("/sdl2wdata/" + path);
#else
  Logger().get(DEBUG) << "Saving file " << (PREFIX + path) << Logger::endl;
  std::ofstream file(PREFIX + path);
#endif

  if (!file) {
    // TODO emscripten is set to not catch errors
#ifdef __EMSCRIPTEN__
    return;
#else
    throw std::runtime_error("Error opening file for save: " + path);
#endif
  }
  file << content;
  file.close();

#ifdef __EMSCRIPTEN__
  // Sync the file system so that changes are persisted in IndexedDB.
  EM_ASM({
    FS.syncfs(
        false, function(err) {
          if (err)
            console.error("Error syncing FS after write:", err);
          else
            console.log("FS sync complete after write.");
        });
  });
#endif
}

} // namespace sdl2w
