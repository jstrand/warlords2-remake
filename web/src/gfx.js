// The handful of love.graphics calls the Lua remake draws with, over a
// canvas 2D context, so the drawing code ports nearly line for line.
//
// What LÖVE does and the canvas does not, kept here:
//   - push/pop save the transform only; the scissor is a state of its own
//     that survives them. So the transform is kept here as a matrix (translate
//     and scale are all the game uses) and the scissor is the one thing the
//     context's save/restore is used for.
//   - setColor tints images as well as filling shapes. A tinted image is a
//     cached copy, made the first time that colour is asked of it; nearly
//     everything draws images in white.
//   - Images are nearest-neighbour, always.

let ctx = null;
let color = [1, 1, 1, 1];
let fill = "rgb(255,255,255)";
// the transform, x' = sx * x + tx, y' = sy * y + ty, in device pixels
let T = { sx: 1, sy: 1, tx: 0, ty: 0 };
const stack = [];
let clipped = false;

export function setContext(c) {
  ctx = c;
  ctx.imageSmoothingEnabled = false;
}
export function context() { return ctx; }

/** Start a frame: no transform, no scissor, white. */
export function begin() {
  if (clipped) { ctx.restore(); clipped = false; }
  T = { sx: 1, sy: 1, tx: 0, ty: 0 };
  stack.length = 0;
  setColor(1, 1, 1);
  ctx.setTransform(1, 0, 0, 1, 0, 0);
  ctx.imageSmoothingEnabled = false;
}

export function clear(r = 0, g = 0, b = 0) {
  if (clipped) { ctx.restore(); clipped = false; }
  ctx.setTransform(1, 0, 0, 1, 0, 0);
  ctx.fillStyle = `rgb(${r * 255},${g * 255},${b * 255})`;
  ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);
  ctx.fillStyle = fill;
}

function apply() {
  ctx.setTransform(T.sx, 0, 0, T.sy, T.tx, T.ty);
}

export function push() { stack.push({ ...T }); }
export function pop() { T = stack.pop() || { sx: 1, sy: 1, tx: 0, ty: 0 }; }
export function origin() { T = { sx: 1, sy: 1, tx: 0, ty: 0 }; }
export function translate(x, y) { T.tx += T.sx * x; T.ty += T.sy * y; }
export function scale(s, t = s) { T.sx *= s; T.sy *= t; }
export function transformPoint(x, y) { return [T.sx * x + T.tx, T.sy * y + T.ty]; }
export function inverseTransformPoint(x, y) { return [(x - T.tx) / T.sx, (y - T.ty) / T.sy]; }

export function setColor(r, g, b, a = 1) {
  if (Array.isArray(r)) { [r, g, b, a = 1] = r; }
  color = [r, g, b, a];
  fill = `rgba(${Math.round(r * 255)},${Math.round(g * 255)},${Math.round(b * 255)},${a})`;
  if (ctx) ctx.fillStyle = fill;
}
export function getColor() { return color.slice(); }

/** The scissor, in the coordinates of the current transform; none to clear. */
export function setScissor(x, y, w, h) {
  if (clipped) { ctx.restore(); clipped = false; }
  if (x === undefined) return;
  const [x1, y1] = transformPoint(x, y);
  const [x2, y2] = transformPoint(x + w, y + h);
  ctx.save();
  clipped = true;
  ctx.setTransform(1, 0, 0, 1, 0, 0);
  ctx.beginPath();
  ctx.rect(Math.round(Math.min(x1, x2)), Math.round(Math.min(y1, y2)),
           Math.round(Math.abs(x2 - x1)), Math.round(Math.abs(y2 - y1)));
  ctx.clip();
  ctx.fillStyle = fill;
}

/** "fill" or "line": a line is one pixel wide, centred on the rect's edge,
 *  as LÖVE's rough line style draws it. */
export function rectangle(mode, x, y, w, h) {
  apply();
  ctx.fillStyle = fill;
  if (mode === "fill") {
    ctx.fillRect(x, y, w, h);
  } else {
    ctx.fillRect(x - 0.5, y - 0.5, w + 1, 1);
    ctx.fillRect(x - 0.5, y + h - 0.5, w + 1, 1);
    ctx.fillRect(x - 0.5, y - 0.5, 1, h + 1);
    ctx.fillRect(x + w - 0.5, y - 0.5, 1, h + 1);
  }
}

/** A source rect into an image. */
export function newQuad(x, y, w, h) {
  return { x, y, w, h };
}

/** An image: a canvas and its size. */
export class Image {
  constructor(canvas) {
    this.canvas = canvas;
    this.width = canvas.width;
    this.height = canvas.height;
    this.tints = new Map();
    this.wrap = false;
  }
  getDimensions() { return [this.width, this.height]; }
  getWidth() { return this.width; }
  getHeight() { return this.height; }
  setWrap() { this.wrap = true; }
  setFilter() {}

  tinted() {
    const [r, g, b, a] = color;
    if (r === 1 && g === 1 && b === 1) return this.canvas;
    const key = `${r},${g},${b}`;
    let c = this.tints.get(key);
    if (!c) {
      c = makeCanvas(this.width, this.height);
      const x = c.getContext("2d");
      const src = this.canvas.getContext("2d").getImageData(0, 0, this.width, this.height);
      const d = src.data;
      for (let i = 0; i < d.length; i += 4) {
        d[i] = d[i] * r; d[i + 1] = d[i + 1] * g; d[i + 2] = d[i + 2] * b;
      }
      x.putImageData(src, 0, 0);
      this.tints.set(key, c);
    }
    return c;
  }
}

export function makeCanvas(w, h) {
  if (typeof OffscreenCanvas !== "undefined") return new OffscreenCanvas(Math.max(1, w), Math.max(1, h));
  const c = document.createElement("canvas");
  c.width = Math.max(1, w); c.height = Math.max(1, h);
  return c;
}

/** An image from RGBA bytes. */
export function newImageRGBA(w, h, rgba) {
  const c = makeCanvas(w, h);
  const x = c.getContext("2d");
  const id = x.createImageData(Math.max(1, w), Math.max(1, h));
  id.data.set(rgba.subarray ? rgba.subarray(0, id.data.length) : rgba);
  x.putImageData(id, 0, 0);
  return new Image(c);
}

/** draw(image, x, y) or draw(image, quad, x, y). */
export function draw(img, a, b, c) {
  if (!img) return;
  apply();
  if (color[3] !== 1) ctx.globalAlpha = color[3];
  const src = img.tinted();
  if (typeof a === "object" && a !== null) {
    const q = a;
    if (img.wrap && (q.x + q.w > img.width || q.y + q.h > img.height)) {
      // a repeating image, as LÖVE draws one with a quad bigger than it
      for (let y = 0; y < q.h; y += img.height) {
        for (let x = 0; x < q.w; x += img.width) {
          const w = Math.min(img.width, q.w - x), h = Math.min(img.height, q.h - y);
          ctx.drawImage(src, 0, 0, w, h, b + x, c + y, w, h);
        }
      }
    } else {
      // clip the source to the image, as a quad past its edge draws nothing there
      const sx = Math.max(0, q.x), sy = Math.max(0, q.y);
      const sw = Math.min(img.width, q.x + q.w) - sx, sh = Math.min(img.height, q.y + q.h) - sy;
      if (sw > 0 && sh > 0) ctx.drawImage(src, sx, sy, sw, sh, b + (sx - q.x), c + (sy - q.y), sw, sh);
    }
  } else {
    ctx.drawImage(src, a || 0, b || 0);
  }
  if (color[3] !== 1) ctx.globalAlpha = 1;
}
