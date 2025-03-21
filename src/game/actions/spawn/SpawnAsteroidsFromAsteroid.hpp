#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"
#include "lib/sdl2wrapper/Animation.h"
#include "lib/sdl2wrapper/Events.h"
#include "lib/sdl2wrapper/Window.h"
#include <memory>

namespace program {
namespace actions {

class SpawnAsteroidsFromAsteroid : public AbstractAction {
protected:
  void act() override {
    State& state = *statePtr;

    AsteroidType nextType = AsteroidType::MEDIUM;
    double nextR = 10.;
    double nextVMult = 0.27;
    int numAsteroids = 2;
    std::string nextAnimName = "asteroid_medium";

    switch (previousSize) {
    case AsteroidType::LARGE:
      break;
    case AsteroidType::MEDIUM:
      nextType = AsteroidType::SMALL;
      nextR = 6.;
      nextVMult = 0.31;
      nextAnimName = "asteroid_small";
      break;
    case AsteroidType::SMALL:
    case AsteroidType::METAL:
      return;
    }

    for (int i = 0; i < numAsteroids; i++) {
      state.asteroids.push_back(std::unique_ptr<Asteroid>(new Asteroid({
          .anim = std::make_unique<sdl2w::Animation>(),
      })));
      Asteroid& asteroid = *state.asteroids.back();
      sdl2w::Window::setAnimationFromDefinition(nextAnimName,
                                                      *asteroid.anim);
      asteroid.anim->start();

      asteroid.physics.x = spawnPos.first;
      asteroid.physics.y = spawnPos.second;
      asteroid.physics.vx =
          (static_cast<double>(rand() % 100) / 100. - 0.5) * nextVMult;
      asteroid.physics.vy =
          (static_cast<double>(rand() % 100) / 100. - 0.5) * nextVMult;
      asteroid.physics.mass = 1.;
      asteroid.physics.radius = nextR;
      asteroid.physics.friction = 0.;
      asteroid.type = nextType;
    }
  }

public:
  const AsteroidType previousSize;
  std::pair<double, double> spawnPos;

  SpawnAsteroidsFromAsteroid(AsteroidType previousSize,
                             std::pair<double, double> spawnPos)
      : previousSize(previousSize), spawnPos(spawnPos) {}
};
} // namespace actions
} // namespace program
