#pragma once

#include "client/UiComponentDef.h"

namespace program {
namespace ui {

struct ProgressProps {
  LayoutProps layout;
  SDL_Color bgColor;
  SDL_Color fgColor;
};

class ProgressBar {
  SDL_Texture* bgRect = nullptr;
  SDL_Texture* fgRect = nullptr;

  ProgressProps props;

public:
  double pct = 0.;

  ProgressBar() {}

  void setProps(sdl2w::Window& window, const ProgressProps& props);
  const ProgressProps& getProps() const { return props; }

  void draw(sdl2w::Window& window);
};
} // namespace ui
} // namespace program