#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/flow/DoPlayerDestroyed.hpp"
#include "game/actions/spawn/SpawnAnimatedParticle.hpp"
#include "game/physics.hpp"
#include "game/state.h"

namespace program {
namespace actions {

class CollideShip : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    if (ship.isDestroyed) {
      return;
    }

    if (ship.isShielded) {
      // bonk
      const auto [nextAx, nextAy] = getVector(collideAngle - 180, 0.1);
      ship.physics.vx += nextAx;
      ship.physics.vy += nextAy;
      enqueueAction(
          state, std::make_unique<actions::PlaySound>("shield_hit"), 0);
    } else if (ship.armorCount > 0) {
      const auto [nextAx, nextAy] = getVector(collideAngle - 180, 0.1);
      ship.physics.vx += nextAx;
      ship.physics.vy += nextAy;
      ship.armorCount--;
      enqueueAction(
          state, std::make_unique<actions::PlaySound>("vortex_armor"), 0);
    } else {
      enqueueAction(state,
                    std::make_unique<actions::SpawnAnimatedParticle>(
                        ASTEROID_EXPLOSION_SMALL,
                        std::make_pair(ship.physics.x, ship.physics.y)),
                    0);
      enqueueAction(
          state, std::make_unique<actions::PlaySound>("freespace_yell"), 0);
      enqueueAction(
          state, std::make_unique<actions::PlaySound>("explosion_player"), 0);
      enqueueAction(state, std::make_unique<DoPlayerDestroyed>(), 0);
    }
  }

public:
  Ship& ship;
  double collideAngle = 0.;

  CollideShip(Ship& ship, double collideAngle)
      : ship(ship), collideAngle(collideAngle) {}
};
}; // namespace actions
} // namespace program