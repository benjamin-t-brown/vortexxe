#include "GameManager.h"
#include "LibHTML.h"
#include "client/keys.hpp"
#include "game/actions/AbstractAction.h"
#include "game/actions/flow/SetInputMode.hpp"
#include "game/actions/flow/SetupNewGame.hpp"
#include "game/actions/persistent/CheckLevelComplete.hpp"
#include "game/actions/persistent/ControlHomingMines.hpp"
#include "game/actions/persistent/ControlSaucers.hpp"
#include "game/actions/persistent/ControlShip.hpp"
#include "game/actions/persistent/FlipMenu.hpp"
#include "game/actions/ship/SelectNextWeapon.hpp"
#include "game/actions/spawn/SpawnAnimatedParticle.hpp"
#include "game/collisions.hpp"
#include "game/state.h"
#include "game/utils/StructGauge.hpp"
#include "game/utils/StructTimer.hpp"
#include "lib/sdl2wrapper/AssetLoader.h"
#include "physics.hpp"
#include <memory>

namespace program {
GameManager::GameManager(sdl2w::Window& windowA) : window(windowA), r(windowA) {}

GameManager::~GameManager() {}

void GameManager::load() {
  sdl2w::loadAssetsFromFile("sprite", "assets/resSprites.txt");
  sdl2w::loadAssetsFromFile("sound", "assets/resSounds.txt");
  sdl2w::loadAssetsFromFile("animation", "assets/resAnims.txt");
  auto hiscores = hiscore::getHighScores();
  if (hiscores.size()) {
    state.highScore = hiscores[0].score;
  }
}

void GameManager::start() {
  persistentActions.push_back(std::unique_ptr<actions::AbstractAction>(
      new actions::ControlShip(window.getEvents())));
  persistentActions.push_back(
      std::unique_ptr<actions::AbstractAction>(new actions::ControlSaucers()));
  persistentActions.push_back(
      std::unique_ptr<actions::AbstractAction>(new actions::ControlHomingMines()));
  persistentActions.push_back(
      std::unique_ptr<actions::AbstractAction>(new actions::CheckLevelComplete()));
  persistentActions.push_back(
      std::unique_ptr<actions::AbstractAction>(new actions::FlipMenu()));

  window.setAnimationFromDefinition("player_shield", state.ship.shieldAnim);
  state.ship.shieldAnim.start();
  window.setAnimationFromDefinition("player_invincible", state.ship.invincibleAnim);
  state.ship.invincibleAnim.start();

  r.setup();

  enqueueAction(state, std::make_unique<actions::SetInputMode>(MENU), 0);

  libhtml::notifyGameReady();
}

void GameManager::handleKeyPress(const std::string& key) {
  if (state.inputState == InputState::MENU && !state.isStarting) {
    enqueueAction(state, std::make_unique<actions::SetupNewGame>(), 0);
  }

  if (state.inputState == InputState::PRE_LEVEL) {
    enqueueAction(state, std::make_unique<actions::SetInputMode>(InputState::LEVEL), 0);
  }

  const auto& events = window.getEvents();
  if (state.inputState == InputState::LEVEL) {
    if (events.isKeyPressed(getDownKey())) {
      enqueueAction(state, std::make_unique<actions::SelectNextWeapon>(), 0);
    }
  }

  // logger::info(("Keypress: " + key).c_str());
  lastKeyPressed = key;
}

void GameManager::update(int dt) {
  for (auto& action : persistentActions) {
    action->execute(&state);
  }

  updatePhysics(state.ship.physics, dt);
  updateHeading(state.ship.heading, dt);
  structTimerUpdate(state.ship.shootTimer, dt);
  structTimerUpdate(state.ship.shieldSoundTimer, dt);
  structTimerUpdate(state.ship.thrustSoundTimer, dt);
  structTimerUpdate(state.ship.thrustParticleTimer, dt);
  if (state.ship.wantToShield) {
    state.ship.isShielded = !structGaugeIsFull(state.ship.shieldGauge);
    structGaugeFill(state.ship.shieldGauge, static_cast<double>(dt));
    if (state.ship.isShielded) {
      if (structTimerIsComplete(state.ship.shieldSoundTimer)) {
        state.soundsToPlay.push_back("shield");
        structTimerStart(state.ship.shieldSoundTimer);
      }
    }
  } else {
    state.ship.isShielded = false;
  }
  if (state.ship.isAccelerate) {
    if (structTimerIsComplete(state.ship.thrustSoundTimer)) {
      structTimerStart(state.ship.thrustSoundTimer);
      state.soundsToPlay.push_back("vortex_engine");
    }
    if (structTimerIsComplete(state.ship.thrustParticleTimer)) {
      structTimerStart(state.ship.thrustParticleTimer);
      enqueueAction(state,
                    std::make_unique<actions::SpawnAnimatedParticle>(
                        actions::PARTICLE_BOOST,
                        std::make_pair(state.ship.physics.x, state.ship.physics.y)),
                    0);
    }
  }

  if (state.inputState == InputState::MENU || state.inputState == InputState::LEVEL ||
      state.inputState == InputState::WAITING ||
      state.inputState == InputState::DEFEATED ||
      state.inputState == InputState::POST_LEVEL_WAITING_FOR_END_TRANSITION) {
    for (int i = 0; i < static_cast<int>(state.bullets.size()); i++) {
      auto& bullet = state.bullets[i];
      updatePhysics(bullet->physics, dt);
      structTimerUpdate(bullet->timer, dt);
      if (structTimerIsComplete(bullet->timer)) {
        state.bullets.erase(state.bullets.begin() + i);
        i--;
      }
    }
    for (int i = 0; i < static_cast<int>(state.powerups.size()); i++) {
      auto& powerup = state.powerups[i];
      updatePhysics(powerup->physics, dt);
      structTimerUpdate(powerup->timer, dt);
      if (structTimerIsComplete(powerup->timer)) {
        state.powerups.erase(state.powerups.begin() + i);
        i--;
      }
    }
    for (auto& asteroid : state.asteroids) {
      updatePhysics(asteroid->physics, dt);
    }
    for (auto& saucer : state.saucers) {
      updatePhysics(saucer->physics, dt);
      structTimerUpdate(saucer->shootTimer, dt);
    }
    for (auto& homingMine : state.homingMines) {
      updatePhysics(homingMine->physics, dt);
      updateHeading(homingMine->heading, dt);
    }
  }

  for (int i = 0; i < static_cast<int>(state.particles.size()); i++) {
    auto& particle = state.particles[i];
    updatePhysics(particle->physics, dt);
    structTimerUpdate(particle->timer, dt);
    if (structTimerIsComplete(particle->timer)) {
      state.particles.erase(state.particles.begin() + i);
      i--;
    }
  }

  if (state.inputState == InputState::LEVEL) {
    checkCollisions(state);
  }

  structTimerUpdate(state.menuTimer, dt);

  moveSequentialActions(state);
  while (!state.sequentialActions.empty()) {
    auto& delayedActionPtr = state.sequentialActions.front();
    actions::AsyncAction& delayedAction = *delayedActionPtr;
    if (delayedAction.action.get() != nullptr) {
      // sdl2w::Logger().get()
      //     << "Executing action: " << delayedAction.action->getName() << " "
      //     << delayedAction.timer.duration << ":" << delayedAction.timer.t
      //     << sdl2w::Logger::endl;
      delayedAction.action->execute(&state);
      delayedAction.action = nullptr;
    }

    structTimerUpdate(delayedAction.timer, dt);
    if (structTimerIsComplete(delayedAction.timer)) {
      bool shouldLoop = delayedAction.timer.duration == 0;
      // sdl2w::Logger().get()
      //     << "Executing action: " << delayedAction.action->getName() << " "
      //     << delayedAction.timer.duration << ":" << delayedAction.timer.t
      //     << sdl2w::Logger::endl;
      state.sequentialActions.erase(state.sequentialActions.begin());
      if (shouldLoop) {
        moveSequentialActions(state);
        continue;
      } else {
        break;
      }
    } else {
      break;
    }
  }
  if (state.inputState == InputState::LEVEL) {
    for (unsigned int i = 0; i < state.parallelActions.size(); i++) {
      auto& delayedActionPtr = state.parallelActions[i];
      actions::AsyncAction& delayedAction = *delayedActionPtr;
      structTimerUpdate(delayedAction.timer, dt);
      if (structTimerIsComplete(delayedAction.timer)) {
        if (delayedAction.action != nullptr) {
          delayedAction.action->execute(&state);
        }
        state.parallelActions.erase(state.parallelActions.begin() + i);
        i--;
      }
    }
  }
}

void GameManager::render() {
  r.renderStars();

  for (auto& powerup : state.powerups) {
    r.renderPowerup(*powerup);
  }
  for (auto& asteroid : state.asteroids) {
    r.renderAsteroid(*asteroid);
  }
  for (auto& saucer : state.saucers) {
    r.renderSaucer(*saucer);
  }
  for (auto& homingMine : state.homingMines) {
    r.renderHomingMine(*homingMine);
  }
  for (auto& bullet : state.bullets) {
    r.renderBullet(*bullet);
  }
  for (auto& animParticle : state.particles) {
    r.renderAnimatedParticle(*animParticle);
  }

  if (state.inputState != InputState::MENU) {
    r.renderShip(state.ship);
  }

  r.renderUi(state);

  // Debug fun
  // window.setCurrentFont("default", 24);
  // window.drawTextCentered("KEY " + lastKeyPressed, 100, 100, {255, 255, 255});

  for (const auto& soundName : state.soundsToPlay) {
    window.playSound(soundName);
  }
  state.soundsToPlay.clear();

  if (state.shouldPlayMusic) {
    if (!state.isPlayingMusic) {
      window.playMusic("vortex");
      state.isPlayingMusic = true;
    }
  } else {
    if (state.isPlayingMusic) {
      window.stopMusic();
      state.isPlayingMusic = false;
    }
  }
}
} // namespace program