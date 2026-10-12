#!/usr/bin/env python3
"""Place compiled library objects in the retail ELF, section by section.

    tools/lib_layout.py <objdir> [out.tsv]

<objdir> holds unlinked .o files (src/sce/ compiled with tools/compile_c.sh;
the path under <objdir> names the unit). For each object:

  .text      the whole section is searched for in the retail .text with the
             words that carry relocations masked out. A hit whose jal targets
             carry other names in symbol_addrs is rejected; of several hits,
             the one where the object's functions sit at their symbol_addrs
             addresses wins. A unique hit places it.
  data       .data/.rodata/.lit4/.sdata/.sbss/.bss are placed from the
             relocations that point into them: a %hi/%lo pair, a .word, or a
             gp-relative load (li.s, a small static) in the located .text/.data gives
             the absolute address, the object's in-place addend gives the
             offset, and their difference is the section base. All
             relocations must agree. Initialized sections are then compared
             byte for byte (relocated words masked). Sections nothing
             references fall back to a masked byte search: a unique hit (with
             named symbols at their addresses) places them, and failing that,
             the one hit between the same section of the objects before and
             after this one in .text order (all sections share one link order).

Prints one row per (object, section): unit, section, size, alignment, retail
address or '-', how it was placed, and whether the bytes match. Then lists,
per section, the placed units in address order with the gaps between them.
"""
import os
import re
import struct
import sys
from collections import defaultdict

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

R_32, R_26, R_HI16, R_LO16, R_GPREL16, R_LITERAL = 2, 4, 5, 6, 7, 8
SECS = [".text", ".data", ".rodata", ".lit4", ".sdata", ".sbss", ".bss"]
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def load_retail(path):
    with open(path, "rb") as f:
        e = ELFFile(f)
        out = {}
        for s in e.iter_sections():
            if s.name in SECS or s.name == ".vudata":
                data = s.data() if s["sh_type"] != "SHT_NOBITS" else b"\0" * s["sh_size"]
                out[s.name] = (s["sh_addr"], data)
        return out


def gp_value(e):
    """ri_gp_value, the last word of .reginfo: _gp in a linked ELF, and in an
    object the gp0 its gp-relative offsets are taken from."""
    s = e.get_section_by_name(".reginfo")
    return struct.unpack_from("<I", s.data(), 20)[0] if s else 0


def load_gp(path):
    with open(path, "rb") as f:
        return gp_value(ELFFile(f))


