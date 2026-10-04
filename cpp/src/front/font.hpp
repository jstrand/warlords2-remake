// The original's proportional fonts: TEXT, CHANCE17 and CHANCE36.
//
// A .FNT is a .PCK image -- a sheet of glyphs -- and the .FIN beside it holds
// the metrics. docs/formats/font.md. Glyphs are packed left to right into rows
// `lineHeight` tall, each taking a slot rounded up to a multiple of 8. Text
// is bytes, one character a byte, as the game's files give it.
#pragma once

#include <array>
#include <map>
#include <memory>
#include <string>

#include "platform/gfx.hpp"
#include "warlords/pal.hpp"
#include "warlords/pck.hpp"

class Font {
 public:
  /** Load DATA/<name>.FIN and .FNT, keying out `keyIndex`. */
  static std::shared_ptr<Font> load(const std::string& dataDir, const std::string& name, const w2::pal::Palette& palette,
                                    int keyIndex);
  int width(const std::string& text) const;
  int draw(const std::string& text, double x, double y) const;
  /** Draw centred in a rect, the way the original centres a control's text. */
  void drawCentred(const std::string& text, double x, double y, double w, double h) const;
  /** The same font in other colours, as 78a8:06ae asks for it: the sheet's
   *  glyph colour (15) becomes `glyph` and its outline (0) `outline`. */
  const Font& colours(int glyph, int outline) const;
  int lineHeight = 0, baseline = 0;

 private:
  struct Metrics {
    int count = 0, first = 0, sheetWidth = 0;
    std::array<int, 256> ink{}, width{};
  };
  std::shared_ptr<Metrics> m_;
  std::shared_ptr<std::map<int, gfx::Quad>> quad_;
  gfx::ImageP image_;
  std::shared_ptr<const w2::pck::Pixels> px_;
  w2::pal::Palette palette_;
  int key_ = -1;
  mutable std::shared_ptr<std::map<int, std::shared_ptr<Font>>> variants_;
};
using FontP = std::shared_ptr<Font>;
