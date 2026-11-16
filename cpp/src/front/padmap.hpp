// What a game controller's buttons do: not the original's, which knew the
// mouse and the keyboard only.
//
// Every button carries one action of a fixed list -- a mouse button, a key
// of the main screen, a menu item, or one of the two the controller has to
// itself: asking the Military Advisor about the enemy under the pointer, and
// planning a route there without setting off (the keyboard's Shift-click and
// Alt-click, in one press). The defaults are made for the Steam Deck under
// Steam's Gamepad layout; Game > Controller changes them, and they are kept
// in the remake's own settings, a button never changed left to its default.
#pragma once

#include <string>
#include <vector>

namespace padmap {

enum Kind {
  NOTHING,
  MOUSE,     // a mouse button, held while the button is: n is 1 or 2
  HOLD,      // a key held and nothing more: Shift, Alt
  KEY,       // a key pressed, as the keyboard's
  WHEEL,     // the map's zoom: n is +1 or -1
  CONTROL,   // one of the main screen's buttons, by its id
  MENU,      // a menu item, by its accelerator
  ADVISE,    // the Military Advisor, on the enemy under the pointer
  PLAN,      // a route to the tile under the pointer, drawn and not walked
  PREVIOUS   // the army before this one, Next Army's order backwards
};

struct Action {
  const char* id;      // as the settings name it
  const char* label;
  Kind kind;
  const char* arg;     // the key, or the menu item's accelerator
  int n;               // the mouse button, the wheel's way, or the control
  int control;         // the main screen's button that does the same, 0 for none
};
/** Every action a button can carry, "none" first. */
const std::vector<Action>& actions();
/** An action by its id; "none" for one unknown. */
const Action& action(const std::string& id);

struct Button {
  int code;            // pad::Button
  const char* label;   // as the Steam Deck names it
};
/** The buttons that can be set. */
const std::vector<Button>& buttons();

/** What a button does. */
const Action& bound(int button);
const Action& standard(int button);
void bind(int button, const std::string& actionId);
/** Every button back to its default. */
void reset();
/** The first button that carries an action, or -1. */
int buttonFor(const std::string& actionId);
/** How many buttons carry it. */
int count(const std::string& actionId);

/** The left stick is the pointer and the right one slides the map, not the
 *  other way round. */
bool swapped();
void setSwapped(bool on);

/** Each button's symbol is shown by what it does on the screen: by default
 *  while a controller is there. */
bool hints();
void setHints(bool on);

}  // namespace padmap
