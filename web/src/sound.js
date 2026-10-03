// The game's music, effects and advisor voice, through Web Audio.
// docs/re/sound.md.
//
// Three switches, as Game > Settings has them and DATA/OPTIONS.SND keeps
// them -- three characters, '1' or '0', for Music, Effects and Speech.
//
// The samples are DIGPAK's: one plays at a time, and one asked for while
// another is sounding waits for it to end (255e:065e), so they queue here.
// They are decoded at start-up, so an effect's length is known the moment it
// is asked for -- the assault holds its fire cloud for exactly that long.
//
// The songs are recordings (tools/web_assets.py): the AdLib set rendered
// through the Lua remake's OPL emulation, and the MT-32 and Sound Canvas sets
// where they were recorded. Tick 0 is the recording's start; a looping song
// starts again at its own length while the old one's tail rings out beneath.
// The browser only lets sound start after the player has touched the page;
// until then the song asked for waits.

import * as vfs from "./vfs.js";
import * as cues from "./warlords/cues.js";
import * as prefs from "./prefs.js";
import { bytesOf } from "./util.js";

export const SAMPLE_RATE = 11000;
export const SYNTHS = ["fm", "mt32", "sc55"];
const PREFIX = { fm: "S", mt32: "M", sc55: "R" };

const S = {
  on: { music: false, effects: false, speech: false },
  samples: new Map(),     // KEY -> AudioBuffer
  queue: [],
  playing: null,
  cue: null,
  song: null,             // [name, loop]
  songId: 0,
  songPlaying: false,
  synth: "fm",
  rec: null,              // { buffer, src, loop, at, startedAt }
  tails: [],
  ctx: null,
  pending: null,          // a song asked for before sound was allowed
  musicGain: null,
};

const roll = (n) => Math.floor(Math.random() * Math.max(1, n)) + 1;

function sampleKey(name) {
  // "SOUND\\army.8SN" -> SOUND/ARMY.8SN
  return vfs.key(String(name).replace(/\\+/g, "/"));
}

/** Start up: the settings, and every sample decoded. */
export async function init(dataDir, files) {
  S.dataDir = dataDir;
  S.files = files;
  const opts = vfs.readText(dataDir + "/DATA/OPTIONS.SND") || "111";
  S.on.music = opts[0] === "1";
  S.on.effects = opts[1] === "1";
  S.on.speech = opts[2] !== "0";
  const synth = prefs.get("music");
  if (PREFIX[synth] && synthAvailable(synth)) S.synth = synth;
  try {
    S.ctx = new (window.AudioContext || window.webkitAudioContext)();
  } catch (e) {
    S.ctx = null;
    return;
  }
  S.musicGain = S.ctx.createGain();
  S.musicGain.gain.value = 1;
  S.musicGain.connect(S.ctx.destination);
  const manifest = vfs.getManifest();
  const jobs = [];
  for (const rel of manifest.files) {
    if (!/\.8SN$/i.test(rel)) continue;
    jobs.push(fetch(vfs.url(rel.slice(0, -4) + ".wav"))
      .then((r) => r.arrayBuffer())
      .then((b) => S.ctx.decodeAudioData(b))
      .then((buf) => S.samples.set(vfs.key(rel), buf))
      .catch(() => {}));
  }
  await Promise.all(jobs);
}

/** The first touch of the page lets sound start. */
export function unlock() {
  if (!S.ctx || S.ctx.state === "running") return;
  S.ctx.resume().then(() => {
    if (S.pending) {
      const [name, loop] = S.pending;
      S.pending = null;
      startSong(name, loop);
    }
  }).catch(() => {});
}

/** The switches, as Settings shows them: [music, effects, speech]. */
export function options() {
  return [S.on.music, S.on.effects, S.on.speech];
}

function saveOptions() {
  const s = (S.on.music ? "1" : "0") + (S.on.effects ? "1" : "0") + (S.on.speech ? "1" : "0");
  vfs.write(S.dataDir + "/DATA/OPTIONS.SND", bytesOf(s));
}

/** Turn one switch over (64d2:0576). */
export function toggle(which) {
  S.on[which] = !S.on[which];
  if (which === "music") {
    if (S.on.music) {
      S.cue = null;
      music(cues.PLAY);
    } else {
      stopMusic();
    }
  }
  saveOptions();
}

// ------------------------------------------------------------------ music

const songCache = new Map();    // file -> Promise<AudioBuffer>

function recordingName(synth, name) {
  // "INT12.XMI" -> SINT12
  return PREFIX[synth] + String(name).toUpperCase().replace(/\.XMI$/, "");
}

function loadSong(file) {
  if (!songCache.has(file)) {
    songCache.set(file, fetch(vfs.assetBase() + "music/" + file + ".ogg")
      .then((r) => { if (!r.ok) throw new Error(file); return r.arrayBuffer(); })
      .then((b) => S.ctx.decodeAudioData(b)));
  }
  return songCache.get(file);
}

