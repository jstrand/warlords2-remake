// A help screen from a .GFX file (8065:168d), e.g. HELP\HITEM.GFX.
//
// The file's #D number picks the popup -- 14 + n -- and 7ecb:06de lays the
// page out inside it, every position counted from the popup's corner:
//
//   #H          font 1, colour 15        #T      font 2, colour 15
//   #Fnnn       font 2, colour nnn       #E      the end
//   #C(x,y)|t|  centred on x             #L(x,y)|t|  from x
//   #R(x,y)|t|  ending at x
//   #G(x,y,w,h)nnn(dx,dy)  the (x, y) w x h of bitmap nnn, at (dx, dy)
//
// A key or a click puts it away. The control panel's "?" shows HMOUSE and
// then HKEYS in popup 4, (120, 50) 400x360.
#include <regex>

#include "ui/dialogs.hpp"
#include "util/util.hpp"

namespace help {

const kit::Rect POPUP4{120, 50, 400, 360};

namespace {
const kit::Rect POPUPS[2] = {
    {256, 40, 352, 340},       // popup 14
    {32, 40, 352, 340},        // popup 15
};

struct Step {
  int font = 0, colour = 15;   // a font change when font != 0
  bool isText = false;
  std::string text;
  char align = 'L';
  int x = 0, y = 0;
  int bitmap = -1, sx = 0, sy = 0, w = 0, h = 0;
};

int parse(const std::string& text, std::vector<Step>& steps) {
  int page = 0;
  static const std::regex lineRe("^#([A-Z])(.*)$");
  static const std::regex numRe("^(\\d+)");
  static const std::regex textRe("^\\((\\d+),(\\d+)\\)\\|(.*?)\\|");
  static const std::regex gfxRe("^\\((\\d+),(\\d+),(\\d+),(\\d+)\\)(\\d+)\\((\\d+),(\\d+)\\)");
  for (auto& line : w2::splitLines(text)) {
    std::smatch m;
    if (!std::regex_search(line, m, lineRe)) continue;
    char op = m[1].str()[0];
    std::string rest = m[2];
    std::smatch t;
    if (op == 'D') {
      page = std::regex_search(rest, t, numRe) ? std::stoi(t[1]) : 0;
    } else if (op == 'H') {
      Step s; s.font = 1; s.colour = 15; steps.push_back(s);
    } else if (op == 'T') {
      Step s; s.font = 2; s.colour = 15; steps.push_back(s);
    } else if (op == 'F') {
      Step s; s.font = 2;
      s.colour = std::regex_search(rest, t, numRe) ? std::stoi(t[1]) : 15;
      steps.push_back(s);
    } else if (op == 'C' || op == 'L' || op == 'R') {
      if (std::regex_search(rest, t, textRe)) {
        Step s; s.isText = true; s.text = t[3]; s.align = op; s.x = std::stoi(t[1]); s.y = std::stoi(t[2]);
        steps.push_back(s);
      }
    } else if (op == 'G') {
      if (std::regex_search(rest, t, gfxRe)) {
        Step s;
        s.bitmap = std::stoi(t[5]);
        s.sx = std::stoi(t[1]); s.sy = std::stoi(t[2]); s.w = std::stoi(t[3]); s.h = std::stoi(t[4]);
        s.x = std::stoi(t[6]); s.y = std::stoi(t[7]);
        steps.push_back(s);
      }
    } else if (op == 'E') {
      break;
    }
  }
  return page;
}

struct Page : kit::Modal {
  std::vector<Step> steps;
  kit::Rect R;
  kit::Done after;
  void close() {
    auto a = after;
    kit::pop(this);
    if (a) a();
  }
  void draw() override {
    kit::popup(R);
    const Font* font = &kit::font(2);
    int colour = 15;
    for (const Step& s : steps) {
      if (s.font) {
        font = &kit::font(s.font);
        colour = s.colour;
      } else if (s.isText) {
        const Font& fc = font->colours(colour, 0);
        gfx::setColor(1, 1, 1);
        if (s.align == 'C') kit::centred(fc, s.text, R.x + s.x, R.y + s.y);
        else if (s.align == 'R') kit::right(fc, s.text, R.x + s.x, R.y + s.y);
        else fc.draw(s.text, R.x + s.x, R.y + s.y);
      } else if (s.bitmap >= 0) {
        if (auto art = G.screen->artFor(s.bitmap)) {
          gfx::setColor(1, 1, 1);
          gfx::draw(art->image, gfx::newQuad(s.sx, s.sy, s.w, s.h), R.x + s.x, R.y + s.y);
        }
      }
    }
  }
  void mousepressed(int, int, int) override { close(); }
  void keypressed(const std::string&) override { close(); }
};
}  // namespace

bool open(const std::string& name, kit::Done after, const kit::Rect* R) {
  std::string path = name;
  for (auto& c : path) if (c == '\\') c = '/';
  auto text = w2::readFile(G.dataDir + "/" + path);
  if (!text) return false;
  auto d = std::make_shared<Page>();
  int page = parse(*text, d->steps);
  d->R = R ? *R : (page == 1 ? POPUPS[1] : POPUPS[0]);
  d->after = after;
  kit::push(d);
  return true;
}

}  // namespace help
