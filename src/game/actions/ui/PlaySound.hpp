#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class PlaySound : public AbstractAction {
  void act() override {
    State& state = *statePtr;
    state.soundsToPlay.push_back(soundName);
  }

public:
  const std::string soundName;

  PlaySound(const std::string& soundName) : soundName(soundName) {}
};
}; // namespace actions
} // namespace program