function stopRecordings() {
  if (S.rec && S.rec.src) { try { S.rec.src.stop(); } catch (e) { /* done */ } }
  S.rec = null;
  for (const t of S.tails) { try { t.stop(); } catch (e) { /* done */ } }
  S.tails = [];
}

function playBuffer(buffer) {
  const src = S.ctx.createBufferSource();
  src.buffer = buffer;
  src.connect(S.musicGain);
  src.start();
  return src;
}

function startSong(name, loop) {
  if (!S.ctx) return false;
  let synth = S.synth;
  if (!synthAvailable(synth)) synth = "fm";
  const file = recordingName(synth, name);
  const manifest = vfs.getManifest();
  if (!manifest.music.includes(file)) return false;
  S.songId++;
  S.song = [name, loop];
  S.songPlaying = true;
  if (S.ctx.state !== "running") {
    S.pending = [name, loop];
    return true;
  }
  const id = S.songId;
  stopRecordings();
  loadSong(file).then((buffer) => {
    if (id !== S.songId) return;            // another song was asked for meanwhile
    stopRecordings();
    const at = manifest.songs[file] || buffer.duration;
    S.rec = { buffer, src: playBuffer(buffer), loop, at, startedAt: S.ctx.currentTime };
  }).catch(() => { if (id === S.songId) S.songPlaying = false; });
  return true;
}

/** Play a cue -- unless it is the one already playing (6dda:0000). */
export function music(cue, sides) {
  if (!S.on.music) return;
  if (cue === S.cue && S.songPlaying) return;
  const [name, loop] = cues.song(S.files, cue, sides, roll);
  if (name && startSong(name, loop)) S.cue = cue;
}

export function stopMusic() {
  S.cue = null; S.song = null; S.songPlaying = false; S.pending = null;
  S.songId++;
  stopRecordings();
}

/** The synthesizer the music is played on: "fm", "mt32" or "sc55". */
export function synth() { return S.synth; }

/** Can it be? Each wants its recordings. */
export function synthAvailable(sy) {
  const m = vfs.getManifest();
  return !!PREFIX[sy] && m.music.includes(PREFIX[sy] + "STARTUP");
}

/** Play the music on another synthesizer from now on. */
export function setSynth(sy) {
  if (!PREFIX[sy] || sy === S.synth) return;
  S.synth = sy;
  prefs.set("music", sy);
  if (S.on.music && S.song && S.songPlaying) startSong(S.song[0], S.song[1]);
}

// ------------------------------------------------------------------ samples

function enqueue(name, done) {
  const buf = S.ctx && S.samples.get(sampleKey(name));
  if (!buf) {
    if (done) done();
    return 0;
  }
  S.queue.push({ buf, done });
  update();
  return buf.duration;
}

// FILE.DAT's sample names, by what the game calls them for
const EFFECT = {
  army: [64, 0], army2: [64, 5], ding: [64, 1], chord: [64, 2],
  dramatic: [37, 0], orch: [37, 1], war: [38, 0], splash: [39, 0],
  turn: [63, 0],
};

/** Play one of the effects, if Effects is on; its length in seconds. */
export function effect(what) {
  if (!S.on.effects || !S.files) return 0;
  const e = EFFECT[what];
  const g = e && S.files[e[0]];
  const name = g && g[e[1]];
  if (!name) return 0;
  return enqueue(name);
}

/** Say an advisor clip, if Speech is on: its length, or null when he stays
 *  silent; `done` runs when the voice has ended. */
export function speak(group, done) {
  if (!S.on.speech || !S.files) return null;
  const [name] = cues.clip(S.files, group, roll);
  if (!name || !(S.ctx && S.samples.get(sampleKey(name)))) return null;
  return enqueue(name, done);
}

export function speechOn() { return S.on.speech; }

/** True while a sample is sounding or waiting. */
export function busy() {
  return S.playing !== null || S.queue.length > 0;
}

/** Call every frame: start the next sample when the last has ended, and
 *  start a looping song again at its end. */
export function update() {
  if (!S.ctx) return;
  if (S.playing && S.playing.ended) {
    const done = S.playing.done;
    S.playing = null;
    if (done) done();
  }
  if (!S.playing && S.queue.length > 0) {
    const q = S.queue.shift();
    const src = S.ctx.createBufferSource();
    src.buffer = q.buf;
    src.connect(S.ctx.destination);
    q.src = src;
    src.onended = () => { q.ended = true; };
    if (S.ctx.state !== "running") q.ended = true;   // no sound yet: it is over at once
    else src.start();
    S.playing = q;
  }
  const r = S.rec;
  if (r && S.ctx.currentTime - r.startedAt >= r.at) {
    S.tails.push(r.src);
    if (r.loop) {
      r.src = playBuffer(r.buffer);
      r.startedAt = S.ctx.currentTime;
    } else {
      S.rec = null;
      S.songPlaying = false;
    }
  }
  if (S.tails.length > 4) S.tails.splice(0, S.tails.length - 4);
}

