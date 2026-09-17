#!/usr/bin/env python3
"""WARLORD2.EXE: parse the Borland C++ 3.x VROOMM overlay layout and rebuild
a flat, overlay-free MZ that ordinary disassemblers can load.

See docs/formats/exe.md for the layout.

    python3 tools/exe.py info    original/WARLORD2.EXE
    python3 tools/exe.py flatten original/WARLORD2.EXE build/WAR2FLAT.EXE [map.json]
"""
import json
import struct
import sys

SEG_CODE, SEG_STUB = 1, 3
STUB_HDR = 0x20
STUB_ENTRY = 5          # CD 3F <u16 offset> 00  ->  EA <u16 off> <u16 seg>


def decode_fpu_emulation(image, start, end):
    """Rewrite Borland's x87 emulator interrupts back into real FPU opcodes.

    INT 34h..3Bh  -> FWAIT; D8h..DFh        (same length, ModRM follows)
    INT 3Ch xx    -> seg prefix; D8h|xx&7   (xx top bits: 11 = ES)
    INT 3Dh       -> NOP; FWAIT

    This is a byte scan, so an immediate that happens to contain CD 34..3D
    would be corrupted; the counts are small and worth the readability.
    """
    n = 0
    i = start
    while i < end - 2:
        if image[i] == 0xCD:
            v = image[i + 1]
            if 0x34 <= v <= 0x3B:
                image[i:i + 2] = bytes((0x9B, 0xD8 + v - 0x34))
                n += 1
                i += 2
                continue
            if v == 0x3C and image[i + 2] >> 6 == 3:
                image[i:i + 3] = bytes((0x90, 0x26, 0xD8 | image[i + 2] & 7))
                n += 1
                i += 3
                continue
            if v == 0x3D:
                image[i:i + 2] = b'\x90\x9b'
                n += 1
                i += 2
                continue
        i += 1
    return n


