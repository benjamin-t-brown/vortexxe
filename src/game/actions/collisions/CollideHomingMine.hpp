#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/spawn/RemoveHomingMine.hpp"
#include "game/actions/spawn/SpawnAnimatedParticle.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include "game/physics.hpp"
#include "game/state.h"

namespace program {
namespace actions {

class CollideHomingMine : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    if (isLargeBullet) {
      state.score += 500;
      enqueueAction(
          state, std::make_unique<actions::RemoveHomingMine>(homingMine), 0);
      enqueueAction(
          state,
          std::make_unique<actions::SpawnAnimatedParticle>(
              ASTEROID_EXPLOSION_BIG,
              std::make_pair(homingMine.physics.x, homingMine.physics.y)),
          0);
      enqueueAction(
          state, std::make_unique<actions::PlaySound>("explosion"), 0);
    } else {
      // bonk
      const auto [nextAx, nextAy] = getVector(collideAngle - 180, 0.41);
      homingMine.physics.vx += nextAx;
      homingMine.physics.vy += nextAy;
      enqueueAction(state,
                    std::make_unique<actions::PlaySound>(
                        rand() % 2 ? "vortex_metal_hit1" : "vortex_metal_hit2"),
                    0);
    }
  }

public:
  HomingMine& homingMine;
  double collideAngle = 0.;
  bool isLargeBullet = false;

  CollideHomingMine(HomingMine& homingMine,
                    double collideAngle,
                    bool isLargeBullet)
      : homingMine(homingMine),
        collideAngle(collideAngle),
        isLargeBullet(isLargeBullet) {}
};
}; // namespace actions
} // namespace program