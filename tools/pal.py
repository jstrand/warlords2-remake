"""Warlords II .PAL reader -- SOLVED.

Format: plain ASCII, 16 lines of "RR GG BB", CRLF-terminated.
Components are PERCENTAGES (0-99), not VGA 0-63 values.
16 entries confirms the game is 4bpp / 16-colour.
"""
import sys

def load(path):
    """Return a list of 16 (r,g,b) tuples scaled to 0-255."""
    out = []
    with open(path, 'r', newline='') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            r, g, b = (int(x) for x in line.split())
            out.append((round(r * 255 / 99), round(g * 255 / 99), round(b * 255 / 99)))
    assert len(out) == 16, f"{path}: expected 16 entries, got {len(out)}"
    return out

if __name__ == '__main__':
    for p in sys.argv[1:]:
        print(p)
        for i, c in enumerate(load(p)):
            print(f"  {i:2}  #{c[0]:02x}{c[1]:02x}{c[2]:02x}  {c}")
