#include "platform/keys.hpp"

#include <SDL.h>

#include <set>

namespace keys {

namespace {
std::set<std::string> down;
}

void press(const std::string& name) { down.insert(name); }
void release(const std::string& name) { down.erase(name); }
void clear() { down.clear(); }

bool isDown(std::initializer_list<const char*> names) {
  for (auto n : names) if (down.count(n)) return true;
  return false;
}
bool isDown(const std::string& name) { return down.count(name) > 0; }

std::string loveName(int k) {
  switch (k) {
    case SDLK_LSHIFT: return "lshift";
    case SDLK_RSHIFT: return "rshift";
    case SDLK_LCTRL: return "lctrl";
    case SDLK_RCTRL: return "rctrl";
    case SDLK_LALT: return "lalt";
    case SDLK_RALT: return "ralt";
    case SDLK_LGUI: return "lgui";
    case SDLK_RGUI: return "rgui";
    case SDLK_RETURN: return "return";
    case SDLK_KP_ENTER: return "kpenter";
    case SDLK_ESCAPE: return "escape";
    case SDLK_BACKSPACE: return "backspace";
    case SDLK_TAB: return "tab";
    case SDLK_SPACE: return "space";
    case SDLK_DELETE: return "delete";
    case SDLK_INSERT: return "insert";
    case SDLK_HOME: return "home";
    case SDLK_END: return "end";
    case SDLK_PAGEUP: return "pageup";
    case SDLK_PAGEDOWN: return "pagedown";
    case SDLK_UP: return "up";
    case SDLK_DOWN: return "down";
    case SDLK_LEFT: return "left";
    case SDLK_RIGHT: return "right";
    case SDLK_KP_PLUS: return "kp+";
    case SDLK_KP_MINUS: return "kp-";
    case SDLK_KP_MULTIPLY: return "kp*";
    case SDLK_KP_DIVIDE: return "kp/";
    case SDLK_KP_PERIOD: return "kp.";
    case SDLK_KP_EQUALS: return "kp=";
    case SDLK_KP_0: return "kp0";
    case SDLK_KP_1: return "kp1";
    case SDLK_KP_2: return "kp2";
    case SDLK_KP_3: return "kp3";
    case SDLK_KP_4: return "kp4";
    case SDLK_KP_5: return "kp5";
    case SDLK_KP_6: return "kp6";
    case SDLK_KP_7: return "kp7";
    case SDLK_KP_8: return "kp8";
    case SDLK_KP_9: return "kp9";
    case SDLK_CAPSLOCK: return "capslock";
    case SDLK_NUMLOCKCLEAR: return "numlock";
    default: break;
  }
  if (k >= SDLK_F1 && k <= SDLK_F12) return "f" + std::to_string(k - SDLK_F1 + 1);
  if (k >= 32 && k < 127) {
    char c = (char)k;
    if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
    return std::string(1, c);
  }
  return "";
}

}  // namespace keys
