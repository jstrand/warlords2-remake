#include "platform/crt.hpp"

#include <SDL.h>
#include <SDL_opengl.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "platform/display.hpp"

namespace crt {

namespace {

// How far the glass bows: at a corner of the frame the canvas shows from this
// much further out, across and down, as a share of half the frame.
constexpr double BOW_X = 1.0 / 64, BOW_Y = 1.0 / 48;
constexpr int GRID_X = 64, GRID_Y = 48;   // the mesh's cells
constexpr double SCANLINE = 0.45;         // how dark between rows, at full strength
constexpr double MASK = 0.2;              // how much of the other colours a stripe holds back
constexpr int GLOW_NEAR = 70, GLOW_FAR = 90;   // the two halos added back, out of 255
constexpr double CORNER = 0.05, EDGE = 0.012;  // the glass's corner and its soft rim, in frame heights

struct Tex {
  SDL_Texture* t = nullptr;
  int w = 0, h = 0;
  double key = 0;   // what it was made for, besides its size
};

Tex half, quarter, eighth;   // the frame, smaller and smaller, for the glow
Tex lines;                   // one column: a UI row's brightness, top to bottom
Tex stripes;                 // one row: the mask, across the window
Tex shade;                   // the glass's darkening towards its rim

// (Re)make a texture of this size; true when it is new, so wants filling.
bool need(SDL_Renderer* r, Tex& x, int w, int h, int access, double key = 0) {
  w = std::max(1, w);
  h = std::max(1, h);
  if (x.t && x.w == w && x.h == h && x.key == key) return false;
  if (x.t) SDL_DestroyTexture(x.t);
  x.t = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, access, w, h);
  x.w = w;
  x.h = h;
  x.key = key;
  if (x.t) SDL_SetTextureScaleMode(x.t, SDL_ScaleModeLinear);
  return x.t != nullptr;
}

void fill(Tex& x, const std::vector<Uint32>& px) {
  if (x.t) SDL_UpdateTexture(x.t, nullptr, px.data(), x.w * 4);
}

Uint32 grey(double v) {
  Uint32 c = (Uint32)std::clamp((int)std::lround(v * 255), 0, 255);
  return 0xff000000u | c << 16 | c << 8 | c;
}

// Where a canvas point shows on the glass: the bow's inverse, in the frame's
// own -1..1, found by going round until it settles.
std::pair<double, double> bow(double cx, double cy) {
  double sx = cx, sy = cy;
  for (int i = 0; i < 8; i++) {
    sx = cx / (1 + BOW_X * sy * sy);
    sy = cy / (1 + BOW_Y * sx * sx);
  }
  return {sx, sy};
}

// A UI row's brightness at `k` texels a row, `s` its strength: the beam
// lights the row's top k - 1 texels and leaves the last the darkest, so at
// two pixels a row it is one bright and one dark, as on the glass.
void makeLines(int rows, int k, double s) {
  std::vector<Uint32> px((size_t)rows * k);
  double mid = 0.5 - 0.5 / k;
  for (size_t i = 0; i < px.size(); i++) {
    double t = (i % k + 0.5) / k - mid;
    double beam = std::exp(-(t * t) / (0.3 * 0.3));
    px[i] = grey(1 - s * (1 - beam));
  }
  fill(lines, px);
}

// The aperture grille: red, green and blue stripes, each `sw` pixels wide.
void makeStripes(int sw) {
  std::vector<Uint32> px(stripes.w);
  Uint32 off = (Uint32)std::lround(255 * (1 - MASK));
  for (int x = 0; x < stripes.w; x++) {
    int c = x / sw % 3;
    Uint32 r = c == 0 ? 255 : off, g = c == 1 ? 255 : off, b = c == 2 ? 255 : off;
    px[x] = 0xff000000u | r << 16 | g << 8 | b;
  }
  fill(stripes, px);
}

// Dark at the glass's rounded rim, and a little towards it everywhere.
void makeShade(double aspect) {
  std::vector<Uint32> px((size_t)shade.w * shade.h);
  for (int y = 0; y < shade.h; y++) {
    for (int x = 0; x < shade.w; x++) {
      double u = (x + 0.5) / shade.w, v = (y + 0.5) / shade.h;
      // how far inside the rounded rim, in frame heights
      double qx = std::abs(u * 2 - 1) * aspect - (aspect - CORNER), qy = std::abs(v * 2 - 1) - (1 - CORNER);
      double out = std::hypot(std::max(qx, 0.0), std::max(qy, 0.0)) + std::min(std::max(qx, qy), 0.0) - CORNER;
      double rim = std::clamp(-out / EDGE, 0.0, 1.0);
      rim = rim * rim * (3 - 2 * rim);
      double vig = std::pow(std::max(0.0, 16 * u * (1 - u) * v * (1 - v)), 0.12);
      px[(size_t)y * shade.w + x] = grey(rim * vig);
    }
  }
  fill(shade, px);
}

// --- the shader, where the renderer is OpenGL ------------------------------
//
// SDL draws everything else; the tube is one triangle over the window, drawn
// between SDL's own commands. SDL2 and SDL3 (under sdl2-compat) both keep a
// copy of the GL state they last set, so what is changed here goes back as it
// was: the program, the viewport, the blend, the clip and the textures.

struct GLFuns {
  decltype(&glGetIntegerv) GetIntegerv;
  decltype(&glIsEnabled) IsEnabled;
  decltype(&glEnable) Enable;
  decltype(&glDisable) Disable;
  decltype(&glViewport) Viewport;
  decltype(&glBindTexture) BindTexture;
  decltype(&glBegin) Begin;
  decltype(&glEnd) End;
  decltype(&glVertex2f) Vertex2f;
  PFNGLACTIVETEXTUREPROC ActiveTexture;
  PFNGLCREATESHADERPROC CreateShader;
  PFNGLSHADERSOURCEPROC ShaderSource;
  PFNGLCOMPILESHADERPROC CompileShader;
  PFNGLGETSHADERIVPROC GetShaderiv;
  PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog;
  PFNGLDELETESHADERPROC DeleteShader;
  PFNGLCREATEPROGRAMPROC CreateProgram;
  PFNGLATTACHSHADERPROC AttachShader;
  PFNGLLINKPROGRAMPROC LinkProgram;
  PFNGLGETPROGRAMIVPROC GetProgramiv;
  PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog;
  PFNGLUSEPROGRAMPROC UseProgram;
  PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation;
  PFNGLUNIFORM1IPROC Uniform1i;
  PFNGLUNIFORM1FPROC Uniform1f;
  PFNGLUNIFORM2FPROC Uniform2f;
  PFNGLUNIFORM4FPROC Uniform4f;
} gl;

enum class Shader { UNTRIED, READY, NONE };
Shader shader = Shader::UNTRIED;
GLuint program = 0;
struct { GLint screen, nearer, farther, height, k, size, frame, rows, rowPx, scan, stripe; } at;

// A constant as GLSL reads it: a float, never an int.
std::string lit(double v) {
  char b[32];
  snprintf(b, sizeof b, "%.9g", v);
  std::string s = b;
  if (s.find_first_of(".e") == std::string::npos) s += ".0";
  return s;
}

const char* VERTEX = R"(#version 120
void main() { gl_Position = vec4(gl_Vertex.xy, 0.0, 1.0); }
)";

