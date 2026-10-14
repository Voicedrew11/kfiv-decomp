#!/usr/bin/env python3
"""Decode the readable parts of the game's data archive from the user's disc.

    tools/extract_gamedata.py [path/to/disc.iso]

Reads DATA/KF4.DAT from the disc image (default: baserom/kfiv_us.iso) and writes, under
build/gamedata/ (which git ignores, like everything under build/):

  files.txt     the archive's file table: offset, size, name
  text.txt      every message of font.txc: the id the code uses for it, then the text
  items.txt     the 380 item names of param/item_nam.prm, by item number
  spells.txt    the names in param/magic.prm, by record number
  equipment.txt weight, length and attack (weapons) or defence (armour) of every item from 150 on
  maps.txt      for each NNN.map, the maps its neighbour table leads to
  ini/*.ini     the developers' debug presets, converted from Shift-JIS to UTF-8
  signs/*.png   the sign and inscription textures of msg/*.tm2

The formats are described in docs/GAME_DATA.md. Everything this writes is the game's own content:
keep it out of commits.
"""
import struct
import sys
import zlib
from pathlib import Path

import pycdlib

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "build" / "gamedata"
ARCHIVE = "/DATA/KF4.DAT;1"
TABLE_START = 0x40
ENTRY_SIZE = 64
NAME_SIZE = 52
TEXT_INDEX_ENTRIES = 0x2000
TEXT_END = 0xFFFF
TEXT_LINE_BREAK = 0x8000
ITEM_COUNT = 380
ITEM_RECORD_SIZE = 64
SPELL_RECORD_SIZE = 0x110
ITEM_WEIGHT = 0x33        # in item_nam.prm: weight in 0.1 kg
FIRST_WEAPON = 150
WEAPON_RECORD_SIZE = 0x90  # item_wpn.prm, from item 150
WEAPON_LENGTH = 0x68       # in cm
FIRST_ARMOUR = 220
ARMOUR_RECORD_SIZE = 0x40  # item_pct.prm, from item 220
KINDS = ("slash", "hit", "stab", "fire", "earth", "wind", "water", "light", "dark")
MAP_RECORDS = 0x17C0
MAP_RECORD_SIZE = 0x80
MAP_FLAG_BLOCK_SIZE = 0x40
MAP_NEIGHBOURS = 32        # 16-byte entries: x, y, z offset, u16 map at 0x0C
PSMT8 = 0x13
PSMT4 = 0x14


def decode_text(data: bytes, offset: int) -> str:
    """A string of 16-bit codes up to 0xFFFF: page 0x08 is ASCII less 0x20, 0x8000 a line break."""
    out = []
    while offset + 1 < len(data):
        (code,) = struct.unpack_from("<H", data, offset)
        offset += 2
        if code == TEXT_END:
            break
        if code >> 8 == 0x08:
            out.append(chr((code & 0xFF) + 0x20))
        elif code == TEXT_LINE_BREAK:
            out.append(" / ")
        elif code == 0:
            out.append(" ")
        else:
            out.append(f"<{code:04X}>")
    return " ".join("".join(out).split())


def png(width: int, height: int, rgb: bytes) -> bytes:
    def chunk(kind: bytes, body: bytes) -> bytes:
        return struct.pack(">I", len(body)) + kind + body + struct.pack(">I", zlib.crc32(kind + body))

    rows = b"".join(b"\0" + rgb[y * width * 3:(y + 1) * width * 3] for y in range(height))
    return (b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(rows, 6)) + chunk(b"IEND", b""))


