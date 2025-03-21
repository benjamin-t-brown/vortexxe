#pragma once

// An Action is a segment of code + context that can be run in sequence or after a period
// of time.  This code's purpose is to alter the state of the game.  By chaining action
// sequences, the game can temporally control the flow of what it shows and how it
// behaves.

// Persistent actions are actions that are run every frame.

// All actions have access to a mutable reference of state.  This state can can be
// modified how the actions please save for a few critical rules:
//  - Actions cannot add themselves to the list of actions currently running
//  - Actions cannot remove themselves from the list of actions currently running
// Instead a helper function to enqueue new actions is provided, and these will be
// added to the queue after all currently-executing actions.

#include "game/utils/StructTimer.hpp"
#include "lib/sdl2wrapper/Logger.h"
#include "logger.h"
#include <memory>
#ifdef __GNUG__
#include <cxxabi.h>
#endif

namespace program {

struct State;

namespace actions {

using Logger = sdl2w::Logger;

class AbstractAction {
protected:
  State* statePtr = nullptr;

  virtual void act() { logger::warn("AbstractAction::act() called noop"); };

public:
  virtual std::string getName() const {
#ifdef __GNUG__
    int status;
    char* realname = abi::__cxa_demangle(typeid(*this).name(), 0, 0, &status);
    std::string name = (status == 0) ? realname : typeid(*this).name();
    free(realname);
    return name;
#else
    return typeid(*this).name();
#endif
  }

  void setState(State* statePtr) { this->statePtr = statePtr; }

  void execute(State* statePtr) {
    this->statePtr = statePtr;
    act();
  }
};

struct AsyncAction {
  std::unique_ptr<actions::AbstractAction> action;
  StructTimer timer;
};

} // namespace actions

} // namespace program