#pragma once

#include <functional>

namespace sdl2w {
class Timer {

protected:
  bool removeFlag;
  double aggTime;
  double maxTime;

public:
  Timer(int maxTimeMs);
  bool isComplete() const;
  double getPctComplete() const;
  void restart();
  double getMs() const;
  void setMs(int msA);
  virtual bool shouldRemove() const;
  virtual void remove();
  virtual void update(double dt);
};

class FuncTimer : public Timer {
  std::function<void()> cb;

public:
  FuncTimer(int maxTimeMs, std::function<void()> cbA);
  void remove() override;
};

class BoolTimer : public Timer {
  bool& ref;

public:
  BoolTimer(int maxTimeMs, bool& refA);
  void remove() override;
};

} // namespace sdl2w
