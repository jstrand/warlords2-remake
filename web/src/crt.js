// The frame as an old monitor showed it (View > CRT screen): not the
// original's, which drew plain pixels for whatever screen the player had.
//
// The game still draws on the 2D canvas; each frame that canvas goes up to a
// WebGL canvas laid over it, and one shader draws it through the tube: the
// frame bowed like the glass, each UI row darkened at its foot into a
// scanline, the phosphor's red, green and blue stripes, a glow from the
// canvas's smaller mipmaps, and the corners rounded and shaded. The rows, the
// glow and the shade follow the bow; the stripes are the screen's own, as a
// real mask is. The same tube as the C++ port's (cpp/src/platform/crt.cpp).
//
// The tube takes no input: the pointer goes on to the 2D canvas beneath, and
// toUI undoes the bow, so a click lands on what it is over.

import { display } from "./display.js";

// How far the glass bows: at a corner of the frame the canvas shows from this
// much further out, across and down, as a share of half the frame.
const BOW_X = 1 / 64, BOW_Y = 1 / 48;
const SCANLINE = 0.45;                 // how dark between rows, at full strength
const MASK = 0.2;                      // how much of the other colours a stripe holds back
const GLOW_NEAR = 70, GLOW_FAR = 90;   // the two halos added back, out of 255
const CORNER = 0.05, EDGE = 0.012;     // the glass's corner and its soft rim, in frame heights

const VERTEX = `#version 300 es
in vec2 at;
void main() { gl_Position = vec4(at, 0.0, 1.0); }
`;

const FRAGMENT = `#version 300 es
precision highp float;
uniform sampler2D screen;
uniform vec2 size;      // the canvas, in device pixels
uniform vec4 frame;     // the frame on it: x, y, w, h
uniform float rows;     // the frame's UI rows
uniform float rowPx;    // device pixels to a UI row
uniform float scan;     // the scanlines' strength
uniform float stripe;   // a stripe's width, in device pixels
out vec4 color;

void main() {
  vec2 p = vec2(gl_FragCoord.x, size.y - gl_FragCoord.y);
  vec2 n = (p - frame.xy) / frame.zw * 2.0 - 1.0;
  if (abs(n.x) > 1.0 || abs(n.y) > 1.0) {
    // off the frame: only black, or the pointer
    color = vec4(textureLod(screen, p / size, 0.0).rgb, 1.0);
    return;
  }
  vec2 c = n * vec2(1.0 + ${BOW_X} * n.y * n.y, 1.0 + ${BOW_Y} * n.x * n.x);
  vec2 u = (c + 1.0) / 2.0;
  vec2 uv = (frame.xy + u * frame.zw) / size;
  vec3 rgb = textureLod(screen, uv, 0.0).rgb;

  // the beam lights the top of the row and leaves its foot the darkest, so
  // at two pixels a row it is one bright and one dark. It is taken as the
  // mean over the pixel's height, not at its middle: where the bow slides
  // the rows across the pixels, a point would catch the beam on some and
  // miss it on others, in bands
  float r = u.y * rows - (0.5 - 0.5 / rowPx), foot = dFdy(r), beam = 0.0;
  for (int i = 0; i < 4; i++) {
    float t = fract(r + foot * (float(i) - 1.5) / 4.0 + 0.5) - 0.5;
    beam += exp(-(t * t) / 0.09) / 4.0;
  }
  rgb *= 1.0 - scan * (1.0 - beam);

  // the aperture grille
  float k = mod(floor(p.x / stripe), 3.0);
  rgb *= mix(vec3(1.0 - ${MASK}), vec3(1.0), vec3(k == 0.0, k == 1.0, k == 2.0));

  // the glow: the frame two and three halvings down
  rgb += textureLod(screen, uv, 2.0).rgb * ${GLOW_NEAR / 255} + textureLod(screen, uv, 3.0).rgb * ${GLOW_FAR / 255};

  // dark at the glass's rounded rim, and a little towards it everywhere
  float aspect = frame.z / frame.w;
  vec2 q = abs(c) * vec2(aspect, 1.0) - vec2(aspect - ${CORNER}, 1.0 - ${CORNER});
  float outside = length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - ${CORNER};
  float rim = smoothstep(0.0, 1.0, clamp(-outside / ${EDGE}, 0.0, 1.0));
  float vig = pow(max(0.0, 16.0 * u.x * (1.0 - u.x) * u.y * (1.0 - u.y)), 0.12);
  color = vec4(rgb * rim * vig, 1.0);
}
`;

