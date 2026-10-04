// Hero > Inspect: the hero info dialog.
//
// 6c1b:0000 lists the side's heroes -- by army, last first -- and opens
// dialog 8 over popup 2 on the one nearest the cursor. 6c1b:00f9 draws the
// strategic map with every hero's figure, the hero's name, its stack along
// y = 110, where it is, its battle and command bonuses, level and
// experience; 6c1b:0724 the items it carries and those on the ground.
// Controls: 242 Done, 243/244 next and previous hero, 245/246 the item
// above and below, 247 Drop It, 248 Take It, 250/249 Carried and Ground.
#include <cstdlib>

#include "platform/sound.hpp"
#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/armytype.hpp"
#include "warlords/combat.hpp"
#include "warlords/hero.hpp"
#include "warlords/rules.hpp"

namespace heroinfo {

namespace rules = w2::rules;

namespace {
const kit::Rect R{80, 60, 480, 312};      // popup 2
const int MAP_X = 80, MAP_Y = 60;
const int DIALOG = 8;
const int DONE = 242, NEXT = 243, PREV = 244, UP = 245, DOWN = 246, DROP = 247, TAKE = 248, GROUND = 249, CARRIED = 250;
const int S = 0x83;                       // STRING.DAT group 131
const int CHECKED[2] = {320, 0}, CLEAR[2] = {320, 20};   // 4125:0ad4

int itemColour(const w2::Item* it) {
  if (it->status == 1) return 10;
  if (it->status == 0) return 5;
  return 7;
}

// What an item does, as the dialog abbreviates it (4125:0d7c-0d9c).
std::string effect(const w2::Item* it) {
  int t = it->type, v = it->value;
  if (t == rules::ITEM_STANDARD) return "com +1";
  if (t == rules::ITEM_COMMAND) return w2::fmt("com +%d", v);
  if (t == rules::ITEM_BATTLE) return w2::fmt("bat +%d", v);
  if (t == rules::ITEM_FLIGHT) return "fly";
  if (t == rules::ITEM_DOUBLE_MOVE) return "move";
  if (t == rules::ITEM_GOLD_PER_CITY) return w2::fmt("gld +%d", v);
  return "";
}

int itemSum(const w2::Army* h, int t) {
  int n = 0;
  for (w2::Item* it : h->items) {
    if (it->type == t) n += it->value;
    if (t == rules::ITEM_COMMAND && it->type == rules::ITEM_STANDARD) n++;
  }
  return n;
}

struct HeroInfo : kit::Modal {
  screen::View v;
  std::vector<w2::Army*> heroes;
  int cur = 0;
  int item = w2::NONE;
  bool ground = false;

  std::vector<w2::Item*> list() { return w2::hero::itemsHere(*G.g, heroes[cur]); }

  // choose item i of the list, from 0 (or none)
  void choose(int i) {
    auto items = list();
    item = (i != w2::NONE && i >= 0 && i < (int)items.size()) ? i : w2::NONE;
    ground = item != w2::NONE && items[item]->status == 1;
    auto& st = v.state;
    st[DONE] = w2::uidata::NORMAL;
    st[NEXT] = heroes.size() > 1 ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    st[PREV] = st[NEXT];
    st[UP] = (item != w2::NONE && item > 0) ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    st[DOWN] = (item != w2::NONE && item < (int)items.size() - 1) ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    st[DROP] = item != w2::NONE ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    st[TAKE] = st[DROP];
    hidden = {ground ? DROP : TAKE};
  }

  void showHero(int i) {
    int n = (int)heroes.size();
    cur = ((i % n) + n) % n;
    choose(!list().empty() ? 0 : w2::NONE);
  }

  // the first item of a kind: 3 carried, 1 on the ground (6c1b:0fc2, 1037)
  void firstOf(int status) {
    auto items = list();
    for (size_t i = 0; i < items.size(); i++)
      if (items[i]->status == status) { choose((int)i); return; }
  }

  void close() {
    kit::pop(this);
    w2::quest::event(*G.g, *G.player, "item");
  }

