#include "warlords/armytype.hpp"

#include <stdexcept>

#include "util/util.hpp"
#include "warlords/bytes.hpp"

namespace w2::armytype {

void load(const std::string& path, Types& out) {
  const int COUNT = 29, STRIDE = 62, N_BONUS = 15;
  std::string s = mustRead(path);
  if ((int)s.size() != COUNT * STRIDE) throw std::runtime_error(fmt("unexpected %s size: %d", path.c_str(), (int)s.size()));
  out.all.clear();
  out.all.reserve(COUNT);
  for (int i = 0; i < COUNT; i++) {
    int o = i * STRIDE;
    ArmyType a;
    for (int b = 0; b < N_BONUS; b++) a.bonus[32 + b * 2] = u16(s, o + 32 + b * 2);
    a.id = u16(s, o);
    a.name = cstr(s, o + 2, 16);
    a.strength = u16(s, o + 22);
    a.time = u16(s, o + 24);
    a.cost = u16(s, o + 26);
    a.move = u16(s, o + 28);
    a.price = i16(s, o + 30);
    a.flies = a.bonus[FLIES] != 0;
    a.siege = a.bonus[SIEGE] == 1;
    a.boat = a.bonus[BOAT] != 0;
    a.woodsMove = a.bonus[WOODS_MOVE] != 0;
    a.hillsMove = a.bonus[HILLS_MOVE] != 0;
    out.all.push_back(a);
  }
  out.ids.fill(nullptr);
  for (auto& a : out.all) if (a.id >= 0 && a.id < 64) out.ids[a.id] = &a;
}

}  // namespace w2::armytype
