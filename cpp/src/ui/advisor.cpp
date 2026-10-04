// The advisor: a horned helmet that speaks (6dda:026f). docs/re/sound.md.
//
// He waits for any sample still sounding (255e:0736), then draws VOICE.PCK
// at (152, 15) through its mask, colour 10. While his clip plays he blinks:
// a count starts at dice(1, 30, 10), goes up a BIOS tick at a time, and past
// 40 it restarts at 0 and the eyes -- VOICEBIT.PCK, three 160x47 frames, open,
// half shut and shut -- go over his at (232, 261): half, shut, half, open, two
// ticks each. When the clip ends the game goes on. With Speech off he never
// appears.
#include "front/pckimage.hpp"
#include "platform/sound.hpp"
#include "ui/dialogs.hpp"

namespace advisor {

namespace {
const int AT_X = 152, AT_Y = 15;
const int EYES_X = 232, EYES_Y = 261, EYES_W = 160, EYES_H = 47;
const int BLINK[4] = {1, 2, 1, 0};            // VOICEBIT rows, 47 apart
const double TICK = 1 / 18.2;
gfx::ImageP voicePic, voiceBits;

struct Helmet : kit::Modal {
  int group = 0;
  kit::Done after;
  bool waiting = true;
  double nextTick = 0;
  int count = 0;
  int blinkFrame = 0, blinkTicks = 0;   // blinkFrame 0: not blinking

  void start() {
    waiting = false;
    std::weak_ptr<Modal> self = weak_from_this();
    kit::Done a = after;
    bool spoke = sound::speak(group, [self, a]() {
      if (auto s = self.lock()) kit::pop(s.get());
      if (a) a();
    });
    if (!spoke) {
      kit::pop(this);
      if (a) a();
      return;
    }
    count = 11 + front::roll(30) - 1;
    nextTick = kit::now() + TICK;
    blinkFrame = 0;
  }

  void update() override {
    if (waiting) {
      if (!sound::busy()) start();
      return;
    }
    double t = kit::now();
    while (nextTick > 0 && t >= nextTick) {
      nextTick += TICK;
      if (blinkFrame) {
        blinkTicks++;
        if (blinkTicks >= 2) {
          blinkTicks = 0;
          blinkFrame++;
          if (blinkFrame > 4) blinkFrame = 0;
        }
      } else {
        count++;
        if (count > 40) { count = 0; blinkFrame = 1; blinkTicks = 0; }
      }
    }
  }

  void draw() override {
    if (waiting) return;
    if (!voicePic) {
      voicePic = pckimage::load(G.dataDir + "/PICS/VOICE.PCK", G.palette, 10);
      voiceBits = pckimage::load(G.dataDir + "/PICS/VOICEBIT.PCK", G.palette);
    }
    gfx::setColor(1, 1, 1);
    gfx::draw(voicePic, AT_X, AT_Y);
    if (blinkFrame) {
      int row = BLINK[blinkFrame - 1];
      gfx::draw(voiceBits, gfx::newQuad(0, row * EYES_H, EYES_W, EYES_H), EYES_X, EYES_Y);
    }
  }
  // he holds the game while he speaks: input goes nowhere
};
}  // namespace

void say(int group, kit::Done after) {
  if (group == w2::NONE || !sound::speechOn()) {
    if (after) after();
    return;
  }
  auto d = std::make_shared<Helmet>();
  d->group = group;
  d->after = after;
  kit::push(d);
  if (!sound::busy()) d->start();
}

}  // namespace advisor
