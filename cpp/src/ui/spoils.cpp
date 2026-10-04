// What a pillage or a sack took (auto_ui_pillage_sack_raze, 63fa:0508),
// shown over the spoils dialog and closed, the two together, by any key or
// click. Popup 12, (176, 60) 288x300, marble.
#include "ui/dialogs.hpp"
#include "util/util.hpp"

namespace spoils {

using w2::format;

namespace {
const kit::Rect R{176, 60, 288, 300};     // popup 12
const int TITLE = 0x44, LINE = 0x45, GOLD = 0x46, LOST = 0x47, LEFT = 0x48;

struct Spoils : kit::Modal {
  bool sacked = false;
  w2::City* city = nullptr;
  int gold = 0, left = 0;
  std::vector<std::pair<int, int>> lost;
  kit::Done after;
  void close() {
    auto a = after;
    kit::pop(this);
    if (a) a();
  }
  void draw() override {
    int which = sacked ? 1 : 0;
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), kit::text(TITLE, which), 320, 64);
    const Font& f = kit::font(2);
    kit::centred(f, format(kit::text(LINE, which), city->name), 320, 115);
    kit::centred(f, format(kit::text(GOLD, 0), gold), 320, 135);
    kit::centred(f, format(kit::text(LOST, lost.size() == 1 ? 0 : 1), (int)lost.size()), 320, 165);
    kit::centred(f, format(kit::text(LEFT, left == 1 ? 0 : 1), left), 320, 185);
    kit::bevel(184, 220, 272, 130, 4, 2);
    kit::bevel(185, 221, 270, 128, 2, 4);
    gfx::setColor(1, 1, 1);
    f.draw(kit::text(TITLE, 2), 192, 230);
    f.draw(kit::text(TITLE, 3), 368, 230);
    for (int i = 0; i <= 2; i++) {
      int y = 250 + 30 * i;
      bool have = i < (int)lost.size();
      kit::army(have ? lost[i].first : w2::NONE, G.player->index, 208, y, 1);
      if (have) {
        gfx::setColor(1, 1, 1);
        const w2::ArmyType* t = G.g->types.byId(lost[i].first);
        f.draw(t ? t->name : "", 248, y + 6);
        f.draw(w2::fmt("%d gp", lost[i].second), 368, y + 6);
      }
    }
  }
  void mousepressed(int, int, int) override { close(); }
  void keypressed(const std::string&) override { close(); }
};
}  // namespace

void open(bool sacked, w2::City* city, int gold, const std::vector<std::pair<int, int>>& lost, int left, kit::Done after) {
  auto d = std::make_shared<Spoils>();
  d->sacked = sacked;
  d->city = city;
  d->gold = gold;
  d->lost = lost;
  d->left = left;
  d->after = after;
  kit::push(d);
}

}  // namespace spoils
