#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/ui/SetFadeGaugeValue.hpp"
#include "game/state.h"
#include <memory>

namespace program {
namespace actions {

class FadeIn : public AbstractAction {
  void act() override {
    State& state = *statePtr;
    double maxD = static_cast<double>(state.fadeGauge.max);
    int timeInc = 33;
    for (int i = 0; i < 7; i++) {
      int v = static_cast<int>(maxD - (static_cast<double>(i) / 7.) * maxD);
      enqueueAction(state, std::make_unique<SetFadeGaugeValue>(v), timeInc);
    }
    enqueueAction(state, std::make_unique<SetFadeGaugeValue>(0), timeInc);
  }

public:
  void executeImmediate(State* state) {
    statePtr = state;
    act();
  };
};
}; // namespace actions
} // namespace program