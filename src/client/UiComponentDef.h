#pragma once

#include "lib/sdl2wrapper/Window.h"

struct SDL_Texture;

namespace sdl2w {
class Window;
} // namespace sdl2w

namespace program {
namespace ui {

struct LayoutProps {
  int x = 0;
  int y = 0;
  int width = 100;
  int height = 22;
};

} // namespace ui
} // namespace program