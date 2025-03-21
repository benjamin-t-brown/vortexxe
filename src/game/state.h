#pragma once

#ifdef MIYOOA30
#include <SDL_pixels.h>
#else
#include <SDL2/SDL_pixels.h>
#endif
#include "game/actions/AbstractAction.h"
#include "game/utils/StructGauge.hpp"
#include "game/utils/StructTimer.hpp"
#include "lib/sdl2wrapper/Animation.h"
#include <memory>
#include <vector>

namespace program {

constexpr const int WINDOW_WIDTH = 640;
constexpr const int WINDOW_HEIGHT = 480;

struct Physics {
  double x = 0.;
  double y = 0.;
  double vx = 0.;
  double vy = 0.;
  double ax = 0.;
  double ay = 0.;
  double mass = 1.;
  double radius = 32.;
  double friction = 0.0;
};

enum HeadingTurnDirection {
  NONE,
  LEFT,
  RIGHT,
};

struct Heading {
  double angle = 0.;
  double rotationRate = 0.25;
  HeadingTurnDirection turnDirection = NONE;
};

enum AsteroidType { SMALL, MEDIUM, LARGE, METAL };

struct Asteroid {
  std::unique_ptr<sdl2w::Animation> anim;
  Physics physics;
  AsteroidType type;
};

enum BulletType { BULLET_SHIP, BULLET_SHIP_LARGE, BULLET_SAUCER };

struct Bullet {
  std::unique_ptr<sdl2w::Animation> anim;
  Physics physics;
  BulletType type;
  StructTimer timer = {2500, 0};
};

enum ShipGunSelected {
  SELECTED_GUN_NORMAL,
  SELECTED_GUN_LARGE,
  SELECTED_GUN_RAPID,
  SELECTED_GUN_SPREAD,
};

struct Ship {
  sdl2w::Animation invincibleAnim;
  sdl2w::Animation shieldAnim;
  Physics physics;
  Heading heading;
  StructGauge shieldGauge = {4000., 0.};
  StructTimer shootTimer = {180, 0};
  StructTimer shieldSoundTimer = {150, 0};
  StructTimer thrustSoundTimer = {150, 0};
  StructTimer thrustParticleTimer = {75, 0};
  double accelRate = 0.0009;
  ShipGunSelected selectedGun = SELECTED_GUN_NORMAL;
  int burstCount = 1;
  int armorCount = 0;
  int numBigShots = 0;
  int numRapidShots = 0;
  int numSpreadShots = 0;
  bool isAccelerate = false;
  bool isShielded = false;
  bool wantToShield = false;
  bool isDestroyed = false;
};

struct Saucer {
  std::unique_ptr<sdl2w::Animation> anim;
  Physics physics;
  Heading heading;
  StructTimer shootTimer = {1200, 0};
};

struct HomingMine {
  std::unique_ptr<sdl2w::Animation> anim;
  Physics physics;
  Heading heading;
  double accelRate = 0.0010;
};

enum PowerupType {
  EXTRA_LIFE,
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
  BONUS_MULTIPLIER_3,
  BONUS_MULTIPLIER_4,
  BONUS_MULTIPLIER_5,
};

struct Powerup {
  std::unique_ptr<sdl2w::Animation> anim;
  Physics physics;
  PowerupType type;
  StructTimer timer = {2000, 0};
};

struct AnimatedParticle {
  std::unique_ptr<sdl2w::Animation> anim;
  Physics physics;
  StructTimer timer;
  std::string text;
  SDL_Color color;
};

enum InputState {
  MENU,
  PRE_LEVEL,
  LEVEL,
  WAITING,
  DEFEATED,
  POST_LEVEL,
  POST_LEVEL_WAITING_FOR_END_TRANSITION,
  POST_LEVEL_WAITING_FOR_SCORE_TO_SHOW,
  GAME_OVER,
};

struct State {
  std::vector<std::unique_ptr<Asteroid>> asteroids;
  std::vector<std::unique_ptr<Saucer>> saucers;
  std::vector<std::unique_ptr<HomingMine>> homingMines;
  std::vector<std::unique_ptr<Bullet>> bullets;
  std::vector<std::unique_ptr<Powerup>> powerups;
  std::vector<std::unique_ptr<AnimatedParticle>> particles;

  std::vector<std::unique_ptr<actions::AsyncAction>> sequentialActions;
  std::vector<std::unique_ptr<actions::AsyncAction>> sequentialActionsNext;

  std::vector<std::unique_ptr<actions::AsyncAction>> parallelActions;
  std::vector<std::string> soundsToPlay;

  Ship ship;

  StructGauge fadeGauge = {256., 0.};
  StructTimer menuTimer = {5000, 0};
  InputState inputState = MENU;
  int highScore = 0;
  int menuInd = 0;
  int score = 0;
  int scoreLevelStart = 0;
  int lastScore = 0;
  int lives = 3;
  int bonus = 1;
  int bonusMult = 1;
  int level = 1;
  bool shouldPlayMusic = false;
  bool isPlayingMusic = false;
  bool isStarting = false;
};

inline void
enqueueAction(State& state, std::unique_ptr<actions::AbstractAction> action, int ms) {
  auto actionPtr = new actions::AsyncAction{std::move(action), StructTimer{ms, 0}};
  state.sequentialActionsNext.push_back(std::unique_ptr<actions::AsyncAction>(actionPtr));
}

inline void
addParallelAction(State& state, std::unique_ptr<actions::AbstractAction> action, int ms) {
  state.parallelActions.push_back(std::unique_ptr<actions::AsyncAction>(
      new actions::AsyncAction{std::move(action), StructTimer{ms, 0}}));
}

inline void moveSequentialActions(State& state) {
  state.sequentialActions.insert(
      state.sequentialActions.end(),
      std::make_move_iterator(state.sequentialActionsNext.begin()),
      std::make_move_iterator(state.sequentialActionsNext.end()));
  state.sequentialActionsNext.clear();
}

} // namespace program