#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace sdl2w {
class AnimationDefinition;

struct AnimSprite {
  std::string name = "";
  unsigned int duration = 100;
  bool flip = false;
};

class Animation {
public:
  std::string name;
  unsigned int totalDuration;
  uint64_t timestampStart;
  unsigned int spriteIndex;
  bool loop;
  std::vector<AnimSprite> sprites;

  Animation();
  Animation(const std::string& nameA, const bool loopA);
  Animation(const Animation& a);
  ~Animation();

  Animation& operator=(const Animation& a);

  bool isInitialized() const;
  const AnimSprite& getCurrentSprite() const;
  std::string toString() const;
  unsigned int getAnimIndex() const;

  void
  addSprite(const std::string& spriteName, unsigned int ms, bool flip);
  void start();
  void update();
};

class AnimationDefinition : public Animation {
public:
  AnimationDefinition(const std::string& nameA, const bool loopA);
  ~AnimationDefinition();
};
} // namespace sdl2w