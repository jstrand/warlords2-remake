// XMIDI, the music format of Miles' Audio Interface Library (SOUND/*.XMI).
//
// An IFF file: FORM XDIR (how many sequences), then CAT XMID holding one
// FORM XMID per sequence, each with a TIMB chunk (the timbres it uses) and
// EVNT, the events. EVNT is MIDI with two differences:
//
//   - time is counted in intervals of AIL's 120 Hz timer, and a delay is a
//     run of bytes below 0x80 that are simply added up;
//   - a note-on carries its own duration, a MIDI variable-length number
//     after the velocity, and there are no note-offs.
//
// Tempo meta events are left in by the converter but mean nothing: the
// delays already have the tempo baked in.
//
// parse() flattens the first sequence into time order, turning each note's
// duration into a note-off. At a tick where notes end and others begin the
// ends come first, as AIL's timer handler retires expired notes before it
// reads on in the stream -- which matters when voices are short.
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace w2::xmi {

constexpr int TICK_RATE = 120;

struct Event {
  int t;               // the tick
  uint8_t st, a, b;    // MIDI status and data; note-offs are 0x80 | channel
};

struct Sequence {
  int length = 0;      // in ticks, to the end of the last note
  std::vector<Event> events;
};

/** The first sequence of an XMI file's bytes, or nothing without an EVNT
 *  chunk. Meta events are dropped. */
std::optional<Sequence> parse(const std::string& bytes);

}  // namespace w2::xmi
