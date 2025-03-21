#pragma once

#include "Animation.h"
#include "Events.h"
#include "SDL2Includes.h"
#include <deque>
#include <map>
#include <memory>
#include <string>
#ifdef MIYOOA30
#include <SDL_pixels.h>
#else
#include <SDL2/SDL_pixels.h>
#endif

struct SDL_Window;
struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Color;

namespace sdl2w {

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

class Window {
  std::unique_ptr<SDL_Window, SDL_Deleter> window;
  std::unique_ptr<SDL_Renderer, SDL_Deleter> renderer;
  std::unique_ptr<SDL_Joystick, SDL_Deleter> joystick;
  std::function<bool(void)> renderCb;
  void (*onresize)(void);
  Events events;

  SDL_Texture*
  getTextTexture(const std::string& text, const int sz, const SDL_Color& color);
  SDL_Texture* getEmptyTexture(int w, int h);

  void onResize(int w, int h);
  void clear();
  void swap();
  void createWindow(const std::string& title,
                    const int w,
                    const int h,
                    const int xPos,
                    const int yPos);

  std::string currentFontName = "default";
  int currentFontSize = 18;
  double deltaTime = 0.;
  bool isLooping = false;
  bool firstLoop = false;
  uint64_t lastFrameTime = 0;
  bool shouldRender = true;
  std::deque<double> pastFrameTimes;
  int deadZoneXDir = 0;
  int deadZoneYDir = 0;
  bool useIntermediateRenderTarget = false;
  SDL_Texture* intermediate = nullptr;
  double renderRotationAngle = 0.0;

public:
  int width = 800;
  int height = 600;
  int renderWidth = 800;
  int renderHeight = 600;
  int countedFrames = 0;
  int fps = 0;
  int globalAlpha = 255;
  uint32_t colorkey = 0;
  bool isInputEnabled = true;
  std::map<std::string, int> soundChannels;
  std::vector<std::pair<int, int>> externalEvents;
  std::pair<int, int> mousePos;
  SDL_Color backgroundColor = {0, 0, 0, 255};

  static bool soundForcedDisabled;
  static uint64_t now;
  static const double targetFrameMS;
  static bool soundEnabled;
  static int soundPercent;
  static bool soundCanBeLoaded;
  static Window* globalWindow;
  static Window& getGlobalWindow();

  Window();
  Window(const std::string& title,
         const int widthA,
         const int heightA,
         const int windowPosX = -1,
         const int windowPosY = -1);
  ~Window();

  static void disableStaticSound();

  Events& getEvents();
  SDL_Renderer* getRenderer();
  SDL_Window* getSDLWindow();
  void setLogicalSize(int w, int h);
  void setCurrentFont(const std::string& fontName, const int sz);
  const std::string& getCurrentFontName() const;
  int getCurrentFontSize() const;
  SDL_Texture*
  getStaticColorTexture(int width, int height, const SDL_Color& color);
  SDL_Texture* getStreamTexture(const std::string& id, int width, int height);
  void
  updateDrawStreamTexture(const std::string& id, const void* pixels, int pitch);
  static uint64_t staticGetNow();
  static double getNow();
  double getDeltaTime() const;
  int getDeltaTimeMs() const;
  double getAvgFrameTime() const;
  int getAvgFps() const;

  static void setAnimationFromDefinition(const std::string& name,
                                         Animation& anim);

  void disableSound();
  void enableSound();
  void playSound(const std::string& name);
  void stopSound(const std::string& name);
  void playMusic(const std::string& name);
  void stopMusic();
  // between 0 and 100
  void setVolume(double volumePct);

  void setBackgroundColor(const SDL_Color& color);

  void drawTexture(SDL_Texture* tex,
                   int x,
                   int y,
                   int w,
                   int h,
                   bool centered = true,
                   double angleDeg = 0.,
                   std::pair<double, double> scale = std::make_pair(1.0, 1.0));
  void drawSprite(const std::string& name,
                  int x,
                  int y,
                  bool centered = true,
                  double angleDeg = 0.,
                  std::pair<double, double> scale = std::make_pair(1.0, 1.0));
  void drawAnimation(Animation& anim,
                     int x,
                     int y,
                     bool centered = true,
                     bool updateAnim = true,
                     double angleDeg = 0,
                     std::pair<double, double> scale = std::make_pair(1.0,
                                                                      1.0));
  void drawText(const std::string& text, int x, int y, const SDL_Color& color);
  void drawTextCentered(const std::string& text,
                        int x,
                        int y,
                        const SDL_Color& color);
  std::pair<int, int> measureTextWithCurrentFont(const std::string& text);

  void handleExternalEvents(std::map<int, std::function<void(int)>> handlers);
  void renderLoop();
  void startRenderLoop(std::function<bool(void)> cb);

  void timedLoop();
  void startTimedLoop(std::function<bool(void)> cb, int ms);
};

} // namespace sdl2w