#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class SpawnNewShip : public AbstractAction {
protected:
  void act() override {
    State& state = *statePtr;
    state.ship.heading.angle = 0;
    state.ship.heading.turnDirection = NONE;
    state.ship.physics.vx = 0;
    state.ship.physics.vy = 0;
    state.ship.physics.x = WINDOW_WIDTH / 2.;
    state.ship.physics.y = WINDOW_HEIGHT / 2.;
    state.ship.physics.friction = 0.002;
    state.ship.physics.radius = 14.;
    structGaugeSetEmpty(state.ship.shieldGauge);
    if (resetPowerups) {
      state.ship.selectedGun = SELECTED_GUN_NORMAL;
      state.ship.armorCount = 0;
      state.ship.burstCount = 1;
      state.ship.numBigShots = 0;
      state.ship.numRapidShots = 0;
      state.ship.numSpreadShots = 0;
    }
  }

public:
  bool resetPowerups;

  SpawnNewShip(bool resetPowerups = true) : resetPowerups(resetPowerups) {}
};

} // namespace actions
} // namespace program
