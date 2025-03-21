#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/spawn/RemoveSaucer.hpp"
#include "game/actions/spawn/SpawnAnimatedParticle.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include "game/state.h"

namespace program {
namespace actions {

class CollideSaucer : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    state.score += 500;
    enqueueAction(state, std::make_unique<actions::RemoveSaucer>(saucer), 0);
    enqueueAction(state,
                  std::make_unique<actions::SpawnAnimatedParticle>(
                      ASTEROID_EXPLOSION_BIG,
                      std::make_pair(saucer.physics.x, saucer.physics.y)),
                  0);
    enqueueAction(state, std::make_unique<actions::PlaySound>("explosion"), 0);
  }

public:
  Saucer& saucer;
  double collideAngle = 0.;

  CollideSaucer(Saucer& saucer, double collideAngle)
      : saucer(saucer), collideAngle(collideAngle) {}
};
}; // namespace actions
} // namespace program