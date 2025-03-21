#include "Transform.h"
#include "Logger.h"

namespace sdl2w {

double Transform::easeOut(double t, double b, double c, double d) {
  auto t2 = t / d;
  return -c * t2 * (t2 - 2) + b;
}
double Transform::normalize(double x, double a, double b, double c, double d) {
  return c + ((x - a) * (d - c)) / (b - a);
}
double
Transform::normalizeClamp(double x, double a, double b, double c, double d) {
  auto n = normalize(x, a, b, c, d);
  if (d > c) {
    if (n > d) {
      n = d;
    } else if (n < c) {
      n = c;
    }
  } else {
    if (n < d) {
      n = d;
    } else if (n > c) {
      n = c;
    }
  }
  return n;
}
double
Transform::normalizeEaseOut(double x, double a, double b, double c, double d) {
  auto t = normalize(x, a, b, 0, 1);
  return easeOut(t, c, d - c, 1);
}
double Transform::normalizeEaseOutClamp(
    double x, double a, double b, double c, double d) {
  auto t = normalizeClamp(x, a, b, 0, 1);
  return easeOut(t, c, d - c, 1);
}

// Transform::Transform(int ms,
//                      double x1A,
//                      double x2A,
//                      Ease ease)
//     : timer(Timer(ms)),
//       ease(ease),
//       x1(x1A),
//       y1(x1A),
//       z1(x1A),
//       x2(x2A),
//       y2(x2A),
//       z2(x2A) {}

Transform::Transform(int ms,
                     double x1A,
                     double y1A,
                     double z1A,
                     double x2A,
                     double y2A,
                     double z2A,
                     Ease ease)
    : timer(Timer(ms)),
      ease(ease),
      x1(x1A),
      y1(y1A),
      z1(z1A),
      x2(x2A),
      y2(y2A),
      z2(z2A) {}

std::string Transform::toString() const { return "[TRANSFORM]"; }

Vec3 Transform::getStart() const { return Vec3{x1, y1, z1}; }

Vec3 Transform::getEnd() const { return Vec3{x2, y2, z2}; }
Vec3 Transform::getCurrent() const {
  auto pct = timer.getPctComplete();

  const double offsetX = 0;
  const double offsetY = 0;
  const double offsetZ = 0;

  switch (ease) {
  case Ease::EASE_OUT: {
    return Vec3{
        normalizeEaseOutClamp(pct, 0, 1, x1, x2) + offsetX,
        normalizeEaseOutClamp(pct, 0, 1, y1, y2) + offsetY,
        normalizeEaseOutClamp(pct, 0, 1, z1, z2) + offsetZ,
    };
  }
  case Ease::LINEAR:
  default: {
    return Vec3{
        normalizeClamp(pct, 0, 1, x1, x2) + offsetX,
        normalizeClamp(pct, 0, 1, y1, y2) + offsetY,
        normalizeClamp(pct, 0, 1, z1, z2) + offsetZ,
    };
  }
  }
}
void Transform::set(double startVal, double endVal) {
  x1 = startVal;
  y1 = startVal;
  z1 = startVal;
  x2 = endVal;
  y2 = endVal;
  z2 = endVal;

  timer.restart();
}
void Transform::set(
    double x1A, double y1A, double z1A, double x2A, double y2A, double z2A) {
  x1 = x1A;
  y1 = y1A;
  z1 = z1A;
  x2 = x2A;
  y2 = y2A;
  z2 = z2A;

  timer.restart();
}

void Transform::setEase(Ease easeA) { ease = easeA; }
void Transform::setMs(int ms) { timer = Timer(ms); }
float Transform::getPctComplete() const { return timer.getPctComplete(); }

Transform Transform::createInverse() const {
  return Transform{static_cast<int>(timer.getMs()), x2, y2, z2, x1, y1, z1};
}

void Transform::start() { timer.restart(); }
void Transform::update(double dt) { timer.update(dt); }

} // namespace sdl2w