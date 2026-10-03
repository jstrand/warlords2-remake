// Reading the original's little-endian records. Offsets are 0-based, as in
// docs/formats.

export function u16(b, off) {
  return b[off] + b[off + 1] * 256;
}

export function i16(b, off) {
  const v = u16(b, off);
  return v >= 0x8000 ? v - 0x10000 : v;
}

/** A NUL-terminated string in a fixed field, one character a byte. */
export function cstr(b, off, len) {
  let s = "";
  for (let i = off; i < off + len && i < b.length; i++) {
    if (b[i] === 0) break;
    s += String.fromCharCode(b[i]);
  }
  return s;
}
