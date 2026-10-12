// A turn in Erythea, the way a player takes it: past the hero offer, select
// the hero, look at the pointer round it, end the turn, watch the computers
// play it out. Copy it as the start of a check of your own.

export default async ({ page, shot, settle, speechOff, tileCentre, click, altE, waitTurn, pointer }) => {
  await speechOff();
  console.log("settled:", await settle());
  await shot("smoke-main");

  // select the hero with a click on its tile
  const hero = await page.evaluate(() => {
    const h = W2.g.armies.find((a) => a.owner === W2.player.index && a.type === 28);
    W2.centreOn(h.x, h.y);
    return [h.x, h.y];
  });
  await click(...await tileCentre(...hero));
  console.log("selected:", await page.evaluate(() => W2.selection && W2.selection.stack.map((a) => `${a.name}@${a.x},${a.y}`)));
  for (const [dx, dy] of [[1, 0], [0, 2]]) {
    console.log(`pointer at hero+(${dx},${dy}):`, await pointer(...await tileCentre(hero[0] + dx, hero[1] + dy)));
  }
  await shot("smoke-selected");

  await altE();
  console.log("turn 2 reached:", await waitTurn(1));
  console.log("settled:", await settle());
  console.log(await page.evaluate(() => ({ turn: W2.g.turn, player: W2.player.name, gold: W2.player.gold })));
  await shot("smoke-turn2");
};
