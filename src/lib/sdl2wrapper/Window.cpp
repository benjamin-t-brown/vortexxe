#include "Window.h"
#include "Logger.h"
#include "Store.h"

#include <math.h>
#include <sstream>
#include <stdio.h>
#include <time.h>

#ifdef MIYOOA30
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_video.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#endif

// #define __EMSCRIPTEN__ 1

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

extern "C" {
EMSCRIPTEN_KEEPALIVE
void enableSound() {
  sdl2w::Window::soundEnabled = true;
  int volumePct = sdl2w::Window::soundPercent;
  Mix_VolumeMusic((double(volumePct) / 100.0) * double(MIX_MAX_VOLUME));
  Mix_Volume(-1, (double(volumePct) / 100.0) * double(MIX_MAX_VOLUME));
  sdl2w::Logger().get(sdl2w::DEBUG) << "Enable sound" << sdl2w::Logger::endl;
}
EMSCRIPTEN_KEEPALIVE
void disableSound() {
  sdl2w::Window::soundEnabled = false;

  Mix_VolumeMusic(0);
  Mix_Volume(-1, 0);
  sdl2w::Logger().get(sdl2w::DEBUG) << "Disable sound" << sdl2w::Logger::endl;
}
EMSCRIPTEN_KEEPALIVE
void setVolume(int volumePct) {
  sdl2w::Window& window = sdl2w::Window::getGlobalWindow();
  window.setVolume(double(volumePct));
  sdl2w::Logger().get(sdl2w::DEBUG)
      << "Set volume:" << volumePct << "%" << sdl2w::Logger::endl;
}
EMSCRIPTEN_KEEPALIVE
void setKeyDown(int key) {
  sdl2w::Window& window = sdl2w::Window::getGlobalWindow();
  sdl2w::Events& events = window.getEvents();
  events.keydown(key);
  sdl2w::Logger().get(sdl2w::DEBUG)
      << "External set key down: " << key << sdl2w::Logger::endl;
}
EMSCRIPTEN_KEEPALIVE
void setKeyUp(int key) {
  sdl2w::Window& window = sdl2w::Window::getGlobalWindow();
  sdl2w::Events& events = window.getEvents();
  events.keyup(key);
  sdl2w::Logger().get(sdl2w::DEBUG)
      << "External set key up: " << key << sdl2w::Logger::endl;
}
EMSCRIPTEN_KEEPALIVE
void setKeyStatus(int status) {
  sdl2w::Window& window = sdl2w::Window::getGlobalWindow();
  window.isInputEnabled = !!status;
  sdl2w::Logger().get(sdl2w::DEBUG)
      << "External set key status: " << window.isInputEnabled
      << sdl2w::Logger::endl;
}
EMSCRIPTEN_KEEPALIVE
void sendEvent(int event, int payload) {
  sdl2w::Window& window = sdl2w::Window::getGlobalWindow();
  window.externalEvents.emplace_back(event, payload);
  sdl2w::Logger().get(sdl2w::DEBUG) << "External event received: " << event
                                    << ":" << payload << sdl2w::Logger::endl;
}
}
#endif

#ifdef __vita__
#define SDL_JOYSTICK_BUTTON_CROSS 2
#define SDL_JOYSTICK_BUTTON_CIRCLE 1
#define SDL_JOYSTICK_BUTTON_SQUARE 3
#define SDL_JOYSTICK_BUTTON_TRIANGLE 4
#define SDL_JOYSTICK_BUTTON_START 11
#define SDL_JOYSTICK_BUTTON_SELECT 12
#define SDL_JOYSTICK_BUTTON_LEFT 7
#define SDL_JOYSTICK_BUTTON_RIGHT 9
#define SDL_JOYSTICK_BUTTON_UP 8
#define SDL_JOYSTICK_BUTTON_DOWN 6
#define JOYSTICK_DEAD_ZONE 8000

#define SDL_BTN_LEFT 1073741904
#define SDL_BTN_RIGHT 1073741903
#define SDL_BTN_UP 1073741906
#define SDL_BTN_DOWN 1073741905

