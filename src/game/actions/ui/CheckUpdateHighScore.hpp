#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/ui/PlaySound.hpp"
#include "game/state.h"
#include "lib/hiscore/hiscore.h"

namespace program {
namespace actions {

class CheckUpdateHighScore : public AbstractAction {
  void act() override {
    State& state = *statePtr;
    if (state.score > state.highScore) {
      state.highScore = state.score;
      hiscore::saveHighScores({{"player", state.highScore}});
    }
  }

public:
  int score;
  CheckUpdateHighScore(int score) : score(score) {}
};
}; // namespace actions
} // namespace program