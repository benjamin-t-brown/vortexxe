#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/ui/PlaySound.hpp"
#include "game/physics.hpp"
#include "game/state.h"
#include "game/utils/StructTimer.hpp"
#include "lib/sdl2wrapper/Events.h"
#include "lib/sdl2wrapper/Window.h"

namespace program {
namespace actions {

class SpawnBullet : public AbstractAction {
protected:
  void act() override {
    State& state = *statePtr;
    state.bullets.push_back(std::unique_ptr<Bullet>(new Bullet({
        .anim = std::make_unique<sdl2w::Animation>(),
    })));
    Bullet& bullet = *state.bullets.back();

    double nextR = 3.;
    std::string soundName = "vortex_lazer";

    switch (type) {
    case BULLET_SHIP: {
      sdl2w::Window::setAnimationFromDefinition("player_bullet",
                                                      *bullet.anim);
      break;
    }
    case BULLET_SHIP_LARGE: {
      sdl2w::Window::setAnimationFromDefinition("player_bullet_big",
                                                      *bullet.anim);
      nextR = 9;
      soundName = "vortex_lazer_big";
      break;
    }
    case BULLET_SAUCER: {
      sdl2w::Window::setAnimationFromDefinition("enemy_bullet",
                                                      *bullet.anim);
      break;
    }
    default:
      // TODO the rest of these
      return;
      break;
    }

    bullet.anim->start();
    bullet.type = type;

    bullet.physics.x = pos.first;
    bullet.physics.y = pos.second;
    bullet.physics.vx = startingMomentum.first;
    bullet.physics.vy = startingMomentum.second;
    bullet.physics.radius = nextR;
    structTimerStart(bullet.timer, 600);

    auto [nextVx, nextVy] = getVector(angle, 0.4);
    bullet.physics.vx += nextVx;
    bullet.physics.vy += nextVy;

    if (playSound) {
      enqueueAction(state, std::make_unique<actions::PlaySound>(soundName), 0);
    }
  }

public:
  const BulletType type;
  const std::pair<double, double> pos;
  const std::pair<double, double> startingMomentum;
  const double angle;
  const bool playSound;

  SpawnBullet(BulletType type,
              std::pair<double, double> pos,
              std::pair<double, double> startingMomentum,
              double angle,
              bool playSound = true)
      : type(type),
        pos(pos),
        startingMomentum(startingMomentum),
        angle(angle),
        playSound(playSound) {}
};
} // namespace actions
} // namespace program