int sdlButtonToKeyboardButton(int btn) {
  switch (btn) {
  case SDL_JOYSTICK_BUTTON_CROSS:
    return 32; // space-bar
  case SDL_JOYSTICK_BUTTON_CIRCLE:
    return 1073742049; // shift
  case SDL_JOYSTICK_BUTTON_SQUARE:
    return 122;
  case SDL_JOYSTICK_BUTTON_TRIANGLE:
    return -1;
  case SDL_JOYSTICK_BUTTON_LEFT:
    return SDL_BTN_LEFT;
  case SDL_JOYSTICK_BUTTON_RIGHT:
    return SDL_BTN_RIGHT;
  case SDL_JOYSTICK_BUTTON_UP:
    return SDL_BTN_UP;
  case SDL_JOYSTICK_BUTTON_DOWN:
    return SDL_BTN_DOWN;
  case SDL_JOYSTICK_BUTTON_SELECT:
    return -1;
  case SDL_JOYSTICK_BUTTON_START:
    return 13; // enter
  default:
    return 0;
  }
}
#endif

namespace sdl2w {

const std::string SDL2_WRAPPER_WINDOW_ERR = "SDL2_WRAPPER_WINDOW_ERR";

Uint64 Window::now = 0;
bool Window::soundEnabled = true;
int Window::soundPercent = 100;
bool Window::soundCanBeLoaded = true;
const double Window::targetFrameMS = 16.0;
Window* Window::globalWindow = nullptr;
bool Window::soundForcedDisabled = false;

void windowThrowError(const std::string& errorMessage) {
  Logger().get(ERROR) << errorMessage;
  throw std::string(errorMessage);
}

Window::Window()
    : events(*this),
      currentFontSize(18),
      deltaTime(0),
      globalAlpha(255),
      colorkey(0) {
  // Window::instanceCount++;
  firstLoop = true;
  Window::soundEnabled = true;
  Window::globalWindow = this;
  fps = 60;
  countedFrames = 0;
  height = 512;
  width = 512;
  renderWidth = 512;
  renderHeight = 512;
  firstLoop = true;
  onresize = nullptr;
  soundEnabled = true;
  colorkey = 0x00FFFFFF;
}
Window::Window(const std::string& title,
               const int widthA,
               const int heightA,
               const int windowPosX,
               const int windowPosY)
    : events(*this),
      currentFontSize(18),
      deltaTime(0),
      globalAlpha(255),
      colorkey(0) {
  firstLoop = true;
  Window::soundEnabled = true;
  Window::globalWindow = this;
  fps = 60;
  countedFrames = 0;
  height = widthA;
  width = heightA;
  renderWidth = widthA;
  renderHeight = heightA;
  firstLoop = true;
  onresize = nullptr;
  soundEnabled = true;
  colorkey = 0x00FFFFFF;
  createWindow(title, widthA, heightA, windowPosX, windowPosY);
  Logger().get(INFO) << "sdl2w Window initialized" << Logger::endl;
}

Window::~Window() {
  Store::clear();
  Mix_Quit();
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
  if (intermediate != nullptr) {
    SDL_DestroyTexture(intermediate);
  }
  Logger().get(INFO) << "sdl2w Window removed" << Logger::endl;
}

void Window::disableStaticSound() {
  Window::soundForcedDisabled = true;
  Window::soundCanBeLoaded = false;
}

Window& Window::getGlobalWindow() { return *Window::globalWindow; }

void Window::createWindow(const std::string& title,
                          const int w,
                          const int h,
                          const int xPos,
                          const int yPos) {
  // SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS |
  //          SDL_INIT_GAMECONTROLLER);
  SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO |
           SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS);
  TTF_Init();
  colorkey = 0x00FFFFFF;
  width = w;
  height = h;

