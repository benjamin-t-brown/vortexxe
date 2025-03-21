#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/ui/PlaySound.hpp"
#include "game/state.h"

namespace program {
namespace actions {

class SelectNextWeapon : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    int nextSelectedGun =
        overrideGun == -1 ? state.ship.selectedGun : (overrideGun - 1);

    bool selecting = true;
    while (selecting) {
      nextSelectedGun++;
      if (nextSelectedGun == SELECTED_GUN_LARGE) {
        if (state.ship.numBigShots > 0) {
          selecting = false;
        }
      } else if (nextSelectedGun == SELECTED_GUN_RAPID) {
        if (state.ship.numRapidShots > 0) {
          selecting = false;
        }
      } else if (nextSelectedGun == SELECTED_GUN_SPREAD) {
        if (state.ship.numSpreadShots > 0) {
          selecting = false;
        }
      } else {
        nextSelectedGun = SELECTED_GUN_NORMAL;
        selecting = false;
      }
    }
    state.ship.selectedGun = static_cast<ShipGunSelected>(nextSelectedGun);

    enqueueAction(state, std::make_unique<actions::PlaySound>("score_add"), 0);
  }

public:
  int overrideGun = -1;

  SelectNextWeapon(int overrideGun = -1) : overrideGun(overrideGun) {}
};
}; // namespace actions
} // namespace program