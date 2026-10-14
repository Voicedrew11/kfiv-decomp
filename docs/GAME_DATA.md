# The game's data files

Notes on the data archive of *King's Field: The Ancient City* (SLUS-20318), as far as they help with
naming: where the game keeps its text and names, and how to read them.

Nothing from the disc is in this repository. `tools/extract_gamedata.py` decodes the files described
here from your own disc image into `build/gamedata/`, which git ignores:

```sh
.venv/bin/python tools/extract_gamedata.py          # uses baserom/kfiv_us.iso
```

| output | from | what |
| --- | --- | --- |
| `files.txt` | the archive's table | offset, size and name of every file |
| `text.txt` | `font.txc` | every message, by the id the code uses for it |
| `items.txt` | `param/item_nam.prm` | the 380 item names, by item number |
| `spells.txt` | `param/magic.prm` | the spell names, by record number |
| `ini/*.ini` | `ini/*.ini` | the developers' debug presets, as UTF-8 |
| `signs/*.png` | `msg/*.tm2` | the signs and inscriptions of the maps |

Keep that output out of commits and out of pull request text. A name taken from it as an identifier
(`ITEM_FIERY_SWORD`) or a few words in a comment are fine; lists of names, descriptions and dialogue
are the game's content, not ours.

## The archive: `DATA/KF4.DAT`

One file holds all the game's data. The game opens files in it by name (`kf_file.c`).

| offset | size | |
| --- | --- | --- |
| 0x04 | 4 | number of files (2,001) |
| 0x40 | 64 × count | the file table |

Each table entry is 64 bytes:

| offset | size | |
| --- | --- | --- |
| 0x00 | 52 | name, NUL-padded, with `/` as separator (`param/chr.prm`, `menu/main_para.dat`) |
| 0x34 | 4 | size in bytes |
| 0x38 | 4 | size rounded up to the archive's block size |
| 0x3C | 4 | offset of the file in the archive |

What is in it, by name:

| files | count | |
| --- | --- | --- |
| `NNN.map` | 64 | the maps |
| `chr/cNNNN.chr` | 146 | character models, by number |
| `param/*.prm` | 8 | parameter tables: `chr`, `obj`, `item_nam`, `item_cmn`, `item_wpn`, `item_pct`, `magic`, `sfx` |
| `menu/...` | 1,216 | menu layouts (`*_para.dat`), textures and the item preview models |
| `msg/mNNNN.tm2` | 109 | signs and inscriptions |
| `ini/NNNN.ini` | 72 | debug presets |
| `snd/...` | 279 | music and sound banks |
| `item_arm/...`, `item_ms/...`, `item/...` | 75 | item models and textures |
| `title/...`, `map_view/...`, `sfx/...` | 28 | title screen, map viewer and effect files |
| `font.tmr`, `font.txc` | 2 | the font texture and the game's text |

## The text code

Text is stored as 16-bit little-endian codes, not as ASCII and not as images.

| code | meaning |
| --- | --- |
| `0x08xx` | a Latin character: `xx + 0x20` is its ASCII code (`0x082E` is `N`) |
| `0x8000` | line break |
| `0x0000` | padding |
| `0xFFFF` | end of the string |
| `0x00xx`–`0x04xx` | glyph numbers of the Japanese font; a few untranslated strings still use them |
| other | control codes inside a message |

`Message_Format` and `Message_DrawGlyphs` in `kf_message.c` are the code that reads it.

## `font.txc`: every message

`Message_Init` loads this file whole. It starts with a table of 0x2000 32-bit offsets; each points at
a string in the text code, from the start of the file, and 0 means unused. Entry 0 holds the table's
size (0x2000). The message with id *n* is at entry *n* + 1: `Message_GetText` reads word `id + 1`.

The id is what the code passes to `Message_OpenBox` and `Message_ShowNotice` and what the data
tables store; `text.txt` lists messages by id, so it says what a call such as
`Message_ShowNotice(..., MSG_NOTHING_HAPPENED, ...)` (id 10) shows. Some ranges are computed rather
than written out: an item's text is id 500 + the item number, a spell's 900 + the spell number, and
the Recollection screen's pages start at 3000, 50 ids per page.

The file has 1,280 messages. In order of their numbers they are: the system messages, the "key
used" messages and the names of chests and doors, the area names, the descriptions of items and
spells, the names of the things the player can examine in the maps (corpses, barrels, altars,
statues, gravestones), and then the dialogue. Each character's lines are followed by that
character's name and a one-line description, which is what the menu's Recollection screen shows.
The game has no such names for its ordinary enemies.

