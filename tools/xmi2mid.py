#!/usr/bin/env python3
# XMI (Miles XMIDI) -> Standard MIDI File, format 0.
# XMI time is 120 Hz ticks; we write PPQN 60 at 120 bpm (500000 us/qn) = 120 ticks/s.
# Note-ons carry their duration; we emit explicit note-offs. SysEx is kept (GS setup),
# XMI tempo metas are dropped (the delays already include tempo), AIL-private
# controllers 110-120 are dropped.
import struct, sys, os

def find_evnt(b):
    def walk(p, end):
        while p + 8 <= end:
            cid, ln = b[p:p+4], struct.unpack(">I", b[p+4:p+8])[0]
            if cid in (b"FORM", b"CAT "):
                r = walk(p + 12, p + 8 + ln)
                if r: return r
            elif cid == b"EVNT":
                return p + 8, ln
            p += 8 + ln + (ln & 1)
    return walk(0, len(b))

def convert(b):
    start, ln = find_evnt(b)
    p, end, t, order = start, start + ln, 0, 0
    ev = []  # (tick, prio, order, bytes)
    def vl():
        nonlocal p
        v = 0
        while True:
            c = b[p]; p += 1
            v = (v << 7) | (c & 0x7f)
            if c < 0x80: return v
    while p < end:
        c = b[p]
        if c < 0x80:
            t += c; p += 1; continue
        p += 1; hi = c & 0xf0; order += 1
        if c == 0xff:
            kind = b[p]; p += 1; n = vl(); data = b[p:p+n]; p += n
            if kind == 0x2f: break
            if kind != 0x51:
                ev.append((t, 1, order, bytes([0xff, kind]) + enc(n) + data))
        elif c in (0xf0, 0xf7):
            n = vl(); data = b[p:p+n]; p += n
            ev.append((t, 1, order, bytes([c]) + enc(n) + data))
        elif hi == 0x90:
            key, vel = b[p], b[p+1]; p += 2; dur = vl()
            ev.append((t, 1, order, bytes([c, key, vel])))
            ev.append((t + dur, 0, order, bytes([0x80 | (c & 15), key, 0x40])))
        elif hi in (0xc0, 0xd0):
            ev.append((t, 1, order, bytes([c, b[p]]))); p += 1
        else:
            a, d = b[p], b[p+1]; p += 2
            if hi == 0xb0 and 110 <= a <= 120: continue
            ev.append((t, 1, order, bytes([c, a, d])))
    ev.sort(key=lambda e: (e[0], e[1], e[2]))
    trk = bytearray(enc(0) + b"\xff\x51\x03\x07\xa1\x20")  # 500000 us/qn
    last = 0
    for tick, _, _, data in ev:
        trk += enc(tick - last) + data; last = tick
    trk += enc(0) + b"\xff\x2f\x00"
    return b"MThd" + struct.pack(">IHHH", 6, 0, 1, 60) + b"MTrk" + struct.pack(">I", len(trk)) + trk

def enc(v):
    out = [v & 0x7f]; v >>= 7
    while v: out.append((v & 0x7f) | 0x80); v >>= 7
    return bytes(reversed(out))

if __name__ == "__main__":
    # python3 tools/xmi2mid.py original/SOUND out/ R   -- every R*.XMI
    src, dst, prefix = sys.argv[1], sys.argv[2], sys.argv[3].upper()
    os.makedirs(dst, exist_ok=True)
    for f in sorted(os.listdir(src)):
        if f.upper().startswith(prefix) and f.upper().endswith(".XMI"):
            with open(os.path.join(src, f), "rb") as fh:
                mid = convert(fh.read())
            with open(os.path.join(dst, f[:-4] + ".mid"), "wb") as fh:
                fh.write(mid)
            print(f, "->", len(mid), "bytes")
