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
| `equipment.txt` | `param/item_wpn.prm`, `param/item_pct.prm`, `param/item_nam.prm` | weight, length, attack and defence of the equipment |
| `maps.txt` | `NNN.map` | the maps each map's neighbour table leads to |
| `ini/*.ini` | `ini/*.ini` | the developers' debug presets, as UTF-8 |
| `signs/*.png` | `msg/*.tm2` | the signs and inscriptions of the maps |

Keep that output out of commits and out of pull request text. A name taken from it as an identifier
(`ITEM_FIERY_SWORD`) or a few words in a comment are fine; lists of names, descriptions and dialogue
are the game's content, not ours.

## The archive: `DATA/KF4.DAT`

One file holds all the game's data. The game opens files in it by name (`core/file.c`).

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

`Message_Format` and `Message_DrawGlyphs` in `ui/message.c` are the code that reads it.

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
250, gloves at 300; unused numbers are named "None". Byte 0x33 of a record is the item's weight in
tenths of a kilogram.

## `param/magic.prm`: spells

256 records of 0x110 bytes (the size of one entry of `D_00402DA8`). Each starts with the spell's
name in the text code. Most names appear in three consecutive records.

## `param/item_wpn.prm`: weapons

70 records of 0x90 bytes; record *k* is item 150 + *k*.

| offset | size | |
| --- | --- | --- |
| 0x00 | 2 × 9 | attack at weapon level 1, signed, per kind: slash, hit, stab, fire, earth, wind, water, light, dark |
| 0x12 | 2 × 9 | the same at weapon level 2 |
| 0x24 | 2 × 9 | the same at weapon level 3 |
| 0x68 | 2 | length in centimetres |

## `param/item_pct.prm`: armour

160 records of 0x40 bytes; record *k* is item 220 + *k* (shields, helmets, armour, gloves, boots,
rings, bracelets, amulets). Offset 0x00 holds nine signed defence values in the same order as a
weapon's attack.

Both tables were confirmed against the item tables of Prima's official strategy guide (2004), which
gives weight in pounds (kilograms × 2.2), length in feet (centimetres ÷ 30.48) and the level-1 values:
55 of its 56 weapon rows agree in every column, and 89 of its 103 armour rows. Where they differ the
guide's printed table is at fault (rows shifted against their names, or one Knight Sword at 0.6 times
its values).

## `chr/cNNNN.chr`: models

A `chr.prm` record names its model in its first 16 bits: `entity/entity_pool.c` loads
`chr/c%4.4d.chr` with that number. Most records use the file of their own number; variants share
another's (records 19 and 152, both Beetles, use `c0011`; the Widda, record 160, uses `c0044`), and
some scripted records borrow a character's model (record 169 uses Harmurah's, record 170 the Earth
King's). [ToolsForKFIV](https://github.com/TheStolenBattenberg/ToolsForKFIV) shows the models.

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

Text files (Shift-JIS, comments in Japanese) that set up a game for testing. `core/ini.c` reads them.
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
| 0x00 | 2 | model: the number of its `chr/cNNNN.chr` file, 0xFFFF none |
| 0x02 | 1 | entity type: the index into `D_003E6E40`, `entity->params->unk02`, and so which `enemy/*.c` drives it |
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
`enemy/*.c` files come from; each file's comment lists its creatures. Six of the wiki's experience
values differ from the game's (the game's are in the records). Thirteen spawned records match no
creature on the list, mostly scripted things with no experience or 65,535 hit points.

The wiki's table is the bestiary of Prima's official strategy guide (2004): its names and numbers are
the guide's. Three of the six differing experience values are the guide's own errors (Lava Face,
Dark Succubus, Greater Demon: a factor of ten each) and three are errors in copying it. The guide also
names one creature the wiki lacks: the Ancient King, record 57 (`enemy/ancient_king.c`), whose
description matches the record's resistances and its code. The last boss, record 99 (`enemy/dark_one.c`), is the
Dark One of the game's own text; the guide's account of the fight is the code's.
Three more types are props of the King's Throne maps, placed by the guide's walkthrough and their
models: the eye pedestals (type 31, record 91), the bell (type 38, record 60) and the skeleton with a
hammer beside it (type 37, record 154, named for its look only). Prima's "Beetle" is the segmented creature
of records 11, 19 and 152, all drawn with model `c0011`.

## Areas

An area's name reaches the code only through its save point (map object kind 20): the save point
stores the area's message id in `GameStatus.save_area` for the save list. That ties the 20 maps that
have a save point to their area names; the other maps have none to go by.

## Maps

Each map file has a table of its neighbours: 32 entries of 16 bytes (an x, y, z offset, then at 0x0C
the number of the map) right after the flag blocks (0x17C0 + 0x80 × record count + 0x40 × flag block
count, the counts at 0x3C and 0x3E of the header). Byte 0x3A of a map record (`MapRecordInfo.unk3A`)
picks the entry whose map `func_00117CC8` loads in the background while the player stands on that
record, 0xFF none. These links are how the world joins up without loading screens.

