#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/spawn/SpawnBullet.hpp"
#include "game/collisions.hpp"
#include "game/state.h"
#include "game/utils/StructTimer.hpp"
#include <memory>

namespace program {
namespace actions {

class ControlHomingMines : public AbstractAction {

protected:
  void act() override {
    State& state = *statePtr;

    if (state.inputState == InputState::LEVEL) {

      for (auto& homingMinePtr : state.homingMines) {
        auto& homingMine = *homingMinePtr;
        double nextAngle =
            getAngleTowards(homingMine.physics, state.ship.physics);
        double currAngle = homingMine.heading.angle;
        if (currAngle <= nextAngle) {
          if (std::abs(currAngle - nextAngle) < 180) {
            homingMine.heading.turnDirection = HeadingTurnDirection::RIGHT;
          } else {
            homingMine.heading.turnDirection = HeadingTurnDirection::LEFT;
          }
        } else {
          if (std::abs(currAngle - nextAngle) < 180) {
            homingMine.heading.turnDirection = HeadingTurnDirection::LEFT;
          } else {
            homingMine.heading.turnDirection = HeadingTurnDirection::RIGHT;
          }
        }

        applyForce(
            homingMine.physics, homingMine.heading.angle, homingMine.accelRate);
      }
    }
  }
};
} // namespace actions
} // namespace program