class Obj:
    def __init__(self, path, unit):
        self.unit = unit
        with open(path, "rb") as f:
            e = ELFFile(f)
            self.gp0 = gp_value(e)
            self.secs = {}      # name -> (index, size, align, bytes)
            idx = {}
            for i, s in enumerate(e.iter_sections()):
                if s.name in SECS and s["sh_size"]:
                    data = s.data() if s["sh_type"] != "SHT_NOBITS" else b"\0" * s["sh_size"]
                    self.secs[s.name] = (i, s["sh_size"], s["sh_addralign"], data)
                    idx[i] = s.name
            symtab = e.get_section_by_name(".symtab")
            syms = list(symtab.iter_symbols())
            # relocations: section name -> list of (offset, type, target section or None,
            # symbol value, symbol name)
            self.relocs = defaultdict(list)
            for s in e.iter_sections():
                if not isinstance(s, RelocationSection):
                    continue
                tgt = e.get_section(s["sh_info"]).name
                if tgt not in self.secs:
                    continue
                for r in s.iter_relocations():
                    sym = syms[r["r_info_sym"]]
                    shndx = sym["st_shndx"]
                    tsec = idx.get(shndx) if isinstance(shndx, int) else None
                    self.relocs[tgt].append((r["r_offset"], r["r_info_type"], tsec,
                                             sym["st_value"], sym.name))
            # globals in the data sections: (section, offset, name)
            self.datasyms = [(idx[s["st_shndx"]], s["st_value"], s.name) for s in syms
                             if s["st_info"]["bind"] == "STB_GLOBAL" and isinstance(s["st_shndx"], int)
                             and idx.get(s["st_shndx"]) in (".data", ".rodata", ".sdata")]
            self.funcs = sorted((sym["st_value"], sym.name) for sym in syms
                                if sym["st_info"]["type"] == "STT_FUNC" and
                                idx.get(sym["st_shndx"] if isinstance(sym["st_shndx"], int) else -1) == ".text")

    def mask(self, sec):
        """Per-word masks for `sec`: bits that the linker leaves alone."""
        _, size, _, _ = self.secs[sec]
        m = [0xFFFFFFFF] * (size // 4)
        for off, typ, *_ in self.relocs[sec]:
            if off // 4 < len(m):
                m[off // 4] &= {R_26: 0xFC000000, R_HI16: 0xFFFF0000,
                                R_LO16: 0xFFFF0000, R_GPREL16: 0xFFFF0000,
                                R_LITERAL: 0xFFFF0000}.get(typ, 0)
        return m


def load_symbols(generated=True):
    """symbol_addrs.txt (and, with `generated`, tools/carve.py's
    build/symbol_addrs.carve.txt) as (name -> addr, addr -> name)."""
    n2a, a2n = {}, {}
    files = ["config/symbol_addrs.txt"] + (["build/symbol_addrs.carve.txt"] if generated else [])
    lines = []
    for fn in files:
        if os.path.exists(os.path.join(ROOT, fn)):
            lines += [(fn, l) for l in open(os.path.join(ROOT, fn)).readlines()]
    named = set()   # addresses symbol_addrs.txt names
    for fn, line in lines:
        note = line.split("//", 1)[1] if "//" in line else ""
        line = line.split("//", 1)[0].strip().rstrip(";")
        if "=" in line:
            name, addr = (x.strip() for x in line.split("=", 1))
            if "ignore:True" in note:
                continue
            # a repeated static name has no single address: it names no call.
            # symbol_addrs.txt is read first; a static that shares its name
            # does not take the name from it
            if "allow_duplicated:True" not in note:
                n2a.setdefault(name, int(addr, 0))
            # a name symbol_addrs.txt gives an address beats the generated
            # one, which may be left over from before a rename
            if fn == files[0]:
                named.add(int(addr, 0))
            elif int(addr, 0) in named:
                continue
            a2n[int(addr, 0)] = name
    return n2a, a2n


def calls_agree(obj, base, rtext, rbase, syms):
    """Do the object's jal targets agree with the retail ones at `base`?

    Calls inside the object must land on the same offsets; calls out of it
    must not land on an address symbol_addrs.txt gives another name (masked
    search alone cannot tell wf_acos from wf_asin)."""
    n2a, a2n = syms
    data = obj.secs[".text"][3]
    for off, typ, tsec, sval, sname in obj.relocs[".text"]:
        if typ != R_26:
            continue
        ow = struct.unpack_from("<I", data, off)[0]
        rw = struct.unpack_from("<I", rtext, base - rbase + off)[0]
        target = ((rw & 0x3FFFFFF) << 2) | ((base + off) & 0xF0000000)
        if tsec == ".text":
            if target != base + sval + ((ow & 0x3FFFFFF) << 2):
                return False
        elif sname:
            if sname in n2a and n2a[sname] != target:
                return False
            if a2n.get(target, sname) != sname:
                return False
    return True


def words(b):
    return struct.unpack("<%dI" % (len(b) // 4), b[: len(b) // 4 * 4])


def search(hay_words, needle_words, mask):
    """Positions (word index) where needle matches hay under mask."""
    if not needle_words:
        return []
    # anchor on the first fully unmasked word
    anchor = next((i for i, m in enumerate(mask) if m == 0xFFFFFFFF), None)
    n = len(needle_words)
    if anchor is None:
        # every word carries a relocation (e.g. a gp store in a j delay slot): scan
        return [b for b in range(len(hay_words) - n + 1)
                if all((hay_words[b + i] ^ needle_words[i]) & mask[i] == 0 for i in range(n))]
    hits = []
    aw = needle_words[anchor]
    start = 0
    while True:
        try:
            p = hay_words.index(aw, start)
        except ValueError:
            break
        start = p + 1
        base = p - anchor
        if base < 0 or base + n > len(hay_words):
            continue
        if all((hay_words[base + i] ^ needle_words[i]) & mask[i] == 0 for i in range(n)):
            hits.append(base)
    return hits


def sext16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def bases_from(obj, sec, sec_addr, retail, gp):
    """Section bases implied by relocations in `sec` (placed at sec_addr)."""
    _, size, _, data = obj.secs[sec]
    rbase, rdata = retail[sec]
    out = defaultdict(set)    # target section -> {base}
    pending_hi = []
    for off, typ, tsec, sval, sname in sorted(obj.relocs[sec], key=lambda r: r[0]):
        if typ not in (R_32, R_HI16, R_LO16, R_GPREL16, R_LITERAL):
            continue
        ow = struct.unpack_from("<I", data, off)[0]
        roff = sec_addr - rbase + off
        rw = struct.unpack_from("<I", rdata, roff)[0]
        if typ == R_32 and tsec:
            out[tsec].add(rw - (ow + sval))
        elif typ in (R_GPREL16, R_LITERAL) and tsec:
            # li.s, or a small static (.sdata/.sbss): both words hold an offset
            # from their gp (the object's is gp0, set by the assembler)
            out[tsec].add(gp + sext16(rw) - (obj.gp0 + sext16(ow) + sval))
        elif typ == R_HI16:
            pending_hi.append((off, tsec, sval))
        elif typ == R_LO16:
            # pair with every pending HI16 of the same target (gcc may emit several)
            for hoff, htsec, hval in pending_hi:
                if htsec != tsec or hval != sval:
                    continue
                how = struct.unpack_from("<I", data, hoff)[0]
                hrw = struct.unpack_from("<I", rdata, sec_addr - rbase + hoff)[0]
                addend = ((how & 0xFFFF) << 16) + sext16(ow)
                absolute = ((hrw & 0xFFFF) << 16) + sext16(rw)
                if tsec:
                    out[tsec].add(absolute - (addend + sval))
            pending_hi = [h for h in pending_hi if not (h[1] == tsec and h[2] == sval)]
    return out


def layout(objdir):
    """Rows of (unit, section, size, align, addr or None, how, bytes_match)."""
    retail = load_retail(os.path.join(ROOT, "baserom/us/SLUS_203.18"))
    gp = load_gp(os.path.join(ROOT, "baserom/us/SLUS_203.18"))
    rwords = {s: words(d) for s, (a, d) in retail.items()}
    objs = []
    for root, _, files in os.walk(objdir):
        for fn in sorted(files):
            if fn.endswith(".o"):
                p = os.path.join(root, fn)
                objs.append(Obj(p, os.path.relpath(p, objdir)[:-2]))
    objs.sort(key=lambda o: o.unit)

    syms = load_symbols()
    place = {}   # (unit, sec) -> (addr, how)
    tbase, tdata = retail[".text"]
    for o in objs:
        if ".text" not in o.secs:
            continue
        _, size, _, data = o.secs[".text"]
        hits = search(rwords[".text"], words(data), o.mask(".text"))
        hits = [h for h in hits if calls_agree(o, tbase + h * 4, tdata, tbase, syms)]
        # identical code in two members (sceGsSetHalfOffset/2): the names in
        # symbol_addrs.txt say which copy is which
        # (a game function still named func_XXXXXXXX says its own address)
        named = [(v, n, syms[0].get(n) or int(n[5:], 16)) for v, n in o.funcs
                 if n in syms[0] or re.fullmatch(r"func_[0-9A-F]{8}", n)]
        if len(hits) > 1 and named:
            hits = [h for h in hits
                    if all(a == tbase + h * 4 + v for v, n, a in named)]
        if len(hits) == 1:
            place[(o.unit, ".text")] = (retail[".text"][0] + hits[0] * 4, "text-search")
        elif hits:
            place[(o.unit, ".text")] = (None, f"ambiguous x{len(hits)}")
    def reloc_pass():
        for o in objs:
            for sec in list(o.secs):
                got = place.get((o.unit, sec))
                if not got or got[0] is None or sec in (".sbss", ".bss"):
                    continue
                for tsec, bases in bases_from(o, sec, got[0], retail, gp).items():
                    if tsec == ".text" or tsec not in o.secs:
                        continue
                    key = (o.unit, tsec)
                    old = place.get(key)
                    if len(bases) == 1:
                        b = next(iter(bases))
                        if old and old[0] is not None and old[0] != b:
                            place[key] = (None, f"conflict {old[0]:#x}/{b:#x}")
                        elif not old or old[0] is None and not old[1].startswith("conflict"):
                            place[key] = (b, f"reloc<-{sec}")
                    else:
                        place[key] = (None, "conflict " + "/".join(f"{b:#x}" for b in sorted(bases)))

    def data_hits(o, sec):
        """Retail addresses where the section's bytes are (relocated words
        masked), aligned as the section is and agreeing with symbol names."""
        _, size, align, data = o.secs[sec]
        rbase, rdata = retail[sec]
        m = o.mask(sec)
        hits, start = [], 0
        probe = data[:4] if m and m[0] == 0xFFFFFFFF else None
        while True:
            if probe is not None:
                i = rdata.find(probe, start)
            else:
                i = start if start + size <= len(rdata) else -1
            if i < 0 or i + size > len(rdata):
                break
            start = i + 1
            if (rbase + i) % max(align, 1):
                continue
            seg = rdata[i:i + size]
            ok = all((struct.unpack_from("<I", seg, k * 4)[0] ^ struct.unpack_from("<I", data, k * 4)[0])
                     & m[k] == 0 for k in range(size // 4))
            if ok and seg[size // 4 * 4:] == data[size // 4 * 4:]:
                hits.append(rbase + i)
        named = [(v, n) for s, v, n in o.datasyms if s == sec and n in syms[0]]
        if named:
            hits = [h for h in hits if all(syms[0][n] == h + v for v, n in named)]
        return hits

    def search_pass():
        """Unreferenced initialized sections: a unique byte match places them."""
        for o in objs:
            for sec in o.secs:
                if sec in (".text", ".sbss", ".bss"):
                    continue
                got = place.get((o.unit, sec))
                if got and (got[0] is not None or got[1].startswith("conflict")):
                    continue
                hits = data_hits(o, sec)
                if len(hits) == 1:
                    place[(o.unit, sec)] = (hits[0], "data-search")
                elif hits:
                    place[(o.unit, sec)] = (None, f"ambiguous x{len(hits)}")

    def order_pass():
        """Still ambiguous or unplaced: the unit's section must lie between those
        of its neighbours in .text order (one link order for all sections)."""
        taddr = {o.unit: place[(o.unit, ".text")][0] for o in objs
                 if place.get((o.unit, ".text"), (None,))[0] is not None}
        for o in objs:
            if o.unit not in taddr:
                continue
            for sec in o.secs:
                got = place.get((o.unit, sec))
                if sec in (".text", ".sbss", ".bss") or (got and got[0] is not None):
                    continue
                if got and got[1].startswith("conflict"):
                    continue
                lo, hi = 0, 1 << 32
                for p in objs:
                    pa = place.get((p.unit, sec), (None,))[0]
                    if p.unit not in taddr or pa is None or p is o:
                        continue
                    if taddr[p.unit] < taddr[o.unit]:
                        lo = max(lo, pa + p.secs[sec][1])
                    else:
                        hi = min(hi, pa)
                size = o.secs[sec][1]
                hits = [h for h in data_hits(o, sec) if lo <= h and h + size <= hi]
                if len(hits) == 1:
                    place[(o.unit, sec)] = (hits[0], "order-search")

    for _ in range(3):
        reloc_pass()
        search_pass()
    order_pass()
    reloc_pass()

    rows = []
    for o in objs:
        for sec in SECS:
            if sec not in o.secs:
                continue
            _, size, align, data = o.secs[sec]
            addr, how = place.get((o.unit, sec), (None, "unplaced"))
            ok = "-"
            if addr is not None and sec not in (".sbss", ".bss"):
                rbase, rdata = retail[sec]
                seg = rdata[addr - rbase: addr - rbase + size]
                m = o.mask(sec)
                ok = "yes"
                for i in range(size // 4):
                    a = struct.unpack_from("<I", seg, i * 4)[0]
                    b = struct.unpack_from("<I", data, i * 4)[0]
                    if (a ^ b) & m[i]:
                        ok = "NO"
                        break
                if ok == "yes" and size % 4 and seg[size // 4 * 4:] != data[size // 4 * 4:]:
                    ok = "NO"
            rows.append((o.unit, sec, size, align, addr, how, ok))
    return rows


def main():
    objdir = sys.argv[1]
    out_tsv = sys.argv[2] if len(sys.argv) > 2 else None
    rows = layout(objdir)
    lines = ["unit\tsection\tsize\talign\taddr\thow\tbytes_match"]
    for u, s, size, al, addr, how, ok in rows:
        lines.append(f"{u}\t{s}\t{size:#x}\t{al}\t{addr:#010x}\t{how}\t{ok}" if addr is not None
                     else f"{u}\t{s}\t{size:#x}\t{al}\t-\t{how}\t{ok}")
    text = "\n".join(lines) + "\n"
    if out_tsv:
        open(out_tsv, "w").write(text)
    else:
        sys.stdout.write(text)

    # per-section layout with gaps
    for sec in SECS:
        placed = sorted((addr, size, u) for u, s, size, al, addr, how, ok in rows
                        if s == sec and addr is not None)
        if not placed:
            continue
        print(f"\n## {sec}: {len(placed)} placed", file=sys.stderr)
        prev_end = None
        for addr, size, u in placed:
            if prev_end is not None and addr != prev_end:
                print(f"   gap {prev_end:#010x}..{addr:#010x} ({addr - prev_end:#x})", file=sys.stderr)
            print(f"  {addr:#010x} {size:#8x} {u}", file=sys.stderr)
            prev_end = addr + size


if __name__ == "__main__":
    main()
