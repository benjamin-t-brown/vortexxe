#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/ui/PlaySound.hpp"
#include "game/state.h"
#include "lib/sdl2wrapper/Events.h"
#include "lib/sdl2wrapper/Window.h"

namespace program {
namespace actions {

class SpawnHomingMine : public AbstractAction {
protected:
  void act() override {
    State& state = *statePtr;
    state.homingMines.push_back(std::unique_ptr<HomingMine>(new HomingMine({
        .anim = std::make_unique<sdl2w::Animation>(),
    })));
    HomingMine& homingMine = *state.homingMines.back();

    sdl2w::Window::setAnimationFromDefinition("mine", *homingMine.anim);
    homingMine.anim->start();

    homingMine.physics.x = pos.first;
    homingMine.physics.y = pos.second;
    homingMine.physics.vx = startingMomentum.first;
    homingMine.physics.vy = startingMomentum.second;
    homingMine.physics.radius = 14;
    homingMine.physics.friction = 0.004;
    homingMine.heading.rotationRate = 0.2;

    enqueueAction(state, std::make_unique<actions::PlaySound>("alarm"), 0);
  }

public:
  const std::pair<double, double> pos;
  const std::pair<double, double> startingMomentum;

  SpawnHomingMine(std::pair<double, double> pos,
                  std::pair<double, double> startingMomentum)
      : pos(pos), startingMomentum(startingMomentum) {}
};
} // namespace actions
} // namespace program
