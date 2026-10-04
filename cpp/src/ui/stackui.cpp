// View > Stack (89e0:0c9c): the selected stack laid out at length, to be
// grouped the way the bar under the map groups it.
//
// It works on a copy of the bar's slots and its clicks are the bar's: an army
// (336-343) joins the group that moves or leaves it, a mark (344-351) makes
// its group the one, Group (334) puts the lot together and Ungroup (335)
// breaks it up. OK (332) keeps it, Cancel (333) throws it away.
//
// Popup 0, (80, 60) 480x320 (89e0:0e85): STACK.PCK's column heads with the
// combat cap, and eight rows 30 apart from (112, 90): the mark, the army on a
// ring of its group's colour (its shadow when not moving), name, strength --
// and for the group that moves its strength in a fight here (89e0:1b9c) --
// moves, how it moves and its bonus.
#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/combat.hpp"
#include "warlords/rules.hpp"

namespace stackui {

namespace {
const kit::Rect R{80, 60, 480, 320};      // popup 0
const int DIALOG = 18, OK = 332, CANCEL = 333, GROUP = 334, UNGROUP = 335, ARMY = 336, MARK = 344;
const int ROWS = 8;

bool flies(const w2::Army* a, const w2::ArmyType* t) {
  if (t->flies) return true;
  if (!a->hero()) return false;
  for (w2::Item* it : a->items) if (it->type == w2::rules::ITEM_FLIGHT) return true;
  return false;
}

struct Stack : kit::Modal {
  screen::View v;
  w2::slots::Slots s;
  void refresh() {
    auto& st = v.state;
    st[OK] = st[CANCEL] = w2::uidata::NORMAL;
    st[GROUP] = s.n > 1 ? w2::uidata::NORMAL : w2::uidata::DISABLED;
    st[UNGROUP] = st[GROUP];
    for (int i = 0; i < ROWS; i++) {
      st[ARMY + i] = i < s.n ? w2::uidata::NORMAL : w2::uidata::DISABLED;
      st[MARK + i] = st[ARMY + i];
    }
  }
  void draw() override {
    w2::Side* side = G.player;
    kit::popup(R);
    gfx::setColor(1, 1, 1);
    if (auto head = G.screen->artFor(46)) gfx::draw(head->image, gfx::newQuad(0, 0, 400, 23), 80, 62);
    const Font& f = kit::font(2);
    f.draw(w2::fmt("(max +%d)", G.g->map->combatCap), 480, 66);      // 4125:320d
    auto abits = [](int sx, int sy, int w, int h, int x, int y) {
      gfx::setColor(1, 1, 1);
      gfx::draw(G.abits, gfx::newQuad(sx, sy, w, h), x, y);
    };
    std::vector<w2::Army*> moving;
    for (int i = 0; i < s.n; i++) if (s.inGroup[i]) moving.push_back(s.army[i]);
    w2::Army* a1 = s.n > 0 ? s.army[0] : nullptr;
    auto fight = a1 ? w2::combat::stackStrengths(*G.g, moving, a1->x, a1->y) : std::map<const w2::Army*, int>{};
    for (int i = 0; i < ROWS; i++) {
      int x = 112, y = 90 + 30 * i;
      if (i < s.n && s.army[i]) {
        w2::Army* a = s.army[i];
        const w2::ArmyType* t = G.g->types.byId(a->type);
        if (s.mark[i] != w2::slots::NOMARK) abits(448, s.mark[i] == w2::slots::TICK ? 16 : 0, 32, 16, x - 32, y + 5);
        kit::army(a->type, side->index, x, y, (side->index + s.group[i]) % 8 + 2, !s.inGroup[i]);
        gfx::setColor(1, 1, 1);
        f.draw(a->hero() ? a->name : t->name, x + 48, y + 5);
        int str = a->strength;
        if (a->hero()) str += w2::combat::battleItems(*a);
        f.draw(w2::fmt("%d", std::min(9, str)), x + 168, y + 5);
        auto fi = fight.find(a);
        if (s.inGroup[i] && fi != fight.end()) f.draw(w2::fmt("(%d)", fi->second), x + 184, y + 5);
        f.draw(w2::fmt("%d", a->moves), x + 232, y + 5);
        int src = -1;
        if (flies(a, t)) src = 184;
        else if (a->atSea) src = 424;
        else if (t->woodsMove && t->hillsMove) src = 216;
        else if (t->woodsMove) src = 248;
        else if (t->hillsMove) src = 152;
        if (src >= 0) abits(src, 30, 32, 10, x + 256, y + 8);
        gfx::setColor(1, 1, 1);
        f.draw(armybonus::bonusText(*t, a), x + 304, y + 5);
      } else {
        kit::army(w2::NONE, side->index, x, y, 1);
      }
    }
    std::set<int> h;
    for (int i = 0; i < ROWS; i++) { h.insert(ARMY + i); h.insert(MARK + i); }
    kit::drawControls(v, h);
  }
  void keep() {
    w2::slots::Slots kept = s;
    kit::pop(this);
    if (G.selection) G.selection->slots = kept;
    front::afterSlotChange();
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y);
    if (!c || v.stateOf(c->id) == w2::uidata::DISABLED) return;
    int id = c->id;
    if (id == OK) return keep();
    if (id == CANCEL) { kit::pop(this); return; }
    if (id == GROUP) w2::slots::all(s, *G.g);
    else if (id == UNGROUP) w2::slots::single(s, *G.g);
    else if (id >= ARMY && id < ARMY + ROWS) w2::slots::toggle(s, *G.g, id - ARMY);
    else if (id >= MARK && id < MARK + ROWS) w2::slots::pickGroup(s, *G.g, id - MARK);
    refresh();
  }
  // the right button on a row (sub-ids 37-44, 89e0:1747)
  bool info(int sub, int sx, int sy) override {
    int n = sub - 37;
    if (n >= 0 && n < s.n) infobox::army(sx, sy, s.army[n]);
    else infobox::lines(sx, sy, "Select Army", "Select armies when present");
    return true;
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter") keep();
    else if (key == "escape") kit::pop(this);
  }
};
}  // namespace

void open() {
  if (!G.selection) return;
  auto d = std::make_shared<Stack>();
  d->s = G.selection->slots;
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->refresh();
  kit::push(d);
}

}  // namespace stackui
