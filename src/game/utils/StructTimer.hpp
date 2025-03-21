#pragma once

namespace program {

struct StructTimer {
  int duration = 0;
  int t = 0;
};

inline void structTimerStart(StructTimer& timer, const int duration) {
  timer.t = 0;
  timer.duration = duration;
}

inline void structTimerStart(StructTimer& timer) {
  timer.t = 0;
}

inline bool structTimerIsComplete(const StructTimer& timer) {
  return timer.t >= timer.duration;
}

inline bool structTimerIsRunning(const StructTimer& timer) {
  return timer.t < timer.duration;
}

inline void structTimerUpdate(StructTimer& timer, const int dt) {
  if (structTimerIsRunning(timer)) {
    timer.t += dt;
  }
}

} // namespace program