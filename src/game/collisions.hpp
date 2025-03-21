#pragma once

#include "game/actions/collisions/CollideAsteroid.hpp"
#include "game/actions/collisions/CollideHomingMine.hpp"
#include "game/actions/collisions/CollidePowerup.hpp"
#include "game/actions/collisions/CollideSaucer.hpp"
#include "game/actions/collisions/CollideShip.hpp"
#include "game/actions/spawn/RemoveBullet.hpp"
#include "game/physics.hpp"
#include "game/state.h"
#include <cmath>
#include <memory>

namespace program {

inline double getAngleTowards(const Physics& p1, const Physics& p2) {
  const double x1 = p1.x;
  const double y1 = p1.y;
  const double x2 = p2.x;
  const double y2 = p2.y;
  const double lenY = y2 - y1;
  const double dx = p1.x - p2.x;
  const double dy = p1.y - p2.y;
  const double hyp = std::sqrt(dx * dx + dy * dy);
  double ret = 0.;
  if (y2 >= y1 && x2 >= x1) {
    ret = (std::asin(lenY / hyp) * 180.) / PI + 90.;
  } else if (y2 >= y1 && x2 < x1) {
    ret = (std::asin(lenY / -hyp) * 180.) / PI - 90.;
  } else if (y2 < y1 && x2 > x1) {
    ret = (std::asin(lenY / hyp) * 180.) / PI + 90.;
  } else {
    ret = (std::asin(-lenY / hyp) * 180.) / PI - 90.;
  }
  if (ret >= 360) {
    ret = 360. - ret;
  }
  if (ret < 0) {
    ret = 360. + ret;
  }
  return ret;
};

struct CircleCollision {
  double angle = 0.;
  bool isOverlapping = false;
};

// check if p1 collides with p2, and if it does, return what angle you'd have to
// go to from p1 to get to p2
inline CircleCollision getCircleCollisionVector(const Physics& p1,
                                                const Physics& p2) {
  const double dx = p1.x - p2.x;
  const double dy = p1.y - p2.y;
  const double distance = std::sqrt(dx * dx + dy * dy);

  if ((p1.radius + p2.radius) >= distance) {
    return {getAngleTowards(p1, p2), true};
  }
  return {0., false};
}

inline void doCol_ShipAsteroid(Ship& ship, Asteroid& asteroid, State& state) {
  if (ship.isDestroyed) {
    return;
  }

  const auto c = getCircleCollisionVector(ship.physics, asteroid.physics);
  if (c.isOverlapping) {
    enqueueAction(
        state, std::make_unique<actions::CollideShip>(ship, c.angle), 0);
    enqueueAction(
        state,
        std::make_unique<actions::CollideAsteroid>(asteroid, c.angle, false),
        0);
  }
}

inline void
doCol_BulletAsteroid(Bullet& bullet, Asteroid& asteroid, State& state) {
  if (bullet.type == BULLET_SAUCER) {
    return;
  }

  const auto c = getCircleCollisionVector(bullet.physics, asteroid.physics);
  if (c.isOverlapping) {
    enqueueAction(state, std::make_unique<actions::RemoveBullet>(bullet), 0);
    enqueueAction(state,
                  std::make_unique<actions::CollideAsteroid>(
                      asteroid, c.angle, bullet.type == BULLET_SHIP_LARGE),
                  0);
  }
}

inline void doCol_BulletSaucer(Bullet& bullet, Saucer& saucer, State& state) {
  if (bullet.type == BULLET_SAUCER) {
    return;
  }

  const auto c = getCircleCollisionVector(saucer.physics, bullet.physics);
  if (c.isOverlapping) {
    enqueueAction(state, std::make_unique<actions::RemoveBullet>(bullet), 0);
    enqueueAction(
        state, std::make_unique<actions::CollideSaucer>(saucer, c.angle), 0);
  }
}

inline void doCol_BulletShip(Bullet& bullet, Ship& ship, State& state) {
  if (ship.isDestroyed || bullet.type != BULLET_SAUCER) {
    return;
  }

  const auto c = getCircleCollisionVector(ship.physics, bullet.physics);
  if (c.isOverlapping) {
    enqueueAction(state, std::make_unique<actions::RemoveBullet>(bullet), 0);
    enqueueAction(
        state, std::make_unique<actions::CollideShip>(ship, c.angle), 0);
  }
}

inline void
doCol_BulletHomingMine(Bullet& bullet, HomingMine& homingMine, State& state) {
  if (bullet.type == BULLET_SAUCER) {
    return;
  }

  const auto c = getCircleCollisionVector(homingMine.physics, bullet.physics);
  if (c.isOverlapping) {
    enqueueAction(state, std::make_unique<actions::RemoveBullet>(bullet), 0);
    enqueueAction(state,
                  std::make_unique<actions::CollideHomingMine>(
                      homingMine,
                      getAngleTowards(homingMine.physics, state.ship.physics),
                      bullet.type == BULLET_SHIP_LARGE),
                  0);
  }
}

inline void
doCol_BulletPowerup(Bullet& bullet, Powerup& powerup, State& state) {
  if (bullet.type == BULLET_SAUCER) {
    return;
  }

  const auto c = getCircleCollisionVector(bullet.physics, powerup.physics);
  if (c.isOverlapping) {
    enqueueAction(state, std::make_unique<actions::RemoveBullet>(bullet), 0);
    enqueueAction(
        state,
        std::make_unique<actions::CollidePowerup>(powerup, c.angle, true),
        0);
  }
}

inline void doCol_PowerupShip(Powerup& powerup, Ship& ship, State& state) {
  if (ship.isDestroyed) {
    return;
  }

  const auto c = getCircleCollisionVector(ship.physics, powerup.physics);
  if (c.isOverlapping) {
    enqueueAction(
        state,
        std::make_unique<actions::CollidePowerup>(powerup, c.angle, false),
        0);
  }
}

inline void doCol_SaucerShip(Saucer& saucer, Ship& ship, State& state) {
  if (ship.isDestroyed) {
    return;
  }

  const auto c = getCircleCollisionVector(ship.physics, saucer.physics);
  if (c.isOverlapping) {
    enqueueAction(
        state, std::make_unique<actions::CollideSaucer>(saucer, c.angle), 0);
    enqueueAction(
        state, std::make_unique<actions::CollideShip>(ship, c.angle), 0);
  }
}

inline void doCol_HomingMineShip(HomingMine& saucer, Ship& ship, State& state) {
  if (ship.isDestroyed) {
    return;
  }

  const auto c = getCircleCollisionVector(ship.physics, saucer.physics);
  if (c.isOverlapping) {
    enqueueAction(state,
                  std::make_unique<actions::CollideHomingMine>(
                      saucer, c.angle, ship.isShielded || ship.armorCount > 0),
                  0);
    enqueueAction(
        state, std::make_unique<actions::CollideShip>(ship, c.angle), 0);
  }
}

inline void checkCollisions(State& state) {
  for (auto& asteroid : state.asteroids) {
    doCol_ShipAsteroid(state.ship, *asteroid, state);

    for (auto& bullet : state.bullets) {
      doCol_BulletAsteroid(*bullet, *asteroid, state);
    }
  }

  for (auto& bullet : state.bullets) {
    doCol_BulletShip(*bullet, state.ship, state);
    for (auto& powerup : state.powerups) {
      doCol_BulletPowerup(*bullet, *powerup, state);
    }
    for (auto& saucer : state.saucers) {
      doCol_BulletSaucer(*bullet, *saucer, state);
    }
    for (auto& homingMine : state.homingMines) {
      doCol_BulletHomingMine(*bullet, *homingMine, state);
    }
  }

  for (auto& saucer : state.saucers) {
    doCol_SaucerShip(*saucer, state.ship, state);
  }

  for (auto& homingMine : state.homingMines) {
    doCol_HomingMineShip(*homingMine, state.ship, state);
  }

  for (auto& powerup : state.powerups) {
    doCol_PowerupShip(*powerup, state.ship, state);
  }
}

} // namespace program