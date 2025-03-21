#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"
#include "game/utils/StructTimer.hpp"
#include "lib/sdl2wrapper/Events.h"
#include "lib/sdl2wrapper/Window.h"

namespace program {
namespace actions {

class SpawnPowerup : public AbstractAction {
protected:
  void act() override {
    State& state = *statePtr;
    state.powerups.push_back(std::unique_ptr<Powerup>(new Powerup({
        .anim = std::make_unique<sdl2w::Animation>(),
    })));
    Powerup& powerup = *state.powerups.back();
    powerup.type = type;
    double nextR = 14;

    switch (type) {
    case EXTRA_LIFE: {
      sdl2w::Window::setAnimationFromDefinition("heart", *powerup.anim);
      break;
    }
    case POINTS_CANDY_SMALL:
    case POINTS_CANDY_MEDIUM:
    case SHIELD_RECHARGE:
    case ARMOR_UP: {
      sdl2w::Window::setAnimationFromDefinition("candy", *powerup.anim);
      break;
    }
    case GUN_LEVEL_UP:
    case GUN_SPREAD_FIRE:
    case GUN_LARGE:
    case GUN_RAPID_FIRE: {
      sdl2w::Window::setAnimationFromDefinition("capsule", *powerup.anim);
      break;
    }
    case POINTS_STAR: {
      sdl2w::Window::setAnimationFromDefinition("star", *powerup.anim);
      break;
    }
    case BONUS_SHOOTING_STAR: {
      sdl2w::Window::setAnimationFromDefinition("shooting_star",
                                                      *powerup.anim);
      break;
    }
    case BONUS_MULTIPLIER_2: {
      sdl2w::Window::setAnimationFromDefinition("bonus0", *powerup.anim);
      nextR = 17;
      break;
    }
    case BONUS_MULTIPLIER_3: {
      sdl2w::Window::setAnimationFromDefinition("bonus1", *powerup.anim);
      nextR = 17;
      break;
    }
    case BONUS_MULTIPLIER_4: {
      sdl2w::Window::setAnimationFromDefinition("bonus2", *powerup.anim);
      nextR = 17;
      break;
    }
    case BONUS_MULTIPLIER_5: {
      sdl2w::Window::setAnimationFromDefinition("bonus3", *powerup.anim);
      nextR = 17;
      break;
    }
    default:
      return;
      break;
    }

    powerup.anim->start();

    powerup.physics.x = pos.first;
    powerup.physics.y = pos.second;
    powerup.physics.vx = startingMomentum.first;
    powerup.physics.vy = startingMomentum.second;
    powerup.physics.radius = nextR;
    structTimerStart(powerup.timer, 10000);
  }

public:
  const PowerupType type;
  const std::pair<double, double> pos;
  const std::pair<double, double> startingMomentum;

  SpawnPowerup(PowerupType type,
               std::pair<double, double> pos,
               std::pair<double, double> startingMomentum)
      : type(type), pos(pos), startingMomentum(startingMomentum) {}
};
} // namespace actions
} // namespace program
