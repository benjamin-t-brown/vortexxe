#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/spawn/RemoveAsteroid.hpp"
#include "game/actions/spawn/SpawnAnimatedParticle.hpp"
#include "game/actions/spawn/SpawnAsteroidsFromAsteroid.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include "game/physics.hpp"
#include "game/state.h"

namespace program {
namespace actions {

class CollideAsteroid : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    if (asteroid.type == METAL && !isLargeBullet) {
      // bonk
      const auto [nextAx, nextAy] = getVector(collideAngle, 0.1);
      asteroid.physics.vx += nextAx;
      asteroid.physics.vy += nextAy;
      enqueueAction(state,
                    std::make_unique<actions::PlaySound>(
                        rand() % 2 ? "vortex_metal_hit1" : "vortex_metal_hit2"),
                    0);
    } else {
      state.score += 100;
      enqueueAction(state,
                    std::make_unique<actions::SpawnAsteroidsFromAsteroid>(
                        asteroid.type,
                        std::make_pair(asteroid.physics.x, asteroid.physics.y)),
                    0);
      enqueueAction(
          state, std::make_unique<actions::RemoveAsteroid>(asteroid), 0);
      enqueueAction(state,
                    std::make_unique<actions::SpawnAnimatedParticle>(
                        asteroid.type == AsteroidType::SMALL
                            ? ASTEROID_EXPLOSION_SMALL
                            : ASTEROID_EXPLOSION_BIG,
                        std::make_pair(asteroid.physics.x, asteroid.physics.y)),
                    0);
      enqueueAction(state,
                    std::make_unique<actions::PlaySound>(
                        asteroid.type == AsteroidType::LARGE ? "explosion2"
                                                             : "explosion"),
                    0);
    }
  }

public:
  Asteroid& asteroid;
  double collideAngle = 0.;
  bool isLargeBullet = false;

  CollideAsteroid(Asteroid& asteroid, double collideAngle, bool isLargeBullet)
      : asteroid(asteroid),
        collideAngle(collideAngle),
        isLargeBullet(isLargeBullet) {}
};
}; // namespace actions
} // namespace program