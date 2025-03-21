#pragma once

#include "game/actions/AbstractAction.h"
#include "game/physics.hpp"
#include "game/state.h"
#include "lib/sdl2wrapper/Events.h"
#include "lib/sdl2wrapper/Window.h"

namespace program {
namespace actions {

class SpawnAsteroidsForLevel : public AbstractAction {
  int getNumMetalBallsFromLevel(int level) {
    int numMetalBalls = 0;
    int i = 0;
    int sCtr = level + 1;
    while (i < level) {
      if (sCtr % 3 == 0) {
        numMetalBalls += 1;
      }
      i++;
      sCtr++;
    }
    return numMetalBalls;
  }

  void setupAsteroid(Asteroid& asteroid) {
    const double heading = rand() % 360;
    // assuming width gte height
    const double distance =
        static_cast<double>(125 + (rand() % (125 - (WINDOW_HEIGHT + 50) / 2)));
    // const double distance = 100;

    auto [dx, dy] = getVector(heading, distance);
    dx += static_cast<double>(WINDOW_WIDTH) / 2.;
    dy += static_cast<double>(WINDOW_HEIGHT) / 2.;

    asteroid.physics.x = dx;
    asteroid.physics.y = dy;
    asteroid.physics.vx =
        (static_cast<double>(rand() % 100) / 100. - 0.5) * 0.28;
    asteroid.physics.vy =
        (static_cast<double>(rand() % 100) / 100. - 0.5) * 0.28;
    asteroid.physics.mass = 1.;
    asteroid.physics.radius = 15.;
    asteroid.physics.friction = 0.;
  }

protected:
  void act() override {
    State& state = *statePtr;

    state.asteroids.clear();

    for (int i = 0; i < getNumMetalBallsFromLevel(state.level); i++) {
      state.asteroids.push_back(std::unique_ptr<Asteroid>(
          new Asteroid{.anim = std::make_unique<sdl2w::Animation>()}));
      Asteroid& asteroid = *state.asteroids.back();
      sdl2w::Window::setAnimationFromDefinition("asteroid_metal",
                                                      *asteroid.anim);
      asteroid.anim->start();
      asteroid.type = AsteroidType::METAL;
      setupAsteroid(asteroid);
    }

    const int numAsteroids = 1 + level;
    // const int numAsteroids = 1;
    for (int i = 0; i < numAsteroids; i++) {
      state.asteroids.push_back(std::unique_ptr<Asteroid>(
          new Asteroid{.anim = std::make_unique<sdl2w::Animation>()}));
      Asteroid& asteroid = *state.asteroids.back();
      sdl2w::Window::setAnimationFromDefinition("asteroid_large",
                                                      *asteroid.anim);
      asteroid.anim->start();
      asteroid.type = AsteroidType::LARGE;
      setupAsteroid(asteroid);
    }
  }

public:
  const int level;

  SpawnAsteroidsForLevel(int level) : level(level) {}
};
} // namespace actions
} // namespace program
