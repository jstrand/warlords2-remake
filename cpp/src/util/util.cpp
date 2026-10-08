#include "util/util.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <sstream>

namespace fs = std::filesystem;

namespace w2 {

std::string fmt(const char* f, ...) {
  va_list ap;
  va_start(ap, f);
  char buf[512];
  va_list ap2;
  va_copy(ap2, ap);
  int n = vsnprintf(buf, sizeof buf, f, ap);
  va_end(ap);
  std::string out;
  if (n < (int)sizeof buf) {
    out.assign(buf, n < 0 ? 0 : n);
  } else {
    out.resize(n + 1);
    vsnprintf(out.data(), n + 1, f, ap2);
    out.resize(n);
  }
  va_end(ap2);
  return out;
}

std::string sfmt(const std::string& f, const std::vector<FmtArg>& args) {
  std::string out;
  size_t n = 0;
  for (size_t i = 0; i < f.size(); i++) {
    char c = f[i];
    if (c != '%' || i + 1 >= f.size()) { out += c; continue; }
    size_t j = i + 1;
    std::string flags;
    while (j < f.size() && strchr("-+ 0#", f[j])) flags += f[j++];
    int width = 0;
    while (j < f.size() && isdigit((unsigned char)f[j])) width = width * 10 + (f[j++] - '0');
    int prec = -1;
    if (j < f.size() && f[j] == '.') {
      prec = 0;
      j++;
      while (j < f.size() && isdigit((unsigned char)f[j])) prec = prec * 10 + (f[j++] - '0');
    }
    while (j < f.size() && (f[j] == 'l' || f[j] == 'h')) j++;
    if (j >= f.size()) { out += f.substr(i); break; }
    char conv = f[j];
    i = j;
    if (conv == '%') { out += '%'; continue; }
    FmtArg a = n < args.size() ? args[n] : FmtArg("nil");
    n++;
    std::string s;
    switch (conv) {
      case 'd': case 'i': case 'u': {
        long v = a.str ? (toInt(a.s) ? *toInt(a.s) : 0) : a.n;
        s = std::to_string(v < 0 ? -v : v);
        if (prec >= 0) while ((int)s.size() < prec) s = "0" + s;
        if (v < 0) s = "-" + s;
        else if (flags.find('+') != std::string::npos) s = "+" + s;
        break;
      }
      case 'x': case 'X': {
        char buf[32];
        snprintf(buf, sizeof buf, conv == 'x' ? "%lx" : "%lX", (unsigned long)(a.str ? 0 : a.n));
        s = buf;
        break;
      }
      case 'c': s = std::string(1, (char)(a.str ? (a.s.empty() ? ' ' : a.s[0]) : a.n)); break;
      case 's': default:
        s = a.str ? a.s : std::to_string(a.n);
        if (prec >= 0 && (int)s.size() > prec) s = s.substr(0, prec);
        break;
    }
    if ((int)s.size() < width) {
      if (flags.find('-') != std::string::npos) s += std::string(width - s.size(), ' ');
      else if (flags.find('0') != std::string::npos && conv != 's') {
        bool sign = !s.empty() && (s[0] == '-' || s[0] == '+');
        std::string body = sign ? s.substr(1) : s;
        s = (sign ? s.substr(0, 1) : "") + std::string(width - s.size(), '0') + body;
      } else {
        s = std::string(width - s.size(), ' ') + s;
      }
    }
    out += s;
  }
  return out;
}

// ------------------------------------------------------------------- files

namespace {
std::mutex dirLock;
std::map<std::string, std::vector<std::string>> dirCache;

const std::vector<std::string>& listing(const std::string& dir) {
  auto it = dirCache.find(dir);
  if (it != dirCache.end()) return it->second;
  std::vector<std::string> names;
  std::error_code ec;
  for (auto& e : fs::directory_iterator(dir.empty() ? "." : dir, ec)) names.push_back(e.path().filename().string());
  return dirCache[dir] = names;
}
}  // namespace

std::string resolvePath(const std::string& path) {
  std::error_code ec;
  if (fs::exists(path, ec)) return path;
  std::string p = path;
  std::replace(p.begin(), p.end(), '\\', '/');
  std::lock_guard<std::mutex> lock(dirLock);
  std::string cur;
  size_t i = 0;
  if (!p.empty() && p[0] == '/') { cur = "/"; i = 1; }
  while (i <= p.size()) {
    size_t j = p.find('/', i);
    if (j == std::string::npos) j = p.size();
    std::string part = p.substr(i, j - i);
    i = j + 1;
    if (part.empty() || part == ".") continue;
    std::string next = cur.empty() ? part : (cur.back() == '/' ? cur + part : cur + "/" + part);
    if (part != ".." && !fs::exists(next, ec)) {
      std::string found;
      for (auto& n : listing(cur)) {
        if (n.size() == part.size() && upper(n) == upper(part)) { found = n; break; }
      }
      if (found.empty()) return "";
      next = cur.empty() ? found : (cur.back() == '/' ? cur + found : cur + "/" + found);
    }
    cur = next;
  }
  return cur;
}

namespace {
std::map<std::string, std::string>& madeFiles() {
  static std::map<std::string, std::string> made;
  return made;
}
std::string madeKey(const std::string& path) {
  std::string k;
  for (char c : path) {
    char d = c == '\\' ? '/' : (char)std::toupper((unsigned char)c);
    if (d == '/' && !k.empty() && k.back() == '/') continue;
    k += d;
  }
  return k;
}
}  // namespace

void installFile(const std::string& path, const std::string& bytes) { madeFiles()[madeKey(path)] = bytes; }

std::optional<std::string> readFile(const std::string& path) {
  auto made = madeFiles().find(madeKey(path));
  if (made != madeFiles().end()) return made->second;
  std::string real = resolvePath(path);
  if (real.empty()) return std::nullopt;
  std::ifstream f(real, std::ios::binary);
  if (!f) return std::nullopt;
  std::ostringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

std::string mustRead(const std::string& path) {
  auto s = readFile(path);
  if (!s) throw std::runtime_error("cannot open: " + path);
  return *s;
}

bool fileExists(const std::string& path) {
  return madeFiles().count(madeKey(path)) > 0 || !resolvePath(path).empty();
}

bool writeFile(const std::string& path, const std::string& bytes) {
  std::string real = resolvePath(path);
  if (real.empty()) real = path;
  std::string tmp = real + ".tmp";
  {
    std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
    if (!f) return false;
    f.write(bytes.data(), (std::streamsize)bytes.size());
    if (!f) return false;
  }
  std::error_code ec;
  fs::rename(tmp, real, ec);
  if (ec) return false;
  std::lock_guard<std::mutex> lock(dirLock);
  dirCache.clear();
  return true;
}

bool removeFile(const std::string& path) {
  std::string real = resolvePath(path);
  if (real.empty()) return false;
  std::error_code ec;
  bool ok = fs::remove(real, ec);
  std::lock_guard<std::mutex> lock(dirLock);
  dirCache.clear();
  return ok;
}

// ----------------------------------------------------------------- strings

std::string upper(std::string s) {
  for (auto& c : s) c = (char)std::toupper((unsigned char)c);
  return s;
}
std::string lower(std::string s) {
  for (auto& c : s) c = (char)std::tolower((unsigned char)c);
  return s;
}
std::string trimRight(const std::string& s) {
  size_t e = s.size();
  while (e > 0 && std::isspace((unsigned char)s[e - 1])) e--;
  return s.substr(0, e);
}
std::string trim(const std::string& s) {
  size_t b = 0;
  while (b < s.size() && std::isspace((unsigned char)s[b])) b++;
  return trimRight(s.substr(b));
}
std::vector<std::string> splitLines(const std::string& s) {
  std::vector<std::string> out;
  std::string cur;
  for (char c : s) {
    if (c == '\r' || c == '\n') {
      if (!cur.empty()) out.push_back(cur);
      cur.clear();
    } else {
      cur += c;
    }
  }
  if (!cur.empty()) out.push_back(cur);
  return out;
}
bool startsWith(const std::string& s, const std::string& p) { return s.compare(0, p.size(), p) == 0; }
bool endsWith(const std::string& s, const std::string& p) {
  return s.size() >= p.size() && s.compare(s.size() - p.size(), p.size(), p) == 0;
}
bool contains(const std::string& s, const std::string& p) { return s.find(p) != std::string::npos; }

std::string latin1ToUtf8(const std::string& s) {
  std::string out;
  for (unsigned char c : s) {
    if (c < 0x80) out += (char)c;
    else { out += (char)(0xc0 | (c >> 6)); out += (char)(0x80 | (c & 0x3f)); }
  }
  return out;
}
std::string utf8ToLatin1(const std::string& s) {
  std::string out;
  for (size_t i = 0; i < s.size();) {
    unsigned char c = s[i];
    if (c < 0x80) { out += (char)c; i++; continue; }
    int n = c >= 0xf0 ? 4 : c >= 0xe0 ? 3 : 2;
    unsigned cp = c & (0x3f >> (n - 1));
    for (int k = 1; k < n && i + k < s.size(); k++) cp = (cp << 6) | (s[i + k] & 0x3f);
    out += cp < 256 ? (char)cp : '?';
    i += n;
  }
  return out;
}
std::optional<long> toInt(const std::string& s) {
  std::string t = trim(s);
  if (t.empty()) return std::nullopt;
  size_t i = 0;
  bool neg = false;
  if (t[0] == '-' || t[0] == '+') { neg = t[0] == '-'; i = 1; }
  if (i >= t.size()) return std::nullopt;
  long v = 0;
  for (; i < t.size(); i++) {
    if (!std::isdigit((unsigned char)t[i])) return std::nullopt;
    v = v * 10 + (t[i] - '0');
  }
  return neg ? -v : v;
}

double now() {
  using namespace std::chrono;
  static const auto start = steady_clock::now();
  return duration<double>(steady_clock::now() - start).count();
}

}  // namespace w2