  Logger().printf("[SDL] Init window\n");
#ifdef MIYOOA30
  window = std::unique_ptr<SDL_Window, SDL_Deleter>(
      SDL_CreateWindow(title.c_str(),
                       SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED,
                       width,
                       height,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN |
                           SDL_WINDOW_OPENGL),
      SDL_Deleter());
#else
  window = std::unique_ptr<SDL_Window, SDL_Deleter>(
      SDL_CreateWindow(
          title.c_str(), xPos, yPos, width, height, SDL_WINDOW_SHOWN),
      SDL_Deleter());
#endif
  if (window == NULL) {
    windowThrowError("Window could not be created! SDL Error: " +
                     std::string(SDL_GetError()));
    throw std::runtime_error(SDL2_WRAPPER_WINDOW_ERR);
  }
  Logger().printf("[SDL] Init sound\n");
  if (!soundForcedDisabled) {
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 2048) < 0) {
      Logger().get(ERROR) << "SDL_mixer could not initialize! "
                          << std::string(Mix_GetError()) << Logger::endl;
      soundForcedDisabled = true;
      Window::soundCanBeLoaded = false;
    }
    Mix_AllocateChannels(16);
  } else {
    Logger().printf("Warning: Sound forced disabled!\n");
  }

  Logger().printf("[SDL] Init joysticks\n");
  if (SDL_NumJoysticks() < 1) {
    Logger().printf("Warning: No joysticks connected!\n");
    joystick = nullptr;
  } else {
    joystick = std::unique_ptr<SDL_Joystick, SDL_Deleter>(SDL_JoystickOpen(0),
                                                          SDL_Deleter());
    if (joystick == NULL) {
      joystick = nullptr;
      Logger().printf(
          "[SDL] Warning: Unable to open game controller! SDL Error: %s\n",
          SDL_GetError());
    }
  }

  Logger().printf("[SDL] Create renderer\n");
#ifdef __EMSCRIPTEN__
  EM_ASM({
    if (!FS.analyzePath('/sdl2wdata').exists) {
      FS.mkdir('/sdl2wdata');
    }
    FS.mount(IDBFS, {}, '/sdl2wdata');
    FS.syncfs(
        true, function(err) {
          if (err)
            console.error("Initial FS sync error:", err);
          else
            console.log("Initial FS sync complete.");
        });
  });

  renderer = std::unique_ptr<SDL_Renderer, SDL_Deleter>(
      SDL_CreateRenderer(window.get(),
                         -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |
                             SDL_RENDERER_TARGETTEXTURE),
      SDL_Deleter());
#elif MIYOOA30
  renderer = std::unique_ptr<SDL_Renderer, SDL_Deleter>(
      SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED),
      SDL_Deleter());
  SDL_RenderSetScale(renderer.get(), 1.0, (float)width / (float)height);
  useIntermediateRenderTarget = true;
  renderRotationAngle = -90.0;
#else
  renderer = std::unique_ptr<SDL_Renderer, SDL_Deleter>(
      SDL_CreateRenderer(window.get(),
                         -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
      SDL_Deleter());
#endif
  useIntermediateRenderTarget = true;
  Uint32 format = SDL_GetWindowPixelFormat(window.get());
  intermediate = SDL_CreateTexture(
      renderer.get(), format, SDL_TEXTUREACCESS_TARGET, width, height);
  if (intermediate == nullptr) {
    windowThrowError(
        "intermediate render target could not be created! SDL Error: " +
        std::string(SDL_GetError()));
    throw std::runtime_error(SDL2_WRAPPER_WINDOW_ERR);
  }
  SDL_SetRenderDrawColor(renderer.get(), 0x11, 0x11, 0xFF, 0xFF);

  SDL_SetRenderTarget(renderer.get(),
                      useIntermediateRenderTarget ? intermediate : nullptr);

  Store::setRenderer(renderer);
}

Events& Window::getEvents() { return events; }

SDL_Renderer* Window::getRenderer() { return renderer.get(); }

SDL_Window* Window::getSDLWindow() { return window.get(); }

void Window::setBackgroundColor(const SDL_Color& color) {
  backgroundColor = color;
  SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);
}

void Window::setCurrentFont(const std::string& fontName, const int sz) {
  currentFontName = fontName;
  currentFontSize = sz;
}
const std::string& Window::getCurrentFontName() const {
  return currentFontName;
}
int Window::getCurrentFontSize() const { return currentFontSize; }
Uint64 Window::staticGetNow() { return Window::now; }
double Window::getNow() {
  return static_cast<double>(SDL_GetPerformanceCounter());
}
double Window::getDeltaTime() const { return deltaTime; }
int Window::getDeltaTimeMs() const { return static_cast<int>(deltaTime); }
double Window::getAvgFrameTime() const {
  double sum = 0;
  for (const double r : pastFrameTimes) {
    sum += r;
  }
  return sum / static_cast<double>(pastFrameTimes.size());
}
int Window::getAvgFps() const {
  double sum = 0;
  for (const double r : pastFrameTimes) {
    sum += r;
  }
  return static_cast<int>(1000.0 /
                          (sum / static_cast<double>(pastFrameTimes.size())));
}
void Window::setAnimationFromDefinition(const std::string& name,
                                        Animation& anim) {
  auto& a = Store::getAnimationDefinition(name);
  anim.sprites.clear();
  for (const auto& spriteAnim : a.sprites) {
    anim.addSprite(spriteAnim.name, spriteAnim.duration, spriteAnim.flip);
  }
  anim.name = a.name;
  anim.totalDuration = a.totalDuration;
  anim.loop = a.loop;
}

