#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/spawn/SpawnBullet.hpp"
#include "game/collisions.hpp"
#include "game/state.h"
#include "game/utils/StructTimer.hpp"
#include <memory>

namespace program {
namespace actions {

class ControlSaucers : public AbstractAction {

protected:
  void act() override {
    State& state = *statePtr;

    if (state.inputState == InputState::LEVEL) {

      for (auto& saucer : state.saucers) {
        if (structTimerIsComplete(saucer->shootTimer)) {
          double angle = getAngleTowards(saucer->physics, state.ship.physics);

          structTimerStart(saucer->shootTimer, 1200);
          addParallelAction(
              state,
              std::unique_ptr<SpawnBullet>(new actions::SpawnBullet(
                  BULLET_SAUCER,
                  {saucer->physics.x, saucer->physics.y},
                  {0, 0},
                  angle)),
              0);
        }
      }
    }
  }
};
} // namespace actions
} // namespace program