// The web port's shader (web/src/crt.js) in GLSL 1.20, the version SDL's
// OpenGL renderer gets on every system (on a Mac, OpenGL 2.1). The glow is
// the frame two and three halvings down, as the mesh's is, not mipmaps.
std::string fragment() {
  return R"(#version 120
uniform sampler2D screen;   // the canvas
uniform sampler2D nearer;   // the frame two halvings down
uniform sampler2D farther;  // and three
uniform float height;       // the window, in its pixels
uniform vec2 k;             // window pixels to a canvas pixel
uniform vec2 size;          // the canvas, in its pixels
uniform vec4 frame;         // the frame on it: x, y, w, h
uniform float rows;         // the frame's UI rows
uniform float rowPx;        // window pixels to a UI row
uniform float scan;         // the scanlines' strength
uniform float stripe;       // a stripe's width, in window pixels
const float BOW_X = )" + lit(BOW_X) + R"(, BOW_Y = )" + lit(BOW_Y) + R"(;
const float MASK = )" + lit(MASK) + R"(;
const float GLOW_NEAR = )" + lit(GLOW_NEAR / 255.0) + R"(, GLOW_FAR = )" + lit(GLOW_FAR / 255.0) + R"(;
const float CORNER = )" + lit(CORNER) + R"(, EDGE = )" + lit(EDGE) + R"(;

void main() {
  vec2 w = vec2(gl_FragCoord.x, height - gl_FragCoord.y);
  vec2 p = w / k;
  vec2 n = (p - frame.xy) / frame.zw * 2.0 - 1.0;
  if (abs(n.x) > 1.0 || abs(n.y) > 1.0) {
    // off the frame: only black, or the pointer
    gl_FragColor = vec4(texture2D(screen, p / size).rgb, 1.0);
    return;
  }
  vec2 c = n * vec2(1.0 + BOW_X * n.y * n.y, 1.0 + BOW_Y * n.x * n.x);
  vec2 u = (c + 1.0) / 2.0;
  vec3 rgb = texture2D(screen, (frame.xy + u * frame.zw) / size).rgb;

  // the beam lights the top of the row and leaves its foot the darkest,
  // taken as the mean over the pixel's height, so the bow's slant across the
  // rows does not band
  float r = u.y * rows - (0.5 - 0.5 / rowPx), foot = dFdy(r), beam = 0.0;
  for (int i = 0; i < 4; i++) {
    float t = fract(r + foot * (float(i) - 1.5) / 4.0 + 0.5) - 0.5;
    beam += exp(-(t * t) / 0.09) / 4.0;
  }
  rgb *= 1.0 - scan * (1.0 - beam);

  // the aperture grille
  float s = mod(floor((w.x - floor(frame.x * k.x)) / stripe), 3.0);
  rgb *= mix(vec3(1.0 - MASK), vec3(1.0), vec3(float(s == 0.0), float(s == 1.0), float(s == 2.0)));

  rgb += texture2D(nearer, u).rgb * GLOW_NEAR + texture2D(farther, u).rgb * GLOW_FAR;

  // dark at the glass's rounded rim, and a little towards it everywhere
  float aspect = frame.z / frame.w;
  vec2 q = abs(c) * vec2(aspect, 1.0) - vec2(aspect - CORNER, 1.0 - CORNER);
  float outside = length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - CORNER;
  float rim = smoothstep(0.0, 1.0, clamp(-outside / EDGE, 0.0, 1.0));
  float vig = pow(max(0.0, 16.0 * u.x * (1.0 - u.x) * u.y * (1.0 - u.y)), 0.12);
  gl_FragColor = vec4(rgb * rim * vig, 1.0);
}
)";
}