void Window::disableSound() { Window::soundEnabled = false; }
void Window::enableSound() { Window::soundEnabled = true; }
void Window::playSound(const std::string& name) {
  if (soundForcedDisabled || !soundEnabled || !Window::soundCanBeLoaded) {
    return;
  }

  Mix_Chunk* sound = Store::getSound(name);
  const int channel = Mix_PlayChannel(-1, sound, 0);
  if (channel == -1) {
    Logger().get(WARN) << "Unable to play sound in channel.  sound=" << name
                       << " err=" << SDL_GetError() << Logger::endl;
    return;
  }
  Mix_Volume(channel,
             static_cast<int>(double(Window::soundPercent) / 100.0 *
                              double(MIX_MAX_VOLUME)));
  soundChannels[name] = channel;
}
void Window::stopSound(const std::string& name) {
  if (soundChannels.find(name) != soundChannels.end()) {
    const int channel = soundChannels[name];
    Mix_HaltChannel(channel);
  }
}
void Window::playMusic(const std::string& name) {
  if (soundForcedDisabled || !Window::soundCanBeLoaded) {
    return;
  }

  Mix_Music* music = Store::getMusic(name);
  if (Mix_PlayingMusic()) {
    stopMusic();
  }
  Mix_PlayMusic(music, -1);
}
void Window::stopMusic() {
  if (Mix_PlayingMusic()) {
    Mix_HaltMusic();
  }
}
void Window::setVolume(double volumePct) {
  sdl2w::Window::soundPercent = volumePct;
  Mix_VolumeMusic((double(volumePct) / 100.0) * double(MIX_MAX_VOLUME));
  Mix_Volume(-1, (double(volumePct) / 100.0) * double(MIX_MAX_VOLUME));
}

SDL_Texture* Window::getTextTexture(const std::string& text,
                                    const int sz,
                                    const SDL_Color& color) {
  if (!currentFontName.size()) {
    windowThrowError("No font has been set.");
    throw std::runtime_error("");
  }

  std::stringstream keyStream;
  keyStream << text << sz << currentFontName << color.r << color.g << color.b;
  const std::string key = keyStream.str();
  SDL_Texture* tex = Store::getTextTexture(key);
  if (tex) {
    return tex;
  } else {
    TTF_Font* font = Store::getFont(currentFontName, sz);
    SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* texPtr = SDL_CreateTextureFromSurface(renderer.get(), surf);
    SDL_FreeSurface(surf);
    Store::storeTextTexture(key, texPtr);
    return Store::getTextTexture(key);
  }
}

SDL_Texture*
Window::getStaticColorTexture(int width, int height, const SDL_Color& color) {
  std::stringstream ss;
  ss << width << "," << height << "," << color.r << "," << color.g << ","
     << color.b << "," << color.a;
  auto textureName = ss.str();
  if (Store::textureExists(textureName)) {
    return Store::getTexture(ss.str());
  }

  auto texture = SDL_CreateTexture(renderer.get(),
                                   SDL_PIXELFORMAT_RGBA32,
                                   SDL_TEXTUREACCESS_TARGET,
                                   width,
                                   height);

  SDL_SetRenderTarget(renderer.get(), texture);
  SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);
  SDL_RenderClear(renderer.get());
  SDL_SetRenderTarget(renderer.get(),
                      useIntermediateRenderTarget ? intermediate : nullptr);
  setBackgroundColor(backgroundColor);
  sdl2w::Store::storeTexture(textureName, texture);

  return texture;
}

SDL_Texture* Window::getStreamTexture(const std::string& id, int w, int h) {
  if (Store::textureExists(id)) {
    return Store::getTexture(id);
  }
  auto texture = SDL_CreateTexture(renderer.get(),
                                   SDL_PIXELFORMAT_RGBA32,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   w,
                                   h);
  sdl2w::Store::storeTexture(id, texture);
  return texture;
}

