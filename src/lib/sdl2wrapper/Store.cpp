#include "Store.h"
#include "Logger.h"
#include "Window.h"
#include <algorithm>
#include <stdexcept>
#ifdef MIYOOA30
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#endif

namespace sdl2w {

unsigned int Store::numSoundChunks = 2;
SDL_Renderer* Store::rendererPtr = nullptr;
std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDL_Deleter>>
    Store::textures;
std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDL_Deleter>>
    Store::textTextures;
std::unordered_map<std::string, std::unique_ptr<Sprite>> Store::sprites;
std::unordered_map<std::string, std::unique_ptr<AnimationDefinition>> Store::anims;
std::unordered_map<std::string, std::unique_ptr<TTF_Font, SDL_Deleter>> Store::fonts;
std::unordered_map<std::string, std::unique_ptr<Mix_Chunk, SDL_Deleter>> Store::sounds;
std::unordered_map<std::string, unsigned int> Store::soundInds;
std::unordered_map<std::string, std::unique_ptr<Mix_Music, SDL_Deleter>> Store::musics;

void throwError(const std::string& errorMessage) {
  Logger().get(ERROR) << errorMessage << Logger::endl;
  throw std::runtime_error(errorMessage);
}

Store::Store() {}

Store::~Store() {}

void Store::setRenderer(std::unique_ptr<SDL_Renderer, SDL_Deleter>& rendererA) {
  Store::rendererPtr = rendererA.get();
}

void Store::storeTexture(const std::string& name, SDL_Texture* tex) {
  textures[name] = std::unique_ptr<SDL_Texture, SDL_Deleter>(tex, SDL_Deleter());
}

void Store::storeTextTexture(const std::string& name, SDL_Texture* tex) {
  textTextures[name] = std::unique_ptr<SDL_Texture, SDL_Deleter>(tex, SDL_Deleter());
}

void Store::createTexture(const std::string& name, const std::string& path) {
  if (Store::rendererPtr == nullptr) {
    return throwError("[sdl2w] ERROR Cannot create textures without a "
                      "renderer (initialized in Window class).");
  }

  SDL_Texture* tex = nullptr;
  SDL_Surface* loadedImage = nullptr;

  loadedImage = IMG_Load(path.c_str());

  if (loadedImage != nullptr) {
    tex = SDL_CreateTextureFromSurface(Store::rendererPtr, loadedImage);
    if (tex == nullptr) {
      Logger().get(WARN) << "[sdl2w] WARNING Tried to create texture image "
                            "without creating a screen."
                         << Logger::endl;
      return;
    }
    textures[name] = std::unique_ptr<SDL_Texture, SDL_Deleter>(tex, SDL_Deleter());
    createSprite(name, textures[name].get());
    SDL_FreeSurface(loadedImage);
  } else {
    return throwError("[sdl2w] ERROR Failed to load image '" + path + "' (" + name + ")");
  }
}

void Store::createFont(const std::string& name, const std::string& path) {
  if (!TTF_WasInit() && TTF_Init() == -1) {
    return throwError(std::string("[sdl2w] ERROR Failed to initialize TTF: " +
                                  std::string(SDL_GetError())));
  }

  static const std::vector<int> sizes = {
      10, 12, 14, 15, 16, 18, 20, 22, 24, 36, 48, 60, 72};
  for (const int& size : sizes) {
    const std::string key = name + std::to_string(size);
    fonts[key] = std::unique_ptr<TTF_Font, SDL_Deleter>(TTF_OpenFont(path.c_str(), size));

    if (!fonts[key]) {
      return throwError("[sdl2w] ERROR Failed to load font '" + path +
                        "': reason= " + std::string(SDL_GetError()));
    }

    fonts[key + "o"] =
        std::unique_ptr<TTF_Font, SDL_Deleter>(TTF_OpenFont(path.c_str(), size));
    TTF_SetFontOutline(fonts[key + "o"].get(), 1);
  }
}

void Store::createSprite(const std::string& name, SDL_Texture* tex) {
  int width, height;
  SDL_QueryTexture(tex, nullptr, nullptr, &width, &height);
  if (sprites.find(name) == sprites.end()) {
    sprites[name] = std::make_unique<Sprite>(name, 0, 0, width, height, tex);
  } else {
    Logger().get(WARN) << "[sdl2w] WARNING Sprite with name '" << name
                       << "' already exists. '" << name << "'" << Logger::endl;
  }
}

void Store::createSprite(const std::string& name,
                         const std::string& textureName,
                         const int x,
                         const int y,
                         const int w,
                         const int h) {
  if (sprites.find(name) == sprites.end()) {
    SDL_Texture* tex = getTexture(textureName);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    sprites[name] = std::make_unique<Sprite>(name, x, y, w, h, tex);
  } else {
    Logger().get(WARN) << "[sdl2w] WARNING Sprite with name '" << name
                       << "' already exists. '" << name << "'" << Logger::endl;
  }
}

AnimationDefinition& Store::createAnimationDefinition(const std::string& name,
                                                      const bool loop) {
  if (anims.find(name) == anims.end()) {
    anims[name] = std::make_unique<AnimationDefinition>(name, loop);
  } else {
    Logger().get(WARN) << "[sdl2w] WARNING Cannot create new anim, it already "
                          "exists: '" +
                              name + "'"
                       << Logger::endl;
  }
  return *anims[name];
}
void Store::createSound(const std::string& name, const std::string& path) {
  if (Window::getGlobalWindow().soundForcedDisabled || !Window::soundCanBeLoaded) {
    Logger().get(WARN) << "[sdl2w] WARNING sound load skipped due to force sound "
                          "disabled "
                          "exists: '" +
                              name + "'"
                       << Logger::endl;
    return;
  }

  for (unsigned int i = 0; i < Store::numSoundChunks; i++) {
    const std::string innerSoundName = name + "_" + std::to_string(i);
    if (sounds.find(innerSoundName) == sounds.end()) {
      sounds[innerSoundName] = std::unique_ptr<Mix_Chunk, SDL_Deleter>(
          Mix_LoadWAV(path.c_str()), SDL_Deleter());
      if (!sounds[innerSoundName]) {
        throw std::string("[sdl2w] ERROR Failed to load sound '" + path +
                          "': reason= " + std::string(Mix_GetError()));
      }
    } else {
      Logger().get(WARN) << "[sdl2w] WARNING Sound with name '" << name
                         << "' already exists. chunkName='" << innerSoundName << "'"
                         << Logger::endl;
    }
  }

  soundInds[name] = 0;
}
void Store::createMusic(const std::string& name, const std::string& path) {
  if (Window::getGlobalWindow().soundForcedDisabled || !Window::soundCanBeLoaded) {
    return;
  }

  if (musics.find(name) == musics.end()) {
    musics[name] =
        std::unique_ptr<Mix_Music, SDL_Deleter>(Mix_LoadMUS(path.c_str()), SDL_Deleter());
    if (!musics[name]) {
      throw std::string("[sdl2w] ERROR Failed to load music '" + path +
                        "': reason= " + std::string(Mix_GetError()));
    }
  } else {
    Logger().get(WARN) << "[sdl2w] WARNING Music with name '" << name
                       << "' already exists. '" << name << "'" << Logger::endl;
  }
}

void Store::logSprites() {
  Logger().get(DEBUG) << "[sdl2w] Sprites:" << Logger::endl;
  std::vector<std::string> localSprites;
  localSprites.reserve(sprites.size());
  std::transform(sprites.begin(),
                 sprites.end(),
                 std::back_inserter(localSprites),
                 [](const auto& p) -> std::string { return p.first; });
  std::sort(localSprites.begin(), localSprites.end());
  for (auto& it : localSprites) {
    Logger().get(DEBUG) << " " << it << Logger::endl;
  }
}
void Store::logAnimationDefinitions() {
  Logger().get(DEBUG) << "[sdl2w] AnimationDefinitions:" << Logger::endl;
  std::vector<std::string> localAnims;
  localAnims.reserve(anims.size());
  std::transform(anims.begin(),
                 anims.end(),
                 std::back_inserter(localAnims),
                 [](const auto& p) -> std::string { return p.first; });
  std::sort(localAnims.begin(), localAnims.end());
  for (auto& it : localAnims) {
    Logger().get(DEBUG) << " " << it << Logger::endl;
  }
}

void Store::logFonts() {
  Logger().get(DEBUG) << "[sdl2w] Fonts:" << Logger::endl;
  for (auto& it : fonts) {
    Logger().get(DEBUG) << " " << it.first << Logger::endl;
  }
}

void Store::logSounds() {
  Logger().get(DEBUG) << "[sdl2w] Sounds:" << Logger::endl;
  for (auto& it : sounds) {
    Logger().get(DEBUG) << " " << it.first << Logger::endl;
  }
}

bool Store::textureExists(const std::string& name) {
  auto pair = textures.find(name);
  if (pair != textures.end()) {
    return true;
  } else {
    return false;
  }
}

SDL_Texture* Store::getTexture(const std::string& name) {
  auto pair = textures.find(name);
  if (pair != textures.end()) {
    return pair->second.get();
  } else {
    throwError(std::string("[sdl2w] ERROR Cannot get Image '" + name +
                           "' because it has not been loaded."));
    throw std::runtime_error("fail");
  }
}

SDL_Texture* Store::getTextTexture(const std::string& key) {
  auto pair = textTextures.find(key);
  if (pair != textTextures.end()) {
    return pair->second.get();
  } else {
    return nullptr;
  }
}

Sprite& Store::getSprite(const std::string& name) {
  auto pair = sprites.find(name);
  if (pair != sprites.end()) {
    return *pair->second;
  } else {
    logSprites();
    throwError(std::string("[sdl2w] ERROR Cannot get Sprite '" + name +
                           "' because it has not been created."));
    throw std::runtime_error("fail");
  }
}

bool Store::spriteExists(const std::string& name) {
  auto pair = sprites.find(name);
  return pair != sprites.end();
}

AnimationDefinition& Store::getAnimationDefinition(const std::string& name) {
  auto pair = anims.find(name);
  if (pair != anims.end()) {
    return *pair->second;
  } else {
    throwError(std::string("[sdl2w] ERROR Cannot get AnimationDefinition '" + name +
                           "' because it has not been created."));
    throw std::runtime_error("fail");
  }
}

TTF_Font* Store::getFont(const std::string& name, const int sz, const bool isOutline) {
  const std::string key = name + std::to_string(sz) + (isOutline ? "o" : "");
  auto pair = fonts.find(key);
  if (pair != fonts.end()) {
    return pair->second.get();
  } else {
    throwError(std::string("[sdl2w] ERROR Cannot get Font '" + key +
                           "' because it has not been created."));
    throw std::runtime_error("fail");
  }
}

Mix_Chunk* Store::getSound(const std::string& name) {
  auto indPair = soundInds.find(name);
  if (indPair != soundInds.end()) {
    const std::string chunkName = name + "_" + std::to_string(indPair->second);
    auto pair = sounds.find(chunkName);
    if (pair != sounds.end()) {
      soundInds[name] = (indPair->second + 1) % Store::numSoundChunks;
      return pair->second.get();
    } else {
      throwError(
          std::string("[sdl2w] ERROR Cannot get Sound '" + name +
                      "' because chunk has not been loaded. ChunkName=" + chunkName));
      throw std::runtime_error("fail");
    }
  } else {
    throwError(std::string("[sdl2w] ERROR Cannot get Sound '" + name +
                           "' because it has not been loaded."));
    throw std::runtime_error("fail");
  }
}
Mix_Music* Store::getMusic(const std::string& name) {
  auto pair = musics.find(name);
  if (pair != musics.end()) {
    return pair->second.get();
  } else {
    throwError(std::string("[sdl2w] ERROR Cannot get Music '" + name +
                           "' because it has not been loaded."));
    throw std::runtime_error("fail");
  }
}

void Store::clear() {
  textures.clear();
  textTextures.clear();
  sprites.clear();
  anims.clear();
  fonts.clear();
  sounds.clear();
  musics.clear();
}

} // namespace sdl2w