let tube = null;   // { canvas, gl, program, tex, at: uniform locations }, or false: no WebGL2 here

function make() {
  const canvas = document.createElement("canvas");
  canvas.style.cssText = "position: fixed; left: 0; top: 0; pointer-events: none; z-index: 1; display: none;";
  const gl = canvas.getContext("webgl2", { alpha: false, antialias: false });
  if (!gl) return false;
  const shader = (type, src) => {
    const s = gl.createShader(type);
    gl.shaderSource(s, src);
    gl.compileShader(s);
    if (!gl.getShaderParameter(s, gl.COMPILE_STATUS)) throw new Error("CRT shader: " + gl.getShaderInfoLog(s));
    return s;
  };
  const program = gl.createProgram();
  gl.attachShader(program, shader(gl.VERTEX_SHADER, VERTEX));
  gl.attachShader(program, shader(gl.FRAGMENT_SHADER, FRAGMENT));
  gl.linkProgram(program);
  if (!gl.getProgramParameter(program, gl.LINK_STATUS)) throw new Error("CRT shader: " + gl.getProgramInfoLog(program));
  gl.useProgram(program);

  // one triangle over the whole canvas
  gl.bindBuffer(gl.ARRAY_BUFFER, gl.createBuffer());
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([-1, -1, 3, -1, -1, 3]), gl.STATIC_DRAW);
  const loc = gl.getAttribLocation(program, "at");
  gl.enableVertexAttribArray(loc);
  gl.vertexAttribPointer(loc, 2, gl.FLOAT, false, 0, 0);

  const tex = gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D, tex);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

  const at = {};
  for (const name of ["screen", "size", "frame", "rows", "rowPx", "scan", "stripe"]) at[name] = gl.getUniformLocation(program, name);
  document.body.appendChild(canvas);
  return { canvas, gl, program, tex, at };
}

/** Is the screen going through the tube? Not where there is no WebGL2. */
export function shown() {
  return display.crt && tube !== false;
}

/** Show the 2D canvas through the tube, or take the tube away when the CRT
 *  screen is off. Run after each frame is drawn. */
export function present() {
  if (!display.crt) {
    if (tube) tube.canvas.style.display = "none";
    return;
  }
  if (tube === null) {
    try { tube = make(); } catch (e) { console.error(e); tube = false; }
  }
  if (!tube) return;
  const { canvas, gl, at } = tube;
  const src = display.canvas;
  if (canvas.width !== src.width || canvas.height !== src.height) {
    canvas.width = src.width;
    canvas.height = src.height;
  }
  canvas.style.width = src.style.width;
  canvas.style.height = src.style.height;
  canvas.style.display = "block";

  gl.viewport(0, 0, canvas.width, canvas.height);
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, src);
  gl.generateMipmap(gl.TEXTURE_2D);
  gl.uniform1i(at.screen, 0);
  gl.uniform2f(at.size, src.width, src.height);
  gl.uniform4f(at.frame, display.ox, display.oy, display.w * display.scale, display.h * display.scale);
  gl.uniform1f(at.rows, display.h);
  gl.uniform1f(at.rowPx, display.scale);
  // a row of one pixel has no room for a scanline
  gl.uniform1f(at.scan, display.scale >= 2 ? SCANLINE : 0);
  gl.uniform1f(at.stripe, Math.max(1, Math.round(display.dpi)));
  gl.drawArrays(gl.TRIANGLES, 0, 3);
}

/** The canvas point that shows at a point on the tube, both in device
 *  pixels: the bow undone. */
export function unwarp(x, y) {
  const fw = display.w * display.scale, fh = display.h * display.scale;
  if (fw <= 0 || fh <= 0) return [x, y];
  const nx = (x - display.ox) / fw * 2 - 1, ny = (y - display.oy) / fh * 2 - 1;
  const cx = nx * (1 + BOW_X * ny * ny), cy = ny * (1 + BOW_Y * nx * nx);
  return [display.ox + (cx + 1) / 2 * fw, display.oy + (cy + 1) / 2 * fh];
}