void Window::updateDrawStreamTexture(const std::string& id,
                                     const void* pixels,
                                     int pitch) {
  auto tex = getStreamTexture(id, renderWidth, renderHeight);
  SDL_UpdateTexture(tex, NULL, pixels, pitch);
  SDL_RenderCopy(renderer.get(), tex, NULL, NULL);
}

void Window::drawTexture(SDL_Texture* tex,
                         int x,
                         int y,
                         int w,
                         int h,
                         bool centered,
                         double angleDeg,
                         std::pair<double, double> scale) {
  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(tex, globalAlpha);

  SDL_RendererFlip flip = SDL_FLIP_NONE;

  std::pair<double, double> scaleLocal = scale;

  // HACK setting x scale to negative value will flip sprite
  // horizontally.
  if (scale.first < 0) {
    scaleLocal.first = abs(scale.first);
    flip = SDL_FLIP_HORIZONTAL;
  }

  const double scaledX = static_cast<double>(w) * scaleLocal.first;
  const double scaledY = static_cast<double>(h) * scaleLocal.second;
  const double scaledW = scaleLocal.first * w;
  const double scaledH = scaleLocal.second * h;
  const SDL_Rect pos = {x + static_cast<int>(centered ? -scaledW / 2. : 0.),
                        y + static_cast<int>(centered ? -scaledH / 2. : 0.),
                        static_cast<int>(floor(scaledX)),
                        static_cast<int>(floor(scaledY))};
  const SDL_Rect clip = {0, 0, w, h};
  SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);
  SDL_RenderCopyEx(renderer.get(), tex, &clip, &pos, angleDeg, NULL, flip);
}

void Window::drawSprite(const std::string& name,
                        int x,
                        int y,
                        bool centered,
                        double angleDeg,
                        std::pair<double, double> scale) {
  const Sprite& sprite = Store::getSprite(name);
  SDL_Texture* tex = sprite.image;
  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(tex, globalAlpha);

  SDL_RendererFlip flip = SDL_FLIP_NONE;

  std::pair<double, double> scaleLocal = scale;

  // HACK setting x scale to negative value will flip sprite
  // horizontally.
  if (scale.first < 0.) {
    scaleLocal.first = abs(scale.first);
    flip = SDL_FLIP_HORIZONTAL;
  }

  const double scaledX = static_cast<double>(sprite.cw) * scaleLocal.first;
  const double scaledY = static_cast<double>(sprite.ch) * scaleLocal.second;
  const double scaledW = scaleLocal.first * sprite.cw;
  const double scaledH = scaleLocal.second * sprite.ch;
  const SDL_Rect pos = {x + static_cast<int>(centered ? -scaledW / 2. : 0.),
                        y + static_cast<int>(centered ? -scaledH / 2. : 0.),
                        static_cast<int>(floor(scaledX)),
                        static_cast<int>(floor(scaledY))};
  const SDL_Rect clip = {sprite.cx, sprite.cy, sprite.cw, sprite.ch};
  SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);
  SDL_RenderCopyEx(renderer.get(), tex, &clip, &pos, angleDeg, NULL, flip);
}

void Window::drawAnimation(Animation& anim,
                           int x,
                           int y,
                           bool centered,
                           bool updateAnim,
                           double angle,
                           std::pair<double, double> scale) {
  if (anim.isInitialized()) {
    const AnimSprite& sprite = anim.getCurrentSprite();
    if (sprite.flip) {
      scale.first = -scale.first;
    }
    drawSprite(sprite.name, x, y, centered, angle, scale);
    if (updateAnim) {
      anim.update();
    }
  } else {
    windowThrowError("Anim has not been initialized: '" + anim.toString() +
                     "'");
    throw std::runtime_error(SDL2_WRAPPER_WINDOW_ERR);
  }
}

void Window::drawText(const std::string& text,
                      int x,
                      int y,
                      const SDL_Color& color) {
  SDL_Texture* tex = getTextTexture(text, currentFontSize, color);

  int w, h;
  SDL_QueryTexture(tex, NULL, NULL, &(w), &(h));
  SDL_SetTextureAlphaMod(tex, globalAlpha);
  const SDL_Rect pos = {x, y, w, h};
  SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);
  SDL_RenderCopy(renderer.get(), tex, NULL, &pos);
}