  void draw() override {
    w2::Army* h = heroes[cur];
    const Font& f = kit::font(2);
    kit::popup(R);
    front::drawStrategicMap(MAP_X, MAP_Y, nullptr, true);
    for (size_t i = 0; i < heroes.size(); i++)
      if ((int)i != cur) front::drawHeroFigure(MAP_X, MAP_Y, heroes[i]->x, heroes[i]->y);
    front::drawHeroFigure(MAP_X, MAP_Y, h->x, h->y);
    kit::setPal(0);
    kit::outline(308, 206, 248, 129);
    kit::bevel(307, 205, 250, 131, 4, 2);
    gfx::setColor(1, 1, 1);
    kit::centred(kit::font(1), h->name, 432, 62);
    kit::army(w2::armytype::HERO, G.player->index, 304, 110, 1);
    int k = 1;
    for (w2::Army* a : w2::game::armiesAt(*G.g, h->x, h->y)) {
      if (a != h && k < 8) {
        kit::army(a->type, a->owner, 304 + k * 32, 110, 1);
        k++;
      }
    }
    for (int j = k; j <= 7; j++) kit::army(w2::NONE, G.player->index, 304 + j * 32, 110, 1);
    w2::City* inCity = w2::game::cityAt(*G.g, h->x, h->y);
    w2::City* near = nullptr;
    int nearD = w2::NONE;
    for (auto& c : G.g->map->cities) {
      if (w2::game::seen(*G.g, G.player->index, c.x, c.y)) {
        int dd = std::max(std::abs(c.x - h->x), std::abs(c.y - h->y));
        if (nearD == w2::NONE || dd < nearD) { near = &c; nearD = dd; }
      }
    }
    int battle = itemSum(h, rules::ITEM_BATTLE);
    int command = w2::combat::HERO_TABLE[std::max(0, std::min(9, h->strength + battle))] + itemSum(h, rules::ITEM_COMMAND);
    gfx::setColor(1, 1, 1);
    kit::right(f, kit::text(S, inCity ? 1 : 0), 384, 145);
    kit::right(f, kit::text(S, 2), 384, 165);
    kit::right(f, kit::text(S, 3), 384, 185);
    kit::right(f, kit::text(S, 4), 528, 165);
    kit::right(f, kit::text(S, 5), 528, 185);
    f.draw(near ? near->name : "", 392, 145);
    f.draw(w2::fmt("+%d", battle), 392, 165);
    f.draw(w2::fmt("+%d", command), 392, 185);
    f.draw(w2::fmt("%d", h->level ? h->level : 1), 536, 165);
    f.draw(w2::fmt("%d", h->experience), 536, 185);
    f.draw(w2::format(kit::text(S, 6), cur + 1, (int)heroes.size()), 312, 347);
    auto items = list();
    kit::setPal(3);
    gfx::rectangle(gfx::FILL, 367, 236, 184, 66);
    kit::bevel(367, 236, 184, 66, 4, 2);
    kit::bevel(369, 258, 180, 21, 2, 4);
    gfx::setColor(1, 1, 1);
    kit::centred(f, kit::text(S, ground ? 7 : 8), 432, 211);
    if (item != w2::NONE) {
      for (int row = 0; row <= 2; row++) {
        int i = item - 1 + row;
        if (i >= 0 && i < (int)items.size()) f.colours(itemColour(items[i]), 0).draw(items[i]->name, 376, 238 + 22 * row);
      }
      w2::Item* it = items[item];
      kit::centred(f.colours(itemColour(it), 0), effect(it), 340, 260);
    }
    auto box = [](const int* src, int x) {
      gfx::setColor(1, 1, 1);
      gfx::draw(G.abits, gfx::newQuad(src[0], src[1], 24, 20), x, 311);
    };
    box(ground ? CLEAR : CHECKED, 312);
    box(ground ? CHECKED : CLEAR, 400);
    f.colours(7, 0).draw("Carried", 336, 313);
    f.colours(10, 0).draw("Ground", 424, 313);
    kit::drawControls(v, hidden);
  }

  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y, hidden);
    if (!c) return;
    auto items = list();
    if (c->id == DONE) close();
    else if (c->id == NEXT) showHero(cur + 1);
    else if (c->id == PREV) showHero(cur - 1);
    else if (c->id == UP) choose(item - 1);
    else if (c->id == DOWN) choose(item + 1);
    else if (c->id == DROP && item != w2::NONE) {
      w2::Item* it = items[item];
      w2::hero::dropItem(*G.g, heroes[cur], it);
      // 7563:0943: dropped at sea it is gone, with a splash
      if (it->status == 0) sound::effect("splash");
      choose(std::min(item, (int)list().size() - 1));
    } else if (c->id == TAKE && item != w2::NONE) {
      w2::hero::takeItem(*G.g, heroes[cur], items[item]);
      choose(std::min(item, (int)list().size() - 1));
    } else if (c->id == GROUND) {
      firstOf(1);
    } else if (c->id == CARRIED) {
      firstOf(3);
    }
  }

  void keypressed(const std::string& key) override {
    if (key == "escape" || key == "return" || key == "kpenter") close();
  }
};
}  // namespace

void open() {
  auto d = std::make_shared<HeroInfo>();
  for (int i = (int)G.g->armies.size() - 1; i >= 0; i--) {
    w2::Army* a = G.g->armies[i];
    if (a->hero() && a->owner == G.player->index && !a->transit && a->x != w2::NONE) d->heroes.push_back(a);
  }
  if (d->heroes.empty()) return;
  auto [cx, cy] = front::viewCentre();
  int cur = 0, best = w2::NONE;
  for (size_t i = 0; i < d->heroes.size(); i++) {
    int dd = std::max(std::abs(d->heroes[i]->x - cx), std::abs(d->heroes[i]->y - cy));
    if (best == w2::NONE || dd < best) { cur = (int)i; best = dd; }
  }
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->showHero(cur);
  kit::push(d);
}

}  // namespace heroinfo
