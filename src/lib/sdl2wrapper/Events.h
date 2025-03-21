#pragma once

#include <functional>
#include <map>
#include <memory>
#include <stack>
#include <string>

union SDL_Event;

namespace sdl2w {

class Window;
class EventRoute {
public:
  std::function<void(int, int, int)> onmousedown;
  std::function<void(int, int, int)> onmouseup;
  std::function<void(int, int, int)> onmousemove;
  std::function<void(const std::string&)> onkeydown;
  std::function<void(const std::string&)> onkeyup;
  std::function<void(const std::string&)> onkeypress;

  EventRoute();
};

class Events {
private:
  Window& window;
  std::stack<std::unique_ptr<EventRoute>> routes;
  std::map<std::string, bool> keys;
  bool shouldPushRoute;
  bool shouldPopRoute;
  std::function<void(SDL_Event)> cb;

public:
  bool isMouseDown;
  bool isRightMouseDown;
  bool isMiddleMouseDown;
  int mouseX;
  int mouseY;
  int mouseDownX;
  int mouseDownY;
  int wheel;

  static const int MOUSE_BUTTON_LEFT;
  static const int MOUSE_BUTTON_MIDDLE;
  static const int MOUSE_BUTTON_RIGHT;

  Events(Window& windowA);
  ~Events();
  bool isKeyPressed(const std::string& name) const;
  bool isCtrl() const;

  void pushRoute();
  void pushRouteNextTick();
  void popRoute();
  void popRouteNextTick();
  void setMouseEvent(const std::string& name, std::function<void(int, int, int)> cb);
  void setKeyboardEvent(const std::string& name,
                        std::function<void(const std::string&)> cb);

  void mousedown(int x, int y, int button);
  void mouseup(int x, int y, int button);
  void mousemove(int x, int y);
  void keydown(int key);
  void keyup(int key);

  void handleEvent(SDL_Event e);
  void setEventHandler(std::function<void(SDL_Event)> cbA);

  void update();
};
} // namespace sdl2w