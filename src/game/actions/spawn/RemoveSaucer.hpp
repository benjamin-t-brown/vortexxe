
#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class RemoveSaucer : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    for (auto it = state.saucers.begin(); it != state.saucers.end(); ++it) {
      if (it->get() == &saucer) {
        state.saucers.erase(it);
        break;
      }
    }
  }

public:
  Saucer& saucer;

  RemoveSaucer(Saucer& saucer) : saucer(saucer) {}
};
}; // namespace actions
} // namespace program