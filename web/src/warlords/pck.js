// Warlords II .PCK images. The planar LZ77 format itself is decoded at build
// time (tools/web_assets.py writes each sheet as a PNG of its colour
// indices); here the indices become images in a palette, with colours keyed
// out or remapped as the original blits them. See docs/formats/pck.md.

import * as vfs from "../vfs.js";
import * as gfx from "../gfx.js";

/** [w, h, px]: px a Uint8Array of colour indices, 0-15. */
export function decode(path) {
  const img = vfs.image(path);
  return [img.w, img.h, img.px];
}

function toRGBA(w, h, px, palette, map, keys) {
  const out = new Uint8Array(w * h * 4);
  const lut = [];
  for (let i = 0; i < 16; i++) {
    const c = palette[i] || [0, 0, 0];
    lut[i] = [Math.floor(c[0] * 255), Math.floor(c[1] * 255), Math.floor(c[2] * 255)];
  }
  for (let i = 0; i < w * h; i++) {
    let idx = px[i];
    if (keys && keys[idx]) continue;           // left transparent
    if (map && map[idx] !== undefined) idx = map[idx];
    const c = lut[idx];
    const o = i * 4;
    out[o] = c[0]; out[o + 1] = c[1]; out[o + 2] = c[2]; out[o + 3] = 255;
  }
  return out;
}

/** An image from decoded indices, each passed through `map` (index -> index)
 *  if given; `keys` is a set of the sheet's own indices to leave clear. This
 *  is how a font is drawn in colours other than its own (78a8:0839). */
export function imageFromPixels(w, h, px, palette, map, keys) {
  return gfx.newImageRGBA(w, h, toRGBA(w, h, px, palette, map, keys));
}

/** Decode a .PCK into an image: [image, w, h, px]. `keyIndex` is the index to
 *  make transparent, a list of them, or "corner" for whatever colour the
 *  sheet's top left pixel is. */
export function toImage(path, palette, keyIndex) {
  const [w, h, px] = decode(path);
  const keys = {};
  if (keyIndex === "corner") keys[px[0]] = true;
  else if (Array.isArray(keyIndex)) for (const k of keyIndex) keys[k] = true;
  else if (keyIndex !== undefined && keyIndex !== null) keys[keyIndex] = true;
  return [imageFromPixels(w, h, px, palette, null, keys), w, h, px];
}
