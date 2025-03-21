#pragma once

#include "game/actions/AbstractAction.h"
#include "game/physics.hpp"
#include "game/state.h"
#include "game/utils/StructTimer.hpp"
#include "lib/sdl2wrapper/Events.h"
#include "lib/sdl2wrapper/Window.h"

namespace program {
namespace actions {

enum AnimatedParticleType {
  ASTEROID_EXPLOSION_BIG,
  ASTEROID_EXPLOSION_SMALL,
  PARTICLE_CAPSULE_EXPLOSION,
  PARTICLE_YES,
  PARTICLE_NO,
  PARTICLE_BOOST,
};

class SpawnAnimatedParticle : public AbstractAction {
protected:
  void act() override {
    State& state = *statePtr;
    state.particles.push_back(
        std::unique_ptr<AnimatedParticle>(new AnimatedParticle{
            .anim = std::make_unique<sdl2w::Animation>(),
        }));
    AnimatedParticle& p = *state.particles.back();

    switch (type) {
    case ASTEROID_EXPLOSION_BIG: {
      sdl2w::Window::setAnimationFromDefinition("explosion", *p.anim);
      break;
    }
    case ASTEROID_EXPLOSION_SMALL: {
      sdl2w::Window::setAnimationFromDefinition("explosion2", *p.anim);
      break;
    }
    case PARTICLE_YES: {
      sdl2w::Window::setAnimationFromDefinition("yes", *p.anim);
      break;
    }
    case PARTICLE_NO: {
      sdl2w::Window::setAnimationFromDefinition("no", *p.anim);
      break;
    }
    case PARTICLE_CAPSULE_EXPLOSION: {
      sdl2w::Window::setAnimationFromDefinition("capsule_shatter", *p.anim);
      break;
    }
    case PARTICLE_BOOST: {
      auto [nextVx, nextVy] = getVector(state.ship.heading.angle, 0.1);
      
      sdl2w::Window::setAnimationFromDefinition("boost", *p.anim);
      auto [offsetX1, offsetY1] = getVector(state.ship.heading.angle - 18, 20.);
      p.physics.vx -= nextVx;
      p.physics.vy -= nextVy;
      p.physics.x = pos.first - offsetX1;
      p.physics.y = pos.second - offsetY1;

      structTimerStart(p.timer, p.anim->totalDuration);
      p.anim->start();

      state.particles.push_back(
          std::unique_ptr<AnimatedParticle>(new AnimatedParticle{
              .anim = std::make_unique<sdl2w::Animation>(),
          }));
      AnimatedParticle& p2 = *state.particles.back();

      sdl2w::Window::setAnimationFromDefinition("boost", *p2.anim);
      auto [offsetX2, offsetY2] = getVector(state.ship.heading.angle + 18, 20.);
      p2.physics.vx -= nextVx;
      p2.physics.vy -= nextVy;
      p2.physics.x = pos.first - offsetX2;
      p2.physics.y = pos.second - offsetY2;

      structTimerStart(p2.timer, p2.anim->totalDuration);
      p2.anim->start();
      return;
      break;
    }
    default:
      return;
      break;
    }

    structTimerStart(p.timer, p.anim->totalDuration);
    p.anim->start();

    p.physics.x = pos.first;
    p.physics.y = pos.second;
  }

public:
  const AnimatedParticleType type;
  std::pair<double, double> pos;

  SpawnAnimatedParticle(AnimatedParticleType type,
                        std::pair<double, double> pos)
      : type(type), pos(pos) {}
};
} // namespace actions
} // namespace program