template <class F>
bool load(F& f, const char* name) {
  f = (F)SDL_GL_GetProcAddress(name);
  return f != nullptr;
}

GLuint compile(GLenum type, const char* src) {
  GLuint s = gl.CreateShader(type);
  gl.ShaderSource(s, 1, &src, nullptr);
  gl.CompileShader(s);
  GLint ok = 0;
  gl.GetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[1024] = "";
    gl.GetShaderInfoLog(s, sizeof log, nullptr, log);
    fprintf(stderr, "CRT shader: %s\n", log);
    gl.DeleteShader(s);
    return 0;
  }
  return s;
}

// Load OpenGL and build the program, with SDL's context current.
bool makeShader() {
  bool ok = load(gl.GetIntegerv, "glGetIntegerv") && load(gl.IsEnabled, "glIsEnabled") &&
            load(gl.Enable, "glEnable") && load(gl.Disable, "glDisable") && load(gl.Viewport, "glViewport") &&
            load(gl.BindTexture, "glBindTexture") && load(gl.Begin, "glBegin") && load(gl.End, "glEnd") &&
            load(gl.Vertex2f, "glVertex2f") && load(gl.ActiveTexture, "glActiveTexture") &&
            load(gl.CreateShader, "glCreateShader") && load(gl.ShaderSource, "glShaderSource") &&
            load(gl.CompileShader, "glCompileShader") && load(gl.GetShaderiv, "glGetShaderiv") &&
            load(gl.GetShaderInfoLog, "glGetShaderInfoLog") && load(gl.DeleteShader, "glDeleteShader") &&
            load(gl.CreateProgram, "glCreateProgram") && load(gl.AttachShader, "glAttachShader") &&
            load(gl.LinkProgram, "glLinkProgram") && load(gl.GetProgramiv, "glGetProgramiv") &&
            load(gl.GetProgramInfoLog, "glGetProgramInfoLog") && load(gl.UseProgram, "glUseProgram") &&
            load(gl.GetUniformLocation, "glGetUniformLocation") && load(gl.Uniform1i, "glUniform1i") &&
            load(gl.Uniform1f, "glUniform1f") && load(gl.Uniform2f, "glUniform2f") && load(gl.Uniform4f, "glUniform4f");
  if (!ok) return false;
  std::string frag = fragment();
  GLuint vs = compile(GL_VERTEX_SHADER, VERTEX), fs = compile(GL_FRAGMENT_SHADER, frag.c_str());
  if (!vs || !fs) return false;
  program = gl.CreateProgram();
  gl.AttachShader(program, vs);
  gl.AttachShader(program, fs);
  gl.LinkProgram(program);
  gl.DeleteShader(vs);
  gl.DeleteShader(fs);
  GLint linked = 0;
  gl.GetProgramiv(program, GL_LINK_STATUS, &linked);
  if (!linked) {
    char log[1024] = "";
    gl.GetProgramInfoLog(program, sizeof log, nullptr, log);
    fprintf(stderr, "CRT shader: %s\n", log);
    return false;
  }
  at.screen = gl.GetUniformLocation(program, "screen");
  at.nearer = gl.GetUniformLocation(program, "nearer");
  at.farther = gl.GetUniformLocation(program, "farther");
  at.height = gl.GetUniformLocation(program, "height");
  at.k = gl.GetUniformLocation(program, "k");
  at.size = gl.GetUniformLocation(program, "size");
  at.frame = gl.GetUniformLocation(program, "frame");
  at.rows = gl.GetUniformLocation(program, "rows");
  at.rowPx = gl.GetUniformLocation(program, "rowPx");
  at.scan = gl.GetUniformLocation(program, "scan");
  at.stripe = gl.GetUniformLocation(program, "stripe");
  return true;
}

