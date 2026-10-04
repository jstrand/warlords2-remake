#include "front/font.hpp"

#include <stdexcept>

#include "front/pckimage.hpp"
#include "util/util.hpp"

std::shared_ptr<Font> Font::load(const std::string& dataDir, const std::string& name, const w2::pal::Palette& palette,
                                 int keyIndex) {
  auto f = std::make_shared<Font>();
  auto s = w2::readFile(dataDir + "/" + name + ".FIN");
  if (!s) throw std::runtime_error("cannot open font metrics: " + name);
  auto m = std::make_shared<Metrics>();
  auto b = [&](size_t i) { return i < s->size() ? (int)(uint8_t)(*s)[i] : 0; };
  m->count = b(0);
  m->first = b(1);
  m->sheetWidth = b(2) * 256 + b(3);   // big-endian, unlike everything else
  f->lineHeight = b(4);
  f->baseline = b(5);
  // how wide each glyph's art is, then how far the pen moves on (78a8:03ea)
  for (int i = 0; i < m->count; i++) {
    int c = (m->first + i) & 255;
    m->ink[c] = b(11 + i);
    m->width[c] = b(11 + m->count + i);
  }
  f->m_ = m;
  f->px_ = w2::pck::decode(dataDir + "/" + name + ".FNT");
  f->palette_ = palette;
  f->key_ = keyIndex;
  f->image_ = pckimage::fromPixels(*f->px_, palette, {}, keyIndex >= 0 ? std::set<int>{keyIndex} : std::set<int>{});
  f->quad_ = std::make_shared<std::map<int, gfx::Quad>>();
  int x = 0, row = 0;
  for (int i = 0; i <= m->count - 2; i++) {      // the last entry has no glyph
    int c = (m->first + 1 + i) & 255;
    int gw = m->ink[c];
    int slot = (gw + 7) / 8 * 8;
    if (x + slot > m->sheetWidth) { x = 0; row++; }
    if (gw > 0) (*f->quad_)[c] = gfx::newQuad(x, row * f->lineHeight, gw, f->lineHeight);
    x += slot;
  }
  f->variants_ = std::make_shared<std::map<int, std::shared_ptr<Font>>>();
  return f;
}

int Font::width(const std::string& text) const {
  int n = 0;
  for (unsigned char c : text) n += m_->width[c];
  return n;
}

int Font::draw(const std::string& text, double x0, double y0) const {
  double cx = x0;
  for (unsigned char c : text) {
    auto q = quad_->find(c);
    if (q != quad_->end()) gfx::draw(image_, q->second, cx, y0);
    cx += m_->width[c];
  }
  return (int)(cx - x0);
}

void Font::drawCentred(const std::string& text, double rx, double ry, double rw, double rh) const {
  draw(text, rx + std::floor((rw - width(text)) / 2), ry + std::floor((rh - lineHeight) / 2) + 1);
}

const Font& Font::colours(int glyph, int outline) const {
  if (glyph == 15 && outline == 0) return *this;
  int k = glyph * 16 + outline;
  auto it = variants_->find(k);
  if (it != variants_->end()) return *it->second;
  auto v = std::make_shared<Font>(*this);
  v->image_ = pckimage::fromPixels(*px_, palette_, {{15, glyph}, {0, outline}},
                                   key_ >= 0 ? std::set<int>{key_} : std::set<int>{});
  (*variants_)[k] = v;
  return *v;
}
