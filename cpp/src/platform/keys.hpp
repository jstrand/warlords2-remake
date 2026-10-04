// Which keys are down, by LÖVE's names for them ("lshift", "ralt", "a",
// "return", "kp+" ...), which the front end and the dialogs ask about as the
// Lua remake asked love.keyboard.isDown.
#pragma once

#include <initializer_list>
#include <string>

namespace keys {

void press(const std::string& name);
void release(const std::string& name);
void clear();
/** Is any of the named keys down? */
bool isDown(std::initializer_list<const char*> names);
bool isDown(const std::string& name);

/** An SDL keycode as LÖVE names the key, or "" for none it knows. */
std::string loveName(int sdlKeycode);

}  // namespace keys
