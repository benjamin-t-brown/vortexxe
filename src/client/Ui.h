#pragma once

#include "client/ProgressBar.h"
#include "client/UiComponentDef.h"
#include "game/state.h"
#include "lib/sdl2wrapper/Animation.h"

namespace sdl2w {
class Window;
}

namespace program {

namespace ui {

class Ui {
  sdl2w::Window& window;

  ProgressBar shieldBar;

  std::string shieldText;

  std::vector<sdl2w::Animation> shootThese;
  std::vector<sdl2w::Animation> dontShootThese;

  LayoutProps scoreTextLayout;
  LayoutProps bonusTextLayout;
  LayoutProps shieldBarTextLayout;
  LayoutProps livesTextLayout;
  LayoutProps levelTextLayout;

  LayoutProps menuTitleTextLayout;
  LayoutProps menuSubtitleTextLayout;
  LayoutProps menuLastScoreTextLayout;
  LayoutProps menuInstructionsShootTheseLayout;
  LayoutProps menuInstructionsDontShootTheseLayout;

  SDL_Texture* bgRect;

public:
  Ui(sdl2w::Window& window);
  void setup();

  void drawLevelUi(const State& state);

  void drawPostLevelUi(const State& state);

  void drawGameOverUi();

  void drawMenuUi(const State& state);
};
} // namespace ui
} // namespace program
