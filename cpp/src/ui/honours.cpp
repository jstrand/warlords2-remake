// A hero's promotion and an army's medal, as the original shows them to a
// human: a picture popup at (160, 90) 320x200 with lines of text over it,
// closed by any key or click.
//
// A promotion (hero_promote, 7563:0672) is popup 16, 17 or 18 -- LEVEL2-4.PCK
// -- with music cue 7: STRING.DAT group 101-103 (lines 6 on for a heroine),
// the title in font 1 at y 93, the hero's name at 140 and the rest in font 2
// at 160, 180, 200, 232 and 252, all centred on 320.
//
// A medal (auto_ui_medal_effect, 67cc:2274) is popup 24-27 -- MEDAL0-3.PCK
// for the army's first to fourth -- with music cue 8 and then 1 again: the
// army at (176, 155), and in font 2 a line of group 158 at (216, 155), "your
// unit of %s" (159) at (216, 175), a line of 160 at (176, 195), the medal's
// name (161) at (176, 215), and what grew (162) at (176, 245) and (176, 265).
#include "ui/dialogs.hpp"
#include "platform/sound.hpp"
#include "util/util.hpp"
#include "warlords/cues.hpp"

namespace honours {

using w2::format;

namespace {
const kit::Rect R{160, 90, 320, 200};
const int PROMOTION_GROUP = 101, MEDAL_FOR = 158, MEDAL_UNIT = 159, MEDAL_GIVEN = 160;
const int MEDAL_NAME = 161, MEDAL_GREW = 162;

struct Popup : kit::Modal {
  gfx::ImageP pic;
  std::function<void()> lines;
  kit::Done after;
  void close() {
    auto a = after;
    kit::pop(this);
    if (a) a();
  }
  void draw() override {
    kit::popupFrame(R);
    gfx::setColor(1, 1, 1);
    if (pic) gfx::draw(pic, R.x, R.y);
    lines();
  }
  void mousepressed(int, int, int) override { close(); }
  void keypressed(const std::string&) override { close(); }
};

void show(gfx::ImageP pic, std::function<void()> lines, kit::Done after) {
  auto d = std::make_shared<Popup>();
  d->pic = pic;
  d->lines = std::move(lines);
  d->after = std::move(after);
  kit::push(d);
}

void promotion(std::vector<w2::Promotion> list, size_t i, kit::Done after) {
  if (i >= list.size()) {
    if (after) after();
    return;
  }
  w2::Army* hero = list[i].hero;
  int gain = list[i].gain;
  int level = hero->level ? hero->level : 2;             // the new level, 2-4
  int group = PROMOTION_GROUP + level - 2;
  int base = hero->female ? 6 : 0;
  std::string name = hero->name;
  int strength = hero->strength, moves = hero->maxMoves;
  sound::music(w2::cues::PROMOTION);
  auto pic = G.levelPic.count(level) ? G.levelPic[level] : nullptr;
  show(pic, [=]() {
    kit::centred(kit::font(1), kit::text(group, base), 320, 93);
    const Font& f = kit::font(2);
    kit::centred(f, name, 320, 140);
    kit::centred(f, kit::text(group, base + 1), 320, 160);
    kit::centred(f, kit::text(group, base + 2), 320, 180);
    kit::centred(f, kit::text(group, base + 3), 320, 200);
    kit::centred(f, kit::text(group, base + 4), 320, 232);
    kit::centred(f, format(kit::text(group, base + 5), level, strength, gain, moves, 2), 320, 252);
  }, [list, i, after]() { promotion(list, i + 1, after); });
}
}  // namespace

void promotions(const std::vector<w2::Promotion>& list, kit::Done after) { promotion(list, 0, after); }

void medal(w2::Army* a, kit::Done after) {
  int n = a->medals ? a->medals : 1;
  bool grewMove = a->strength >= 15;
  int now = grewMove ? a->maxMoves : a->strength;
  auto pick = [](int group, int count) { return kit::text(group, G.g->rng.dice(1, count, -1)); };
  std::string line1 = pick(MEDAL_FOR, 7), line3 = pick(MEDAL_GIVEN, 4);
  const w2::ArmyType* t = G.g->types.byId(a->type);
  std::string tname = t ? t->name : "";
  int type = a->type, owner = a->owner;
  sound::music(w2::cues::MEDAL);
  show(G.medalPic[n - 1], [=]() {
    kit::army(type, owner, 176, 155, 0);
    gfx::setColor(1, 1, 1);
    const Font& f = kit::font(2);
    f.draw(line1, 216, 155);
    f.draw(format(kit::text(MEDAL_UNIT, 0), tname), 216, 175);
    f.draw(line3, 176, 195);
    f.draw(kit::text(MEDAL_NAME, n - 1), 176, 215);
    f.draw(kit::text(MEDAL_GREW, grewMove ? 1 : 0), 176, 245);
    f.draw(format(kit::text(MEDAL_GREW, 2), now - 1, now), 176, 265);
  }, [after]() {
    sound::music(w2::cues::PLAY);
    if (after) after();
  });
}

}  // namespace honours
