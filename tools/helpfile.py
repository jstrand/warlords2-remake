"""Warlords II HELP/WARLORD2.HLP reader -- SOLVED.

A flat table of context-help entries: every button, list control and menu item
in the game, with the one-line tooltip the game shows for it.

    0      u16   record count (388)
    2      388 x 104-byte records

Record (104 bytes):
    +0     u16       help id
    +2     u16       sub-id, for controls that repeat (see below); 0 normally,
                     and 65535 (-1) on a block of placeholder entries
    +4     char[50]  title, e.g. "- Raze City -"
    +54    char[50]  description, e.g. "Permanently ruin city"

`2 + 388 * 104 == 40354`, the exact file size.

The sub-id keys repeated controls: ids 336-343 are eight "- Select Army -"
buttons with sub-ids 37-44, and 430-456 are twenty-seven "- Swap Armies -"
buttons with sub-ids 45-71. So a lookup is by (id, sub_id), not id alone --
records are not stored in id order and one id appears twice.

Entries with empty title and description are placeholders that still occupy a
slot (ids 224-241 all carry sub-id 65535).

This is effectively a UI specification: 388 named actions covering every screen,
which is a useful checklist when building the engine's interface.
"""
import struct

RECORD_SIZE = 104


def load(path):
    """Return a list of dicts: id, sub_id, title, description."""
    with open(path, 'rb') as f:
        d = f.read()
    n = struct.unpack_from('<H', d, 0)[0]
    if len(d) != 2 + RECORD_SIZE * n:
        raise ValueError(f"{path}: {len(d)} bytes for {n} records")
    out = []
    for i in range(n):
        o = 2 + RECORD_SIZE * i
        hid, sub = struct.unpack_from('<HH', d, o)
        out.append(dict(
            id=hid,
            sub_id=sub,
            title=d[o + 4:o + 54].split(b'\0')[0].decode('latin1'),
            description=d[o + 54:o + RECORD_SIZE].split(b'\0')[0].decode('latin1'),
        ))
    return out


if __name__ == '__main__':
    import sys
    path = sys.argv[1] if len(sys.argv) > 1 else 'original/HELP/WARLORD2.HLP'
    recs = load(path)
    pat = sys.argv[2].lower() if len(sys.argv) > 2 else None
    print(f"# {path}: {len(recs)} entries")
    for r in recs:
        if not r['title'] and not r['description']:
            continue
        if pat and pat not in (r['title'] + r['description']).lower():
            continue
        print(f"{r['id']:>4} {r['sub_id']:>5}  {r['title']:<28} {r['description']}")
