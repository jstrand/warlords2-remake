// Game > Settings (64d2:0000(1)) -- the same screen that sets the sides up
// for a new game.
//
// Popup 4, (120, 50) 400x360, dialog 9 (64d2:0137): a row a side -- its name
// in its colours, "Deceased!" for a side out of play, else the Human box and
// "Human" or its level, Enhanced, and for a computer Observe -- and the boxes
// for Music, Effects and Speech. 252-259 turn a side human or computer,
// 260-267 Enhanced, 268-275 Observe, 276-278 the sounds (64d2:0576 writes
// OPTIONS.SND straight back); OK (251).
#include "platform/sound.hpp"
#include "ui/dialogs.hpp"

namespace settings {

namespace {
const kit::Rect R{120, 50, 400, 360};     // popup 4
const int DIALOG = 9, OK = 251;
const int HUMAN = 252, ENHANCED = 260, OBSERVE = 268, SOUND = 276;
const char* const LEVELS[3] = {"Knight", "Lord", "Warlord"};    // 4125:0bf2
struct SoundBox { int x, y; const char* label; const char* key; };
const SoundBox SOUNDS[3] = {{152, 340, "Music", "music"}, {152, 370, "Effects", "effects"}, {256, 340, "Speech", "speech"}};

struct Settings : kit::Modal {
  screen::View v;
  kit::Done after;
  void refresh() {
    auto& st = v.state;
    st[OK] = w2::uidata::NORMAL;
    for (int k = 0; k < 3; k++) st[SOUND + k] = w2::uidata::NORMAL;
    hidden.clear();
    for (int i = 0; i < 8; i++) {
      if (G.g->map->sides[i].inUse) {
        st[HUMAN + i] = st[ENHANCED + i] = st[OBSERVE + i] = w2::uidata::NORMAL;
      } else {
        hidden.insert(HUMAN + i);
        hidden.insert(ENHANCED + i);
        hidden.insert(OBSERVE + i);
      }
    }
  }
  static void box(bool on, int x, int y) {
    gfx::setColor(1, 1, 1);
    gfx::draw(G.abits, gfx::newQuad(320, on ? 0 : 20, 24, 20), x, y);
  }
  void draw() override {
    kit::popup(R);
    const Font& f = kit::font(2);
    w2::Side* me = G.player;
    const Font& head = f.colours(me->colour, me->edge);
    gfx::setColor(1, 1, 1);
    kit::right(head, "Name", 248, 60);
    kit::centred(head, "Human", 280, 60);
    kit::centred(head, "Enhanced", 396, 60);
    kit::centred(head, "Observe", 468, 60);
    for (int i = 0; i < 8; i++) {
      const w2::Side& s = G.g->map->sides[i];
      int y = 90 + 30 * i;
      gfx::setColor(1, 1, 1);
      kit::right(f.colours(s.colour, s.edge), s.name, 248, y);
      bool playing = s.inUse && s.alive;
      if (!playing) {
        if (s.name != "Not used") f.draw("Deceased!", 280, y);
      } else {
        box(!s.computer, 256, y);
        gfx::setColor(1, 1, 1);
        f.draw(s.computer ? (s.level >= 0 && s.level < 3 ? LEVELS[s.level] : "") : "Human", 280, y);
        box(s.enhanced, 400, y);
        if (s.computer) box(s.observe, 448, y);
      }
    }
    const auto& on = sound::options();
    bool state[3] = {on.music, on.effects, on.speech};
    for (int k = 0; k < 3; k++) {
      box(state[k], SOUNDS[k].x, SOUNDS[k].y);
      gfx::setColor(1, 1, 1);
      f.draw(SOUNDS[k].label, SOUNDS[k].x + 24, SOUNDS[k].y);
    }
    kit::drawControls(v, hidden);
  }
  void finish() {
    auto a = after;
    kit::pop(this);
    if (a) a();
  }
  void mousepressed(int x, int y, int) override {
    auto c = kit::controlAt(v, x, y, hidden);
    if (!c || v.stateOf(c->id) == w2::uidata::DISABLED) return;
    int id = c->id;
    if (id == OK) return finish();
    if (id >= HUMAN && id < HUMAN + 8) {
      w2::Side& s = G.g->map->sides[id - HUMAN];
      s.computer = !s.computer;
    } else if (id >= ENHANCED && id < ENHANCED + 8) {
      w2::Side& s = G.g->map->sides[id - ENHANCED];
      s.enhanced = !s.enhanced;
    } else if (id >= OBSERVE && id < OBSERVE + 8) {
      w2::Side& s = G.g->map->sides[id - OBSERVE];
      if (s.computer) s.observe = !s.observe;
    } else if (id >= SOUND && id < SOUND + 3) {
      sound::toggle(SOUNDS[id - SOUND].key);
    }
    refresh();
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter" || key == "escape") finish();
  }
};
}  // namespace

void open(kit::Done after) {
  auto d = std::make_shared<Settings>();
  d->v = kit::view(DIALOG);
  d->view = &d->v;
  d->after = after;
  d->refresh();
  kit::push(d);
}

}  // namespace settings
