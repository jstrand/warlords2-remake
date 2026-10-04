// Warlords II scenario data: .SCN, .MAP, .RD, .ITM
// See docs/formats/scenario.md, docs/formats/map.md and docs/formats/itm.md.
//
// The .SCN file is loaded verbatim into one segment by the original, so every
// offset here is also a live memory address in WARLORD2.EXE.
#pragma once

#include <memory>
#include <string>

#include "warlords/types.hpp"

namespace w2::scn {

constexpr int TILE = 40;              // terrain tiles are 40x40
constexpr int TILE_MASK = 0x7FFF;     // bit 15 of a map entry is a flag

std::optional<std::vector<PoolItem>> loadItemPool(const std::string& path);
std::map<int, std::array<std::string, 3>> loadDescriptions(const std::string& path);
std::vector<Sign> loadSigns(const std::string& path);

/** Load a scenario from `dir` (the scenario's folder) and its name. */
std::unique_ptr<Map> load(const std::string& dir, const std::string& name);

inline int tileAt(const Map& m, int x, int y) { return m.tiles[y * MAP_W + x]; }
inline int roadAt(const Map& m, int x, int y) { return (uint8_t)m.roads[y * MAP_W + x]; }
inline bool isCrossing(const Map& m, int x, int y) { return m.crossing[y * MAP_W + x]; }
/** The terrain type id (0-11) of a map tile, via the scenario's own table. */
inline int terrainAt(const Map& m, int x, int y) { return m.terrainType[tileAt(m, x, y) % 256]; }

/** A city's castle is drawn from the map itself: the game rewrites its four
 *  tiles whenever the city changes hands (set_city_tiles, 6bd8:0000); ruins
 *  keep the colours of whoever held the city (city_make_ruins, 649c:016b). */
int cityTileBase(const City& city);
void setCityTiles(Map& map, const City& city);
void refreshCityTiles(Map& map);

}  // namespace w2::scn
