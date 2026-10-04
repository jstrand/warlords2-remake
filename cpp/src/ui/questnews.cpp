// A quest's end, as quest_check (4976:1ded) tells a human it.
//
// Failed: the message box with the two lines of its cause, STRING.DAT groups
// 32-42. Done: the triumph's music, and the reward (4976:1520): popup 2 with
// dialog 17 -- Done (331); the strategic map with no city shields and the
// hero's figure on it; "Quest" in font 1; SCROLL.PCK at (304, 95) and on it,
// black on yellow, the hero's quest, the priests' words and what was given.
#include "platform/sound.hpp"
#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/cues.hpp"

namespace questnews {

using w2::format;

namespace {
const kit::Rect R{80, 60, 480, 312};      // popup 2
const int MAP_X = 80, MAP_Y = 60;
const int DIALOG = 17, DONE = 331;

struct Reward : kit::Modal {
  std::shared_ptr<w2::QuestResult> news;
  screen::View v;
  kit::Done after;
  int grp = 0x1f;
  std::string last;
  void close() {
    auto a = after;
    kit::pop(this);
    if (a) a();
  }
  void draw() override {
    const w2::Quest& q = *news->quest;
    const w2::Reward& r = *news->reward;
    w2::Army* h = q.hero;
    kit::popup(R);
    front::drawStrategicMap(MAP_X, MAP_Y, nullptr, true);
    if (r.kind == "revealed" && r.site && h && h->x != w2::NONE) kit::mapTarget(MAP_X, MAP_Y, h->x, h->y, r.site->x, r.site->y);
    if (h && h->x != w2::NONE) front::drawHeroFigure(MAP_X, MAP_Y, h->x, h->y);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), "Quest", 432, 64);                 // 4125:00c8
    gfx::draw(G.scrollPic, 304, 95);
    const Font& f = kit::font(2).colours(0, 7);
    kit::centred(f, (h ? h->name : std::string()) + "'s Quest", 440, 145);  // 4125:00ce
    kit::setPal(0);
    gfx::rectangle(gfx::FILL, 360, 165, 160, 1);
    kit::centred(f, kit::text(0x1c, 0), 440, 175);
    kit::centred(f, kit::text(0x1c, 1), 440, 195);
    kit::centred(f, kit::text(grp, 0), 440, 225);
    kit::centred(f, kit::text(grp, 1), 440, 245);
    kit::centred(f, last, 440, 265);
    kit::drawControls(v);
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (c && c->id == DONE) close();
  }
  void keypressed(const std::string& key) override {
    if (key == "escape" || key == "return" || key == "kpenter") close();
  }
};
}  // namespace

void show(std::shared_ptr<w2::QuestResult> news, kit::Done after) {
  if (!news) return;
  if (!news->failed.empty() || !news->reward) {
    int grp = news->why ? news->why : 0x20;
    search::message(kit::text(grp, 0), kit::text(grp, 1), after);
    return;
  }
  sound::music(w2::cues::TRIUMPH);
  auto d = std::make_shared<Reward>();
  d->news = news;
  d->after = after;
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->v.state[DONE] = w2::uidata::NORMAL;
  const w2::Reward& r = *news->reward;
  if (r.kind == "revealed") { d->grp = 0x1d; d->last = r.site ? r.site->name : ""; }
  else if (r.kind == "item") { d->grp = 0x1e; d->last = r.item ? r.item->name : ""; }
  else if (r.kind == "allies") {
    d->grp = 0x1f;
    d->last = format(kit::text(0x1f, 2), (int)r.armies.size(), r.type ? r.type->name : "");
  } else {
    d->grp = 0x1f;
    d->last = format(kit::text(0x1f, 2), r.gold, "gold");     // 4125:00d9
  }
  kit::push(d);
}

void poll() {
  w2::Side* side = G.player;
  if (!side || side->computer || !side->questNews) return;
  auto news = side->questNews;
  side->questNews.reset();
  show(news);
}

}  // namespace questnews
