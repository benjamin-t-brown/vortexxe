#pragma once

#include "Timer.h"
#include <string>

namespace sdl2w {

enum Ease { LINEAR, EASE_OUT };

struct Vec3 {
  double x;
  double y;
  double z;

  Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
};

class Transform {
  Timer timer;
  Ease ease;

public:
  double x1;
  double y1;
  double z1;
  double x2;
  double y2;
  double z2;

  // Transform(int ms = 100,
  //           double x1A = 0.0,
  //           double x2A = 0.0,
  //           Ease ease = Ease::LINEAR);
  Transform(int ms = 100,
            double x1A = 0.0,
            double y1A = 0.0,
            double z1A = 0.0,
            double x2A = 0.0,
            double y2A = 0.0,
            double z2A = 0.0,
            Ease ease = Ease::LINEAR);

  static double easeOut(double t, double b, double c, double d);
  static double normalize(double x, double a, double b, double c, double d);
  static double
  normalizeClamp(double x, double a, double b, double c, double d);
  static double
  normalizeEaseOut(double x, double a, double b, double c, double d);
  static double
  normalizeEaseOutClamp(double x, double a, double b, double c, double d);

  std::string toString() const;

  Vec3 getStart() const;
  Vec3 getEnd() const;
  Vec3 getCurrent() const;

  void set(double startVal, double endVal);
  void
  set(double x1A, double y1A, double z1A, double x2A, double y2A, double z2A);

  void setEase(Ease ease);
  void setMs(int ms);
  float getPctComplete() const;

  Transform createInverse() const;

  void start();
  void update(double dt);
};

} // namespace sdl2w