// Game controllers, through SDL's GameController API: the Steam Deck's own
// controls as Steam Input hands them to the game (a virtual Xbox pad), and
// any pad plugged in or paired. Not the original's, which knew the mouse and
// the keyboard only.
//
// This layer only reads the pads: buttons by position, the triggers as two
// more buttons, and the sticks past their dead zones. What they do is the
// front end's (main.cpp > the controller).
#pragma once

#include <string>

union SDL_Event;

namespace pad {

// SDL's buttons (SDL_GameControllerButton) by position, the bottom face
// button A, and the triggers after them. The paddles are the Steam Deck's
// back grips, where Steam hands them on: R4, L4, R5, L5.
enum Button {
  A = 0, B, X, Y, BACK, GUIDE, START, LEFT_STICK, RIGHT_STICK, LEFT_SHOULDER, RIGHT_SHOULDER,
  DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT, MISC1, PADDLE1, PADDLE2, PADDLE3, PADDLE4,
  LEFT_TRIGGER = 100, RIGHT_TRIGGER = 101
};

/** SDL's hints, set before SDL_Init: left to the environment where it sets
 *  them. */
void hints();

/** A controller's event: pads opened and closed as they come and go, and a
 *  button (or a trigger pulled past halfway) pressed or let go. True when
 *  `button` and `down` were set. */
bool event(const SDL_Event& e, int& button, bool& down);
/** The triggers: read once a frame, as SDL sends their travel and not a
 *  press. */
bool trigger(int& button, bool& down);

struct Sticks {
  double lx = 0, ly = 0, rx = 0, ry = 0;   // -1..1 each, 0 inside the dead zone
};
/** Every pad's sticks, together. */
Sticks sticks();
/** A script's sticks in place of the pads' (stick LX LY RX RY), until set back
 *  to nothing. */
void script(const Sticks& s);

/** A button by SDL's name for it ("a", "dpup", "leftshoulder"), or
 *  "lefttrigger" and "righttrigger"; -1 for none. */
int fromName(const std::string& name);
/** SDL's name for a button, as fromName reads it; "" for none. */
std::string name(int button);
/** Is a pad there to play with? */
bool connected();

}  // namespace pad