// A texture's GL name, through SDL: 0 where it is not a plain 2D one.
GLuint name(SDL_Texture* t) {
  float tw = 0, th = 0;
  if (SDL_GL_BindTexture(t, &tw, &th) != 0) return 0;
  GLint n = 0;
  gl.GetIntegerv(GL_TEXTURE_BINDING_2D, &n);
  SDL_GL_UnbindTexture(t);
  return tw == 1 && th == 1 ? (GLuint)n : 0;
}

struct Look {
  int ow, oh;                      // the window, in its pixels
  double kx, ky;                   // window pixels to a canvas pixel
  double rowPx, strength, stripe;  // a UI row and a stripe in window pixels; the scanlines' strength
};

// The tube in one pass, with the window the target and nothing of SDL's
// waiting; false where it cannot be, and the mesh draws it.
bool throughShader(SDL_Renderer* r, SDL_Texture* canvas, const Look& k) {
  if (shader == Shader::NONE) return false;
  if (shader == Shader::UNTRIED) {
    SDL_RendererInfo info{};
    bool isGL = SDL_GetRendererInfo(r, &info) == 0 && info.name && strcmp(info.name, "opengl") == 0;
    shader = isGL && makeShader() ? Shader::READY : Shader::NONE;
    if (shader == Shader::NONE) return false;
  }
  const auto& d = display::d;
  SDL_RenderFlush(r);

  // as SDL left it
  GLint active = 0, prog = 0, view[4] = {};
  GLint bound[3] = {};
  GLboolean enabled[3] = {};
  gl.GetIntegerv(GL_ACTIVE_TEXTURE, &active);
  for (int i = 0; i < 3; i++) {
    gl.ActiveTexture(GL_TEXTURE0 + i);
    gl.GetIntegerv(GL_TEXTURE_BINDING_2D, &bound[i]);
    enabled[i] = gl.IsEnabled(GL_TEXTURE_2D);
  }
  gl.ActiveTexture(GL_TEXTURE0);
  gl.GetIntegerv(GL_CURRENT_PROGRAM, &prog);
  gl.GetIntegerv(GL_VIEWPORT, view);
  GLboolean blend = gl.IsEnabled(GL_BLEND), scissor = gl.IsEnabled(GL_SCISSOR_TEST);

  GLuint tex[3] = {name(canvas), name(quarter.t), name(eighth.t)};
  bool ok = tex[0] && tex[1] && tex[2];
  if (ok) {
    for (int i = 0; i < 3; i++) {
      gl.ActiveTexture(GL_TEXTURE0 + i);
      gl.BindTexture(GL_TEXTURE_2D, tex[i]);
    }
    gl.UseProgram(program);
    gl.Uniform1i(at.screen, 0);
    gl.Uniform1i(at.nearer, 1);
    gl.Uniform1i(at.farther, 2);
    gl.Uniform1f(at.height, (float)k.oh);
    gl.Uniform2f(at.k, (float)k.kx, (float)k.ky);
    gl.Uniform2f(at.size, (float)d.pw, (float)d.ph);
    gl.Uniform4f(at.frame, (float)d.ox, (float)d.oy, (float)(d.w * d.scale), (float)(d.h * d.scale));
    gl.Uniform1f(at.rows, (float)d.h);
    gl.Uniform1f(at.rowPx, (float)k.rowPx);
    gl.Uniform1f(at.scan, (float)k.strength);
    gl.Uniform1f(at.stripe, (float)k.stripe);
    gl.Viewport(0, 0, k.ow, k.oh);
    gl.Disable(GL_BLEND);
    gl.Disable(GL_SCISSOR_TEST);
    // one triangle over the whole window
    gl.Begin(GL_TRIANGLES);
    gl.Vertex2f(-1, -1);
    gl.Vertex2f(3, -1);
    gl.Vertex2f(-1, 3);
    gl.End();
  }

  // back as SDL left it
  for (int i = 2; i >= 0; i--) {
    gl.ActiveTexture(GL_TEXTURE0 + i);
    gl.BindTexture(GL_TEXTURE_2D, bound[i]);
    (enabled[i] ? gl.Enable : gl.Disable)(GL_TEXTURE_2D);
  }
  gl.ActiveTexture(active);
  gl.UseProgram(prog);
  gl.Viewport(view[0], view[1], view[2], view[3]);
  (blend ? gl.Enable : gl.Disable)(GL_BLEND);
  (scissor ? gl.Enable : gl.Disable)(GL_SCISSOR_TEST);
  if (!ok) shader = Shader::NONE;
  return ok;
}

}  // namespace

