// What Lua gave the remake for free and JavaScript does not.

/**
 * Lua 5.1's table.sort, as LuaJIT runs it (lib_table.c's auxsort): not stable,
 * so arrays with equal keys come out in the order the Lua remake leaves them,
 * which keeps a seeded game the same in both. `lt(a, b)` is "a sorts before
 * b"; the array is sorted in place and returned.
 */
export function luaSort(a, lt = (x, y) => x < y) {
  // 1-based, as the original algorithm is written
  const get = (i) => a[i - 1];
  const set = (i, v) => { a[i - 1] = v; };
  const swap = (i, j) => { const t = a[i - 1]; a[i - 1] = a[j - 1]; a[j - 1] = t; };
  function auxsort(l, u) {
    while (l < u) {
      if (lt(get(u), get(l))) swap(l, u);
      if (u - l === 1) break;
      let i = Math.floor((l + u) / 2);
      if (lt(get(i), get(l))) swap(i, l);
      else if (lt(get(u), get(i))) swap(i, u);
      if (u - l === 2) break;
      const P = get(i);
      swap(i, u - 1);
      i = l;
      let j = u - 1;
      for (;;) {
        while (lt(get(++i), P)) {
          if (i > u) throw new Error("invalid order function for sorting");
        }
        while (lt(P, get(--j))) {
          if (j < l) throw new Error("invalid order function for sorting");
        }
        if (j < i) break;
        swap(i, j);
      }
      swap(u - 1, i);
      if (i - l < u - i) {
        j = l; i = i - 1; l = i + 2;
      } else {
        j = i + 1; i = u; u = j - 2;
      }
      auxsort(j, i);
    }
  }
  auxsort(1, a.length);
  return a;
}

/** Lua's string.format, for the conversions the remake uses. */
export function fmt(f, ...args) {
  let n = 0;
  return f.replace(/%([-+ 0#]*)(\d*)(?:\.(\d+))?([diuxXfgsc%])/g, (m, flags, width, prec, conv) => {
    if (conv === "%") return "%";
    let v = args[n++];
    let s;
    switch (conv) {
      case "d": case "i": case "u":
        v = Math.trunc(Number(v) || 0);
        s = String(Math.abs(v));
        if (prec !== undefined) s = s.padStart(Number(prec), "0");
        if (v < 0) s = "-" + s;
        else if (flags.includes("+")) s = "+" + s;
        break;
      case "x": case "X":
        s = (Math.trunc(Number(v) || 0) >>> 0).toString(16);
        if (conv === "X") s = s.toUpperCase();
        break;
      case "f":
        s = Number(v).toFixed(prec === undefined ? 6 : Number(prec));
        break;
      case "g":
        s = String(Number(v));
        break;
      case "c":
        s = String.fromCharCode(v);
        break;
      default:
        s = v === undefined || v === null ? "nil" : String(v);
        if (prec !== undefined) s = s.slice(0, Number(prec));
    }
    const w = Number(width || 0);
    if (s.length < w) {
      if (flags.includes("-")) s = s.padEnd(w, " ");
      else if (flags.includes("0") && conv !== "s") {
        const neg = s[0] === "-" || s[0] === "+";
        s = (neg ? s[0] : "") + s.slice(neg ? 1 : 0).padStart(w - (neg ? 1 : 0), "0");
      } else s = s.padStart(w, " ");
    }
    return s;
  });
}

/** Bytes to a string of one character a byte, as Lua strings are. */
export function latin1(bytes) {
  let s = "";
  const CH = 0x8000;
  for (let i = 0; i < bytes.length; i += CH) {
    s += String.fromCharCode.apply(null, bytes.subarray(i, i + CH));
  }
  return s;
}

/** A string of byte characters back to bytes. */
export function bytesOf(s) {
  const out = new Uint8Array(s.length);
  for (let i = 0; i < s.length; i++) out[i] = s.charCodeAt(i) & 0xff;
  return out;
}

/** Lua's `#t` on a table used as a list. */
export const len = (t) => (t ? t.length : 0);

/** Remove and return the last element, or the one at `i` (table.remove). */
export function remove(t, i) {
  if (i === undefined) return t.pop();
  return t.splice(i, 1)[0];
}

/** Lua's math.random(n): 1..n. */
export const random = (n) => Math.floor(Math.random() * Math.max(1, n)) + 1;

/** Wall time in seconds, for what plays out by the clock. */
export const now = () => (typeof performance !== "undefined" ? performance.now() : Date.now()) / 1000;
