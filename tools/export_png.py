"""Batch-export every .PCK in the game to PNG for visual inspection.

    python3 tools/export_png.py [original_dir] [out_dir]

Output mirrors the source tree, e.g. original/TERRAIN0/A0.PCK -> preview/TERRAIN0/A0.png

Palette selection (confirmed visually -- see docs/formats/pck.md):
  STANDARD.PAL / TERRAIN0/WAR2.PAL  byte-identical; everything outside START/
  START/LOGO.PAL                    the gold SSG logo and its sparkle overlay
  START/SCREEN0.PAL                 the title screen, Warlords II wordmark, etc.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pal import load as load_pal
from pck import load as load_pck, write_png


def main(src='original', out='preview'):
    std = load_pal(os.path.join(src, 'STANDARD.PAL'))
    logo = load_pal(os.path.join(src, 'START', 'LOGO.PAL'))
    screen0 = load_pal(os.path.join(src, 'START', 'SCREEN0.PAL'))

    n = 0
    for root, _, names in os.walk(src):
        for name in sorted(names):
            if not name.upper().endswith('.PCK'):
                continue
            path = os.path.join(root, name)
            rel = os.path.relpath(root, src)
            dst_dir = os.path.join(out, '' if rel == '.' else rel)
            os.makedirs(dst_dir, exist_ok=True)
            w, h, px = load_pck(path)
            stem = os.path.splitext(name)[0]

            if os.path.basename(root).upper() == 'START':
                pal = logo if stem.upper() in ('SSG', 'SPARKLE') else screen0
            else:
                pal = std
            variants = [(f'{stem}.png', pal)]

            for fn, pal in variants:
                write_png(os.path.join(dst_dir, fn), w, h, px, pal)
                n += 1
            print(f"  {path:<40} {w:>4}x{h:<4} -> {os.path.join(dst_dir, variants[0][0])}")
    print(f"\nwrote {n} PNGs to {out}/")


if __name__ == '__main__':
    main(*(sys.argv[1:] or ['original', 'preview']))
