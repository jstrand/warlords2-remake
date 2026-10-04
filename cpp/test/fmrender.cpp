// Render a song through the FM driver and print a checksum per second, to
// compare with the Lua remake's (test/fmrender.lua).
//     w2fm original/SOUND/SINT12.XMI 10
#include <cstdio>
#include <cstdlib>

#include "util/util.hpp"
#include "warlords/ailfm.hpp"

int main(int argc, char** argv) {
  if (argc < 3) return 2;
  auto seq = w2::xmi::parse(w2::mustRead(argv[1]));
  if (!seq) return 1;
  w2::AilFm drv(w2::mustRead("original/ADLIB.ADV"), w2::mustRead("original/MIDPAK.AD"));
  drv.play(&*seq, false);
  int secs = atoi(argv[2]);
  const int N = 49716;
  std::vector<int16_t> buf(N);
  for (int s = 0; s < secs; s++) {
    drv.render(buf.data(), N);
    long sum = 0, peak = 0;
    for (int i = 0; i < N; i++) {
      sum = (sum * 31 + buf[i] + 65536) % 1000000007;
      peak = std::max(peak, (long)std::abs(buf[i]));
    }
    printf("%d %ld %ld\n", s, sum, peak);
  }
}
