#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class SetFadeGaugeValue : public AbstractAction {
  void act() override {
    State& state = *statePtr;
    state.fadeGauge.v = v;
  }

public:
  int v;
  SetFadeGaugeValue(int v) : v(v) {}
};
}; // namespace actions
} // namespace program