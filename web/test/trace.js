// An all-computer game, one line per side's turn: the JS half of the
// Lua-vs-JS comparison (test/trace.lua prints the same lines).
//
//     node web/test/trace.js ERYTHEA 1 10 [hidden] [diplo]
import { loadData } from "./node.js";
import * as game from "../src/warlords/game.js";
import * as ai from "../src/warlords/ai.js";
import { fmt } from "../src/util.js";
import * as save from "../src/warlords/save.js";
import * as randommap from "../src/warlords/randommap.js";
import { Rng } from "../src/warlords/rng.js";

loadData({ images: false });
const [scen = "ERYTHEA", seedS = "1", turnsS = "10", ...flags] = process.argv.slice(2);
const hidden = flags.includes("hidden") ? "hidden" : null;
// "save": write the game out and read it back after every turn, which must
// change nothing
const roundTrip = flags.includes("save");
const opts = { seed: Number(seedS) };
opts.options = {};
if (hidden === "hidden") opts.options.hiddenMap = 1;
// "diplo": with Diplomacy on, so that sides make peace
if (flags.includes("diplo")) opts.options.diplomacy = 1;
// RANDOM: a world made from the seed first, as test/trace.lua makes it
if (scen === randommap.DIR) {
  randommap.install("", randommap.generateNow({ dataDir: "", rng: new Rng(Number(seedS)) }));
}
let g = game.newGame("", scen, opts);
for (const s of g.sides) s.computer = true;

function trace(side) {
  let h = 0;
  for (const a of g.armies) {
    h = (h * 31 + (a.x ?? 999) * 7 + (a.y ?? 999) * 13 + a.type * 17
         + (a.owner ?? 15) * 19 + (a.strength || 0) + (a.moves || 0) * 3) % 1000000007;
  }
  const owners = g.map.cities.map((c) => (c.razed ? "x" : c.ownerIndex != null ? String.fromCharCode(48 + c.ownerIndex) : "."));
  console.log(fmt("%d %d gold=%d armies=%d rng=%d h=%d %s", g.turn, side.index, side.gold,
                  g.armies.length, g.rng.state, h, owners.join("")));
}

let side = game.begin(g);
while (side && g.turn <= Number(turnsS)) {
  ai.runSync(ai.playTurn(g, side));
  trace(side);
  side = game.endTurn(g);
  if (roundTrip && side) {
    g = save.decode(save.encode(g), "");
    side = g.side;
  }
}
