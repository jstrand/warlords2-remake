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


def _modrm_len(image, i):
    """Length of a 16-bit-addressing ModRM (+ displacement) starting at i."""
    m = image[i]
    mod, rm = m >> 6, m & 7
    if mod == 0:
        return 3 if rm == 6 else 1
    if mod == 1:
        return 2
    if mod == 2:
        return 3
    return 1


def _insn_len(image, i, end):
    """Length of the 16-bit x86 instruction at i, understanding Borland's
    emulated-FPU encodings. Returns 1 for anything unrecognised so a sweep
    through data resynchronises quickly."""
    j = i
    opsize = 2
    while j < end and image[j] in (0x26, 0x2E, 0x36, 0x3E, 0x64, 0x65, 0x66, 0x67,
                                   0xF0, 0xF2, 0xF3):
        if image[j] == 0x66:
            opsize = 4
        j += 1
    if j + 1 >= end:
        return max(1, end - i)
    op = image[j]
    k = j + 1
    try:
        if op == 0xCD and 0x34 <= image[k] <= 0x3B:          # emulated D8..DF
            return k + 1 - i + _modrm_len(image, k + 1)
        if op == 0xCD and image[k] == 0x3C:                   # segment-override FPU
            return k + 2 - i + _modrm_len(image, k + 2)
        if op < 0x40:
            low = op & 7
            if op in (0x0F,):
                op2 = image[k]
                if 0x80 <= op2 <= 0x8F:
                    return k + 1 - i + opsize
                if op2 in (0xA0, 0xA1, 0xA8, 0xA9):
                    return k + 1 - i
                return k + 1 - i + _modrm_len(image, k + 1)
            if op in (0x26, 0x2E, 0x36, 0x3E):
                return 1
            if low <= 3:
                return k - i + _modrm_len(image, k)
            if low == 4:
                return k - i + 1
            if low == 5:
                return k - i + opsize
            return k - i
        if op < 0x62 or 0x6C <= op <= 0x6F or 0x90 <= op <= 0x99 or 0x9B <= op <= 0x9F:
            return k - i
        if op in (0x62, 0x63) or 0x84 <= op <= 0x8F or 0xD0 <= op <= 0xD3 or \
                0xD8 <= op <= 0xDF or op in (0xC4, 0xC5, 0xFE, 0xFF):
            return k - i + _modrm_len(image, k)
        if op == 0x68:
            return k - i + opsize
        if op == 0x69 or op == 0x81 or op == 0xC7:
            return k - i + _modrm_len(image, k) + opsize
        if op in (0x6A, 0xA8, 0xCD, 0xD4, 0xD5, 0xE4, 0xE5, 0xE6, 0xE7) or \
                0x70 <= op <= 0x7F or 0xB0 <= op <= 0xB7 or 0xE0 <= op <= 0xE3 or op == 0xEB:
            return k - i + 1
        if op in (0x6B, 0x80, 0x82, 0x83, 0xC0, 0xC1, 0xC6):
            return k - i + _modrm_len(image, k) + 1
        if op in (0x9A, 0xEA):
            return k - i + 2 + opsize
        if 0xA0 <= op <= 0xA3:
            return k - i + 2
        if 0xA4 <= op <= 0xA7 or 0xAA <= op <= 0xAF or op in (0xC3, 0xC9, 0xCB, 0xCC, 0xCE,
                                                              0xCF, 0xD6, 0xD7, 0xF1, 0xF4, 0xF5) \
                or 0xEC <= op <= 0xEF or 0xF8 <= op <= 0xFD:
            return k - i
        if op == 0xA9 or 0xB8 <= op <= 0xBF or op in (0xE8, 0xE9):
            return k - i + opsize
        if op in (0xC2, 0xCA):
            return k - i + 2
        if op == 0xC8:
            return k - i + 3
        if op in (0xF6, 0xF7):
            reg = (image[k] >> 3) & 7
            imm = (1 if op == 0xF6 else opsize) if reg in (0, 1) else 0
            return k - i + _modrm_len(image, k) + imm
    except IndexError:
        pass
    return 1


def decode_fpu_emulation(image, start, end):
    """Rewrite Borland's x87 emulator interrupts back into real FPU opcodes.

    INT 34h..3Bh  -> FWAIT; D8h..DFh        (same length, ModRM follows)
    INT 3Ch xx    -> seg prefix; D8h|xx&7   (xx top bits: 11 = ES)
    INT 3Dh       -> NOP; FWAIT

    Only sites on an instruction boundary are rewritten, found by a linear
    sweep with `_insn_len` from the start of the code range, so bytes like
    CD 34 inside `jmp cs:[bx+34CDh]` are left alone.
    """
    n = 0
    i = start
    while i < end - 2:
        if image[i] == 0xCD:
            v = image[i + 1]
            if 0x34 <= v <= 0x3B:
                length = _insn_len(image, i, end)
                image[i:i + 2] = bytes((0x9B, 0xD8 + v - 0x34))
                n += 1
                i += length
                continue
            if v == 0x3C and image[i + 2] >> 6 == 3:
                length = _insn_len(image, i, end)
                image[i:i + 3] = bytes((0x90, 0x26, 0xD8 | image[i + 2] & 7))
                n += 1
                i += length
                continue
            if v == 0x3D:
                image[i:i + 2] = b'\x90\x9b'
                n += 1
                i += 2
                continue
        i += _insn_len(image, i, end)
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
