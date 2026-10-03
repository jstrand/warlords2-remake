// The remake's own settings, none of them the original's: the interface's
// scale, the map's zoom, full screen or a window, 4:3, and the synthesizer
// the music plays on. Kept in the browser's localStorage, a key each; a
// setting never changed is left to its default.

const PREFIX = "w2pref:";

/** A setting's value as written, a string; null when there is none. */
export function get(key) {
  try {
    return localStorage.getItem(PREFIX + key);
  } catch (e) {
    return null;
  }
}

/** Change a setting. */
export function set(key, value) {
  try {
    if (value === null || value === undefined) localStorage.removeItem(PREFIX + key);
    else localStorage.setItem(PREFIX + key, String(value));
  } catch (e) { /* no storage: it lasts the session */ }
}