std::pair<double, double> unwarp(double x, double y) {
  const auto& d = display::d;
  double fw = d.w * d.scale, fh = d.h * d.scale;
  if (fw <= 0 || fh <= 0) return {x, y};
  double nx = (x - d.ox) / fw * 2 - 1, ny = (y - d.oy) / fh * 2 - 1;
  double cx = nx * (1 + BOW_X * ny * ny), cy = ny * (1 + BOW_Y * nx * nx);
  return {d.ox + (cx + 1) / 2 * fw, d.oy + (cy + 1) / 2 * fh};
}

void present(SDL_Renderer* r, SDL_Texture* canvas) {
  const auto& d = display::d;
  int fx = d.ox, fy = d.oy, fw = d.w * d.scale, fh = d.h * d.scale;
  SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_NONE);

  // the glow: the frame halved three times over, each step a box blur
  SDL_Rect frame{fx, fy, fw, fh};
  need(r, half, fw / 2, fh / 2, SDL_TEXTUREACCESS_TARGET);
  need(r, quarter, fw / 4, fh / 4, SDL_TEXTUREACCESS_TARGET);
  need(r, eighth, fw / 8, fh / 8, SDL_TEXTUREACCESS_TARGET);
  bool glow = half.t && quarter.t && eighth.t;
  if (glow) {
    SDL_RenderSetClipRect(r, nullptr);
    SDL_SetRenderTarget(r, half.t);
    SDL_RenderCopy(r, canvas, &frame, nullptr);
    SDL_SetTextureBlendMode(half.t, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(r, quarter.t);
    SDL_RenderCopy(r, half.t, nullptr, nullptr);
    SDL_SetTextureBlendMode(quarter.t, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(r, eighth.t);
    SDL_RenderCopy(r, quarter.t, nullptr, nullptr);
  }

  SDL_SetRenderTarget(r, nullptr);
  SDL_RenderSetClipRect(r, nullptr);
  int ow = 0, oh = 0;
  SDL_GetRendererOutputSize(r, &ow, &oh);
  double kx = (double)ow / d.pw, ky = (double)oh / d.ph;   // window pixels to a device pixel
  double rowPx = d.scale * ky;   // window pixels to a UI row
  // scanlines: on whole pixels at any scale, else fading out where a row is
  // too few pixels to show one without beating against them
  bool whole = rowPx >= 2 && std::abs(rowPx - std::round(rowPx)) < 0.01;
  double strength = SCANLINE * (whole ? 1 : std::clamp((rowPx - 1.5) / 1.5, 0.0, 1.0));
  // the mask, a stripe to a pixel of the screen (more on a dense one)
  int sw = std::max(1, (int)std::lround(d.dpi * d.shrink));
  if (glow && throughShader(r, canvas, {ow, oh, kx, ky, rowPx, strength, (double)sw})) return;

  // off the frame there is only black, and the pointer when it is there
  SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
  SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
  SDL_RenderClear(r);
  SDL_RenderCopy(r, canvas, nullptr, nullptr);
  SDL_Rect onWindow{(int)std::floor(fx * kx), (int)std::floor(fy * ky), (int)std::ceil(fw * kx) + 1, (int)std::ceil(fh * ky) + 1};
  SDL_RenderFillRect(r, &onWindow);

  // the mesh: a grid on the canvas, each point where the glass shows it
  std::vector<SDL_Vertex> onCanvas, onFrame, onLines;
  std::vector<int> idx;
  int rowTexels = 2;
  {
    SDL_RendererInfo info{};
    SDL_GetRendererInfo(r, &info);
    int maxH = info.max_texture_height > 0 ? info.max_texture_height : 8192;
    rowTexels = std::clamp((int)std::lround(rowPx), 2, 8);
    rowTexels = std::max(2, std::min(rowTexels, maxH / std::max(1, d.h)));
  }
  for (int j = 0; j <= GRID_Y; j++) {
    for (int i = 0; i <= GRID_X; i++) {
      double u = (double)i / GRID_X, v = (double)j / GRID_Y;
      auto [sx, sy] = bow(u * 2 - 1, v * 2 - 1);
      SDL_Vertex vx{};
      vx.position = {(float)((fx + (sx + 1) / 2 * fw) * kx), (float)((fy + (sy + 1) / 2 * fh) * ky)};
      vx.color = {255, 255, 255, 255};
      vx.tex_coord = {(float)((fx + u * fw) / d.pw), (float)((fy + v * fh) / d.ph)};
      onCanvas.push_back(vx);
      vx.tex_coord = {(float)u, (float)v};
      onFrame.push_back(vx);
      vx.tex_coord = {0.5f, (float)v};
      onLines.push_back(vx);
    }
  }
  for (int j = 0; j < GRID_Y; j++) {
    for (int i = 0; i < GRID_X; i++) {
      int a = j * (GRID_X + 1) + i, b = a + 1, c = a + GRID_X + 1, e = c + 1;
      idx.insert(idx.end(), {a, b, c, b, e, c});
    }
  }
  auto draw = [&](SDL_Texture* t, std::vector<SDL_Vertex>& vs, SDL_BlendMode mode, Uint8 tint = 255) {
    if (!t) return;
    for (SDL_Vertex& vx : vs) vx.color = {tint, tint, tint, 255};
    SDL_SetTextureBlendMode(t, mode);
    SDL_RenderGeometry(r, t, vs.data(), (int)vs.size(), idx.data(), (int)idx.size());
  };

  draw(canvas, onCanvas, SDL_BLENDMODE_NONE);

  if (need(r, lines, 1, d.h * rowTexels, SDL_TEXTUREACCESS_STATIC, rowTexels * 1000 + strength)) makeLines(d.h, rowTexels, strength);
  draw(lines.t, onLines, SDL_BLENDMODE_MOD);

  if (need(r, stripes, ow, 1, SDL_TEXTUREACCESS_STATIC, sw)) {
    SDL_SetTextureScaleMode(stripes.t, SDL_ScaleModeNearest);
    makeStripes(sw);
  }
  if (stripes.t) {
    SDL_Rect src{0, 0, std::min(onWindow.w, stripes.w), 1}, dst = onWindow;
    dst.w = src.w;
    SDL_SetTextureBlendMode(stripes.t, SDL_BLENDMODE_MOD);
    SDL_RenderCopy(r, stripes.t, &src, &dst);
  }

  if (glow) {
    draw(quarter.t, onFrame, SDL_BLENDMODE_ADD, GLOW_NEAR);
    draw(eighth.t, onFrame, SDL_BLENDMODE_ADD, GLOW_FAR);
  }

  double aspect = fh > 0 ? (double)fw / fh : 4.0 / 3;
  if (need(r, shade, 256, std::clamp((int)std::lround(256 / aspect), 64, 512), SDL_TEXTUREACCESS_STATIC, aspect)) makeShade(aspect);
  draw(shade.t, onFrame, SDL_BLENDMODE_MOD);
}

}  // namespace crt