void Window::drawTextCentered(const std::string& text,
                              int x,
                              int y,
                              const SDL_Color& color) {
  SDL_Texture* tex = getTextTexture(text, currentFontSize, color);

  int w, h;
  SDL_QueryTexture(tex, NULL, NULL, &(w), &(h));
  SDL_SetTextureAlphaMod(tex, globalAlpha);
  const SDL_Rect pos = {x - w / 2, y - h / 2, w, h};
  SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);
  SDL_RenderCopy(renderer.get(), tex, NULL, &pos);
}

std::pair<int, int>
Window::measureTextWithCurrentFont(const std::string& text) {
  int w = 0;
  int h = 0;
  TTF_Font* font = Store::getFont(currentFontName, currentFontSize);
  TTF_SizeUTF8(font, text.c_str(), &w, &h);
  return std::make_pair(w, h);
}

void Window::handleExternalEvents(
    std::map<int, std::function<void(int)>> handlers) {
  for (std::pair<int, int> event : externalEvents) {
    int eventInt = event.first;
    int eventPayload = event.second;
    auto it = handlers.find(eventInt);
    if (it != handlers.end()) {
      it->second(eventPayload);
    }
  }
  externalEvents.clear();
}

void Window::renderLoop() {
  const Uint64 div = 1000;
  const Uint64 nowMicroSeconds = SDL_GetPerformanceCounter();
  auto freq = SDL_GetPerformanceFrequency();
  now = (nowMicroSeconds * div) / freq;

  if (!static_cast<bool>(freq)) {
    freq = 1;
  }
  if (firstLoop) {
    deltaTime = 16.6666;
    firstLoop = false;
    pastFrameTimes.push_back(deltaTime);
  } else {
    deltaTime = static_cast<double>((nowMicroSeconds - lastFrameTime) * div) /
                static_cast<double>(freq);
  }

  lastFrameTime = nowMicroSeconds;
  pastFrameTimes.push_back(deltaTime);
  if (pastFrameTimes.size() > 10) {
    pastFrameTimes.pop_front();
  }

  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
#ifdef __EMSCRIPTEN__
    if (e.type == SDL_QUIT) {
      Logger().get(WARN) << "QUIT is overridden in EMSCRIPTEN" << Logger::endl;
      break;
    }
#else
    if (e.type == SDL_QUIT) {
      isLooping = false;
      break;
    } else if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
      break;
    } else if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
      break;
    }
#endif

#ifdef __vita__
    if (e.type == SDL_JOYBUTTONDOWN) {
      if (isInputEnabled) {
        int keyValue = e.jbutton.button;
        int buttonValue = sdlButtonToKeyboardButton(keyValue);
        if (buttonValue > 0) {
          events.keydown(buttonValue);
        }
      }
    } else if (e.type == SDL_JOYBUTTONUP) {
      if (isInputEnabled) {
        int keyValue = e.jbutton.button;
        int buttonValue = sdlButtonToKeyboardButton(keyValue);
        if (buttonValue > 0) {
          events.keyup(buttonValue);
        }
      }
    } else if (e.type == SDL_JOYAXISMOTION) {
      if (isInputEnabled) {
        // Motion on controller 0
        if (e.jaxis.which == 0) {
          // X axis motion
          if (e.jaxis.axis == 0) {
            if (e.jaxis.value < -JOYSTICK_DEAD_ZONE) {
              if (deadZoneXDir != -1) {
                events.keydown(SDL_BTN_LEFT);
                deadZoneXDir = -1;
              }
            } else if (e.jaxis.value > JOYSTICK_DEAD_ZONE) {
              if (deadZoneXDir != 1) {
                events.keydown(SDL_BTN_RIGHT);
                deadZoneXDir = 1;
              }
            } else {
              if (deadZoneXDir != 0) {
                if (events.isKeyPressed("Left")) {
                  events.keyup(SDL_BTN_LEFT);
                }
                if (events.isKeyPressed("Right")) {
                  events.keyup(SDL_BTN_RIGHT);
                }
                deadZoneXDir = 0;
              }
            }
          } // Y axis motion
          else if (e.jaxis.axis == 1) {
            if (e.jaxis.value < -JOYSTICK_DEAD_ZONE) {
              if (deadZoneYDir != 1) {
                events.keydown(SDL_BTN_DOWN);
                deadZoneYDir = 1;
              }
            } else if (e.jaxis.value > JOYSTICK_DEAD_ZONE) {
              if (deadZoneXDir != -1) {
                events.keydown(SDL_BTN_UP);
                deadZoneYDir = -1;
              }
            } else {
              if (deadZoneYDir != 0) {
                if (events.isKeyPressed("Down")) {
                  events.keyup(SDL_BTN_DOWN);
                }
                if (events.isKeyPressed("Up")) {
                  events.keyup(SDL_BTN_UP);
                }
                deadZoneYDir = 0;
              }
            }
          }
        }
      }
    }
