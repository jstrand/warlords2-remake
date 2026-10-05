// The end of the game (8065:1aed and the screens it leads to).
//
// The three pictures are popups 20-22, (136, 40) 368x390, each its own
// bitmap with the words painted in:
//   20 RESIGN.PCK   "An offer of Peace" -- dialog 30: No (486), Yes (485)
//   21 RESIGNNO.PCK "Peace is not an option!" -- dialog 31: Done (484)
//   22 RESIGNYE.PCK "Congratulations!" -- dialog 32: Done (483)
// Each comes with its own music.
//
// The round's end puts out the sides left without a city first, each told
// with a line of group 11 -- in a box, or in the status bar when no human
// plays (8065:18ab) -- and says once when the last human has fallen (group
// 13). When the last computer side has triumphed (group 15) it is handed to
// the player, its turn opening as a human's, to look the world over.
#include "platform/sound.hpp"
#include "ui/dialogs.hpp"
#include "util/util.hpp"
#include "warlords/cues.hpp"

namespace ending {

namespace {
const kit::Rect R{136, 40, 368, 390};     // popups 20-22
int picture(int popup) { return popup == 20 ? 12 : popup == 21 ? 14 : 13; }   // FILE.DAT group 3
std::string t(int g, int i) { return kit::text(g, i); }

struct Picture : kit::Modal {
  int popup = 20;
  screen::View v;
  int dflt = 0, cancel = 0;
  std::function<void(int)> pick;
  void draw() override {
    kit::popupFrame(R);
    if (auto art = G.screen->artFor(picture(popup))) {
      gfx::setColor(1, 1, 1);
      gfx::draw(art->image, gfx::newQuad(0, 0, R.w, R.h), R.x, R.y);
    }
    kit::drawControls(v);
  }
  void press(int id) {
    auto p = pick;
    kit::pop(this);
    p(id);
  }
  void mousepressed(int x, int y, int) override {
    if (auto c = kit::controlAt(v, x, y)) press(c->id);
  }
  void keypressed(const std::string& key) override {
    if (key == "return" || key == "kpenter") press(dflt);
    else if (key == "escape") press(cancel);
  }
};

void showPicture(int popup, int dialog, std::vector<int> ids, int dflt, int cancel, std::function<void(int)> pick) {
  auto d = std::make_shared<Picture>();
  d->popup = popup;
  d->v = kit::view(dialog);
  d->view = &d->v;
  for (int id : ids) d->v.state[id] = w2::uidata::NORMAL;
  d->dflt = dflt;
  d->cancel = cancel;
  d->pick = pick;
  kit::push(d);
}

// Congratulations (8065:1fbd), then the map shown whole (8065:2004).
void victory(kit::Done after) {
  sound::music(w2::cues::TRIUMPH);
  showPicture(22, 32, {483}, 483, 483, [after](int) {
    G.g->map->options.hiddenMap = 0;
    front::stratDirty();
    search::message(t(0x10, 0), t(0x10, 1), after);
  });
}
}  // namespace

std::string fallenText(const w2::Fallen& f) { return w2::format(t(0xb, f.line), f.side->name); }

namespace {
// What the round's end put out and whether the last human fell, in boxes,
// unless the computer's turns have told it already; then `after`.
void tell(w2::Ending& e, kit::Done after) {
  auto done = [after]() { if (after) after(); };
  if (e.told) return done();
  e.told = true;
  std::vector<std::string> lines;
  for (auto& f : e.fallen) lines.push_back(fallenText(f));
  bool noHumans = e.noHumans;
  auto next = std::make_shared<std::function<void(size_t)>>();
  std::weak_ptr<std::function<void(size_t)>> self = next;
  *next = [lines, noHumans, done, self](size_t i) {
    if (i < lines.size()) {
      auto again = self.lock();
      search::say(lines[i], [again, i]() { (*again)(i + 1); });
      return;
    }
    if (noHumans) {
      search::message(t(0xd, 0), t(0xd, 1), [done]() { search::message(t(0xd, 2), t(0xd, 3), done); });
      return;
    }
    done();
  };
  (*next)(0);
}
}  // namespace

void show(w2::Ending& e, kit::Done after) {
  auto done = [after]() { if (after) after(); };
  if (!e.told) {
    tell(e, [&e, after]() { show(e, after); });
    return;
  }
  // the last computer side has triumphed, and is the player's now
  if (e.triumph && e.winner == G.player && !e.shown) {
    e.shown = true;
    search::say(w2::format(t(0xf, 0), G.player->name), done);
    return;
  }
  if (e.surrender && !e.shown) {
    e.shown = true;
    sound::music(w2::cues::SURRENDER);                  // 8065:1f68
    showPicture(20, 30, {485, 486}, 485, 486, [after, done](int id) {
      if (id == 485) {
        w2::game::acceptSurrender(*G.g, *G.player);
        victory(after);
      } else {
        sound::music(w2::cues::DEFIANCE);               // 8065:1ecd
        showPicture(21, 31, {484}, 484, 484, [done](int) { done(); });
      }
    });
    return;
  }
  if (e.won && e.winner == G.player) {
    search::message(w2::format(t(0xf, 0), G.player->name), t(0xf, 1), [after]() { victory(after); });
    return;
  }
  done();
}

void over(w2::Ending& e) {
  if (!e.told && !e.fallen.empty()) {
    tell(e, [&e]() { over(e); });
    return;
  }
  if (e.winner) {
    search::say(w2::format(t(0xf, 0), e.winner->name));
    return;
  }
  bool humans = false;
  for (w2::Side* s : G.g->sides)
    if (s->alive && !s->computer && !w2::game::sideCities(*G.g, *s).empty()) humans = true;
  if (!humans && w2::contains(e.message, "No more players")) {
    search::message(t(0xc, 0), t(0xc, 1), []() {
      // "return to DOS" -- here, the start screens
      search::message(t(0xc, 2), t(0xc, 3), []() { front::quit(); });
    });
    return;
  }
  search::message(t(0xd, 0), t(0xd, 1), []() { search::message(t(0xd, 2), t(0xd, 3)); });
}

}  // namespace ending
