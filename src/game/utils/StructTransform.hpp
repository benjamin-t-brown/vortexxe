#pragma once

#include <utility>

namespace program {
struct StructTransform {
  int t = 0;
  int duration = 0;
  bool isMoving = false;

  int x = 0;
  int y = 0;

  int prevX = 0;
  int prevY = 0;

  int nextX = 0;
  int nextY = 0;
};

inline int normalize(int x, int a, int b, int c, int d) {
  return c + ((x - a) * (d - c)) / (b - a);
}

inline void structTransformMoveTo(StructTransform& transform,
                                  const std::pair<int, int> nextPos,
                                  const int tMs) {
  const auto [nextX, nextY] = nextPos;
  transform.t = 0;
  transform.duration = tMs;
  transform.prevX = transform.x;
  transform.prevY = transform.y;
  transform.nextX = nextX;
  transform.nextY = nextY;
  transform.isMoving = true;
}

inline void structTransformSetAt(StructTransform& transform,
                                 const std::pair<int, int> pos) {
  const auto [x, y] = pos;
  transform.x = x;
  transform.y = y;
  transform.prevX = x;
  transform.prevY = y;
  transform.nextX = x;
  transform.nextY = y;
  transform.t = 1;
  transform.duration = 1;
  transform.isMoving = false;
}

inline bool structTransformIsMoving(const StructTransform& transform) {
  return transform.isMoving;
}

inline void structTransformUpdate(StructTransform& transform, const int dt) {
  if (!structTransformIsMoving(transform)) {
    return;
  }
  transform.t += dt;
  if (transform.t >= transform.duration) {
    transform.x = transform.nextX;
    transform.y = transform.nextY;
    transform.isMoving = false;
  } else {
    transform.x = normalize(
        transform.t, 0, transform.duration, transform.prevX, transform.nextX);
    transform.y = normalize(
        transform.t, 0, transform.duration, transform.prevY, transform.nextY);
  }
}

} // namespace program