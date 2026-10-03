// Which keys are down, by LÖVE's names for them ("lshift", "ralt", "a",
// "return", "kp+" ...), which the front end and the dialogs ask about as the
// Lua remake asked love.keyboard.isDown.

const down = new Set();

export function press(name) { down.add(name); }
export function release(name) { down.delete(name); }
export function clear() { down.clear(); }

/** Is any of the named keys down? */
export function isDown(...names) {
  for (const n of names) if (down.has(n)) return true;
  return false;
}

/** A KeyboardEvent as LÖVE names the key. */
export function loveName(e) {
  const code = e.code || "";
  const map = {
    ShiftLeft: "lshift", ShiftRight: "rshift", ControlLeft: "lctrl", ControlRight: "rctrl",
    AltLeft: "lalt", AltRight: "ralt", MetaLeft: "lgui", MetaRight: "rgui",
    Enter: "return", NumpadEnter: "kpenter", Escape: "escape", Backspace: "backspace",
    Tab: "tab", Space: "space", Delete: "delete", Insert: "insert",
    Home: "home", End: "end", PageUp: "pageup", PageDown: "pagedown",
    ArrowUp: "up", ArrowDown: "down", ArrowLeft: "left", ArrowRight: "right",
    NumpadAdd: "kp+", NumpadSubtract: "kp-", NumpadMultiply: "kp*", NumpadDivide: "kp/",
    NumpadDecimal: "kp.", Slash: "/", Comma: ",", Period: ".", Equal: "=", Minus: "-",
    Semicolon: ";", Quote: "'", BracketLeft: "[", BracketRight: "]", Backslash: "\\",
    Backquote: "`",
  };
  if (map[code]) return map[code];
  let m = code.match(/^Key([A-Z])$/);
  if (m) return m[1].toLowerCase();
  m = code.match(/^Digit(\d)$/);
  if (m) return m[1];
  m = code.match(/^Numpad(\d)$/);
  if (m) return "kp" + m[1];
  m = code.match(/^F(\d+)$/);
  if (m) return "f" + m[1];
  return (e.key || "").toLowerCase();
}
