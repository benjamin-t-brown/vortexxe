#include "Render.h"
#include "game/state.h"
#include "game/utils/StructGauge.hpp"
#include "lib/sdl2wrapper/Localization.h"
#include "lib/sdl2wrapper/Window.h"
#include <sstream>

namespace program {

Render::Render(sdl2w::Window& windowA) : window(windowA), ui(windowA) {}

void Render::setup() {
  ui.setup();
  int tileWidth = WINDOW_WIDTH / 32;
  int tileHeight = WINDOW_HEIGHT / 32;
  int ctr = 0;
  for (int i = 0; i < tileHeight; i++) {
    for (int j = 0; j < tileWidth; j++) {
      ctr++;
      background.push_back((rand()) % 4);
    }
  }
}

void Render::renderUi(const State& state) {
  if (state.inputState == InputState::PRE_LEVEL) {
    window.setCurrentFont("default", 18);
    window.drawTextCentered(sdl2w::L10n::trans(LOCSTR("Press to start")),
                            WINDOW_WIDTH / 2,
                            WINDOW_HEIGHT / 2 - 64,
                            {255, 255, 255});
  }

  if (state.inputState == InputState::POST_LEVEL_WAITING_FOR_END_TRANSITION) {
    window.setCurrentFont("default", 18);
    window.drawTextCentered(sdl2w::L10n::trans(LOCSTR("Level completed")),
                            WINDOW_WIDTH / 2,
                            WINDOW_HEIGHT / 2,
                            {0, 255, 0});
  }

  if (state.inputState == InputState::POST_LEVEL_WAITING_FOR_SCORE_TO_SHOW ||
      state.inputState == InputState::POST_LEVEL) {
    ui.drawPostLevelUi(state);
  }

  if (state.inputState == InputState::GAME_OVER) {
    ui.drawGameOverUi();
  }

  if (state.inputState == InputState::MENU) {
    ui.drawMenuUi(state);
  }

  if (!structGaugeIsEmpty(state.fadeGauge)) {
    Uint8 alpha = static_cast<Uint8>(255. * structGaugeGetPct(state.fadeGauge));
    static SDL_Texture* bgRect = window.getStaticColorTexture(
        WINDOW_WIDTH, WINDOW_HEIGHT, {0, 0, 0, 255});
    window.globalAlpha = alpha;
    window.drawTexture(bgRect, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, false);
    window.globalAlpha = 255;
  }

  if (state.inputState == InputState::LEVEL ||
      state.inputState == InputState::POST_LEVEL_WAITING_FOR_SCORE_TO_SHOW ||
      state.inputState == InputState::POST_LEVEL_WAITING_FOR_END_TRANSITION ||
      state.inputState == InputState::PRE_LEVEL ||
      state.inputState == InputState::DEFEATED) {
    ui.drawLevelUi(state);
  }
}

int Render::headingToShipSpriteIndex(double heading) const {
  double intHeading = static_cast<int>(heading);
  while (intHeading < 0) {
    intHeading += 360;
  }
  while (intHeading >= 360) {
    intHeading -= 360;
  }

  const int numSprites = 40;
  return (intHeading / 360) * numSprites;
}

void Render::renderAsteroid(Asteroid& asteroid) {
  window.drawAnimation(*asteroid.anim,
                       asteroid.physics.x,
                       asteroid.physics.y,
                       true,
                       true,
                       0.,
                       {1., 1.});
}

void Render::renderSaucer(Saucer& saucer) {
  window.drawAnimation(*saucer.anim,
                       saucer.physics.x,
                       saucer.physics.y,
                       true,
                       true,
                       0.,
                       {1., 1.});
}

void Render::renderHomingMine(HomingMine& homingMine) {
  window.drawAnimation(*homingMine.anim,
                       homingMine.physics.x,
                       homingMine.physics.y,
                       true,
                       true,
                       0.,
                       {1., 1.});
}

void Render::renderBullet(Bullet& bullet) {
  window.drawAnimation(*bullet.anim,
                       bullet.physics.x,
                       bullet.physics.y,
                       true,
                       true,
                       0.,
                       {1., 1.});
}

void Render::renderPowerup(Powerup& powerup) {
  window.drawAnimation(*powerup.anim,
                       powerup.physics.x,
                       powerup.physics.y,
                       true,
                       true,
                       0.,
                       {1., 1.});
}

void Render::renderShip(Ship& ship) {
  std::stringstream ss;

  if (!ship.isDestroyed) {
    ss << "ship";
    if (ship.armorCount > 0) {
      ss << "_armor";
    }
    ss << "_" << headingToShipSpriteIndex(ship.heading.angle + 5);

    window.drawSprite(
        ss.str(), ship.physics.x, ship.physics.y, true, 0., {1., 1.});
    if (ship.isShielded) {
      window.drawAnimation(ship.shieldAnim,
                           ship.physics.x,
                           ship.physics.y,
                           true,
                           true,
                           0.,
                           {1., 1.});
    }
  }
}

void Render::renderAnimatedParticle(AnimatedParticle& particle) {
  if (particle.text.size()) {
    window.setCurrentFont("default", 18);
    window.drawTextCentered(
        particle.text, particle.physics.x, particle.physics.y, particle.color);
  } else {
    window.drawAnimation(*particle.anim,
                         particle.physics.x,
                         particle.physics.y,
                         true,
                         true,
                         0.,
                         {1., 1.});
  }
}

void Render::renderStars() {
  int tileWidth = WINDOW_WIDTH / 32;
  int tileHeight = WINDOW_HEIGHT / 32;
  for (int i = 0; i < tileHeight; i++) {
    for (int j = 0; j < tileWidth; j++) {
      unsigned int ind = background[i * tileWidth + j];
      window.drawSprite(
          "stars_" + std::to_string(ind), j * 32 + 16, i * 32 + 16, true);
    }
  }
}

} // namespace program