The maps of Prima's guide have numbers of their own (1 to 64, in the order of its walkthrough). Its
"To Map" labels and the people, bosses and save points drawn on them, laid over these links and the
spawn records, place every one of its maps:

| map | Prima | | placed by |
| --- | --- | --- | --- |
| 000 | 2 | Village Ruins | save point, Duhrin, links |
| 002 | 6 | Miner's Cemetery | Duhrin, links |
| 004 | 3 | East Valley | Lynn and Sharyl |
| 005 | 9 | Catacombs (Giant Spider's Lair) | Giant Spider, Lee Maynor, Mista Fopa |
| 006 | 23 | The King's Throne, Upper | Ancient King, save point |
| 007 | 24 | The King's Throne, Lower | links |
| 008 | 7 | Upper Mines | links |
| 009 | 8 | Lower Mines | links |
| 011 | 14 | The Armory | the dog |
| 012 | 13 | The Ancient City Exterior | links |
| 013 | 21 | The Holy Forest | Aeaine, Forest Golem, save point |
| 014 | 22 | The Heart of the Forest | links |
| 015 | 15 | The Ancient City, Level 3, West | Ramirez, save point |
| 016 | 27 | South Cliffs | links |
| 017, 018, 019 | 29 | The Forge | Earth King (018), save points; three maps with the same links, one place |
| 020 | 28 | The Earth Folks' Village | Harmurah, save point |
| 021 | 43 | The Ocean Fortress | links |
| 022 | 45 | The Queen's Palace | Akryal |
| 023 | 47 | The Widda Queen's Lair | Widda Queen, save point |
| 024 | 30 | The Cavern of Lava | links |
| 025 | 31 | The Fire Lord's Domain | Fire Lord, Fire Genie |
| 026 | 41 | Cliff Passage | Lyle, John and Joshua |
| 027 | 42 | Ocean Shore | save point |
| 028 | 44 | Seabed Cave | Paul |
| 029 | 26 | The Underground Canal | links |
| 030 | 57 | The Passage of Light | Sentinels |
| 031 | 18 | North Cliffs | Kyle Campbell |
| 032 | 48 | The Egg Mines | Oagh, save point |
| 033 | 54 | The Mansion of Howling Winds, Exterior | links, the Giant |
| 034 | 55 | The Mansion of Howling Winds, Interior | links, the Giant |
| 035 | 59 | The Guardian's Room | links |
| 036 | 61 | The Underground Jail | Dark Knight |
| 037 | 60 | The Old Battlefield | save point |
| 038 | 62 | The Dark Sanctuary | Dark Succubus |
| 039 | 63 | The Dark Reality, Entrance | links; the mutants the walkthrough meets first |
| 040 | 64 | The Dark Reality, Conclusion | links |
| 041 | 25 | The King's Throne, Alcoves | links |
| 042 | 58 | The Sanctuary of Light | links |
| 043 | 46 | The Queen's Palace, Catacombs | links |
| 044 | 49 | The Snake Dens | links |
| 095 | 10, 11 | Upper and Lower Underground Graveyard | links; one map, two floors |
| 096 | 5 | The Shop | links |
| 097 | 12 | Icon of Healing | Mista Fopa |
| 098 | 4 | Palace of Oath Interior | Chad |
| 099 | 1 | Palace of Oath Exterior | the start, save point |
| 100 | 16 | The Ancient City, Level 3, Central Tower | Chad, save point |
| 101 | 17 | The Ancient City, Level 3, North | links |
| 102 | 19 | The Ancient City, Level 3, East | links |
| 103 | 20 | The Ancient City, Level 3, East Catacombs | links |
| 104 | 35 | The Ancient City, Level 3, South, Craftsman's Room | Belric |
| 105 | 32 | The Ancient City, Level 3, South | Shane Floyd |
| 106 | 33, 34 | The Ancient City, Level 3, South, Widda Areas | links; one map |
| 107 | 37 | The Ancient City, Level 2, Central Tower | links, save point |
| 108 | 36 | The Ancient City, Level 2, South | links, save point |
| 109 | 38 | The Ancient City, Level 2, West | JaMarc, Giant Beetle |
| 110 | 39 | The Ancient City, Level 2, West, Sorcerer's Area | Dark Sorcerer |
| 111 | 40 | The Ancient City, Level 2, North | save point |
| 112 | 52 | The Ancient City, Level 1, Central Tower | links, save point |
| 113 | 50 | The Ancient City, Level 1, West | links |
| 114 | 51 | The Ruins of the Dark Folk | links |
| 115 | 53 | The Ancient City, Level 1 and 2, East | Chad, save point |
| 116 | 56 | The Treasury | links |

Two save points name their area differently from the guide: 102's says Level 3, South and 113's says
Graveyard. The links leave no other place for either map.
