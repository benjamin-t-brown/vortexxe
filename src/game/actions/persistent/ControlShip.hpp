#pragma once

#include "client/keys.hpp"
#include "game/actions/AbstractAction.h"
#include "game/actions/ship/SelectNextWeapon.hpp"
#include "game/actions/spawn/SpawnBullet.hpp"
#include "game/physics.hpp"
#include "game/state.h"
#include "game/utils/StructTimer.hpp"
#include "lib/sdl2wrapper/Events.h"
#include <memory>

namespace program {
namespace actions {

class ControlShip : public AbstractAction {
  int getBulletsAvailable(Ship& ship) {
    switch (ship.selectedGun) {
    case SELECTED_GUN_RAPID:
      return ship.numRapidShots;
    case SELECTED_GUN_LARGE:
      return ship.numBigShots;
    case SELECTED_GUN_SPREAD:
      return ship.numSpreadShots;
    default:
      return 1;
    }
  }

protected:
  void act() override {
    State& state = *statePtr;

    state.ship.heading.turnDirection = HeadingTurnDirection::NONE;
    state.ship.wantToShield = false;

    if (state.inputState == InputState::LEVEL) {
      if (state.ship.isDestroyed) {
        return;
      }

      if (events.isKeyPressed(getLeftKey())) {
        state.ship.heading.turnDirection = HeadingTurnDirection::LEFT;
      } else if (events.isKeyPressed(getRightKey())) {
        state.ship.heading.turnDirection = HeadingTurnDirection::RIGHT;
      } else {
        state.ship.heading.turnDirection = HeadingTurnDirection::NONE;
      }

      if (events.isKeyPressed(getUpKey()) || events.isKeyPressed(getCancelKey()) ||
          events.isKeyPressed("Left Ctrl")) {
        state.ship.isAccelerate = true;
      } else {
        state.ship.isAccelerate = false;
      }

      if (events.isKeyPressed(getConfirmKey()) || events.isKeyPressed("Space")) {
        if (structTimerIsComplete(state.ship.shootTimer)) {
          if (state.ship.selectedGun == SELECTED_GUN_LARGE) {
            state.ship.numBigShots--;
            structTimerStart(state.ship.shootTimer, 180);
            addParallelAction(state,
                              std::unique_ptr<SpawnBullet>(new actions::SpawnBullet(
                                  BULLET_SHIP_LARGE,
                                  {state.ship.physics.x, state.ship.physics.y},
                                  {state.ship.physics.vx, state.ship.physics.vy},
                                  state.ship.heading.angle)),
                              0);
          } else if (state.ship.selectedGun == SELECTED_GUN_RAPID) {
            state.ship.numRapidShots--;
            structTimerStart(state.ship.shootTimer, 75);
            addParallelAction(state,
                              std::unique_ptr<SpawnBullet>(new actions::SpawnBullet(
                                  BULLET_SHIP,
                                  {state.ship.physics.x, state.ship.physics.y},
                                  {state.ship.physics.vx, state.ship.physics.vy},
                                  state.ship.heading.angle)),
                              0);
          } else if (state.ship.selectedGun == SELECTED_GUN_SPREAD) {
            state.ship.numSpreadShots--;
            structTimerStart(state.ship.shootTimer, 180);
            for (int i = 0; i < 3; i++) {
              addParallelAction(state,
                                std::unique_ptr<SpawnBullet>(new actions::SpawnBullet(
                                    BULLET_SHIP,
                                    {state.ship.physics.x, state.ship.physics.y},
                                    {state.ship.physics.vx, state.ship.physics.vy},
                                    state.ship.heading.angle + (i - 1) * 10,
                                    i == 0)),
                                0);
            }
          } else {
            structTimerStart(state.ship.shootTimer,
                             180 + 33 * (state.ship.burstCount - 1));
            for (int i = 0; i < state.ship.burstCount; i++) {
              addParallelAction(state,
                                std::unique_ptr<SpawnBullet>(new actions::SpawnBullet(
                                    BULLET_SHIP,
                                    {state.ship.physics.x, state.ship.physics.y},
                                    {state.ship.physics.vx, state.ship.physics.vy},
                                    state.ship.heading.angle)),
                                i * 60);
            }
          }
          int availableBullets = getBulletsAvailable(state.ship);
          if (availableBullets <= 0) {
            enqueueAction(state, std::make_unique<actions::SelectNextWeapon>(), 0);
          }
        }
      } else if (events.isKeyPressed(getAuxKey()) || events.isKeyPressed("Left Shift") ||
                 events.isKeyPressed("Left Alt")) {
        state.ship.wantToShield = true;
      }

      if (state.ship.isAccelerate) {
        applyForce(state.ship.physics, state.ship.heading.angle, state.ship.accelRate);
      }
    }
  }

public:
  const sdl2w::Events& events;

  ControlShip(const sdl2w::Events& events) : events(events) {}
};
} // namespace actions
} // namespace program
