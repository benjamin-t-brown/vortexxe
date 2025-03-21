#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class ModifyScoreAndBonus : public AbstractAction {
  void act() override {
    State& state = *statePtr;
    state.score += v;
    state.bonus -= v;
  }

public:
  int v;
  ModifyScoreAndBonus(int v) : v(v) {}
};
}; // namespace actions
} // namespace program