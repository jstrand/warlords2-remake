#include "warlords/uidata.hpp"

#include <stdexcept>

#include "util/util.hpp"
#include "warlords/bytes.hpp"

namespace w2::uidata {

static const int MAGIC = 0x7d00;

Strings strings(const std::string& path) {
  std::string s = mustRead(path);
  int nGroups = u16(s, 0) / 4;              // the first offset IS the index size
  Strings groups;
  for (int g = 0; g < nGroups; g++) {
    int off = u16(s, g * 4), count = u16(s, g * 4 + 2);
    std::vector<std::string> out;
    for (int i = 0; i < count; i++) {
      int at = u16(s, off + i * 2);
      out.push_back(cstr(s, at, s.size() > (size_t)at ? s.size() - at : 0));
    }
    groups.push_back(out);
  }
  return groups;
}

std::map<int, Join> joins(const std::string& path) {
  std::string s = mustRead(path);
  std::map<int, Join> out;
  for (size_t i = 0; i < s.size() / 6; i++) {
    size_t at = i * 6;
    out[u16(s, at)] = Join{u16(s, at + 2), u16(s, at + 4)};
  }
  return out;
}

std::map<int, Area> areas(const std::string& path) {
  std::string s = mustRead(path);
  std::map<int, Area> out;
  size_t at = 0;
  while (at + 13 < s.size()) {
    if (u16(s, at) != MAGIC) throw std::runtime_error("AREA.DAT: lost the record boundary");
    Area a;
    a.id = u16(s, at + 2);
    a.enabled = u16(s, at + 4);
    int count = u16(s, at + 6);
    for (int i = 0; i < count; i++) {
      size_t r = at + 14 + i * 14;
      a.regions.push_back(Region{u16(s, r), u16(s, r + 4), u16(s, r + 6), u16(s, r + 8), u16(s, r + 10)});
    }
    out[a.id] = a;
    at += 14 + count * 14;
  }
  return out;
}

std::map<int, ButtonGroup> buttons(const std::string& path) {
  std::string s = mustRead(path);
  std::map<int, ButtonGroup> out;
  size_t at = 0;
  while (at + 32 < s.size()) {
    if (u16(s, at) != MAGIC) throw std::runtime_error("BUTTON.DAT: lost the record boundary");
    ButtonGroup b;
    b.id = u16(s, at + 2);
    int count = u16(s, at + 4);
    for (int i = 0; i < count; i++) {
      size_t c = at + 33 + i * 33;
      Control ctl;
      for (int st = 0; st <= 2; st++) ctl.src[st] = Point{u16(s, c + 19 + st * 4), u16(s, c + 21 + st * 4)};
      ctl.id = u16(s, c);
      ctl.x = u16(s, c + 6);
      ctl.y = u16(s, c + 8);
      ctl.w = u8(s, c + 10);
      ctl.h = u8(s, c + 11);
      ctl.bitmap = u16(s, c + 31);
      b.controls.push_back(ctl);
    }
    out[b.id] = b;
    at += 33 + count * 33;
  }
  return out;
}

std::map<int, ShortcutItem> shortcutItems(const std::string& path) {
  std::string s = mustRead(path);
  std::map<int, ShortcutItem> out;
  for (size_t i = 0; (i + 1) * 68 <= s.size(); i++) {
    size_t at = i * 68;
    int id = u16(s, at + 2);
    std::string name = cstr(s, at + 4, 50);
    int x = u16(s, at + 54), y = u16(s, at + 56);
    if (!name.empty()) {
      ShortcutItem it;
      it.name = name;
      it.w = u16(s, at + 66);
      it.h = u16(s, at + 60);
      it.src = {Point{x + u16(s, at + 66), y}, Point{x, y}, Point{u16(s, at + 62), u16(s, at + 64)}};
      out[id] = it;
    }
  }
  return out;
}

std::map<int, int> shortcuts(const std::string& path) {
  std::string s = mustRead(path);
  std::map<int, int> out;
  for (size_t i = 0; i < s.size() / 2; i++) out[(int)i] = u16(s, i * 2);
  return out;
}

UI load(const std::string& dataDir) {
  std::string d = dataDir + "/DATA/";
  UI ui;
  ui.files = strings(d + "FILE.DAT");
  if (ui.files.size() > 3)
    for (size_t i = 0; i < ui.files[3].size(); i++) ui.bitmaps[(int)i] = lower(ui.files[3][i]);
  try {
    ui.shortcutItems = shortcutItems(dataDir + "/UDB/UDB.DAT");
    ui.shortcuts = shortcuts(dataDir + "/UDB/UDB.CUR");
  } catch (const std::exception&) {
    ui.shortcutItems.clear();
    ui.shortcuts.clear();
  }
  for (auto& [id, it] : ui.shortcutItems) ui.shortcutNames[id] = it.name;
  ui.joins = joins(d + "JOIN.DAT");
  ui.areas = areas(d + "AREA.DAT");
  ui.buttons = buttons(d + "BUTTON.DAT");
  ui.text = strings(d + "STRING.DAT");
  return ui;
}

std::string text(const UI& ui, int group, int index) {
  if (group < 0 || group >= (int)ui.text.size()) return "";
  const auto& g = ui.text[group];
  if (index < 0 || index >= (int)g.size()) return "";
  return g[index];
}

Dialog dialog(const UI& ui, int id) {
  Dialog d;
  d.id = id;
  auto j = ui.joins.find(id);
  if (j == ui.joins.end()) return d;
  d.found = true;
  auto b = ui.buttons.find(j->second.button);
  auto a = ui.areas.find(j->second.area);
  if (b != ui.buttons.end()) d.controls = b->second.controls;
  if (a != ui.areas.end()) {
    d.regions = a->second.regions;
    d.enabled = a->second.enabled;
  }
  return d;
}

}  // namespace w2::uidata
