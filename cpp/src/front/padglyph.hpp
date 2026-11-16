// A controller's buttons as symbols, small enough to sit beside what they
// do on the screen: A B X Y in a disc, the shoulders, triggers, stick clicks
// and back grips by name in a lozenge, the D-pad with one arm lit, and View
// and Menu by the marks the Steam Deck prints on them. Not the original's.
//
// Drawn in the game's palette a pixel at a time, as there is no art for
// them: 11 rows tall, 11 or 13 columns wide.
#pragma once

#include <string>

namespace padglyph {

constexpr int H = 11;
/** How wide a button's symbol is. */
int width(int button);
/** A button's (pad::Button) symbol, its top left at (x, y): `fill` under
 *  `ink`, both palette colours. */
void draw(int button, double x, double y, int fill = 0, int ink = 15);
/** The symbols' own letters, 3 by 5 with a column between: A B L R X Y and
 *  the digits 1 to 5; anything else is a gap. */
void text(const std::string& s, double x, double y, int ink = 15);

}  // namespace padglyph
