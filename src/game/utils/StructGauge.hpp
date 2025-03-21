#pragma once

namespace program {

struct StructGauge {
  double max = 0;
  double v = 0;
};

inline void structGaugeSetEmpty(StructGauge& g) { g.v = 0; }

inline void structTimerSetFull(StructGauge& g) { g.v = g.max; }

inline void structGaugeFill(StructGauge& g, double amt) {
  g.v += amt;
  if (g.v > g.max) {
    g.v = g.max;
  } else if (g.v < 0) {
    g.v = 0;
  }
}

inline double structGaugeGetPct(const StructGauge& g) { return g.v / g.max; }

inline bool structGaugeIsEmpty(const StructGauge& g) { return g.v <= 0; }

inline bool structGaugeIsFull(const StructGauge& g) { return g.v >= g.max; }

} // namespace program