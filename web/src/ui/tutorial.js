// The tutorial's help pages (the Tutorial option, .SCN 0x12e): each shown
// once, the first time its moment comes. The pages are FILE.DAT group 0x19
// -- TUTORIA\T*.GFX -- shown by 8065:168d.

import * as kit from "./kit.js";
import * as help from "./help.js";

const PAGES = {
  hero: 0, prod: 1, select: 2, move: 3, fight: 4, search: 5,
  prod2: 6, turn2: 7, endturn: 9, fresult: 10,
};
const FILES = ["THERO", "TPROD", "TSELECT", "TMOVE", "TFIGHT", "TSEARCH",
  "TPROD2", "TTURN2", "TWARLORD", "TENDTURN", "TFRESULT"];

/** Show the page for `moment`, if the tutorial is on and it has not been
 *  shown yet; `after` runs once it is put away, or at once. */
export function show(moment, after) {
  const G = kit.G;
  const g = G.g;
  after = after || (() => {});
  const i = PAGES[moment];
  if (!g || i === undefined || (g.map.options.tutorial || 0) === 0) return after();
  g.tutorialSeen = g.tutorialSeen || {};
  if (g.tutorialSeen[moment]) return after();
  g.tutorialSeen[moment] = true;
  if (!help.open("TUTORIA\\" + FILES[i] + ".GFX", after)) after();
}

/** Several moments one after another. */
export function chain(moments, after) {
  let k = 0;
  const nextOne = () => {
    const m = moments[k++];
    if (m) show(m, nextOne);
    else if (after) after();
  };
  nextOne();
}
