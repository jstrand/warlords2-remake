// SSG > About Warlords II, and the "?" key (7721:0084).
//
// Popup 23, (160, 55) 336x347, BSCROLL.PCK through its mask and with no
// frame. 8065:1471 writes up to six lines of group 139 centred on x = 328,
// black edged in colour 7: "", "Version 1.02", "", and three lines of DOS
// memory the remake has none of. Any key or click puts it away.
#include "front/pckimage.hpp"
#include "ui/dialogs.hpp"

namespace about {

namespace {
const kit::Rect R{160, 55, 336, 347};     // popup 23
gfx::ImageP bscroll;

struct About : kit::Modal {
  std::vector<std::string> lines;
  void draw() override {
    gfx::setColor(1, 1, 1);
    gfx::draw(bscroll, gfx::newQuad(0, 0, R.w, R.h), R.x, R.y);
    const Font& f = kit::font(2).colours(0, 7);
    for (size_t i = 0; i < lines.size(); i++)
      if (!lines[i].empty()) kit::centred(f, lines[i], 328, 141 + 20 * (int)(i + 1));
  }
  void mousepressed(int, int, int) override { kit::pop(this); }
  void keypressed(const std::string&) override { kit::pop(this); }
};
}  // namespace

void open() {
  if (!bscroll) bscroll = pckimage::load(G.dataDir + "/PICS/BSCROLL.PCK", G.palette, 10);
  auto d = std::make_shared<About>();
  d->lines = {kit::text(0x8b, 1), "Version 1.02", kit::text(0x8b, 3)};   // 4125:1292
  kit::push(d);
}

}  // namespace about