def decode_sign(data: bytes):
    """msg/*.tm2: pixel byte count at 4, a GS TEX0 value at 0x10, pixels at 0x20, then the CLUT.
    Returns (width, height, rgb) with the texture drawn over black, or None for another format."""
    (pixel_bytes,) = struct.unpack_from("<I", data, 4)
    (tex0,) = struct.unpack_from("<Q", data, 0x10)
    psm = (tex0 >> 20) & 0x3F
    width = 1 << ((tex0 >> 26) & 0xF)
    height = 1 << ((tex0 >> 30) & 0xF)
    pixels = data[0x20:0x20 + pixel_bytes]
    clut = data[0x20 + pixel_bytes:]

    def colour(entry: int) -> bytes:
        red, green, blue, alpha = clut[entry * 4:entry * 4 + 4]
        alpha = min(255, alpha * 2)  # the GS's 0x80 is opaque
        return bytes((red * alpha // 255, green * alpha // 255, blue * alpha // 255))

    if psm == PSMT8:
        # the GS stores 256-colour tables with entries 8-15 and 16-23 of every 32 swapped
        palette = [colour((i & 0xE7) | ((i & 8) << 1) | ((i & 16) >> 1)) for i in range(256)]
        indices = pixels[:width * height]
    elif psm == PSMT4:
        palette = [colour(i) for i in range(16)]
        indices = bytes(nibble for byte in pixels[:width * height // 2] for nibble in (byte & 15, byte >> 4))
    else:
        return None
    return width, height, b"".join(palette[i] for i in indices)


def main() -> int:
    iso_path = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "baserom" / "kfiv_us.iso"
    if not iso_path.exists():
        print(f"no disc image at {iso_path}", file=sys.stderr)
        return 1
    iso = pycdlib.PyCdlib()
    iso.open(str(iso_path))
    try:
        with iso.open_file_from_iso(iso_path=ARCHIVE) as archive:
            head = archive.read(TABLE_START)
            (count,) = struct.unpack_from("<I", head, 4)
            table = archive.read(count * ENTRY_SIZE)
            files = {}
            for i in range(count):
                entry = table[i * ENTRY_SIZE:(i + 1) * ENTRY_SIZE]
                name = entry[:NAME_SIZE].split(b"\0")[0].decode("ascii")
                size, _padded, offset = struct.unpack_from("<III", entry, NAME_SIZE)
                files[name] = (offset, size)

            def read(name: str) -> bytes:
                offset, size = files[name]
                archive.seek(offset)
                return archive.read(size)

            OUT.mkdir(parents=True, exist_ok=True)
            (OUT / "files.txt").write_text(
                "".join(f"{offset:08X} {size:9d} {name}\n" for name, (offset, size) in files.items()), encoding="utf-8")

            text = read("font.txc")
            index = struct.unpack_from(f"<{TEXT_INDEX_ENTRIES}I", text, 0)
            # the message with id n is entry n + 1 (Message_GetText); entry 0 holds the table's size
            lines = [f"{number - 1:4d}  {decode_text(text, offset)}\n"
                     for number, offset in enumerate(index) if number and offset]
            (OUT / "text.txt").write_text("".join(lines), encoding="utf-8")

            names = read("param/item_nam.prm")
            (OUT / "items.txt").write_text(
                "".join(f"{i:3d}  {decode_text(names, i * ITEM_RECORD_SIZE)}\n" for i in range(ITEM_COUNT)), encoding="utf-8")

            spells = read("param/magic.prm")
            (OUT / "spells.txt").write_text(
                "".join(f"{i:3d}  {decode_text(spells, i * SPELL_RECORD_SIZE)}\n"
                        for i in range(len(spells) // SPELL_RECORD_SIZE)), encoding="utf-8")

            weapons = read("param/item_wpn.prm")
            armour = read("param/item_pct.prm")
            rows = [f"item  name  weight(kg)  length(cm) / {' '.join(KINDS)}: attack at weapon levels 1, 2, 3; defence\n"]
            for item in range(FIRST_WEAPON, ITEM_COUNT):
                name = decode_text(names, item * ITEM_RECORD_SIZE)
                weight = names[item * ITEM_RECORD_SIZE + ITEM_WEIGHT] / 10
                if item < FIRST_ARMOUR:
                    base = (item - FIRST_WEAPON) * WEAPON_RECORD_SIZE
                    (length,) = struct.unpack_from("<h", weapons, base + WEAPON_LENGTH)
                    levels = [struct.unpack_from("<9h", weapons, base + level * 18) for level in range(3)]
                    stats = " | ".join(" ".join(f"{v:3d}" for v in level) for level in levels)
                    rows.append(f"{item:3d}  {name:<20} {weight:4.1f} {length:4d}  {stats}\n")
                else:
                    base = (item - FIRST_ARMOUR) * ARMOUR_RECORD_SIZE
                    stats = " ".join(f"{v:3d}" for v in struct.unpack_from("<9h", armour, base))
                    rows.append(f"{item:3d}  {name:<20} {weight:4.1f}       {stats}\n")
            (OUT / "equipment.txt").write_text("".join(rows), encoding="utf-8")

            links = []
            for name in sorted(n for n in files if n.endswith(".map") and "/" not in n):
                data = read(name)
                records, flag_blocks = struct.unpack_from("<HH", data, 0x3C)
                table = MAP_RECORDS + records * MAP_RECORD_SIZE + flag_blocks * MAP_FLAG_BLOCK_SIZE
                used = {data[MAP_RECORDS + r * MAP_RECORD_SIZE + 0x3A] for r in range(records)}
                neighbours = sorted({struct.unpack_from("<H", data, table + i * 16 + 12)[0]
                                     for i in used if i < MAP_NEIGHBOURS})
                links.append(f"{name[:3]}  " + " ".join(f"{n:03d}" for n in neighbours) + "\n")
            (OUT / "maps.txt").write_text("".join(links), encoding="utf-8")

            (OUT / "ini").mkdir(exist_ok=True)
            (OUT / "signs").mkdir(exist_ok=True)
            presets = signs = 0
            for name in files:
                if name.startswith("ini/"):
                    (OUT / name).write_text(read(name).decode("shift_jis", "replace"), encoding="utf-8")
                    presets += 1
                elif name.startswith("msg/") and name.endswith(".tm2"):
                    image = decode_sign(read(name))
                    if image is not None:
                        (OUT / "signs" / (name[4:-4] + ".png")).write_bytes(png(*image))
                        signs += 1
    finally:
        iso.close()
    print(f"{count} files in the archive; {len(lines)} messages, {ITEM_COUNT} items, "
          f"{len(spells) // SPELL_RECORD_SIZE} spell records, {len(links)} maps, {presets} presets, {signs} signs -> build/gamedata/")
    return 0


if __name__ == "__main__":
    sys.exit(main())
