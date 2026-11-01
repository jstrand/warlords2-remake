#include "platform/pad.hpp"

#include <SDL.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <optional>

namespace pad {

namespace {
struct Pad {
  SDL_GameController* c = nullptr;
  bool lt = false, rt = false;   // the triggers, as buttons
};
std::map<SDL_JoystickID, Pad> pads;
std::optional<Sticks> scripted;

const double DEAD = 0.15;                      // of the stick's travel, all round
const double PULL = 0.5, LET_GO = 0.3;         // a trigger's press, and its release

// One stick, its dead zone taken out and the rest stretched over 0..1, so
// the slowest it goes is the slowest there is.
void stick(SDL_GameController* c, SDL_GameControllerAxis ax, SDL_GameControllerAxis ay, double& x, double& y) {
  double sx = SDL_GameControllerGetAxis(c, ax) / 32767.0, sy = SDL_GameControllerGetAxis(c, ay) / 32767.0;
  double m = std::hypot(sx, sy);
  if (m <= DEAD) return;
  double k = std::min(1.0, (m - DEAD) / (1 - DEAD)) / m;
  x += sx * k;
  y += sy * k;
}
}

void hints() {
  // SDL's own Steam Deck driver turns the trackpads' mouse ("lizard mode")
  // off, and reports no trackpads: with Steam running, Steam has the Deck's
  // controls anyway and hands the game a virtual pad, and without it the
  // trackpad stays the mouse
  SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_STEAMDECK, "0");
  // A is the bottom face button on every pad, whatever its label says
  SDL_SetHint(SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS, "0");
  // the Deck's on-screen keyboard, one line, Enter closes it
  SDL_SetHint(SDL_HINT_RETURN_KEY_HIDES_IME, "1");
}

bool event(const SDL_Event& e, int& button, bool& down) {
  switch (e.type) {
    case SDL_CONTROLLERDEVICEADDED: {
      SDL_GameController* c = SDL_GameControllerOpen(e.cdevice.which);
      if (c) pads[SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(c))] = Pad{c};
      return false;
    }
    case SDL_CONTROLLERDEVICEREMOVED: {
      auto p = pads.find(e.cdevice.which);
      if (p != pads.end()) {
        SDL_GameControllerClose(p->second.c);
        pads.erase(p);
      }
      return false;
    }
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
      button = e.cbutton.button;
      down = e.type == SDL_CONTROLLERBUTTONDOWN;
      return true;
  }
  return false;
}

bool trigger(int& button, bool& down) {
  for (auto& [id, p] : pads) {
    for (int t = 0; t < 2; t++) {
      bool& was = t == 0 ? p.lt : p.rt;
      double v = SDL_GameControllerGetAxis(p.c, t == 0 ? SDL_CONTROLLER_AXIS_TRIGGERLEFT : SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0;
      if (was ? v < LET_GO : v > PULL) {
        was = !was;
        button = t == 0 ? LEFT_TRIGGER : RIGHT_TRIGGER;
        down = was;
        return true;
      }
    }
  }
  return false;
}

Sticks sticks() {
  if (scripted) return *scripted;
  Sticks s;
  for (auto& [id, p] : pads) {
    stick(p.c, SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY, s.lx, s.ly);
    stick(p.c, SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY, s.rx, s.ry);
  }
  s.lx = std::clamp(s.lx, -1.0, 1.0);
  s.ly = std::clamp(s.ly, -1.0, 1.0);
  s.rx = std::clamp(s.rx, -1.0, 1.0);
  s.ry = std::clamp(s.ry, -1.0, 1.0);
  return s;
}

void script(const Sticks& s) {
  if (s.lx == 0 && s.ly == 0 && s.rx == 0 && s.ry == 0) scripted.reset();
  else scripted = s;
}

int fromName(const std::string& name) {
  if (name == "lefttrigger") return LEFT_TRIGGER;
  if (name == "righttrigger") return RIGHT_TRIGGER;
  return SDL_GameControllerGetButtonFromString(name.c_str());
}

}  // namespace pad
