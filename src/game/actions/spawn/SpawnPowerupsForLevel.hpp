#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/spawn/SpawnPowerup.hpp"
#include "game/physics.hpp"
#include "game/state.h"
#include <memory>

namespace program {
namespace actions {

class SpawnPowerupsForLevel : public AbstractAction {
protected:
  bool shouldSpawnNewLife() { return (level + 1) % 3 == 0; }

  std::pair<double, double> getRandomPosAtEdge() {
    const double heading = rand() % 360;
    // assuming width gte height
    const double distance = static_cast<double>(WINDOW_WIDTH) * 1.2;

    auto [dx, dy] = getVector(heading, distance);
    dx += WINDOW_WIDTH / 2.;
    dy += WINDOW_HEIGHT / 2.;

    return {dx, dy};
  }

  std::pair<double, double> getRandomPos() {
    int minX = 32;
    int minY = 32 + 22;
    int maxX = WINDOW_WIDTH - 32;
    int maxY = WINDOW_HEIGHT - 32;

    return {static_cast<double>(minX + (rand() % (maxX - minX))),
            static_cast<double>(minY + (rand() % (maxY - minY)))};
  }

  std::pair<double, double> getRandomSpeedVector() {
    const double heading = rand() % 360;
    const double distance = 0.1 + static_cast<double>(rand() % 10) / 10. * 0.11;
    return getVector(heading, distance);
  }

  void act() override {
    State& state = *statePtr;

    if (shouldSpawnNewLife()) {
      PowerupType type = EXTRA_LIFE;
      int msDelay = 5000 + rand() % 20000;
      std::pair<double, double> pos = getRandomPosAtEdge();
      std::pair<double, double> speed = getRandomSpeedVector();
      addParallelAction(
          state, std::make_unique<SpawnPowerup>(type, pos, speed), msDelay);
    }

    int numPowerupsToSpawn = level > 0 ? rand() % 3 + 1 : 1;

    std::vector<PowerupType> powerupTypes = {
        SHIELD_RECHARGE,
        ARMOR_UP,
        POINTS_CANDY_SMALL,
        POINTS_CANDY_MEDIUM,
        GUN_LEVEL_UP,
        GUN_RAPID_FIRE,
        GUN_SPREAD_FIRE,
        GUN_LARGE,
        POINTS_STAR,
        BONUS_SHOOTING_STAR,
        BONUS_MULTIPLIER_2,
    };

    for (int i = 0; i < numPowerupsToSpawn; i++) {
      PowerupType type = powerupTypes[rand() % powerupTypes.size()];
      int msDelay = 2000 + (rand() % 25000);
      // int msDelay = 1000;
      std::pair<double, double> pos = getRandomPosAtEdge();
      std::pair<double, double> speed = getRandomSpeedVector();
      if (type >= BONUS_MULTIPLIER_2 || type == POINTS_STAR) {
        pos = getRandomPos();
        speed = {0., 0.};
      }
      if (type == BONUS_SHOOTING_STAR) {
        pos.first = static_cast<double>(rand() % (WINDOW_WIDTH / 2)) +
                    static_cast<double>(WINDOW_WIDTH) / 2.;
        pos.second = 22 + 1;
        speed = {(static_cast<double>(rand() % 50) / 100. - 1.) * 0.16, 0.21};
      }
      addParallelAction(
          state, std::make_unique<SpawnPowerup>(type, pos, speed), msDelay);
    }

    std::vector<PowerupType> bonusTypes = {
        BONUS_MULTIPLIER_2,
        BONUS_MULTIPLIER_3,
        BONUS_MULTIPLIER_4,
        BONUS_MULTIPLIER_5,
    };

    PowerupType type = bonusTypes[level % bonusTypes.size()];
    int msDelay = 1000 + (rand() % 25000);
    auto pos = getRandomPos();
    auto speed = std::make_pair<double, double>(0., 0.);
    addParallelAction(
        state, std::make_unique<SpawnPowerup>(type, pos, speed), msDelay);
  }

public:
  const int level;

  SpawnPowerupsForLevel(int level) : level(level) {}
};
} // namespace actions
} // namespace program
