#pragma once

#include "LibHTML.h"
#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class NotifyGameComplete : public AbstractAction {
  void act() override {
    State& state = *statePtr;
    libhtml::notifyGameCompleted(state.lastScore);
  }
};
}; // namespace actions
} // namespace program