## `param/item_nam.prm`: item names

380 records of 64 bytes, one per item number: the name in the text code, ended by `0xFFFF`, then
other fields. The item number is the index used throughout the code (the 380 entries of
`D_004147F8`, the `item_NNN` keys of the presets): weapons start at 150, shields at 220, helmets at
250, gloves at 300; unused numbers are named "None".

## `param/magic.prm`: spells

256 records of 0x110 bytes (the size of one entry of `D_00402DA8`). Each starts with the spell's
name in the text code. Most names appear in three consecutive records.

## `param/chr.prm`: character parameters

The per-kind parameter blocks that `Entity.params` points at. It contains no text: nothing in the
data names an entity type.

## `msg/mNNNN.tm2`: signs and inscriptions

Textures with the text of a sign already drawn on them: place names with arrows, epitaphs, warnings.
Despite the extension they are not TIM2 files:

| offset | size | |
| --- | --- | --- |
| 0x00 | 4 | file size |
| 0x04 | 4 | size of the pixel data |
| 0x10 | 8 | the GS `TEX0` value to draw it with: pixel format in bits 20–25, log2 of width and height in bits 26–29 and 30–33 |
| 0x20 | | the pixels, then the colour table (4 bytes per colour, alpha 0x80 opaque) |

All are 512×256. 105 are 4 bits per pixel (`PSMT4`, low nibble first), 4 are 8 bits (`PSMT8`, with
the GS's usual swap of colour table entries 8–15 and 16–23 in every 32).

## `ini/NNNN.ini`: debug presets

Text files (Shift-JIS, comments in Japanese) that set up a game for testing. `kf_ini.c` reads them.
Most are titled with the area they start in and who they belong to. Their keys name the player state
they set:

- `hp`, `mp`, `power`, `magic`, `level`, `exp`, `gold`
- `equip_weapon`, `equip_magic`, `equip_shield`, `equip_helm`, `equip_armor`, `equip_guntlet`,
  `equip_boot`, `equip_ring`, `equip_bracelet`, `equip_amulet`, `equip_shortcut`
- `item_NNN` for each of the 380 items
- `magic_fire_N`, `magic_earth_N`, `magic_wind_N`, `magic_water_N`, `magic_holy_N`, `magic_dark_N`
- `debug_dash`, `debug_auto_recv`, `debug_magic_rapid_fire`

The comments beside the keys say what each value means, which settles the meaning of the fields the
ini reader fills.

## `param/chr.prm`: the fields found so far

1,120 bytes (0x450) per record; a map's spawn record names its record by number. Fields established
by tracing the code or by matching records against what players have measured:

| offset | size | |
| --- | --- | --- |
| 0x02 | 1 | entity type: the index into `D_003E6E40`, `entity->params->unk02`, and so which `kf_enemy_*.c` drives it |
| 0x0A | 2 | hit points (`Entity_ResetToSpawn` loads `Entity.unk2C` from it) |
| 0x0E | 2 | experience given on death (`Player_AddExperience`) |
| 0xD4–0xE4 | 2 each | signed resistance per attack kind: slash, hit, stab, fire, earth, wind, water, light, dark; below 0 is a weakness |
| 0x232, 0x234 | 2 each | items it may drop |

## Map spawn records

0x60 bytes each, after the map's object records.
The fields used so far: 0x30 the `chr.prm` record, 0x3C the spawn id, 0x40 the mode
(`EntitySpawnInfo.unk40`), 0x4E the talk script, 0x58 the item script.

## Characters

Every character the player can talk to is entity type 15, except one; type 81 is a character's dog.
They differ by their `chr.prm` record, and record 100 + *p* goes with page *p* of the Recollection
screen (messages 3000 + 50*p* onwards: descriptions, the character's lines, the name last).

## Monsters

The game names no monsters anywhere in its data. The community's creature list at
<https://www.cleanhit.wiki/KF4:Creatures> gives hit points, experience, weaknesses, resistances and
drops for 91 creatures. Matching those against the spawned `chr.prm` records places all 91: 74 by hit
points and experience alone, the rest by resistances and drops. That is where the names of the
`kf_enemy_*.c` files come from; each file's comment lists its creatures. Six of the wiki's experience
values differ from the game's (the game's are in the records). Thirteen spawned records match no
creature on the list, mostly scripted things with no experience or 65,535 hit points.

## Areas

An area's name reaches the code only through its save point (map object kind 20): the save point
stores the area's message id in `GameStatus.save_area` for the save list. That ties the 20 maps that
have a save point to their area names; the other maps have none to go by.