#else
    else if (e.type == SDL_KEYDOWN) {
      if (isInputEnabled) {
        events.keydown(e.key.keysym.sym);
      }
    } else if (e.type == SDL_KEYUP) {
      if (isInputEnabled) {
        events.keyup(e.key.keysym.sym);
      }
    } else if (e.type == SDL_MOUSEMOTION) {
      if (isInputEnabled) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        events.mousemove(x, y);
        mousePos = std::make_pair(x, y);
      }
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
      if (isInputEnabled) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        events.mousedown(x, y, static_cast<int>(e.button.button));
      }
    } else if (e.type == SDL_MOUSEBUTTONUP) {
      if (isInputEnabled) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        events.mouseup(x, y, static_cast<int>(e.button.button));
      }
    }
    if (e.type == SDL_MOUSEWHEEL) {
      events.wheel = e.wheel.y;
    } else {
      events.wheel = 0;
    }
    events.handleEvent(e);
#endif
  }
  if (!isLooping) {
    return;
  }

  SDL_RenderClear(renderer.get());
  if (useIntermediateRenderTarget) {
    isLooping = renderCb();
    events.update();
    SDL_SetRenderTarget(renderer.get(), nullptr);
    SDL_RenderClear(renderer.get());
    SDL_RenderCopyEx(renderer.get(),
                     intermediate,
                     nullptr,
                     nullptr,
                     renderRotationAngle,
                     nullptr,
                     SDL_FLIP_NONE);
    SDL_RenderPresent(renderer.get());
    SDL_SetRenderTarget(renderer.get(), intermediate);
  } else {
    isLooping = renderCb();
    events.update();
    SDL_RenderPresent(renderer.get());
  }
  firstLoop = false;
}

#ifdef __EMSCRIPTEN__
void RenderLoopCallback(void* arg) { static_cast<Window*>(arg)->renderLoop(); }
#endif
void Window::startRenderLoop(std::function<bool(void)> cb) {
  firstLoop = true;
  renderCb = cb;
  isLooping = true;
  Window::now = SDL_GetPerformanceCounter();

#ifdef __EMSCRIPTEN__
  // Receives a function to call and some user data to provide it.
  emscripten_set_main_loop_arg(&RenderLoopCallback, this, -1, 1);
#else
  while (isLooping) {
    if (shouldRender) {
      renderLoop();
    } else {
      SDL_Delay(16);
    }
  }
#endif
}

void Window::timedLoop() {
  const Uint64 div = 1000;
  const Uint64 nowMicroSeconds = SDL_GetPerformanceCounter();
  auto freq = SDL_GetPerformanceFrequency();
  now = (nowMicroSeconds * div) / freq;

  if (!static_cast<bool>(freq)) {
    freq = 1;
  }
  if (firstLoop) {
    deltaTime = 16.6666;
    firstLoop = false;
    pastFrameTimes.push_back(deltaTime);
  } else {
    deltaTime = static_cast<double>((nowMicroSeconds - lastFrameTime) * div) /
                static_cast<double>(freq);
  }

  lastFrameTime = nowMicroSeconds;
  pastFrameTimes.push_back(deltaTime);
  if (pastFrameTimes.size() > 10) {
    pastFrameTimes.pop_front();
  }

  isLooping = renderCb();
  events.update();

  if (!isLooping) {
    return;
  }
}

void Window::startTimedLoop(std::function<bool(void)> cb, int ms) {
  firstLoop = true;
  renderCb = cb;
  isLooping = true;
  Window::now = SDL_GetPerformanceCounter();

  while (isLooping) {
    timedLoop();
    SDL_Delay(static_cast<Uint32>(ms));
  }
}

} // namespace sdl2w