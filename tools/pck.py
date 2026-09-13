"""Warlords II .PCK image codec -- SOLVED.

Container
---------
    u16 version    always 1
    u16 width      pixels
    u16 height     pixels
    ...            four compressed bitplane streams, back to back

The image is 4bpp (16 colours, matching the 16-entry .PAL files), stored as
four separate 1-bit planes of (width/8)*height bytes each. Each plane is
compressed independently -- the LZ77 window resets at every plane boundary.

Compression: LZ77 with a signed big-endian offset
-------------------------------------------------
Read a command byte. Its top bit selects the token type, which is really just
the sign bit of a big-endian 16-bit offset:

    cmd < 0x80   LITERAL RUN. The next (cmd + 1) bytes are copied out verbatim.

    cmd >= 0x80  MATCH, 3 bytes total: [cmd][mid][len]
                 offset = (cmd << 8) | mid      -- big-endian, always negative
                 distance = 65536 - offset      -- 1..32768
                 length   = len + 1             -- 1..256
                 Copy `length` bytes from `distance` back, one byte at a time
                 so that overlapping copies replicate (distance 1 = RLE).

Reads before the start of the plane yield 0, so a match may reference the
zero-filled window before any output exists -- this is how uniform images
compress ~85:1 (the ceiling is 256 output bytes per 3 input bytes).

Verified: all 92 .PCK files in the shipped game decode to exactly
4 * (width/8) * height bytes, consuming each file exactly.
"""
import struct
import zlib


def decode_plane(b, i, psz):
    """Decode one bitplane. Returns (plane_bytes, new_offset)."""
    out = bytearray()
    while len(out) < psz:
        cmd = b[i]
        if cmd < 0x80:
            n = cmd + 1
            i += 1
            out += b[i:i + n]
            i += n
        else:
            dist = 65536 - ((cmd << 8) | b[i + 1])
            n = b[i + 2] + 1
            i += 3
            for _ in range(n):
                src = len(out) - dist
                out.append(out[src] if src >= 0 else 0)
    if len(out) != psz:
        raise ValueError(f"plane overran: {len(out)} != {psz}")
    return bytes(out), i


def load(path):
    """Decode a .PCK. Returns (width, height, indices) with one byte per pixel (0-15)."""
    with open(path, 'rb') as f:
        d = f.read()
    ver, w, h = struct.unpack_from('<HHH', d, 0)
    if ver != 1:
        raise ValueError(f"{path}: unexpected version {ver}")
    psz = (w // 8) * h
    i = 6
    planes = []
    for _ in range(4):
        pl, i = decode_plane(d, i, psz)
        planes.append(pl)
    if i != len(d):
        raise ValueError(f"{path}: trailing data ({i} != {len(d)})")

    rowbytes = w // 8
    px = bytearray(w * h)
    for p, plane in enumerate(planes):
        bit = 1 << p
        for y in range(h):
            base = y * rowbytes
            row = y * w
            for xb in range(rowbytes):
                v = plane[base + xb]
                if not v:
                    continue
                o = row + xb * 8
                for k in range(8):
                    if v & (0x80 >> k):        # MSB = leftmost pixel
                        px[o + k] |= bit
    return w, h, bytes(px)


def write_png(path, w, h, px, palette):
    """8-bit palettised PNG. palette = list of 16 (r,g,b)."""
    raw = b''.join(b'\x00' + px[y * w:(y + 1) * w] for y in range(h))

    def chunk(tag, data):
        c = tag + data
        return struct.pack('>I', len(data)) + c + struct.pack('>I', zlib.crc32(c))

    png = (b'\x89PNG\r\n\x1a\n'
           + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 3, 0, 0, 0))
           + chunk(b'PLTE', b''.join(bytes(c) for c in palette))
           + chunk(b'IDAT', zlib.compress(raw, 9))
           + chunk(b'IEND', b''))
    with open(path, 'wb') as f:
        f.write(png)


if __name__ == '__main__':
    import os
    import sys
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from pal import load as load_pal

    palfile, out_dir = sys.argv[1], sys.argv[2]
    pal = load_pal(palfile)
    os.makedirs(out_dir, exist_ok=True)
    for p in sys.argv[3:]:
        w, h, px = load(p)
        name = os.path.basename(p).replace('.PCK', '.png')
        write_png(os.path.join(out_dir, name), w, h, px, pal)
        print(f"  {p}  {w}x{h} -> {name}")
