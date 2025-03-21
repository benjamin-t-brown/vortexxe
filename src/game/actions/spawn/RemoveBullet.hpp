
#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class RemoveBullet : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    for (auto it = state.bullets.begin(); it != state.bullets.end(); ++it) {
      if (it->get() == &bullet) {
        state.bullets.erase(it);
        break;
      }
    }
  }

public:
  Bullet& bullet;

  RemoveBullet(Bullet& bullet) : bullet(bullet) {}
};
}; // namespace actions
} // namespace program