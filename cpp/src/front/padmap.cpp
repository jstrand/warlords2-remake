#include "front/padmap.hpp"

#include <map>

#include "front/prefs.hpp"
#include "platform/pad.hpp"

namespace padmap {

const std::vector<Action>& actions() {
  // the controls are the main screen's (17be:0b0c), the keys 17be:0064's
  static const std::vector<Action> A = {
      {"none", "Nothing", NOTHING, "", 0, 0},
      {"click", "Click", MOUSE, "", 1, 0},
      {"info", "Right click", MOUSE, "", 2, 0},
      {"advise", "Ask the Advisor", ADVISE, "", 0, 0},
      {"plan", "Plan a route", PLAN, "", 0, 0},
      {"walk", "Walk the route", KEY, "end", 0, 173},
      {"next", "Next / OK", KEY, "return", 0, 174},
      {"previous", "Previous army", PREVIOUS, "", 0, 0},
      {"done", "Leave / Cancel", KEY, "escape", 0, 175},
      {"defend", "Defend", CONTROL, "", 176, 176},
      {"deselect", "Deselect", KEY, "delete", 0, 178},
      {"group", "Group the stack", KEY, "space", 0, 240},
      {"centre", "Centre on army", KEY, "home", 0, 177},
      {"goal", "See destination", KEY, "tab", 0, 186},
      {"forget", "Forget route", KEY, "backspace", 0, 187},
      {"north", "Step north", KEY, "8", 0, 0},
      {"south", "Step south", KEY, "2", 0, 0},
      {"west", "Step west", KEY, "4", 0, 0},
      {"east", "Step east", KEY, "6", 0, 0},
      {"zoomin", "Zoom in", WHEEL, "", 1, 0},
      {"zoomout", "Zoom out", WHEEL, "", -1, 0},
      {"shift", "Hold Shift", HOLD, "lshift", 0, 0},
      {"alt", "Hold Alt", HOLD, "lalt", 0, 0},
      {"endturn", "End Turn", MENU, "alt E", 0, 0},
      {"moveall", "Move All", MENU, "m", 0, 0},
      {"search", "Search", MENU, "z", 0, 0},
      {"stack", "View stack", MENU, "s", 0, 0},
      {"production", "Production", MENU, "p", 0, 0},
      {"build", "Build", MENU, "b", 0, 0},
      {"vectoring", "Vectoring", MENU, "v", 0, 0},
      {"cities", "View cities", MENU, "c", 0, 0},
      {"ruins", "View ruins", MENU, ".", 0, 0},
      {"inspect", "Inspect heroes", MENU, ",", 0, 0},
      {"quest", "Quest", MENU, "=", 0, 0},
      {"diplomacy", "Diplomacy", MENU, "d", 0, 0},
      {"armies", "Army report", MENU, "a", 0, 0},
      {"quicksave", "Quick save", KEY, "f5", 0, 0},
  };
  return A;
}

const Action& action(const std::string& id) {
  for (const Action& a : actions()) if (id == a.id) return a;
  return actions()[0];
}

const std::vector<Button>& buttons() {
  static const std::vector<Button> B = {
      {pad::A, "A"}, {pad::B, "B"}, {pad::X, "X"}, {pad::Y, "Y"},
      {pad::DPAD_UP, "D-pad up"}, {pad::DPAD_DOWN, "D-pad down"}, {pad::DPAD_LEFT, "D-pad left"},
      {pad::DPAD_RIGHT, "D-pad right"},
      {pad::LEFT_SHOULDER, "L1"}, {pad::RIGHT_SHOULDER, "R1"}, {pad::LEFT_TRIGGER, "L2"}, {pad::RIGHT_TRIGGER, "R2"},
      {pad::LEFT_STICK, "L3"}, {pad::RIGHT_STICK, "R3"}, {pad::BACK, "View"}, {pad::START, "Menu"},
      {pad::PADDLE2, "L4"}, {pad::PADDLE4, "L5"}, {pad::PADDLE1, "R4"}, {pad::PADDLE3, "R5"},
  };
  return B;
}

// Steam's Gamepad layout makes the Deck's right trackpad the right stick,
// and its click R3: so R3 clicks, under the thumb that steers the pointer.
const Action& standard(int button) {
  static const std::map<int, const char*> D = {
      {pad::A, "click"}, {pad::RIGHT_TRIGGER, "click"}, {pad::RIGHT_STICK, "click"},
      {pad::X, "info"}, {pad::LEFT_TRIGGER, "info"},
      {pad::B, "done"}, {pad::Y, "next"},
      {pad::LEFT_SHOULDER, "zoomout"}, {pad::RIGHT_SHOULDER, "zoomin"},
      {pad::DPAD_UP, "north"}, {pad::DPAD_DOWN, "south"}, {pad::DPAD_LEFT, "west"}, {pad::DPAD_RIGHT, "east"},
      {pad::BACK, "advise"}, {pad::START, "plan"},
      {pad::LEFT_STICK, "group"},
      {pad::PADDLE2, "shift"}, {pad::PADDLE4, "alt"}, {pad::PADDLE1, "walk"}, {pad::PADDLE3, "centre"},
  };
  auto d = D.find(button);
  return action(d == D.end() ? "none" : d->second);
}

static std::string key(int button) { return "pad " + pad::name(button); }

const Action& bound(int button) {
  std::string v = prefs::get(key(button));
  return v.empty() ? standard(button) : action(v);
}

void bind(int button, const std::string& actionId) {
  const Action& a = action(actionId);
  prefs::set(key(button), &a == &standard(button) ? "" : a.id);
}

void reset() {
  for (const Button& b : buttons()) prefs::set(key(b.code), "");
  prefs::set("pad sticks", "");
}

int buttonFor(const std::string& actionId) {
  for (const Button& b : buttons()) if (actionId == bound(b.code).id) return b.code;
  return -1;
}

int count(const std::string& actionId) {
  int n = 0;
  for (const Button& b : buttons()) if (actionId == bound(b.code).id) n++;
  return n;
}

bool swapped() { return prefs::get("pad sticks") == "swapped"; }
void setSwapped(bool on) { prefs::set("pad sticks", on ? "swapped" : ""); }

bool hints() {
  std::string v = prefs::get("pad hints");
  return v.empty() ? pad::connected() : v == "on";
}
void setHints(bool on) { prefs::set("pad hints", on ? "on" : "off"); }

}  // namespace padmap
