#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/ship/SelectNextWeapon.hpp"
#include "game/actions/spawn/RemovePowerup.hpp"
#include "game/actions/spawn/SpawnAnimatedParticle.hpp"
#include "game/actions/spawn/SpawnTextParticle.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include "game/state.h"
#include "game/utils/StructGauge.hpp"
#include "lib/sdl2wrapper/Localization.h"

namespace program {
namespace actions {

class CollidePowerup : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    if (isBullet) {
      switch (powerup.type) {
      case EXTRA_LIFE:
      case SHIELD_RECHARGE:
      case ARMOR_UP:
      case POINTS_CANDY_SMALL:
      case POINTS_CANDY_MEDIUM:
        enqueueAction(state,
                      std::make_unique<actions::SpawnAnimatedParticle>(
                          PARTICLE_NO,
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("no_matt"), 0);
        break;
      case GUN_LEVEL_UP:
      case GUN_LARGE:
      case GUN_RAPID_FIRE:
      case GUN_SPREAD_FIRE:
        enqueueAction(state,
                      std::make_unique<actions::SpawnAnimatedParticle>(
                          PARTICLE_CAPSULE_EXPLOSION,
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("you_dummy"), 0);
        break;
      case POINTS_STAR:
        state.score += 1000;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+1000",
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("star_get"), 0);
        break;
      case BONUS_SHOOTING_STAR:
        state.bonus += 1000;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+1000 " + sdl2w::L10n::trans(LOCSTR("Bonus")),
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("star_get"), 0);
        break;
      case BONUS_MULTIPLIER_2:
        state.bonusMult *= 2;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "2x " + sdl2w::L10n::trans(LOCSTR("Bonus Multiply")),
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("bonus_get"), 0);
        break;
      case BONUS_MULTIPLIER_3:
        state.bonusMult *= 3;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "3x " + sdl2w::L10n::trans(LOCSTR("Bonus Multiply")),
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("bonus_get"), 0);
        break;
      case BONUS_MULTIPLIER_4:
        state.bonusMult *= 4;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "4x " + sdl2w::L10n::trans(LOCSTR("Bonus Multiply")),
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("bonus_get"), 0);
        break;
      case BONUS_MULTIPLIER_5:
        state.bonusMult *= 5;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "5x " + sdl2w::L10n::trans(LOCSTR("Bonus Multiply")),
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("bonus_get"), 0);
        break;
      }

    } else {
      switch (powerup.type) {
      case EXTRA_LIFE:
        state.lives++;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+" + sdl2w::L10n::trans(LOCSTR("Life")),
                          SDL_Color{255, 255, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(state, std::make_unique<actions::PlaySound>("ahh"), 0);
        break;
      case ARMOR_UP:
        state.ship.armorCount++;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+" + sdl2w::L10n::trans(LOCSTR("Armor")),
                          SDL_Color{255, 255, 34, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("protection"), 0);
        break;
      case SHIELD_RECHARGE:
        structGaugeSetEmpty(state.ship.shieldGauge);
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+" + sdl2w::L10n::trans(LOCSTR("Shield")),
                          SDL_Color{65, 63, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("score_add"), 0);
        break;
      case POINTS_CANDY_SMALL:
        state.score += 100;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+100",
                          SDL_Color{255, 255, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("vortex_item_get"), 0);
        break;
      case POINTS_CANDY_MEDIUM:
        state.score += 500;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+500",
                          SDL_Color{255, 78, 78, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("vortex_item_get"), 0);
        break;
      case GUN_LEVEL_UP:
        state.ship.burstCount++;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+" + sdl2w::L10n::trans(LOCSTR("Gun")),
                          SDL_Color{255, 255, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("vortex_yes"), 0);
        break;
      case GUN_LARGE:
        state.ship.numBigShots += 75;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+75 " + sdl2w::L10n::trans(LOCSTR("Big Gun")),
                          SDL_Color{255, 100, 100, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state,
            std::make_unique<actions::SelectNextWeapon>(SELECTED_GUN_LARGE),
            0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("vortex_yes"), 0);
        break;
      case GUN_RAPID_FIRE:
        state.ship.numRapidShots += 300;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+300 " + sdl2w::L10n::trans(LOCSTR("Rapid Gun")),
                          SDL_Color{255, 100, 100, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state,
            std::make_unique<actions::SelectNextWeapon>(SELECTED_GUN_RAPID),
            0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("vortex_yes"), 0);
        break;
      case GUN_SPREAD_FIRE:
        state.ship.numSpreadShots += 50;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+50 " + sdl2w::L10n::trans(LOCSTR("Spread Gun")),
                          SDL_Color{255, 100, 100, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state,
            std::make_unique<actions::SelectNextWeapon>(SELECTED_GUN_SPREAD),
            0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("vortex_yes"), 0);
        break;
      case POINTS_STAR:
        state.score += 1000;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+1000",
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("star_get"), 0);
        break;
      case BONUS_SHOOTING_STAR:
        state.bonus += 1000;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "+1000 " + sdl2w::L10n::trans(LOCSTR("Bonus")),
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("star_get"), 0);
        break;
      case BONUS_MULTIPLIER_2:
        state.bonusMult *= 2;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "2x " + sdl2w::L10n::trans(LOCSTR("Bonus Multiply")),
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("bonus_get"), 0);
        break;
      case BONUS_MULTIPLIER_3:
        state.bonusMult *= 3;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "3x " + sdl2w::L10n::trans(LOCSTR("Bonus Multiply")),
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("bonus_get"), 0);
        break;
      case BONUS_MULTIPLIER_4:
        state.bonusMult *= 4;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "4x " + sdl2w::L10n::trans(LOCSTR("Bonus Multiply")),
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("bonus_get"), 0);
        break;
      case BONUS_MULTIPLIER_5:
        state.bonusMult *= 5;
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          "5x " + sdl2w::L10n::trans(LOCSTR("Bonus Multiply")),
                          SDL_Color{100, 100, 255, 255},
                          std::make_pair(powerup.physics.x, powerup.physics.y)),
                      0);
        enqueueAction(
            state, std::make_unique<actions::PlaySound>("bonus_get"), 0);
        break;
      }
    }

    enqueueAction(state, std::make_unique<actions::RemovePowerup>(powerup), 0);
  }

public:
  Powerup& powerup;
  double collideAngle = 0.;
  bool isBullet = false;

  CollidePowerup(Powerup& powerup, double collideAngle, bool isBullet)
      : powerup(powerup), collideAngle(collideAngle), isBullet(isBullet) {}
};
}; // namespace actions
} // namespace program