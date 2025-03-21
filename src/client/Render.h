#pragma once

#include "client/Ui.h"
#include "game/state.h"

namespace sdl2w {
class Window;
}

namespace program {
class Render {
  sdl2w::Window& window;
  ui::Ui ui;
  std::vector<int> background;;

  int headingToShipSpriteIndex(double heading) const;

public:
  Render(sdl2w::Window& windowA);
  void setup();

  void renderUi(const State& state);

  // can't be const because rendering an anim alters it
  void renderAsteroid(Asteroid& asteroid);
  void renderSaucer(Saucer& saucer);
  void renderHomingMine(HomingMine& homingMine);
  void renderBullet(Bullet& bullet);
  void renderPowerup(Powerup& powerup);
  void renderShip(Ship& ship);
  void renderAnimatedParticle(AnimatedParticle& particle);
  void renderStars();
};
} // namespace program