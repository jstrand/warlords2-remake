#include "warlords/slots.hpp"

#include <algorithm>
#include <deque>
#include <map>

#include "util/util.hpp"

namespace w2::slots {

namespace {
const std::vector<int>& row(const Game& g, int side) { return g.map->fightOrder[side == NONE ? 8 : side]; }

int rank(const Game& g, const Slots& s, int i) { return row(g, s.side)[s.army[i]->type]; }

void swap(Slots& s, int i, int j) {
  std::swap(s.army[i], s.army[j]);
  std::swap(s.group[i], s.group[j]);
  std::swap(s.inGroup[i], s.inGroup[j]);
}

// Insertion-sort the slots so each group is contiguous: by group, then by
// fight order within it.
void order(Slots& s, const Game& g) {
  for (int i = 1; i < s.n; i++) {
    int j = i;
    while (j > 0 && (s.group[j] < s.group[j - 1] ||
                     (s.group[j] == s.group[j - 1] && rank(g, s, j - 1) < rank(g, s, j)))) {
      swap(s, j, j - 1);
      j--;
    }
  }
}

// Work out the marks, and which group is the one that moves.
void remark(Slots& s) {
  for (int i = 0; i < s.n; i++) s.mark[i] = (i > 0 && s.group[i] == s.group[i - 1]) ? NOMARK : CROSS;
  s.active.reset();
  int head = -1;
  for (int i = 0; i < s.n; i++) if (s.inGroup[i]) { head = i; break; }
  if (head < 0) return;
  for (int i = 0; i < s.n; i++) {
    if (s.inGroup[i] != (s.group[i] == s.group[head])) return;
  }
  s.mark[head] = TICK;
  s.active = s.group[head];
}

void refresh(Slots& s, const Game& g, bool resort = false) {
  if (resort) order(s, g);
  remark(s);
}
}  // namespace

std::set<Army*> clicked(const Game& g, const std::vector<Army*>& armies, int side, Army* anchor) {
  const auto& r = row(g, side);
  if (!anchor) {
    for (Army* a : armies) {
      if (!anchor || r[a->type] > r[anchor->type]) anchor = a;
    }
  }
  if (!anchor) return {};
  int key = anchor->group;
  std::set<Army*> out = {anchor};
  if (key != UNGROUPED) {
    for (Army* a : armies) if (a->group == key) out.insert(a);
  }
  return out;
}

Slots build(const Game& g, const std::vector<Army*>& armies, int side, const std::set<Army*>* selectedIn) {
  Slots s;
  s.n = std::min((int)armies.size(), MAX);
  s.side = side;
  std::vector<Army*> list(armies.begin(), armies.begin() + s.n);
  std::set<Army*> chosen = selectedIn ? *selectedIn : clicked(g, list, side);

  // 1b62:0a03's order: the remembered groups first, and within a group the
  // army highest in the fight order.
  std::map<Army*, int> at;
  for (int i = 0; i < s.n; i++) at[list[i]] = i;
  const auto& rowT = row(g, side);
  luaSort(list, [&](Army* p, Army* q) {
    int gp = p->group, gq = q->group;
    if (gp != gq) return gp > gq;
    int rp = rowT[p->type], rq = rowT[q->type];
    if (rp != rq) return rp > rq;
    return at[p] < at[q];
  });
  for (int i = 0; i < s.n; i++) s.army[i] = list[i];

  // 89e0:0d30 numbers the groups off down the list
  int n = -1;
  bool havePrev = false;
  int previous = 0;
  for (int i = 0; i < s.n; i++) {
    int key = s.army[i]->group;
    if (!(havePrev && key == previous && key != UNGROUPED && n >= 0)) n++;
    s.group[i] = n;
    s.inGroup[i] = chosen.count(s.army[i]) > 0;
    previous = key;
    havePrev = true;
  }
  refresh(s, g);
  return s;
}

void toggle(Slots& s, const Game& g, int i) {
  if (i < 0 || i >= s.n) return;
  if (!s.inGroup[i]) {
    for (int j = 0; j < s.n; j++) {
      if (s.inGroup[j]) { s.group[i] = s.group[j]; break; }
    }
    s.inGroup[i] = true;
  } else {
    std::map<int, int> members;
    for (int j = 0; j < s.n; j++) members[s.group[j]]++;
    if (members[s.group[i]] != 1) {
      int free = 0;
      while (members.count(free) && members[free]) free++;
      s.inGroup[i] = false;
      s.group[i] = free;
    }
  }
  refresh(s, g, true);
}

void pickGroup(Slots& s, const Game& g, int i) {
  if (i < 0 || i >= s.n) return;
  int want = s.group[i];
  for (int j = 0; j < s.n; j++) s.inGroup[j] = s.group[j] == want;
  refresh(s, g);
}

void all(Slots& s, const Game& g) {
  for (int i = 0; i < s.n; i++) { s.group[i] = 0; s.inGroup[i] = true; }
  refresh(s, g, true);
}

void single(Slots& s, const Game& g) {
  for (int i = 0; i < s.n; i++) { s.group[i] = i; s.inGroup[i] = false; }
  if (s.n > 0) s.inGroup[0] = true;
  refresh(s, g, true);
}

std::vector<Army*> selected(const Slots& s) {
  std::vector<Army*> out;
  for (int i = 0; i < s.n; i++) if (s.inGroup[i]) out.push_back(s.army[i]);
  return out;
}

int moves(const Slots& s) {
  int least = NONE;
  for (int i = 0; i < s.n; i++) {
    if (s.inGroup[i]) {
      int m = s.army[i]->moves;
      if (least == NONE || m < least) least = m;
    }
  }
  return least == NONE ? 0 : least;
}

bool grouped(const Slots& s) {
  if (s.n == 0) return false;
  if (s.n == 1) return true;
  for (int i = 0; i < s.n; i++) if (!s.inGroup[i]) return false;
  return true;
}

std::optional<Slots> keep(const Slots& s, const Game& g, const std::vector<Army*>& armies) {
  std::set<Army*> was;
  for (int i = 0; i < s.n; i++) if (s.inGroup[i]) was.insert(s.army[i]);
  std::set<Army*> sel;
  for (Army* a : armies) if (was.count(a)) sel.insert(a);
  if (armies.empty()) return std::nullopt;
  return build(g, armies, s.side, sel.empty() ? nullptr : &sel);
}

void commit(Slots& s, Game& g) {
  std::map<int, int> members;
  for (int i = 0; i < s.n; i++) members[s.group[i]]++;

  std::set<int> spareSet;
  std::deque<int> spare;
  for (int i = 0; i < s.n; i++) {
    int id = s.army[i]->group;
    if (id > 1 && !spareSet.count(id)) { spareSet.insert(id); spare.push_back(id); }
  }
  auto claim = [&]() {
    if (!spare.empty()) { int v = spare.front(); spare.pop_front(); return v; }
    std::set<int> used;
    for (Army* a : g.armies) used.insert(a->group);
    int fresh = 2;
    while (used.count(fresh)) fresh++;
    return fresh;
  };

  std::map<int, int> id;
  for (int i = 0; i < s.n; i++) {
    int gp = s.group[i];
    if (!id.count(gp)) {
      if (members[gp] == 1) id[gp] = UNGROUPED;
      else if (s.active && gp == *s.active) id[gp] = claim();
      else id[gp] = 1;
    }
    s.army[i]->group = id[gp];
  }
}

}  // namespace w2::slots
