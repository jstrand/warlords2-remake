#include "warlords/save.hpp"

#include <map>
#include <stdexcept>

#include "util/json.hpp"
#include "warlords/ai/core.hpp"
#include "warlords/armytype.hpp"
#include "warlords/game.hpp"
#include "warlords/move.hpp"
#include "warlords/scn.hpp"

namespace w2::save {

namespace {
Json intMap(const std::map<int, int>& m) {
  Json o = Json::object();
  for (auto& [k, v] : m) o.set(std::to_string(k), v);
  return o;
}
std::map<int, int> readIntMap(const Json& j) {
  std::map<int, int> m;
  for (auto& [k, v] : j.entries()) m[std::stoi(k)] = (int)v.integer();
  return m;
}
template <size_t N>
Json intArray(const std::array<int, N>& a) {
  Json o = Json::array();
  for (int v : a) o.push(v);
  return o;
}
template <size_t N>
void readIntArray(const Json& j, std::array<int, N>& a) {
  for (size_t i = 0; i < N && i < j.size(); i++) a[i] = (int)j[i].integer();
}
Json optInt(int v) { return v == NONE ? Json() : Json(v); }
int readOpt(const Json& j) { return j.isNull() ? NONE : (int)j.integer(); }

// the hidden map, a side's mask as runs of seen and unseen tiles
Json packMask(const std::vector<uint8_t>& mask) {
  Json runs = Json::array();
  int cur = 0, n = 0;
  for (uint8_t m : mask) {
    int v = m ? 1 : 0;
    if (v == cur) n++;
    else { runs.push(n); cur = v; n = 1; }
  }
  runs.push(n);
  return runs;
}
std::vector<uint8_t> unpackMask(const Json& runs, size_t size) {
  std::vector<uint8_t> mask(size, 0);
  size_t i = 0;
  int v = 0;
  for (auto& n : runs.items()) {
    for (long k = 0; k < n.integer() && i < size; k++) mask[i++] = (uint8_t)v;
    v = 1 - v;
  }
  return mask;
}

Json saveAI(const AIData& d, const std::map<const Army*, int>& ids) {
  Json o = Json::object();
  o.set("turns", d.turns); o.set("own", d.own); o.set("enemy", d.enemy); o.set("neutral", d.neutral);
  o.set("unseen", d.unseen); o.set("rebuildLimit", d.rebuildLimit); o.set("bold", d.bold);
  o.set("heroes", d.heroes); o.set("rebuildType", d.rebuildType); o.set("rebuildTypeRich", d.rebuildTypeRich);
  o.set("bought", d.bought); o.set("dieHuman", d.dieHuman); o.set("dieLord", d.dieLord);
  o.set("dieKnight", d.dieKnight); o.set("dieWarlord", d.dieWarlord); o.set("sims", d.sims);
  o.set("searchers", d.searchers); o.set("explorers", d.explorers); o.set("raze", d.raze);
  o.set("sack", d.sack); o.set("pillage", d.pillage); o.set("perCity", d.perCity);
  o.set("bonusHuman", d.bonusHuman); o.set("bonusWarlord", d.bonusWarlord); o.set("bonusLord", d.bonusLord);
  o.set("bonusKnight", d.bonusKnight); o.set("poor", d.poor); o.set("solidarity", d.solidarity);
  o.set("minStrength", d.minStrength); o.set("flyCities", d.flyCities); o.set("strongCities", d.strongCities);
  o.set("fastCities", d.fastCities); o.set("early", d.early); o.set("humanShare", d.humanShare);
  o.set("questCity", optInt(d.questCity)); o.set("cautious", d.cautious); o.set("questsDone", d.questsDone);
  o.set("itemsPassed", d.itemsPassed); o.set("maxGroups", d.maxGroups);
  o.set("roles", intMap(d.roles)); o.set("held", intMap(d.held)); o.set("flags", intMap(d.flags));
  o.set("garrison", intMap(d.garrison)); o.set("keep", intMap(d.keep));
  o.set("heroesKilled", intArray(d.heroesKilled)); o.set("armiesKilled", intArray(d.armiesKilled));
  o.set("battles", intArray(d.battles)); o.set("lost", intArray(d.lost));
  o.set("cityBattles", intArray(d.cityBattles)); o.set("citiesLost", intArray(d.citiesLost));
  if (d.cursor) {
    Json c = Json::array();
    c.push(d.cursor->first);
    c.push(d.cursor->second);
    o.set("cursor", c);
  }
  Json groups = Json::array();
  for (int gi = 1; gi <= ai::core::MAX_GROUPS; gi++) {
    const Group& grp = *d.groups[gi];
    Json gr = Json::object();
    gr.set("active", grp.active); gr.set("target", optInt(grp.target)); gr.set("rally", optInt(grp.rally));
    gr.set("members", intMap(grp.members)); gr.set("cities", intMap(grp.cities)); gr.set("dist", intMap(grp.dist));
    gr.set("bonus", intMap(grp.bonus)); gr.set("taken", intMap(grp.taken));
    Json staged = Json::object();
    for (auto& [k, a] : grp.staged) {
      auto it = ids.find(a);
      if (a && it != ids.end()) staged.set(std::to_string(k), it->second);
    }
    gr.set("staged", staged);
    gr.set("size", grp.size); gr.set("flags", grp.flags);
    groups.push(gr);
  }
  o.set("groups", groups);
  return o;
}

void loadAI(AIData& d, const Json& o, const std::vector<Army*>& armies) {
  auto I = [&](const char* k, int& v) { if (o.has(k)) v = (int)o[k].integer(); };
  I("turns", d.turns); I("own", d.own); I("enemy", d.enemy); I("neutral", d.neutral); I("unseen", d.unseen);
  I("rebuildLimit", d.rebuildLimit); d.bold = o["bold"].boolean();
  I("heroes", d.heroes); I("rebuildType", d.rebuildType); I("rebuildTypeRich", d.rebuildTypeRich);
  I("bought", d.bought); I("dieHuman", d.dieHuman); I("dieLord", d.dieLord); I("dieKnight", d.dieKnight);
  I("dieWarlord", d.dieWarlord); I("sims", d.sims); I("searchers", d.searchers); I("explorers", d.explorers);
  I("raze", d.raze); I("sack", d.sack); I("pillage", d.pillage); I("perCity", d.perCity);
  I("bonusHuman", d.bonusHuman); I("bonusWarlord", d.bonusWarlord); I("bonusLord", d.bonusLord);
  I("bonusKnight", d.bonusKnight); I("poor", d.poor); I("solidarity", d.solidarity);
  I("minStrength", d.minStrength); I("flyCities", d.flyCities); I("strongCities", d.strongCities);
  I("fastCities", d.fastCities); I("early", d.early); I("humanShare", d.humanShare);
  d.questCity = readOpt(o["questCity"]);
  I("cautious", d.cautious); I("questsDone", d.questsDone); I("itemsPassed", d.itemsPassed);
  I("maxGroups", d.maxGroups);
  d.roles = readIntMap(o["roles"]); d.held = readIntMap(o["held"]); d.flags = readIntMap(o["flags"]);
  d.garrison = readIntMap(o["garrison"]); d.keep = readIntMap(o["keep"]);
  readIntArray(o["heroesKilled"], d.heroesKilled); readIntArray(o["armiesKilled"], d.armiesKilled);
  readIntArray(o["battles"], d.battles); readIntArray(o["lost"], d.lost);
  readIntArray(o["cityBattles"], d.cityBattles); readIntArray(o["citiesLost"], d.citiesLost);
  if (o["cursor"].isArray()) d.cursor = std::make_pair((int)o["cursor"][0].integer(), (int)o["cursor"][1].integer());
  else d.cursor.reset();
  const Json& groups = o["groups"];
  for (int gi = 1; gi <= ai::core::MAX_GROUPS; gi++) {
    auto grp = ai::core::emptyGroup();
    const Json& gr = groups[gi - 1];
    if (gr.isObject()) {
      grp->active = (int)gr["active"].integer();
      grp->target = readOpt(gr["target"]);
      grp->rally = readOpt(gr["rally"]);
      grp->members = readIntMap(gr["members"]); grp->cities = readIntMap(gr["cities"]);
      grp->dist = readIntMap(gr["dist"]); grp->bonus = readIntMap(gr["bonus"]); grp->taken = readIntMap(gr["taken"]);
      for (auto& [k, v] : gr["staged"].entries()) {
        long id = v.integer();
        if (id >= 0 && id < (long)armies.size()) grp->staged[std::stoi(k)] = armies[id];
      }
      grp->size = (int)gr["size"].integer();
      grp->flags = (int)gr["flags"].integer();
    }
    d.groups[gi] = grp;
  }
}

Json slotJson(const Slot& s) {
  Json o = Json::object();
  o.set("type", s.type); o.set("name", s.name); o.set("strength", s.strength); o.set("time", s.time);
  o.set("cost", s.cost); o.set("move", s.move); o.set("price", s.price);
  return o;
}
Slot readSlot(const Json& o) {
  return Slot{(int)o["type"].integer(), o["name"].str(), (int)o["strength"].integer(), (int)o["time"].integer(),
              (int)o["cost"].integer(), (int)o["move"].integer(), (int)o["price"].integer()};
}
}  // namespace

std::string encode(const Game& g) {
  std::map<const Army*, int> ids;
  for (size_t i = 0; i < g.armies.size(); i++) ids[g.armies[i]] = (int)i;
  auto idOf = [&](const Army* a) -> Json {
    auto it = ids.find(a);
    return it == ids.end() ? Json() : Json(it->second);
  };

  Json armies = Json::array();
  for (Army* a : g.armies) {
    Json o = Json::object();
    o.set("x", optInt(a->x)); o.set("y", optInt(a->y)); o.set("owner", optInt(a->owner));
    o.set("type", a->type); o.set("name", a->name);
    if (a->female) o.set("female", true);
    o.set("strength", a->strength); o.set("moves", a->moves); o.set("maxMoves", a->maxMoves);
    o.set("upkeep", a->upkeep); o.set("homeCity", optInt(a->homeCity));
    if (a->atSea) o.set("atSea", true);
    if (a->group) o.set("group", a->group);
    if (a->target) {
      Json t = Json::array();
      t.push(a->target->first);
      t.push(a->target->second);
      o.set("target", t);
    }
    if (a->fortified) o.set("fortified", true);
    if (a->level) o.set("level", a->level);
    if (a->experience) o.set("experience", a->experience);
    if (!a->title.empty()) o.set("title", a->title);
    if (!a->items.empty()) {
      Json its = Json::array();
      for (Item* it : a->items) its.push(it->index);
      o.set("items", its);
    }
    if (!a->blessings.empty()) {
      Json b = Json::array();
      for (int t : a->blessings) b.push(t);
      o.set("blessings", b);
    }
    if (a->transit) {
      Json t = Json::object();
      t.set("turns", a->transit->turns);
      t.set("dest", a->transit->dest);
      o.set("transit", t);
    }
    if (a->returning) o.set("returning", true);
    if (a->aiOrder) o.set("aiOrder", a->aiOrder);
    if (a->aiDest != NONE) o.set("aiDest", a->aiDest);
    if (a->aiGroup) o.set("aiGroup", a->aiGroup);
    if (a->aiExplore) o.set("aiExplore", true);
    if (a->aiNeutral) o.set("aiNeutral", true);
    if (a->aiParty) o.set("aiParty", true);
    armies.push(o);
  }

  Json sides = Json::array();
  for (Side* s : g.sides) {
    Json o = Json::object();
    o.set("index", s->index); o.set("gold", s->gold); o.set("alive", s->alive); o.set("computer", s->computer);
    o.set("level", s->level); o.set("enhanced", s->enhanced); o.set("observe", s->observe);
    o.set("diploScore", s->diploScore); o.set("income", s->income); o.set("upkeepTotal", s->upkeepTotal);
    o.set("aiSolidarity", s->aiSolidarity); o.set("card", s->card);
    Json adv = Json::object();
    adv.set("dir", s->advisor.dir);
    adv.set("mark", s->advisor.mark);
    o.set("advisor", adv);
    if (s->ai) o.set("ai", saveAI(*s->ai, ids));
    if (s->quest) {
      const Quest& q = *s->quest;
      Json qq = Json::object();
      qq.set("type", q.type); qq.set("hero", idOf(q.hero)); qq.set("done", q.done);
      qq.set("required", optInt(q.required)); qq.set("targetKind", q.targetKind);
      if (q.targetKind == "army") qq.set("target", idOf(q.army));
      else if (q.targetKind == "armytype" && q.armyType) qq.set("target", q.armyType->id);
      else if (q.targetKind == "city" && q.city) qq.set("target", q.city->index);
      else if (q.targetKind == "side" && q.side) qq.set("target", q.side->index);
      else if (q.targetKind == "item" && q.item) qq.set("target", q.item->index);
      o.set("quest", qq);
    }
    sides.push(o);
  }

  Json cities = Json::array();
  for (auto& c : g.map->cities) {
    Json o = Json::object();
    o.set("index", c.index); o.set("name", c.name); o.set("ownerIndex", optInt(c.ownerIndex));
    o.set("producing", optInt(c.producing)); o.set("countdown", c.countdown);
    if (c.vectorTo != NONE) o.set("vectorTo", c.vectorTo);
    if (c.razed) o.set("razed", true);
    o.set("defence", c.defence); o.set("income", c.income); o.set("claim", optInt(c.claim));
    o.set("razedBy", optInt(c.razedBy));
    Json slots = Json::array();
    for (auto& s : c.slots) slots.push(slotJson(s));
    o.set("slots", slots);
    cities.push(o);
  }

  Json sites = Json::array();
  for (auto& s : g.map->sites) {
    Json o = Json::object();
    o.set("index", s.index); o.set("content", s.content); o.set("item", optInt(s.item));
    o.set("guardian", optInt(s.guardian)); o.set("allyType", optInt(s.allyType));
    if (s.rich) o.set("rich", true);
    o.set("revealed", s.revealed);
    if (s.searched) o.set("searched", true);
    o.set("band", s.band); o.set("templeIndex", optInt(s.templeIndex));
    sites.push(o);
  }

  Json items = Json::array();
  for (auto& it : g.map->items) {
    Json o = Json::object();
    o.set("index", it.index); o.set("name", it.name); o.set("type", it.type); o.set("value", it.value);
    o.set("status", it.status); o.set("x", optInt(it.x)); o.set("y", optInt(it.y));
    if (it.planted) o.set("planted", true);
    o.set("standardOf", optInt(it.standardOf));
    items.push(o);
  }

  Json signs = Json::array();
  for (auto& sg : g.map->signs) {
    Json l = Json::array();
    l.push(sg.lines[0]);
    l.push(sg.lines[1]);
    signs.push(l);
  }
  Json towers = Json::array();
  for (int k : g.towers) towers.push(k);
  Json explored = Json::object();
  for (auto& [k, m] : g.explored) explored.set(std::to_string(k), packMask(m));

  Json opts = Json::object();
  for (auto& n : Options::names()) opts.set(n, *const_cast<Options&>(g.map->options).field(n));

  Json dip = Json::object();
  dip.set("state", intArray(g.diplomacy.state));
  dip.set("proposal", intArray(g.diplomacy.proposal));

  Json fight = Json::array();
  for (auto& row : g.map->fightOrder) {
    Json r = Json::array();
    for (int v : row) r.push(v);
    fight.push(r);
  }

  Json history = Json::array();
  auto deedJson = [](const Deed& e) {
    Json o = Json::object();
    o.set("side", e.side); o.set("type", e.type); o.set("v1", e.v1); o.set("v2", e.v2); o.set("name", e.name);
    return o;
  };
  for (auto& r : g.history) {
    Json o = Json::object();
    o.set("gold", intArray(r.gold)); o.set("score", intArray(r.score)); o.set("cities", intArray(r.cities));
    Json owners = Json::array();
    for (int v : r.owners) owners.push(v);
    o.set("owners", owners);
    Json ev = Json::array();
    for (auto& e : r.events) ev.push(deedJson(e));
    o.set("events", ev);
    history.push(o);
  }
  Json deeds = Json::object();
  for (auto& [k, list] : g.deeds) {
    Json l = Json::array();
    for (auto& e : list) l.push(deedJson(e));
    deeds.set(std::to_string(k), l);
  }
  Json triumphs = Json::object();
  for (auto& [me, row] : g.triumphs) {
    Json r = Json::object();
    for (auto& [opp, counts] : row) r.set(std::to_string(opp), intArray(counts));
    triumphs.set(std::to_string(me), r);
  }
  Json seen = Json::array();
  for (auto& m : g.tutorialSeen) seen.push(m);
  Json log = Json::array();
  for (auto& l : g.log) log.push(l);

  Json out = Json::object();
  out.set("version", VERSION);
  out.set("scenario", g.map->name);
  out.set("turn", g.turn); out.set("current", g.current); out.set("seed", (double)g.rng.state);
  out.set("won", g.won); out.set("over", g.over); out.set("noHumansSaid", g.noHumansSaid);
  out.set("greatest", g.greatest); out.set("surrenderOffered", g.surrenderOffered);
  out.set("options", opts);
  out.set("diplomacy", dip);
  out.set("sides", sides); out.set("cities", cities); out.set("sites", sites); out.set("items", items);
  out.set("armies", armies);
  out.set("signs", signs); out.set("fightOrder", fight); out.set("towers", towers); out.set("explored", explored);
  out.set("history", history); out.set("deeds", deeds); out.set("triumphs", triumphs);
  out.set("tutorialSeen", seen);
  out.set("log", log);
  return out.dump();
}

std::unique_ptr<Game> decode(const std::string& text, const std::string& dataDir) {
  Json state = Json::parse(text);
  if (state["version"].integer() != VERSION) throw std::runtime_error("unsupported save version");

  game::NewGameOptions opts;
  for (auto& [k, v] : state["options"].entries()) opts.options.emplace_back(k, (int)v.integer());
  auto gp = game::newGame(dataDir, state["scenario"].str(), opts);
  Game& g = *gp;
  g.armies.clear();
  g.turn = (int)state["turn"].integer();
  g.current = (int)state["current"].integer();
  g.rng.state = (uint32_t)state["seed"].number();
  g.won = state["won"].boolean(); g.over = state["over"].boolean();
  g.surrenderOffered = state["surrenderOffered"].boolean();
  g.noHumansSaid = state["noHumansSaid"].boolean();
  g.greatest = state["greatest"].boolean();
  readIntArray(state["diplomacy"]["state"], g.diplomacy.state);
  readIntArray(state["diplomacy"]["proposal"], g.diplomacy.proposal);
  g.log.clear();
  for (auto& l : state["log"].items()) g.log.push_back(l.str());

  std::map<int, Item*> itemByIndex;
  for (auto& saved : state["items"].items()) {
    for (auto& it : g.map->items) {
      if (it.index == saved["index"].integer()) {
        it.name = saved["name"].str(); it.type = (int)saved["type"].integer(); it.value = (int)saved["value"].integer();
        it.status = (int)saved["status"].integer();
        it.x = readOpt(saved["x"]); it.y = readOpt(saved["y"]);
        it.planted = saved["planted"].boolean();
        it.standardOf = readOpt(saved["standardOf"]);
        itemByIndex[it.index] = &it;
      }
    }
  }

  for (auto& saved : state["cities"].items()) {
    City* c = g.map->city((int)saved["index"].integer());
    if (!c) continue;
    c->ownerIndex = readOpt(saved["ownerIndex"]);
    c->producing = readOpt(saved["producing"]);
    c->countdown = (int)saved["countdown"].integer();
    c->vectorTo = saved.has("vectorTo") ? (int)saved["vectorTo"].integer() : NONE;
    c->razed = saved["razed"].boolean();
    c->defence = (int)saved["defence"].integer();
    c->income = (int)saved["income"].integer();
    c->slots.clear();
    for (auto& s : saved["slots"].items()) c->slots.push_back(readSlot(s));
    if (!saved["claim"].isNull()) c->claim = (int)saved["claim"].integer();
    c->razedBy = readOpt(saved["razedBy"]);
    if (!saved["name"].str().empty()) c->name = saved["name"].str();
    // ruins belong to nobody; older saves can have them won in a fight
    if (c->razed) c->ownerIndex = NONE;
  }
  scn::refreshCityTiles(*g.map);

  for (auto& saved : state["sites"].items()) {
    int i = (int)saved["index"].integer();
    if (i < 0 || i >= (int)g.map->sites.size()) continue;
    Site& s = g.map->sites[i];
    s.content = (int)saved["content"].integer();
    s.item = readOpt(saved["item"]);
    s.guardian = readOpt(saved["guardian"]);
    s.allyType = readOpt(saved["allyType"]);
    s.rich = saved["rich"].boolean();
    s.revealed = (int)saved["revealed"].integer();
    s.searched = saved["searched"].boolean();
    s.band = saved["band"].str();
    s.templeIndex = readOpt(saved["templeIndex"]);
  }
  if (state["fightOrder"].isArray()) {
    g.map->fightOrder.clear();
    for (auto& row : state["fightOrder"].items()) {
      std::vector<int> r;
      for (auto& v : row.items()) r.push_back((int)v.integer());
      g.map->fightOrder.push_back(r);
    }
  }
  g.history.clear();
  auto readDeed = [](const Json& o) {
    return Deed{(int)o["side"].integer(), (int)o["type"].integer(), (int)o["v1"].integer(), (int)o["v2"].integer(),
                o["name"].str()};
  };
  for (auto& r : state["history"].items()) {
    HistoryRecord rec;
    readIntArray(r["gold"], rec.gold); readIntArray(r["score"], rec.score); readIntArray(r["cities"], rec.cities);
    for (auto& v : r["owners"].items()) rec.owners.push_back((int)v.integer());
    for (auto& e : r["events"].items()) rec.events.push_back(readDeed(e));
    g.history.push_back(rec);
  }
  g.deeds.clear();
  for (auto& [k, list] : state["deeds"].entries())
    for (auto& e : list.items()) g.deeds[std::stoi(k)].push_back(readDeed(e));
  g.triumphs.clear();
  for (auto& [me, row] : state["triumphs"].entries())
    for (auto& [opp, counts] : row.entries()) readIntArray(counts, g.triumphs[std::stoi(me)][std::stoi(opp)]);
  g.tutorialSeen.clear();
  for (auto& m : state["tutorialSeen"].items()) g.tutorialSeen.insert(m.str());
  g.towers.clear();
  for (auto& k : state["towers"].items()) g.towers.insert((int)k.integer());
  const Json& signs = state["signs"];
  for (size_t i = 0; i < g.map->signs.size() && i < signs.size(); i++) {
    g.map->signs[i].lines = {signs[i][0].str(), signs[i][1].str()};
  }
  g.explored.clear();
  for (auto& [k, runs] : state["explored"].entries())
    g.explored[std::stoi(k)] = unpackMask(runs, (size_t)g.map->width * g.map->height);

  std::vector<Army*> armies;
  for (auto& saved : state["armies"].items()) {
    Army a;
    a.x = readOpt(saved["x"]); a.y = readOpt(saved["y"]); a.owner = readOpt(saved["owner"]);
    a.type = (int)saved["type"].integer(); a.name = saved["name"].str();
    a.female = saved["female"].boolean();
    a.strength = (int)saved["strength"].integer(); a.moves = (int)saved["moves"].integer();
    a.maxMoves = (int)saved["maxMoves"].integer(); a.upkeep = (int)saved["upkeep"].integer();
    a.homeCity = readOpt(saved["homeCity"]);
    a.atSea = saved["atSea"].boolean();
    a.group = (int)saved["group"].integer();
    if (saved["target"].isArray()) a.target = std::make_pair((int)saved["target"][0].integer(), (int)saved["target"][1].integer());
    a.fortified = saved["fortified"].boolean();
    a.level = (int)saved["level"].integer(); a.experience = (int)saved["experience"].integer();
    a.title = saved["title"].str("");
    for (auto& idx : saved["items"].items()) {
      auto it = itemByIndex.find((int)idx.integer());
      if (it != itemByIndex.end()) a.items.push_back(it->second);
    }
    for (auto& t : saved["blessings"].items()) a.blessings.insert((int)t.integer());
    if (saved["transit"].isObject()) a.transit = Transit{(int)saved["transit"]["turns"].integer(), (int)saved["transit"]["dest"].integer()};
    a.returning = saved["returning"].boolean();
    a.aiOrder = (int)saved["aiOrder"].integer();
    a.aiDest = saved.has("aiDest") ? (int)saved["aiDest"].integer() : NONE;
    a.aiGroup = (int)saved["aiGroup"].integer();
    a.aiExplore = saved["aiExplore"].boolean();
    a.aiNeutral = saved["aiNeutral"].boolean();
    a.aiParty = saved["aiParty"].boolean();
    armies.push_back(g.add(a));
  }

  for (auto& saved : state["sides"].items()) {
    Side* s = g.map->side((int)saved["index"].integer());
    if (!s) continue;
    s->gold = (int)saved["gold"].integer(); s->alive = saved["alive"].boolean();
    s->computer = saved["computer"].boolean(); s->level = (int)saved["level"].integer();
    s->enhanced = saved["enhanced"].boolean(); s->diploScore = (int)saved["diploScore"].integer();
    s->observe = saved["observe"].boolean();
    s->income = (int)saved["income"].integer(); s->upkeepTotal = (int)saved["upkeepTotal"].integer();
    s->aiSolidarity = (int)saved["aiSolidarity"].integer(); s->card = (int)saved["card"].integer();
    s->advisor.dir = (int)saved["advisor"]["dir"].integer();
    s->advisor.mark = (int)saved["advisor"]["mark"].integer();
    if (saved["ai"].isObject()) {
      s->ai = std::make_shared<AIData>(ai::core::newData());
      loadAI(*s->ai, saved["ai"], armies);
    }
    s->quest.reset();
    if (saved["quest"].isObject()) {
      const Json& qs = saved["quest"];
      auto q = std::make_shared<Quest>();
      q->type = (int)qs["type"].integer();
      q->done = (int)qs["done"].integer();
      q->required = readOpt(qs["required"]);
      long hero = qs["hero"].integer(-1);
      q->hero = hero >= 0 && hero < (long)armies.size() ? armies[hero] : nullptr;
      q->targetKind = qs["targetKind"].str();
      int ref = (int)qs["target"].integer(-1);
      if (q->targetKind == "city") q->city = g.map->city(ref);
      else if (q->targetKind == "side") q->side = g.map->side(ref);
      else if (q->targetKind == "item") q->item = itemByIndex.count(ref) ? itemByIndex[ref] : nullptr;
      else if (q->targetKind == "armytype") q->armyType = g.types.byId(ref);
      else if (q->targetKind == "army") q->army = ref >= 0 && ref < (int)armies.size() ? armies[ref] : nullptr;
      if (q->hero && q->hasTarget()) s->quest = q;
    }
  }

  // a flier is never at sea, nor a hero flying with one (1a8b:04c8): the
  // Lua puts right any save that says otherwise, and so does this
  std::set<int> fliers, walkers;
  for (Army* a : g.armies) {
    if (a->x != NONE) {
      int k = a->y * g.map->width + a->x;
      if (g.types.byId(a->type)->flies) fliers.insert(k);
      else if (a->type != armytype::HERO) walkers.insert(k);
    }
  }
  for (Army* a : g.armies) {
    if (a->atSea && a->x != NONE) {
      int k = a->y * g.map->width + a->x;
      if (g.types.byId(a->type)->flies || (a->type == armytype::HERO && fliers.count(k) && !walkers.count(k))) {
        a->atSea = false;
      }
    }
  }

  g.side = g.current < (int)g.sides.size() ? g.sides[g.current] : nullptr;
  move::invalidate(g);
  return gp;
}

}  // namespace w2::save
