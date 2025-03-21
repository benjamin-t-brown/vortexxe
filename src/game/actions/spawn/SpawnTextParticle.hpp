#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class SpawnTextParticle : public AbstractAction {
protected:
  void act() override {
    State& state = *statePtr;
    state.particles.push_back(
        std::unique_ptr<AnimatedParticle>(new AnimatedParticle{
            .anim = std::make_unique<sdl2w::Animation>(),
        }));
    AnimatedParticle& p = *state.particles.back();

    p.text = text;
    p.color = textColor;
    structTimerStart(p.timer, 1000);

    p.physics.x = pos.first;
    p.physics.y = pos.second;
  }

public:
  std::string text;
  SDL_Color textColor;
  std::pair<double, double> pos;

  SpawnTextParticle(std::string text,
                    const SDL_Color& textColor,
                    std::pair<double, double> pos)
      : text(text), textColor(textColor), pos(pos) {}
};
} // namespace actions
} // namespace program