class Exe:
    def __init__(self, data):
        self.data = data
        (sig, lastpage, pages, nrel, hdrpar, self.minalloc, self.maxalloc,
         self.ss, self.sp, _csum, self.ip, self.cs, relofs, _ovno) = \
            struct.unpack_from('<2s13H', data, 0)
        if sig != b'MZ':
            raise ValueError('not an MZ executable')
        self.hdr = hdrpar * 16
        self.end = (pages - 1) * 512 + lastpage if lastpage else pages * 512
        self.image = bytearray(data[self.hdr:self.end])
        self.relocs = [struct.unpack_from('<HH', data, relofs + 4 * i)[::-1]
                       for i in range(nrel)]            # (seg, off)

        if data[self.end:self.end + 4] != b'FBOV':
            raise ValueError('no FBOV overlay header')
        self.ovr_size, segtab, nseg = struct.unpack_from('<III', data, self.end + 4)
        self.ovr_base = self.end + 16
        # segtab is a file offset; entries are (seg, maxoff, flags, minoff)
        self.segments = [struct.unpack_from('<4H', data, segtab + 8 * i)
                         for i in range(nseg)]
        self.stubs = [self._stub(i, s) for i, s in enumerate(self.segments)
                      if s[2] == SEG_STUB]

    def _stub(self, index, seg):
        base = seg[0] * 16
        fileofs, codesize, fixsize, nentries = struct.unpack_from('<IHHH', self.image, base + 4)
        code = self.data[self.ovr_base + fileofs:self.ovr_base + fileofs + codesize]
        fix = self.data[self.ovr_base + fileofs + codesize:
                        self.ovr_base + fileofs + codesize + fixsize]
        entries = []
        for k in range(nentries):
            e = base + STUB_HDR + k * STUB_ENTRY
            if self.image[e:e + 2] != b'\xcd\x3f':
                raise ValueError('stub %04x entry %d is not INT 3F' % (seg[0], k))
            entries.append(struct.unpack_from('<H', self.image, e + 2)[0])
        return dict(index=index, seg=seg[0], fileofs=fileofs, code=code,
                    fixups=list(struct.unpack('<%dH' % (fixsize // 2), fix)),
                    entries=entries)

    def seg_from_selector(self, value):
        if value % 8 or value // 8 >= len(self.segments):
            raise ValueError('bad overlay segment selector %04x' % value)
        return self.segments[value // 8][0]

    def flatten(self):
        """Return (flat_exe_bytes, segment_map)."""
        image = bytearray(self.image)
        relocs = list(self.relocs)
        # Overlays go above everything already in the image (incl. the stack).
        top = max(len(image), self.ss * 16 + self.sp)
        nextseg = (top + 15) // 16 + 1
        smap = []
        for st in self.stubs:
            newseg = nextseg
            code = bytearray(st['code'])
            for off in st['fixups']:
                sel = struct.unpack_from('<H', code, off)[0]
                struct.pack_into('<H', code, off, self.seg_from_selector(sel))
                relocs.append((newseg, off))
            start = newseg * 16
            image.extend(b'\0' * (start - len(image)))
            image.extend(code)
            # Patch each INT 3F thunk into JMP FAR newseg:target.
            for k, target in enumerate(st['entries']):
                e = st['seg'] * 16 + STUB_HDR + k * STUB_ENTRY
                image[e:e + 5] = struct.pack('<BHH', 0xEA, target, newseg)
                relocs.append((st['seg'], STUB_HDR + k * STUB_ENTRY + 3))
            smap.append(dict(stub=st['seg'], code=newseg, size=len(code),
                             entries=st['entries']))
            nextseg = (len(image) + 15) // 16 + 1

        # Point far pointers that go through a thunk (CALL FAR stub:entry,
        # far function pointers in data) straight at the overlay code, so
        # cross-references land on the real function.
        thunks = {}
        for st, sm in zip(self.stubs, smap):
            for k, target in enumerate(st['entries']):
                thunks[(st['seg'], STUB_HDR + k * STUB_ENTRY)] = (sm['code'], target)
        redirected = 0
        for seg, off in relocs:
            p = seg * 16 + off
            if p < 2:
                continue
            ofs, sseg = struct.unpack_from('<HH', image, p - 2)
            hit = thunks.get((sseg, ofs))
            if hit and seg not in {st['seg'] for st in self.stubs}:
                struct.pack_into('<HH', image, p - 2, hit[1], hit[0])
                redirected += 1

        code_ranges = [(s[0] * 16, s[0] * 16 + s[1]) for s in self.segments
                       if s[2] == SEG_CODE]
        code_ranges += [(sm['code'] * 16, sm['code'] * 16 + sm['size']) for sm in smap]
        fpu = sum(decode_fpu_emulation(image, a, b) for a, b in code_ranges)

        relofs = 0x40
        hdrsize = (relofs + 4 * len(relocs) + 511) // 512 * 512
        total = hdrsize + len(image)
        hdr = bytearray(hdrsize)
        struct.pack_into('<2s13H', hdr, 0, b'MZ', total % 512,
                         (total + 511) // 512, len(relocs), hdrsize // 16,
                         self.minalloc, self.maxalloc, self.ss, self.sp, 0,
                         self.ip, self.cs, relofs, 0)
        for i, (seg, off) in enumerate(relocs):
            struct.pack_into('<HH', hdr, relofs + 4 * i, off, seg)

        segmap = dict(
            segments=[dict(index=i, seg=s[0], size=s[1], flags=s[2])
                      for i, s in enumerate(self.segments)],
            overlays=smap, redirected=redirected, fpu_rewrites=fpu)
        return bytes(hdr + image), segmap


def main(argv):
    if len(argv) < 3 or argv[1] not in ('info', 'flatten'):
        sys.exit(__doc__)
    exe = Exe(open(argv[2], 'rb').read())
    if argv[1] == 'info':
        print('image %06x bytes, %d relocs, entry %04x:%04x, stack %04x:%04x'
              % (len(exe.image), len(exe.relocs), exe.cs, exe.ip, exe.ss, exe.sp))
        print('overlay area %06x bytes, %d segments, %d overlaid'
              % (exe.ovr_size, len(exe.segments), len(exe.stubs)))
        kinds = {0: 'data', 1: 'code', 3: 'ovr-stub', 4: 'data/bss'}
        for i, (seg, size, flags, minoff) in enumerate(exe.segments):
            extra = ''
            if flags == SEG_STUB:
                st = next(s for s in exe.stubs if s['index'] == i)
                extra = 'code %04x fixups %d entries %d' % (
                    len(st['code']), len(st['fixups']), len(st['entries']))
            print('%3d %04x size %04x %-8s %s' % (i, seg, size, kinds.get(flags, flags), extra))
        return
    flat, segmap = exe.flatten()
    open(argv[3], 'wb').write(flat)
    if len(argv) > 4:
        json.dump(segmap, open(argv[4], 'w'), indent=1)
    print('wrote %s (%d bytes, %d overlays inlined, %d far pointers redirected, '
          '%d FPU-emulation opcodes decoded)'
          % (argv[3], len(flat), len(segmap['overlays']), segmap['redirected'],
             segmap['fpu_rewrites']))


if __name__ == '__main__':
    main(sys.argv)
