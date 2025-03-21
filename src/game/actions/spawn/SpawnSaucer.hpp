#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"
#include "game/utils/StructTimer.hpp"
#include "lib/sdl2wrapper/Events.h"
#include "lib/sdl2wrapper/Window.h"

namespace program {
namespace actions {

class SpawnSaucer : public AbstractAction {
protected:
  void act() override {
    State& state = *statePtr;
    state.saucers.push_back(std::unique_ptr<Saucer>(new Saucer({
        .anim = std::make_unique<sdl2w::Animation>(),
    })));
    Saucer& saucer = *state.saucers.back();

    sdl2w::Window::setAnimationFromDefinition("enemy_ship", *saucer.anim);
    saucer.anim->start();

    saucer.physics.x = pos.first;
    saucer.physics.y = pos.second;
    saucer.physics.vx = startingMomentum.first;
    saucer.physics.vy = startingMomentum.second;
    saucer.physics.radius = 14;
    structTimerStart(saucer.shootTimer, 1200);
  }

public:
  const std::pair<double, double> pos;
  const std::pair<double, double> startingMomentum;

  SpawnSaucer(std::pair<double, double> pos,
              std::pair<double, double> startingMomentum)
      : pos(pos), startingMomentum(startingMomentum) {}
};
} // namespace actions
} // namespace program
