"""Warlords II DATA/STRING.DAT reader -- SOLVED.

The game's entire UI text corpus: 169 groups, 651 strings. Every byte of the
file is accounted for -- no padding, no unreferenced strings, no slack.

Layout
------
    0                    index table: `n` entries of { u16 offset, u16 count }
                         `n` is implied -- the first entry's offset IS the size
                         of the index table, so n = read_u16(0) / 4
    <first offset>       pointer region: for each group, `count` u16 file
                         offsets, one per string. The groups' pointer arrays
                         tile this region contiguously, in order.
    <after pointers>     string data: NUL-terminated latin-1 strings.

For the shipped file: 169 groups, index table 676 bytes (169*4 == 676, which is
exactly the first group's offset), pointer region 676..1978, string data
1978..14054. All 651 pointers are ascending and every string is referenced
exactly once.

A group is a screen, dialog or enumeration -- the unit of retrieval the game
uses. Notable ones are listed in docs/formats/string.md; the two that matter
most for the rules are:

    group 105  per-army ability tags, indexed by ARMYTYPE.DAT's +0 field
    group 163  the Army Bonus wording, which names ARMYTYPE.DAT's bonus fields
    group 128  the ten terrain names   \\ paired: [i] names the terrain that
    group 129  their descriptions      /  group 129[i] describes
"""
import struct


def load(path):
    """Return a list of groups, each a list of strings."""
    with open(path, 'rb') as f:
        d = f.read()
    n = struct.unpack_from('<H', d, 0)[0] // 4
    groups = []
    for i in range(n):
        off, cnt = struct.unpack_from('<HH', d, 4 * i)
        strs = []
        for j in range(cnt):
            p = struct.unpack_from('<H', d, off + 2 * j)[0]
            strs.append(d[p:d.index(b'\0', p)].decode('latin1'))
        groups.append(strs)
    return groups


def verify(path):
    """Re-derive the invariants that prove the format. Raises on mismatch."""
    with open(path, 'rb') as f:
        d = f.read()
    n = struct.unpack_from('<H', d, 0)[0] // 4
    hdr = [struct.unpack_from('<HH', d, 4 * i) for i in range(n)]
    assert n * 4 == hdr[0][0], "index table size must equal first group offset"
    for i in range(n - 1):
        assert hdr[i][0] + 2 * hdr[i][1] == hdr[i + 1][0], f"group {i} does not tile"
    end = hdr[-1][0] + 2 * hdr[-1][1]
    ptrs = [struct.unpack_from('<H', d, o + 2 * j)[0] for o, c in hdr for j in range(c)]
    assert ptrs == sorted(ptrs) and ptrs[0] == end, "pointers must ascend from string data"
    pos, count = end, 0
    while pos < len(d):
        pos = d.index(b'\0', pos) + 1
        count += 1
    assert count == len(ptrs) == len(set(ptrs)), "every string referenced exactly once"
    return n, len(ptrs)


if __name__ == '__main__':
    import sys
    path = sys.argv[1] if len(sys.argv) > 1 else 'original/DATA/STRING.DAT'
    ngroups, nstrings = verify(path)
    print(f"# {path}: {ngroups} groups, {nstrings} strings, structure verified\n")
    want = [int(a) for a in sys.argv[2:]]
    for gi, g in enumerate(load(path)):
        if want and gi not in want:
            continue
        print(f"=== group {gi} (n={len(g)})")
        for j, s in enumerate(g):
            print(f"    [{j}] {s!r}")
