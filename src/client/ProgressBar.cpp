#include "ProgressBar.h"
#include "lib/sdl2wrapper/Window.h"

namespace program {
namespace ui {
void ProgressBar::setProps(sdl2w::Window& window,
                           const ProgressProps& props) {
  this->props = props;
  bgRect = window.getStaticColorTexture(
      props.layout.width, props.layout.height, props.bgColor);
  fgRect = window.getStaticColorTexture(
      props.layout.width, props.layout.height, props.fgColor);
}

void ProgressBar::draw(sdl2w::Window& window) {
  if (bgRect == nullptr || fgRect == nullptr) {
    return;
  }

  const int borderSize = 2;

  window.drawTexture(bgRect,
                     props.layout.x,
                     props.layout.y,
                     props.layout.width,
                     props.layout.height,
                     false);
  window.drawTexture(
      fgRect,
      props.layout.x + borderSize,
      props.layout.y + borderSize,
      static_cast<int>(static_cast<double>(props.layout.width - borderSize * 2) *
                       pct),
      props.layout.height - borderSize * 2,
      false);
}
} // namespace ui
} // namespace program