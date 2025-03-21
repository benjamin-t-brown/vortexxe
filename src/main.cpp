#include "game/GameManager.h"
#include "game/state.h"
#include "game/utils/StructTimer.hpp"
#include "lib/sdl2wrapper/Localization.h"
#include "lib/sdl2wrapper/Store.h"
#include "lib/sdl2wrapper/Window.h"
#include "logger.h"

void renderLoadingScreen(sdl2w::Window& window) {
  const std::vector<std::string> lines = {
      //
      "                                _(_)_ ",
      "                               (_)@(_)",
      "                                 (_)  ",
      " _                                |   ",
      "|_)  _  ._  o  _. ._ _  o ._    (\\|/) ",
      "|_) (/_ | | | (_| | | | | | |    \\|/  ",
      "^^^ ^^^^^^ _| ^^^^ ^^^  ^^^^^^ ^^^^^^ ",
      //
  };
  auto windowWidth = window.renderWidth;
  auto windowHeight = window.renderHeight;
  const double x = (windowWidth / 2.f);
  const double y = (windowHeight / 2.f);
  window.setBackgroundColor({16, 30, 41});
  window.setCurrentFont("monofonto", 16);
  for (int i = 0; i < static_cast<int>(lines.size()); i++) {
    window.drawTextCentered(lines[i], x, y + i * 20 - 100, {244, 126, 27});
  }
  window.setCurrentFont("monofonto", 24);
  window.drawTextCentered("Have fun!", x, y + 50, {255, 255, 255});
}

void setLanguage(int argc, char* argv[]) {
  std::string langArg = "en";
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--language" && (i + 1) < argc) {
      langArg = argv[++i];
    }
  }
  sdl2w::L10n::setLanguage(langArg);
}

int main(int argc, char* argv[]) {
#ifndef __EMSCRIPTEN__
  Logger::setLogToFile(true);
#endif
  logger::info("Start program");
  srand(time(NULL));
  try {
    sdl2w::Window window(
        "VortexXe", program::WINDOW_WIDTH, program::WINDOW_HEIGHT, 25, 50);
    window.setBackgroundColor({0, 0, 0});
    auto& events = window.getEvents();

    sdl2w::Store::createFont("monofonto", "assets/monofonto.ttf");
    sdl2w::Store::createFont("default", "assets/monofonto.ttf");
    window.setCurrentFont("default", 18);

#ifdef __EMSCRIPTEN__
    renderLoadingScreen(window);
#else
    program::StructTimer loadingTimer = {1000, 0};
    window.startRenderLoop([&]() {
      renderLoadingScreen(window);
      program::structTimerUpdate(loadingTimer, std::min(window.getDeltaTimeMs(), 100));
      return !structTimerIsComplete(loadingTimer);
    });
#endif

    program::GameManager game = program::GameManager(window);
    game.load();
    window.setVolume(33);
    sdl2w::L10n::init();
    setLanguage(argc, argv);

    // sdl2w::Store::logSprites();

    events.setKeyboardEvent("keypress",
                            [&](const std::string& key) { game.handleKeyPress(key); });

    logger::info("Start loop");
    game.start();
    window.startRenderLoop([&]() {
      window.setBackgroundColor({10, 10, 10});
      window.setCurrentFont("default", 18);

#ifndef __EMSCRIPTEN__
      if (events.isKeyPressed("Escape")) {
        return false;
      }
#endif

      game.update(std::min(window.getDeltaTimeMs(), 100));
      game.render();
      return true;
    });

  } catch (const std::string& e) {
    Logger().get(LogType::ERROR) << e << Logger::endl;
  }

  logger::info("End program");
}