# King's Field IV (SLUS-20318) function index

What each game function does and, where one was proposed, a candidate name. It is a starting point for naming, not a decision: nothing here has been renamed in the source.

Compiled from the working reports the matching agents wrote for #40 and its leftovers (the "source" column gives the list each report belonged to; the reports themselves are not in the repository) and, where no report describes a function, from the comment above its definition in `src/game` (source "src comment", or the module file name). The rows were sampled against the reports, not proofread one by one. Functions matched in #30, #33 and #35 mostly have no description yet, because those rounds did not record one. Candidate names are proposals from the reports (or the SDK sample / the game's own debug strings); names marked "repo name" are the ones already in `config/symbol_addrs.txt`.

- Functions indexed: 2702 (2639 in our ranges 0x100000-0x18FFFF, 0x1A0000-0x1EFFFF, 0x200000-0x21FEE0; 63 outside them that a report describes)
- With a candidate name: 621
- With a description: 1612; in our ranges without one: 1090

## Global data the reports identified

| symbol | meaning | report |
|---|---|---|
| D_00413F80 | PlayerState (0x300 bytes): unkB4 HP, unk58 max HP, unkB8 / unk5C MP and max, unk4C gold, unk50 exp, unk54 level, unk3C..unk45 worn items; D_00414280 is last frame's copy | q10, q11, t03 |
| D_003BEDF0 | the two entity arrays, 0x80 Entity (0x180 bytes) each, one per map slot | o3 |
| D_0038CD20 | map slot array (two MapSlot, .tag is the map tag); D_00446770 = current slot index | o1, o2 |
| D_003A1A30 | per-map object table (stride 0xE000 per map) | g3 |
| D_00369830 | inventory: owned count per item number (99 at most, 1 for unique items); weapons' LevelRecord in D_003679A0, worn items' in D_003682F0 | q03, t03 |
| D_004147F8 | GameDefinition table (0x40-byte records): item names and model file numbers | q15 |
| D_00402DA8 | effect / magic table (0x110 bytes per id: hit descriptions, cost at 0xFC) | q02b, q12 |
| D_003F1750 / D_003F3550 | effect spawner slot pool (0x60 slots) / effect actor pool (0x180 actors, models at D_0054A260) | u02 |
| D_00402550 | effect kind handler table: [kind][0] start, [1] per-frame update | u02 |
| D_00413DA8 | current pad state: [0] held, [1] pressed; button masks D_0044692C..D_00446944 | q10b |
| D_00413F50 | player's item action handlers, called as handler(held, pressed) | q02 |
| D_0057A350 | ring of 32 hit/attack requests (Unk1B1470Request) built by the player's weapons | q02 |
| D_00446928 | g_dmgLassBossCnt (the game's own name): last-boss hit counter | q10 |
| D_00414580 | PlayerScreenTint: the colours laid over the screen | q02 |
| D_0057E240 / D_005B6100 | item nodes lying in the maps (fixed; dropped at D_00580040, object-held at D_005807C0) / gold piles (16 per map) | q06, q07 |
| D_004471D4 | pointer to the sound driver state (SoundState D_005BF380: 16 voices, 4 banks) | q08 |
| D_005C5940 | on-screen message ring (6 slots, text buffers at D_005BF8C8); message box D_005C58E8 | q09 |
| D_00446B20 / D_00446C70 / D_00446C94 | current menu screen handler / its step / menu number being run | q16, q23, q15b |
| D_00446DE8 | menu input word for the frame (0x10/0x40 up/down, 0x20/0x80 right/left, 0x4000 confirm, 0x2000 cancel) | t02, q21 |
| D_00426470 / D_00446C90 | the five menu model LinkNodes around the list cursor / head of the menu model load queue | q17 |
| D_0036A130 / D_00446A28 | save image buffer / play time | o1 |
| D_01DFB3B0 | script threads (24 ScriptThread of 0xC4C bytes); D_00446FD0 = current thread | t07 |
| D_01E0DB00 | PadSystem: eight PadSlot of 0x1C0 bytes | t11 |
| D_01E4F278 | memory card request state (McState) | t12, t18 |
| D_00437850 | movie player state (MovieThreadState; display at D_004378D0, picture ring D_00437838) | t13, t15 |

## Start-up, session reset and main loop (0x1000C8-0x102150)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001000C8 |  | Resets every game system for a new session; a loaded save is then applied on top. | src comment |
| 001001C0 |  | The game proper: places the player (new game) or resumes a loaded one, runs the main loop until GameStatus.bit9 is set, then waits for the sound loads to end. | src comment |
| 001005E8 |  | Draws one frame of the game: the world (once, or with a second view first for the modes 1 and 2), the screen tints, the overlays MapDrawState asks for, the fades and the pause shade. | src comment |
| 00100D10 |  | Finds the map record the camera is in and works out this frame's two camera shake offsets. | src comment |
| 00100FC8 |  | Moves and turns the camera for the current camera mode, then adds the two shake offsets. | src comment |
| 001012D8 |  | Builds the camera matrices: the projection, world to view, and world to screen for the given mode. | src comment |
| 00101618 |  | Runs the queued sound loads, one at a time. | src comment |
| 001017A8 |  | Opens the menu or screen GameStatus asks for (bits 2-5) once sound and map loading allow it (unmatched) | g1 |
| 00101C88 |  |  |  |
| 00101CE0 |  |  |  |
| 00101D48 |  | Draws the centred 0x280 x 0x1C0 logo sprite into both frame buffers, dimmed by the divisor. | src comment |
| 00101EF0 |  | Runs two passes of the frame setup, syncing after the first. | src comment |
| 00101F90 |  | Pause: when the pause button is pressed, holds the game until it is pressed again. | src comment |
| 001020F8 |  |  |  |
| 00102150 |  | Starts the disc drive in DVD mode. | src comment |

## KF4.DAT file layer (kf_file.c) (0x102178-0x102900)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00102178 |  | loads the KF4.DAT table of contents | kf_file.c |
| 00102218 |  |  |  |
| 00102228 |  |  |  |
| 00102230 |  |  |  |
| 00102238 |  |  |  |
| 00102258 |  |  |  |
| 00102278 | KF_File_Open |  | repo name |
| 001024B0 |  | closes a file | kf_file.c |
| 00102558 |  | closes every open file and unlocks the file layer | kf_file.c |
| 001025C0 |  | reads from a file | kf_file.c |
| 00102728 |  | seeks the drive to a file's next sector | kf_file.c |
| 00102790 |  | waits for the read in flight, reissuing it while the drive reports an error | kf_file.c |
| 00102900 |  | the size of a file | kf_file.c |

## Game module: options, event flags, save image, screen fade and GS screen effects (0x102930-0x108020)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00102930 |  | sets the options to their defaults | kf_game.c |
| 001029A8 |  | clears the state a new game starts from | kf_game.c |
| 001030B8 |  | marks what the player has equipped as found and tidies the spell tables | kf_game.c |
| 001034A8 |  | one of the 128 flags in D_0034C918 | kf_game.c |
| 001034F8 |  |  |  |
| 00103548 |  | the 4-bit counter of an entry on a map | kf_game.c |
| 001035A8 |  | adds one to the 4-bit counter of an entry on a map | kf_game.c |
| 00103648 |  |  |  |
| 00103668 |  |  |  |
| 00103698 |  |  |  |
| 001036B8 |  |  |  |
| 001036C8 |  |  |  |
| 001036E8 |  |  |  |
| 001037F8 |  | one of the area flags, numbered from 3000 | kf_game.c |
| 00103858 |  |  |  |
| 00103990 |  | moves an area's flags to another area | kf_game.c |
| 00103A18 |  | finds where an item was placed; returns whether it was | kf_game.c |
| 00103AE0 |  | the item recorded for a trigger, 0 if there is none | kf_game.c |
| 00103B38 |  | frees the record of a trigger; returns whether there was one | kf_game.c |
| 00103BA8 |  | records an item for a trigger in the first free record; returns whether there was room | kf_game.c |
| 00103C18 | KF_Save_Pack | The save writer: packs the whole game state into the save buffer D_0036A130 (SaveHeader magic 0x15400 + size, play time D_00446A28, map tag), one bit per GameDefinition for state 1, then one checksum byte per 0x80 bytes behind the data; calls func_0011CF88 first. (alt name: SaveDataWrite) | o1 |
| 00104EC8 |  | Unpacks the save buffer D_0036A130 into the game state (the reader twin of func_00103C18). (unmatched) | f1 |
| 001060C0 |  | checks the save image: the magic word and a byte sum per 128-byte block | kf_game.c |
| 00106178 |  | starts a fade from a colour to the picture over some frames | kf_game.c |
| 001061B0 |  | starts a fade from the picture to a colour over some frames | kf_game.c |
| 001061E8 |  | counts the fade down a frame | kf_game.c |
| 00106208 |  | draws the fade | kf_game.c |
| 00106318 |  | loads the screen matrix and projects the haze layer distances D_0038A158 to screen depths | kf_game.c |
| 00106520 |  | draws the D_00446744 small sprites whose screen depths are in D_0038A178 | kf_game.c |
| 001067E0 |  | draws the D_00446744 sprites in a strip, each at a column offset from the table D_0043E460 | kf_game.c |
| 00106980 |  | draws one sprite at the depth in D_0044673C; the pad's up and down buttons step that depth | kf_game.c |
| 00106C58 |  | draws a column of 10 gradient sprites, with the Z buffer off for the run | kf_game.c |
| 00106DC8 |  | draws two columns of 10 gradient sprites each, with the Z buffer off for the run | kf_game.c |
| 00106FB8 |  | draws one sprite from the screen rectangle D_0043E480 at the given alpha | kf_game.c |
| 00107270 |  | draws one sprite from the screen rectangle D_0043E480, textured from D_0043E490, at the given alpha | kf_game.c |
| 00107528 |  | draws the sight marker: a 20-strip sprite whose position follows the projected target | kf_game.c |
| 00107840 |  | draws the target marker for the nearest entity in front of the camera, if any | kf_game.c |
| 00107CD8 |  | draws a column of 10 grey sprites at the given brightness, scrolling across the screen | kf_game.c |
| 00107E70 |  | draws a column of 10 sprites in the given colour, scrolling across the screen | kf_game.c |
| 00108020 |  | steps the wobbling grid (laid out afresh when D_00446740 is set) and draws its cells, blended by blend | kf_game.c |

## Model motion playback (0x1086E8-0x1095D8)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001086E8 |  | Adds one motion to the model's channel and poses the parts on its first key (fixes up motion->vectors, sets model->part_count); returns the end of the motion (motion + motion->size). | g1 |
| 00108940 |  | Gives a model a fresh ModelChannel from the work cursor, sets model->motion_file and loads every motion of a MotionFile (calls func_001086E8 per motion, motion_count times); returns the end of the file. | g1 |
| 00108A08 |  | Switches the model to one of its loaded motions, at frame 0. | src comment |
| 00108A78 |  | Whether the model's motion has reached its last frame. | src comment |
| 00108AA0 |  | Whether the model's motion is at the given frame. | src comment |
| 00108AC8 |  | Plays one frame of the model's current motion: every part is posed between the two keys around the frame. | src comment |
| 00109098 |  | Switches the model to a motion and sets its speed. | src comment |
| 001090E0 |  |  |  |
| 00109100 |  | Hermite interpolation: hands the four basis weights for the ratio to func_00216CF0. | src comment |
| 00109178 | KF_Model_BlendToMotion | One frame of blending a pose into the first key: while ModelChannel.value != 0 every part moves 1/divisor of the way to the first key of the current motion; value drops by word per frame. | o1 |
| 00109410 |  |  |  |
| 00109430 |  | Uploads the five VU1 microprograms. | src comment |
| 001095D0 |  |  |  |
| 001095D8 |  |  |  |

## GS layer: packet buffers, screen sprites, texture upload and cache, draw list (0x1095E0-0x10B5D0)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001095E0 |  | Starts filling the other packet buffer. | kf_packet_buffer.c |
| 00109650 |  | Appends a packet (from packet to packet_end) to the chain of the buffer being filled. | kf_packet_buffer.c |
| 001096E8 |  | Ends the chain of the buffer being filled and sends it to the GS by DMA. | kf_packet_buffer.c |
| 00109800 |  | Waits for the GS to finish reading the last buffer sent. | kf_packet_buffer.c |
| 00109878 |  | Lays out the packet buffers in the packet area for the given size (0 = largest). | kf_packet_buffer.c |
| 00109990 |  | Takes quadwords from the packet buffer; once it overflows everything gets the start of the buffer. | kf_packet_buffer.c |
| 00109A38 |  | Returns the spare area's address and size. | kf_packet_buffer.c |
| 00109A50 |  | Starts a list of GS register writes. | src comment |
| 00109A80 |  | Adds a GS register write to the list. | src comment |
| 00109AD0 |  | Ends the list of GS register writes: fills in its DMA, VIF and GIF tags and queues it. | src comment |
| 00109C30 |  | Queues a GS local-to-local image transfer (BITBLTBUF, TRXPOS, TRXREG, TRXDIR). | src comment |
| 00109E18 |  |  |  |
| 00109E88 |  |  |  |
| 00109EB8 |  |  |  |
| 00109F10 |  |  |  |
| 00109F80 |  |  |  |
| 0010A008 |  |  |  |
| 0010A040 |  |  |  |
| 0010A0A0 |  |  |  |
| 0010A100 |  | Queue a textured screen sprite. | src comment |
| 0010A338 |  | Queue a flat-coloured (untextured) screen sprite. | src comment |
| 0010A4E8 |  | Cover the screen with a blended flat colour, in ten vertical strips. | src comment |
| 0010A5D8 |  |  |  |
| 0010A698 |  |  |  |
| 0010A718 |  |  |  |
| 0010A7E0 |  | Queue a packet carrying one 4x4 matrix. | src comment |
| 0010A880 |  | Scrolls a map/scene texture by its step (ScrollTexture, ScrollOffset) and redraws it into its target with a sprite. | f1 |
| 0010AC80 |  | Copy the displayed frame into a width x height texture at `fbp`. | src comment |
| 0010AE58 |  | Forgets every texture slot. | kf_model_textures.c |
| 0010AE90 |  | Sets texture slot arg1 (0..3) to arg0. | kf_model_textures.c |
| 0010AEB8 |  | Send the textures a model needs that are not the ones already loaded. | kf_model_textures.c |
| 0010B000 |  | Forgets every model in the draw lists. | kf_draw_list.c |
| 0010B018 |  | Put a model in the draw list bucket for its distance from the player. | kf_draw_list.c |
| 0010B140 |  | Draws the models of one draw list, from the farthest bucket to the nearest. | kf_draw_list.c |
| 0010B238 |  | Queue every chunk of a texture table for sending. | src comment |
| 0010B2E0 | KF_Texture_QueueTableMasked | Queues the chunks of a texture table (TextureTableHeader) like func_0010B238, but skips chunks switched off (unk0C) or in the skip mask (first 32 only). | o1 |
| 0010B410 |  | Queue one chunk of a texture table for sending. | src comment |
| 0010B4D8 |  |  |  |
| 0010B4E0 |  | Switch one chunk of a texture table on or off for func_0010B2E0. | src comment |
| 0010B510 |  |  |  |
| 0010B5D0 |  | Multiply the matrices of a part's children by the part's own matrix. | src comment |

## Models: instance set-up and drawing, vertex colours, triangle subdivision, texture-coordinate effects (0x10B758-0x112618)

| address | candidate name | what it does | source |
|---|---|---|---|
| 0010B758 | KF_Model_InitFromFile | (model, int *file_cursor, int *memory): sets a model instance up from a model file of the unposed kind (func_0010D018 is the posed one): carves parts and bone positions out of *memory, fills each part's data_count/data_addr, sets alpha 128.0f and handler addresses. | o1 |
| 0010BA30 |  | Draws a model (the draw function func_0010B758 installs): works out a turn from the block at unk148 and the model's rotation, queues a packet with the model's two matrices, its MapSlotUnk0C block, its lights and the vectors at unk160 and unk150, then for ... | src comment |
| 0010C180 |  | Draws a model like func_0010BA30, but in two passes with their own Z buffer state: first the parts with bit 41 of their flag word set, with Z writes on, then the others with Z writes off. | src comment |
| 0010C9E8 |  | Builds 32 transformed bone vectors from the actor source table. | src comment |
| 0010CAB8 |  |  |  |
| 0010CAE8 |  |  |  |
| 0010CB98 |  | Gives every vertex of a model drawn with its own texture the texture coordinates of the place it lands on the screen (0 to 1 across the screen). | src comment |
| 0010CE08 |  |  |  |
| 0010CF10 |  |  |  |
| 0010D018 |  | Sets a ModelInstance up from a "posed" model file (parts carry position/rotation/scale): carves the ModelPart array out of *memory, marks the first part's flagged data block, sets bone anchors/positions and the three handlers (func_0010D308 draw, func_0010E5A0, func_0010E678 get_point). | f2 |
| 0010D308 |  | Draws a model (ModelInstance.unk1A8 of the models func_0010D018 sets up): queues a packet with its two matrices, its MapSlotUnk0C block, its lights and, when it has one, its own colour, then for every part that is switched on a packet with the part's matrix ... | src comment |
| 0010D700 |  | Draws a model like func_0010D308, with the two matrices of func_0020CBC0 on the stack and each part's own matrix sent as it is; parts scaled to nothing are left out. | src comment |
| 0010DB40 |  | Draws a model like func_0010D308 (its own colour included), but as func_0010E010 does the parts: the first packet is queued at once, parts scaled to nothing are left out and each part gets its Z buffer and test state and its own packet. | src comment |
| 0010E010 |  | Draws a model: queues a packet with its two matrices, its MapSlotUnk0C block and its lights, then for every part that is switched on and not scaled to nothing the GS state and a packet with the part's matrix and a reference to each of its data blocks. | src comment |
| 0010E5A0 |  |  |  |
| 0010E678 |  | Where bone `index` of a model is; the model's own position when it has no bones or there is no such bone. | src comment |
| 0010E6A8 |  | Rewrite the 11-bit fields of one word in every record of every part's data. | src comment |
| 0010E770 |  | Set the doublewords at +0x30 and +0x40 of every data block of every part. When bits 37-50 of the second are not 0, the model's first block gets it with 2 in its top three bits. | src comment |
| 0010E820 |  | Set the doubleword at +0x50 of every data block of every part. | src comment |
| 0010E8A0 |  | Set the word that ends the first data block of every part. | src comment |
| 0010E8E0 |  | Set the colour of every vertex of a model; a component outside 0..0xFF is left as it is. | src comment |
| 0010EA08 | KF_Model_FadeVertexColours | Steps a model's vertex colours towards a target colour near a point (sphere); returns how many components are still moving. (alt name: ColorFadeSphere) | o1 |
| 0010ED80 |  |  |  |
| 0010EE98 |  | Looks for the triangles of a model that the GS could not draw as they are (a corner out of its range) but that are partly on the screen, and queues each cut up into smaller ones. | src comment |
| 0010F478 |  | Queues one triangle of a block, the one that ends at `vertex`, cut up into 16 smaller ones: the edge between its last two vertices is cut in four, and so is each line from the first vertex (the apex) to a point of that edge | src comment |
| 0010F898 |  | Writes steps + 1 vertices that lead from one vertex to another in equal steps; the position is stepped as floats and cut off, the rest as whole numbers. | src comment |
| 0010FC30 |  | Writes steps + 1 vertices that lead from one vertex to another in equal steps; the position is worked out afresh for each as floats and rounded, the rest is stepped as whole numbers. | src comment |
| 00110050 |  | Queues one triangle of a block, the one that ends at `vertex`, cut up into 16 smaller ones and with its vertices as floats (func_0010F478 does the same with packed vertices): the edge between its last two vertices is cut in four, and so is each line from the ... | src comment |
| 001108E8 |  | Writes steps + 1 float vertices that lead from one packed vertex to another in equal steps (func_00110D60 does the same between two float vertices). | src comment |
| 00110D60 |  | Writes steps + 1 vertices that lead from one vertex to another in equal steps (func_001108E8 does the same between two packed vertices). | src comment |
| 00111010 |  | Gives every vertex texture coordinates taken from where it lands on the screen. Also steps a 60-frame counter and fills a sine table that nothing reads. | src comment |
| 001112B0 |  | 0x471EE0 filled by func_001112B0 | kf_wobble.c |
| 00111368 |  | Draws a model with its own packets: every vertex where it is, with the texture coordinates of the screen position it would have if it were moved up or down by a sine picked by its x and z (so a texture that holds the screen comes out rippled). | kf_wobble.c |
| 00111BE0 |  | Queues a 12-quadword packet: a three-vertex primitive whose vertices all have 0x8000 at offset 0x0E, with the rest left for the caller to fill in. | kf_wobble.c |
| 00111C78 |  | Whether any data block of the model draws with a texture numbered from first to last. | kf_wobble.c |
| 00111D00 |  | Resets the 16 screen wobble oscillators, spreading their phases over one turn. | kf_wobble.c |
| 00111D58 |  | Advances each of the 16 entries' phase and recomputes its sine offsets. | kf_wobble.c |
| 00111E28 |  | Builds the model's table of wave texture coordinates at *alloc (16-byte aligned) and moves *alloc past it. Each vertex keeps its own coordinates and gets a wave picked from where it is. | kf_wobble.c |
| 00112018 |  | Moves every vertex's texture coordinates by its wave. | kf_wobble.c |
| 00112118 |  | Gives every vertex texture coordinates taken from where it lands on the screen, with each vertex shaken a little first. | src comment |
| 001123F8 |  |  |  |
| 00112400 | KF_Collision_RelocateMesh | Makes a loaded collision mesh usable: stores it in model->strips, turns the eight offsets after 0x18 into pointers and, when asked, the strips' point numbers into scratchpad addresses (COLLISION_WORLD_POINTS); returns the end of the mesh. | o2 |
| 001124E0 |  |  |  |
| 00112560 |  | Surface attribute: the first value of (0-7, 0xFF, 0xFF), the third of (0xFF, 0xFF, 0xFE-0xFF), 0 otherwise. | src comment |
| 001125A8 |  | Surface attribute: the first value when the triple is (0-7, 0xFF, 0xFF), 6 otherwise. | src comment |
| 001125D0 |  | Surface attribute: the third value when the triple is (0xFF, 0xFF, 0xFE-0xFF), 1 otherwise. | src comment |
| 001125F0 |  |  |  |
| 00112618 |  | Surface attribute: the second value when the triple is (0-7, 0-1, 0xFF), 0xFFFFFFFF otherwise. | src comment |

## Maps: data files, loading, map change, record drawing, per-map set-up (0x112648-0x11A430)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00112648 |  | Reads the six game data files, then works out each definition's scaled value. | src comment |
| 001127F8 |  |  |  |
| 001128E0 |  |  |  |
| 00112948 |  | Loads a map into slot 0 in one go, sets everything in it up and makes it the current map. | src comment |
| 00112BD0 |  | (unmatched) | f2 |
| 00114198 |  | Draws the first list of records of both map slots. mode 0 draws every record, the others only some; below 2 the screen is cleared first. | src comment |
| 00114528 |  | Draws the second list of records of both map slots, switching z writes on and off as the records ask. mode 0 draws every record, the others only some; below 2 the screen is cleared first. | src comment |
| 00114840 | KF_Map_LayOutSlot | Lays out map slot &D_0038CD20[map] for a loaded map file: clears the slot's objects and entities, points the slot at the parts of the file, shares out the slot's working memory, relocates two words of the block at header+unk28. | o2 |
| 00114AA8 |  | Builds the models of a map that was just read: one per record, then one per map object, with their collision meshes and motions. | src comment |
| 001152B8 |  | Makes the other map slot the current one: moves everything by the load's offset and switches the sound over. | src comment |
| 001154D8 |  |  |  |
| 00115550 |  | Moves a map record and its model by an offset. | src comment |
| 001155F8 |  | Puts the model of every record of a map where the record says, and builds its matrices. | src comment |
| 001157D8 |  | Inserts the node into one of two lists, keeping each sorted by box area. | src comment |
| 00115878 |  |  |  |
| 00115950 |  | Finds the box of the first list that holds the point (each side widened by margin). Failing that, the box nearest above or below it, then the nearest in z, then the nearest in x. | src comment |
| 00115CD0 |  | Finds the box of the first list that belongs to the map slot and holds the point (each side widened by margin). Failing that, the box nearest above or below it, then the nearest in z, then the nearest in x. | src comment |
| 001160B0 |  | Tells whether the point lies inside the node's box grown by margin on every side. | src comment |
| 00116160 |  | Returns prop prop_id of the map in the given slot, NULL when either is out of range. | src comment |
| 001161A8 |  | The free-camera map viewer: while GameStatus.bit7 is set it runs its own frame loop, in which the pad turns the camera about the player (with 0x2000 held) or moves it, and both 0x400 and 0x800 put it back. | src comment |
| 00116610 | DrawMapViewer | Map viewer draw (kf_map_viewer family): draws every record of both map slots with the viewer's own draw block D_0038CCA0 and the block at D_0038A1B0+0xA0, skipping records with bit 31 of flags set; then four grey quarter-screen sprites. The per-record block choice by flags & 0x10 is overwritten at once (dead). | g2 |
| 00116970 |  | Points both drawing contexts at the frame buffer with a 256x256 scissor and no offset bias. | src comment |
| 00116998 |  | Fills the 640x448 screen with grey, z writes off and with register 8 set to 5 meanwhile. | src comment |
| 00116AA0 |  |  |  |
| 00116AD8 |  | Queues packet index of the map slot for sending. | src comment |
| 00116B60 |  |  |  |
| 00116BD0 |  | Returns packet index of the map slot. | src comment |
| 00116C08 |  | Tests a flag bit in this slot's block, or looks it up among the 16 entries tagged with another slot. | src comment |
| 00116CB0 |  | Tests the corners of a map record's box against the screen: 1 when it has to be drawn whole (or the test is off), -1 when all eight corners are outside, 0 otherwise. The model is not used. | src comment |
| 00116F20 |  |  |  |
| 00116F90 |  |  |  |
| 00116F98 |  | Scrolls each of a map's moving textures of this mode in place: moves the pair that goes with it along, then copies the texture to the spare buffer at 0x3C40 and back in two pieces, shifted by the pair. | src comment |
| 001174D0 |  | Writes a GS packet that copies a rectangle from one buffer to another (BITBLTBUF, TRXPOS, TRXREG, TRXDIR, TEXFLUSH) and returns where the next packet goes. | src comment |
| 001175F0 |  |  |  |
| 001175F8 |  | Draws each of a map's moving textures of this mode into its target texture, shifted by the pair that goes with it, and moves the pair along. | src comment |
| 00117AA8 | CarryMapSlotPairs | CarryMapSlotPairs(from_slot, to_slot): copies D_00471FE0 pairs between the two map slots when the maps' tags are neighbours sharing scrolling textures: 4 and 0x63, 0x62 and 0x63, 0x15 and 0x1B (pairs 0..2, both directions), 0x1A to 0x1B (pairs 5..8 become 4..7) and back. Callers func_001152B8, func_00118680. | g2 |
| 00117C60 |  | Resets the background map load and the pending request. | src comment |
| 00117CC8 |  | Starts, queues and steps the background load of the map next to where the player stands. | src comment |
| 00117F98 |  | Finishes a background map load synchronously (state machine to step 5, then 6 to 0), switches the current slot, puts the player at the map's start position/heading, clears player status bit 34 and movement state 0x1AC-0x1BC, resets map-change request D_0038D640, frees the old slot. | f2 |
| 001181B8 |  | Runs the pending per-slot refresh for the slot opposite the current one, then steps the counter. | src comment |
| 00118270 |  | Map load step: waits for the timer to run out. | src comment |
| 001182A0 |  | Map load step: once the drive is free, opens the requested map file and picks the memory it loads into. | src comment |
| 00118458 |  | Map load step: reads the next 0x80000 bytes of the map file, closing it at the end. | src comment |
| 001184F8 |  | Map load step: once the read in flight is done, goes back to read the next piece. | src comment |
| 00118538 |  |  |  |
| 00118540 |  | Map load step: sets up the loaded map in the other slot, shifted so that it joins the current one. | src comment |
| 00118668 |  |  |  |
| 00118680 |  | Map load step: brings the other slot to life and ends the load. | src comment |
| 00118788 |  |  |  |
| 001187B0 |  |  |  |
| 001187D8 |  |  |  |
| 001187F0 |  | Start a transition to new_value unless one is running or it is already current. | src comment |
| 00118848 |  | Moves the player to position in the current map, or starts the change to another map. | src comment |
| 001188E0 |  | Starts the map change that entry (5 and up) of the table func_00103A18 reads leads to. | src comment |
| 00118958 |  | Runs a pending map change to the end, drawing its own frames meanwhile. | src comment |
| 00118A50 |  | Draws one frame with nothing in it. | src comment |
| 00118A98 |  | Map change drawing step: darkens the screen in ten strips, more each frame, until the timer runs out. | src comment |
| 00118C28 |  | Map change drawing step: keeps the screen black. | src comment |
| 00118CA8 |  | Draws one frame of a 30-frame fade and counts the timer down. | src comment |
| 00118D78 |  |  |  |
| 00118D80 |  | Map change step: lets the background load and everything else in flight settle. | src comment |
| 00118E20 |  | Map change step: frees everything both map slots hold and clears the map tables. | src comment |
| 00118F40 |  | Map change step: opens the new map file and starts reading all of it into the memory of slot 0. | src comment |
| 00119068 |  |  |  |
| 001190C8 |  | Map change step: sets the loaded map up in slot 0, puts the player in it and ends the change. | src comment |
| 00119340 |  |  |  |
| 00119360 |  | Runs the set-up function of the map in the slot, picked by the map's tag. | src comment |
| 00119520 |  |  |  |
| 00119590 |  | Sets map objects 0xA, 0xB and 0x22 and entity 0x17 up from the stage flags. | src comment |
| 00119730 |  | Sets flag 0x20000 of map object 0x65 once the stage flag is set. | src comment |
| 00119770 |  | Sets flag 0x20000 of map object 0x65 once the stage flag is set. | src comment |
| 001197B0 |  | Sets flag 0x20000 of map object 0x65 once the stage flag is set. | src comment |
| 001197F0 |  | Set-up of the map with tag 0x14 (called from func_00119360): stage flag D_0034B696 picks which of records 2, 0x33, 0x6D, 0x6E are shown/collide (records from func_00116160(map, id)); past stage 0 it also rewrites fog block 2 and light 4 of slot D_0038CD20[map]. | g3 |
| 00119A48 |  |  |  |
| 00119B20 |  | Shows the map records that go with the stage flag; at 2 also moves object 0x66 and spawns effect 0x1DF. | src comment |
| 00119D90 |  |  |  |
| 00119E10 |  |  |  |
| 00119E18 |  | Puts map record 5 at the height the stage keeps for it, 590 the first time. | src comment |
| 00119E90 |  | Once the stage flag is set, swaps map records 8-12 for 0x29-0x2D. | src comment |
| 0011A048 |  |  |  |
| 0011A0B8 |  |  |  |
| 0011A138 |  | Parks or reveals the pair of scene objects 0x66/0x67 depending on the stage flags. | src comment |
| 0011A210 |  |  |  |
| 0011A2B8 |  |  |  |
| 0011A370 |  |  |  |
| 0011A3E8 |  | Lowers the model of node 0x37 by 400 after func_001C8C80 has set it up. | src comment |
| 0011A428 |  |  |  |
| 0011A430 |  |  |  |

## Map viewer (0x11A438-0x11C6E8)

| address | candidate name | what it does | source |
|---|---|---|---|
| 0011A438 |  | 0x472080 the pointer model, viewer 4 only | kf_map_viewer.c |
| 0011A500 |  | Loads the picked viewer, runs it until it is left and frees what it loaded. | kf_map_viewer.c |
| 0011A598 |  |  |  |
| 0011AA08 |  |  |  |
| 0011AAE8 |  | Draws the picked viewer's picture and the player's marker on it: a turned, pulsing arrow where the position is known, a pulsing square at a fixed spot otherwise; then the picture arrows when the viewer has more than one picture. | kf_map_viewer.c |
| 0011B1C0 |  | Where the player's marker goes on the map viewer's picture viewer_picture: -1 for a fixed spot on the first picture, TRUE for a spot worked out from the player's position, FALSE for none. | kf_map_viewer.c |
| 0011B4F0 |  | Where the player's marker goes on the map viewer's picture viewer_picture (0 to 2): TRUE with a spot worked out from the player's position when the current map is on that picture, FALSE when it is not. | kf_map_viewer.c |
| 0011BAB8 |  | Map the player position to map-screen coordinates; FALSE if the area has no such map. | kf_map_viewer.c |
| 0011BBC0 |  | Turns the player's position into a point on the map picture of maps 0x1A and 0x1B; FALSE on other maps. | kf_map_viewer.c |
| 0011BCD0 |  | The free-camera viewer of func_0011C160's picture: runs its own frame loop, in which the pad turns the camera about the player (with 0x4000 held) or moves it, and both 0x400 and 0x800 put it back. | kf_map_viewer.c |
| 0011C678 |  | Whether a map object of this kind is one of the ordinary ones. | src comment |
| 0011C6E8 |  | Empties the map objects of both maps. | src comment |

## Map objects: set-up, update, draw and the object kinds (0x11C708-0x123000)

| address | candidate name | what it does | source |
|---|---|---|---|
| 0011C708 |  | Sets up the objects of a map once it is loaded: puts each model where its source says, works out how far the object reaches, picks the functions that draw it, runs the every-frame handler of its kind once, and threads the objects into the two draw lists of ... | src comment |
| 0011CC50 |  | Starts the objects of a map: their flags come from the flag block of the map when it has been visited, else from their sources; each is then put in use or handed to the handler of its kind in D_003BE4B0. | src comment |
| 0011CE80 |  | Unloads the objects of a map: saves four flags of each into the flag block of the map, stops their sounds, then empties the table. | src comment |
| 0011CF88 | KF_MapObject_SaveFlagsAll | Saves four flags (bits 16-19) of every object of both loaded maps (D_003A1A30[map]) into the maps' flag blocks: func_0011CE80's flag saving for both maps, without unloading. | g3 |
| 0011D0D0 |  | Calls each object of a map through its kind's handler in D_003BE430. | src comment |
| 0011D180 |  | Updates the map objects of both maps once a frame: remembers where each was, runs the step handler of its kind (D_003BDA30) and the kind's every-frame handler (D_003BE3B0), works out whether it is in view, and starts or stops the effect tied to it as it comes ... | src comment |
| 0011D600 |  |  |  |
| 0011D660 |  | Calls the map object kind's handler from D_003BE330 (object, value) | t08 |
| 0011D6B0 |  | Draws the map objects of both maps (or of one, when only_one is set): first the jointed ones (D_00447090), then the plain ones (D_00447088). | src comment |
| 0011E0C8 |  | Puts a map object in use: not started, fully opaque. | src comment |
| 0011E140 |  | Takes a map object out of use: ends its kind's state machine and clears its state. | src comment |
| 0011E1F0 |  | Looks a map object up by its object_id (1 to 255); 0 when the object_id is out of range. | src comment |
| 0011E228 |  | Breaks a breakable map object that has started: step 0xF, its breaking animation, an effect, a sound, and the object named by its source comes into use. | src comment |
| 0011E340 |  | Starts one of a map object's animations from its first frame. | src comment |
| 0011E3A0 | KF_MapObject_StepAnim | KF_MapObject_StepAnim(object): advances a map object's animation one frame, starts the frame-triggered sounds of the current animation (and of animation + 2 for animations 4 and 5), sets bit 3 of the flags, stops the looping sound when the animation ends; returns whether it is still running. | g3 |
| 0011E608 |  |  |  |
| 0011E678 |  | Marks the map object with this id when it is one of the kinds 3, 4 or 0x15 and its unk08 lies in the first 0x200 0x4000-byte blocks of its map's data. | src comment |
| 0011E708 |  | The turn that would face a map object toward a point (wrapped by func_0019B838). | src comment |
| 0011E758 |  | Sets bit 20 of a map object and resets its state bits (object, value) | t08 |
| 0011E7B0 |  | Restarts a waiting map object (model flag 1, bit 20) on another animation. | src comment |
| 0011E810 |  | Sets bit 21 of a waiting map object (model flag 1, bit 20). | src comment |
| 0011E858 | KF_MapObject_ClearSlot | (map): zeroes one map's two object counts (D_004467A8[map], D_004467B8[map]), the two draw-list heads (D_00447088[map], D_00447090[map]), the object table and the id lookup table. | f3 |
| 0011E8E8 |  |  |  |
| 0011E8F0 |  | Sets the GS Z-buffer, alpha and test registers for model, then hands it on. | src comment |
| 0011E9D0 |  | Sets the GS registers for drawing a map object's model, then draws it. | src comment |
| 0011EA70 |  |  |  |
| 0011EA78 |  |  |  |
| 0011EA80 |  | Stops the sound a map object is playing, if any. | src comment |
| 0011EAC0 | KF_ScrollTexture_InitAll | Sets up the four scrolling textures: clears and fills the ScrollTexture table D_00474AC0. | o2 |
| 0011ED18 |  |  |  |
| 0011EDC8 |  | Moves a map object and what its unk08 points at by an offset. | src comment |
| 0011EE18 |  | Start handler shared by several kinds: restarts the animation it is on. | src comment |
| 0011EE78 |  | Update handler: keeps a kind 0 object's sound playing while its map is the current one, and runs its animation when it is allowed to. | src comment |
| 0011EFC8 |  | Update handler: runs the animation, fading the model out over it when the definition asks for that; once it has ended the object goes out of use and the object named by its source comes into use. | src comment |
| 0011F0D8 |  |  |  |
| 0011F148 |  | Rebuilds a map object's model matrices from its position and rotation. | src comment |
| 0011F220 |  |  |  |
| 0011F228 |  |  |  |
| 0011F230 |  |  |  |
| 0011F238 |  | Start handler of the door-like kinds: closed (animation 4) or already open (animation 5). Kind 6 remembers where its second part sits and, when it starts open, is run until it gets there. | src comment |
| 0011F340 |  | Update handler of a closed door-like object: when the player uses it, it opens if it needs no item or the player holds the item, else a message says so. | src comment |
| 0011F498 |  | Update handler of an opening door-like object: runs the animation (kind 6 moves with its second part; kind 2 clears unk60 bit 0 once both pairs of its points are more than 225 apart); at its end the object is open. | src comment |
| 0011F708 |  |  |  |
| 0011F7E0 |  | Update handler of a closing door-like object: runs the animation (kind 6 moves with its second part); at its end the object is closed again. | src comment |
| 0011F910 |  | Opens a closed door-like object (step 1 to 2) or closes an open one (3 to 4). | src comment |
| 0011F980 |  | Rebuilds the model matrices of a kind 6 object with its second part put where it was when the object started, then puts the part back. | src comment |
| 0011FAA0 |  |  |  |
| 0011FAB0 |  | Update handler: when the player uses an object that holds an item it is handed out (func_001C9340), else a message is shown, at most every 45 frames. | src comment |
| 0011FBE8 |  | Update handler: func_0011FC18, then the fading-out update. | src comment |
| 0011FC10 |  |  |  |
| 0011FC18 |  |  |  |
| 0011FCF8 |  |  |  |
| 0011FDD0 |  | Update handler of a chest-like kind: used by the player, it opens (when it needs a key the player must hold it, else a message is shown) and hands out its item, or spawns what its third word says. | src comment |
| 0011FFF8 |  | Update handler: runs the opening animation; at its end the object is open (step 3) for 90 frames. Then the open-state update. | src comment |
| 00120058 |  | Update handler: when the player uses an object that still holds something it is handed out (func_001C9580), else message 0x46 is shown, at most every 45 frames. | src comment |
| 00120128 |  |  |  |
| 00120130 |  | Sets D_0038CC50 bit 1 and makes the event the current one while func_0011FDD0 runs. | src comment |
| 001201D8 |  |  |  |
| 001201E0 |  | Start handler of the kind that fades with the distance to the player: takes the two distances and the two alphas from its source and starts invisible. | src comment |
| 00120278 |  | Update handler of the fading kind: the alpha goes from near_alpha to far_alpha as the distance to the player goes from near_distance to far_distance. | src comment |
| 001203F0 |  | Rebuilds the model matrices of an object whose parts always face the camera: each part is put where it sits in the world and turned toward D_004325A0. | src comment |
| 00120668 |  | Start handler: animation 4, step 1. | src comment |
| 00120698 |  |  |  |
| 001206F0 |  |  |  |
| 00120778 |  |  |  |
| 001207D0 |  | Update handler of a switch that is off: used by the player, it starts turning on. | src comment |
| 00120810 |  | Update handler of a switch turning on: runs the animation, then it is on. | src comment |
| 00120868 |  | Update handler of a switch that is on: used by the player, it starts turning off. | src comment |
| 001208B0 |  | Update handler of a switch turning off: runs the animation, then it is off. | src comment |
| 00120908 |  |  |  |
| 00120A10 |  | Start handler of the light kind: makes a light of the colour in its source at a point of the model, and for the flickering sorts (0, 1) rolls the two flicker periods and where in them it starts. | src comment |
| 00120BB8 |  | Update handler of the light kind: the flickering sorts (0 faintly, 1 strongly) step their two periods and scale the colour by the product of the two waves read from the table D_0042C6F0 (0xA00 entries a period). | src comment |
| 00120D38 |  | End handler: releases the handle the kind keeps in its work area, if any. | src comment |
| 00120D78 |  |  |  |
| 00120D88 |  |  |  |
| 00120D90 |  | Start handler of the message kind: takes the message (the definition gives the default) and a flag from its source. | src comment |
| 00120DC8 |  | Update handler of the message kind at rest: used by the player, it goes to step 2 when its flag is set, else to step 3. | src comment |
| 00120E10 |  | Update handler of the message kind, step 2: back to rest once func_00208170 accepts the message. | src comment |
| 00120E48 |  | Update handler of the message kind, step 3: shows the message for 45 frames. | src comment |
| 00120EA0 |  | Update handler of the message kind, step 4: back to rest when the 45 frames are over. | src comment |
| 00120EC8 |  |  |  |
| 00120ED8 |  |  |  |
| 00120EE0 |  |  |  |
| 00120F48 |  | Update handler of a sliding kind, step 1: used by the player it only shows a message; once fired (func_00121620) it puts the part at a point 115 below the player and works out the steps that carry it back (6.67 a frame). | src comment |
| 001211B0 |  | Update handler of a sliding kind, step 2: moves the part one step; after the last one it is at step 3. | src comment |
| 00121228 |  |  |  |
| 001212B0 |  | Update handler of a sliding kind, step 3: used by the player, it gives back the item it holds and goes to step 5, unless the item is of type 5: then a message is shown, at most every 45 frames. | src comment |
| 00121398 |  | Update handler of a sliding kind, step 5: once the animation has ended, works out the steps that carry the part to a point 115 below the player (6.67 a frame). | src comment |
| 00121578 |  |  |  |
| 00121618 |  |  |  |
| 00121620 |  | Fires a resting object of this kind (step 1, not fired yet) when the item is the one it waits for; returns whether it fired. | src comment |
| 00121660 |  | Starts this kind of trigger: stopped, with its state cleared. | src comment |
| 00121680 |  | Update handler of the kind of func_00121660 (a platform): it rises while the player stands on it, after a wait of unk4E frames, and sinks back when he does not. unk46 and unk48 are the speeds up and down, or accelerations when unk4A and unk4C are set | src comment |
| 001217B0 |  |  |  |
| 001217C0 |  | Update handler: when the player (using it, status bit 36 clear) is within 75 in height, within the radius and inside the angle of one of its two points, func_001BD420 is called for that point. | src comment |
| 00121958 |  |  |  |
| 00121968 |  | Update handler of the shooting kind: every so many frames (or all the time when its model is animated), while it is on and the player is in range, it launches something from a point of the model, turned by two angles of its source. | src comment |
| 00121B10 |  | Sets or clears bit 17 of a map object. | src comment |
| 00121B38 |  | Start handler: off (animation 4, step 1) or on (animation 5, step 3), with the wait that goes with it. | src comment |
| 00121BA8 |  | Update handler, step 1: stepped on by the player, it turns on (unless its first wait is negative). | src comment |
| 00121BE8 |  |  |  |
| 00121C68 |  | Update handler, step 3: turns off again (unless its second wait is negative). | src comment |
| 00121CA0 |  | Update handler, step 4: waits, then runs the animation back; at its end the object is off again. | src comment |
| 00121D20 |  |  |  |
| 00121DA8 |  | Start handler: an object that starts on stays on only while func_00103AE0 still counts something for it. | src comment |
| 00121E78 |  | Update handler, step 1: used by the player it only shows a message; once fired it puts the part at a point 220 below the player and works out the steps that carry it back (6.67 a frame). | src comment |
| 001220B8 |  | Update handler, step 2: moves the part one step; after the last one it is at step 3. | src comment |
| 00122138 |  | Update handler: runs the animation; at its end the object is at step 4 and D_0038CC50.unk30 bit 0 is cleared. | src comment |
| 001221C0 |  |  |  |
| 001222C0 |  | Update handler, step 5: once the animation has ended, works out the steps that carry the part from point 0 of the model to a point 115 below the player. | src comment |
| 00122480 |  |  |  |
| 00122530 |  |  |  |
| 00122538 |  | Fires the trigger's one-shot when its type matches; returns whether it fired. | src comment |
| 00122600 |  |  |  |
| 001226D0 |  |  |  |
| 001226E0 |  | Update handler: used by the player, it sets D_0038CC50 bit 5 and hands over the first word of its source. | src comment |
| 00122720 |  |  |  |
| 00122730 |  | Update handler: used by the player, it shows the message of its definition, at most every 45 frames. | src comment |
| 001227C0 |  |  |  |
| 001227C8 |  |  |  |
| 001227D0 |  |  |  |
| 00122828 |  | Update handler, step 1: used by the player, it starts turning on. | src comment |
| 00122868 |  | Update handler of a switch turning on: runs the animation, then it is on. | src comment |
| 001228C0 |  | Update handler, step 3: used by the player, it starts turning off. | src comment |
| 00122908 |  | Update handler of a switch turning off: runs the animation, then it is off. | src comment |
| 00122960 |  | Switches the trigger on (type 1 to 2) or off (type 3 to 4) when it is at rest in the other position. | src comment |
| 001229D0 | KF_MapTrigger_UpdateModelMatrix | Map trigger: rebuilds the model's part matrices, then turns the model about the pivot of its second part (part 1), angle wrapped with pi / 2pi. | o2 |
| 00122BE8 |  | Starts this kind of trigger: at rest in the position its flag gives. | src comment |
| 00122CA8 |  |  |  |
| 00122CB0 |  | Advances the float at +0x50 toward 100000, then calls func_0010EA08; on a zero result sets state 3 and clears the float. | src comment |
| 00122D60 |  |  |  |
| 00122D68 |  | Advances the float at +0x50 toward 2000, then calls func_0010EA08; on a zero result sets state 1 and clears the float. | src comment |
| 00122E18 |  | Switches the trigger on (type 1 to 2) or off (type 3 to 4) when it is at rest in the other position. | src comment |
| 00122E88 |  | Starts this kind of trigger: a random period and a random phase within it. | src comment |
| 00122F48 |  |  |  |
| 00123000 |  |  |  |

## Entities: pool, per-map arrays, update all and draw all (0x123030-0x126820)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00123030 | KF_Entity_ResetAllLists | Clear the entity pool and empty every entity list. | kf_entity_pool.c, repo name |
| 00123108 |  | Returns TRUE while any pool block is held by a loading or unpacking job. | kf_entity_pool.c |
| 00123158 |  |  |  |
| 001231C0 | KF_Entity_FreeAllInGroup | Free every entity in the group from all three lists. | kf_entity_pool.c, repo name |
| 00123288 |  | Per-frame entity update: runs the pool, list and despawn steps in order. | kf_entity_pool.c |
| 00123580 | KF_Entity_DespawnOutOfRange | Free entities that are out of range of the player. | kf_entity_pool.c, repo name |
| 001248F8 |  | Sets the GS Z-buffer, alpha and test registers, then draws the model. | kf_entity_pool.c |
| 00124DE0 |  | Horizontal distance from the entity's spawn point to the player. | src comment |
| 00124E48 |  | Default init of an entity: clears the work area, sets flags 22-28 and the model of its kind. | src comment |
| 00124EF0 |  |  |  |
| 00124EF8 |  |  |  |
| 00124F00 |  | Sets up the entities of a map from its spawn records, filling in defaults for values left at 0. | src comment |
| 00125188 |  | Clears flag bit 4 of every entity in one of the two entity arrays. | src comment |
| 001251E0 | KF_Entity_ResetToSpawn | Puts an entity (Entity *) back to how it spawns: unk2C = params->unk0A, the flag chain reset, state fields zeroed, a 16-byte copy and the (1,1,1,1) vector D_0043E7C0. | o2 |
| 00125560 | KF_Entity_ClearArray | KF_Entity_ClearArray(array_index): zeroes one of the two 0x80-entity arrays D_003BEDF0[array_index] and its count; in the other array clears every Entity.unk14 pointing at a record of the cleared map (bit 27 of MapRecord.unk38 flags is the map slot). | o3 |
| 00125600 |  |  |  |
| 00125620 |  | Moves every entity of one of the two entity arrays by an offset. | src comment |
| 001256C0 |  | The per-frame update of every live entity: respawn, the countdowns, where the player is, the push it is under, its own update function, and the map blocks its model is drawn with. | src comment |
| 00125BD8 |  | Draws the entities of both maps, pool block by pool block: red while one is hit, pulsing in the colour of the timer that is running (unk20, unk24, unk28, unk2A), plain otherwise. | src comment |
| 00126120 |  | Rebuilds the entity's model matrices with the root part's own offset and rotation left out. | src comment |
| 00126240 | KF_Entity_IsRecordActive | Is the entity's map record (Entity.unk14) switched on: finds it in the two map slots; FALSE when the record has 0x40000000 in its high flag word, its slot is not loaded, or its map flag is clear (current map: a bit of flag_blocks[D_004141D0[0]].bits; other map: the block's 16 tags/values pairs). Same test as func_00116C08, written out. | o3 |
| 001263D8 |  | The map record the entity stands on: the one with the nearest floor under it, or failing that the one whose middle is nearest, among the records of the entity's map and then the other map. NULL when there is none. | src comment |
| 001267B0 |  | Looks up an entity of the current map by id. | src comment |
| 001267E8 |  | Looks up an entity of the given map by id. | src comment |
| 00126818 |  |  |  |
| 00126820 |  |  |  |

## Enemy kinds, per-kind AI (first part) (0x126828-0x12FDB8)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00126828 |  | Init of the default enemy kind: clears the work area, fills the handler table, starts walking or its action. | src comment |
| 00126A08 |  |  |  |
| 00126AF8 |  |  |  |
| 00126B38 |  |  |  |
| 00126C28 |  | Sends the entity back to its spawn point with the walk handler of its kind. | src comment |
| 00126C70 |  | wander: every so often walk to a point near the spawn point, turn to a random heading, or stand still | src comment |
| 00126E58 |  | chase: attack when in reach, else go for the player or a point near the spawn point for a while | src comment |
| 00127158 |  | movement of the default kind: turn as the command bits in Entity.unkC8 say and walk towards the target | src comment |
| 001273D8 |  | Action: one of the melee attacks; a hit may also clear the player's first timed flag. | src comment |
| 001278D0 |  | Init of this enemy kind: clears the work area, fills the handler table, starts walking or its action. | src comment |
| 00127AB0 |  |  |  |
| 00127BA0 |  |  |  |
| 00127CA0 |  | wander: every so often stand still, or now and then pick a point near the spawn point to walk to | src comment |
| 00127E08 |  | chase: face the player, then close in, or circle left or right around the player for a while | src comment |
| 00128438 |  | movement of this kind: turn and move as the command bits in Entity.unkC8 say, and pick the animation to go with it (it has turn animations of its own) | src comment |
| 00128950 |  | Init of this enemy kind: one handler table when it starts with its action, another when it starts asleep. | src comment |
| 00128B80 |  |  |  |
| 00128CA0 |  | Per-frame update of this kind: wakes up when the player comes near, else runs the handler of its state. | src comment |
| 00128DD8 |  | wander: every so often pick a point near the spawn point to walk to, or stand still | src comment |
| 00128F40 |  | chase: attack when in reach, else go for the player while he is near the spawn point, or back to it | src comment |
| 00129300 |  | Action: one of the ranged attacks, aimed a little below the player. | src comment |
| 001295B0 |  | Action: plays animation 3 until the entity is far enough from its spawn point, then goes back to its handler. | src comment |
| 00129738 |  |  |  |
| 00129780 |  | Action: dies. Levels out, plays the death animation, drops what it carries and fades away. | src comment |
| 001299F8 |  | Action: drops along its own up axis until it lands on a floor (awake) or on something else (asleep). | src comment |
| 00129CA0 |  | Action: levels the model out (pitch and roll back to 0), then goes back to its handler. | src comment |
| 00129EB0 |  | Init of this enemy kind: clears the work area, fills the handler table, starts walking or its action. | src comment |
| 0012A0A8 |  |  |  |
| 0012A228 |  |  |  |
| 0012A290 |  |  |  |
| 0012A370 |  |  |  |
| 0012A478 |  |  |  |
| 0012A6D8 |  |  |  |
| 0012A778 |  |  |  |
| 0012A8E0 |  |  |  |
| 0012AA48 |  |  |  |
| 0012ACA0 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind12ACA0 | kf_entity.h |
| 0012AE98 |  |  |  |
| 0012B040 | KF_Enemy_BendNeck | Bends the five neck joints (model parts 30-34, angles in EntityUnkE0Kind12ACA0.unk40[2..6]): facing the target (within 0.0001 rad) the first joint returns to 0 and each other follows the joint before it within params->turn_limit; otherwise unk1C is added to the first joint and what exceeds 15 degrees carries to the next. | o3 |
| 0012B338 |  |  |  |
| 0012B5F8 |  |  |  |
| 0012B880 |  |  |  |
| 0012BA50 |  |  |  |
| 0012BC28 |  |  |  |
| 0012BD18 |  |  |  |
| 0012BE50 |  | walk back to the spawn point, turning away from whatever is pushing the entity | src comment |
| 0012C088 |  |  |  |
| 0012C1D0 |  |  |  |
| 0012C7B8 |  | movement of a flying kind: turn, climb and move as the command bits in Entity.unkC8 say, and pick the animation | src comment |
| 0012CCA0 |  |  |  |
| 0012D040 |  |  |  |
| 0012D278 |  |  |  |
| 0012D450 |  |  |  |
| 0012D568 |  |  |  |
| 0012D668 |  | walk back to the spawn point, turning away from whatever is pushing the entity | src comment |
| 0012D8A0 |  |  |  |
| 0012D9A0 |  |  |  |
| 0012DD30 |  | movement of another flying kind: turn, climb and move as the command bits in Entity.unkC8 say, and pick the animation | src comment |
| 0012E0C8 |  |  |  |
| 0012E268 |  |  |  |
| 0012E448 |  |  |  |
| 0012E550 |  |  |  |
| 0012E6B0 |  |  |  |
| 0012E7F0 |  |  |  |
| 0012EAB8 |  |  |  |
| 0012EB98 |  |  |  |
| 0012EC98 |  |  |  |
| 0012EF00 |  |  |  |
| 0012F0E0 |  |  |  |
| 0012F2F8 |  |  |  |
| 0012F448 |  |  |  |
| 0012F638 |  |  |  |
| 0012F918 |  |  |  |
| 0012FB40 |  |  |  |
| 0012FDB8 |  |  |  |

## Per-kind enemy AI (0x1300E0-0x14FF68)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001300E0 |  |  |  |
| 00130300 |  |  |  |
| 00130490 |  |  |  |
| 001306B0 |  |  |  |
| 001308C0 |  |  |  |
| 00130A20 |  |  |  |
| 00130D98 |  |  |  |
| 00130EF8 |  |  |  |
| 00131128 |  |  |  |
| 001311C8 |  |  |  |
| 00131390 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind131390 | kf_entity.h |
| 00131630 |  |  |  |
| 001317D0 |  |  |  |
| 001319B8 |  |  |  |
| 001319F0 |  |  |  |
| 00131B38 |  |  |  |
| 00131BB8 |  |  |  |
| 00131C78 |  |  |  |
| 00131D70 |  |  |  |
| 00132090 |  |  |  |
| 00132428 |  |  |  |
| 001325B8 |  |  |  |
| 00132C70 |  |  |  |
| 00133228 |  |  |  |
| 00133428 |  |  |  |
| 001335D8 |  |  |  |
| 00133708 |  |  |  |
| 001337B0 |  |  |  |
| 001339D8 |  |  |  |
| 00133CA0 |  |  |  |
| 00133D60 |  |  |  |
| 00133E40 |  |  |  |
| 00133FC8 |  |  |  |
| 00134160 |  |  |  |
| 00134398 |  |  |  |
| 00134750 |  |  |  |
| 001349E8 |  |  |  |
| 00134B50 |  | walk back to the spawn point, turning away from whatever is pushing the entity | src comment |
| 00134DC8 |  |  |  |
| 00134F68 |  |  |  |
| 00135280 |  |  |  |
| 00135448 |  |  |  |
| 00135640 |  |  |  |
| 00135730 |  |  |  |
| 00135798 |  |  |  |
| 00135A40 |  |  |  |
| 00135DA0 |  |  |  |
| 00135EF0 |  |  |  |
| 00136040 |  |  |  |
| 001362A0 |  |  |  |
| 001364E8 |  |  |  |
| 001366C8 |  |  |  |
| 001367B8 |  |  |  |
| 001367F8 |  |  |  |
| 001368B8 |  |  |  |
| 00136BB0 |  |  |  |
| 00136D90 |  |  |  |
| 00136EA8 |  |  |  |
| 00136F18 |  |  |  |
| 00137018 |  |  |  |
| 00137158 |  |  |  |
| 001374E0 |  |  |  |
| 001375C0 |  |  |  |
| 001376C0 |  |  |  |
| 00137970 |  |  |  |
| 00137D68 |  |  |  |
| 00137F68 |  |  |  |
| 00138190 |  |  |  |
| 001381F8 |  |  |  |
| 001384C0 |  |  |  |
| 001385B0 |  |  |  |
| 00138608 |  |  |  |
| 001387C0 |  |  |  |
| 00138970 |  |  |  |
| 001389C8 |  |  |  |
| 00138A48 |  |  |  |
| 00138D50 |  |  |  |
| 00139010 |  |  |  |
| 00139538 |  |  |  |
| 00139730 |  |  |  |
| 00139960 |  |  |  |
| 00139AE8 |  |  |  |
| 00139DE0 |  |  |  |
| 00139FE0 |  |  |  |
| 0013A210 |  |  |  |
| 0013A398 |  |  |  |
| 0013A690 |  |  |  |
| 0013A890 |  |  |  |
| 0013AAD8 |  |  |  |
| 0013AC60 |  |  |  |
| 0013AF48 |  |  |  |
| 0013B150 |  |  |  |
| 0013B4C8 |  |  |  |
| 0013B568 |  |  |  |
| 0013B608 |  |  |  |
| 0013B828 |  |  |  |
| 0013B9E8 |  |  |  |
| 0013BA70 |  |  |  |
| 0013BC60 |  |  |  |
| 0013BE18 |  |  |  |
| 0013BF48 |  |  |  |
| 0013C018 |  |  |  |
| 0013C108 |  |  |  |
| 0013C300 |  |  |  |
| 0013C448 |  |  |  |
| 0013C7F0 |  |  |  |
| 0013CBB0 |  |  |  |
| 0013CD00 |  |  |  |
| 0013CF18 |  |  |  |
| 0013D118 |  |  |  |
| 0013D260 |  |  |  |
| 0013D588 |  |  |  |
| 0013D628 |  |  |  |
| 0013D778 |  |  |  |
| 0013D820 |  |  |  |
| 0013D9D0 |  |  |  |
| 0013DAF8 |  |  |  |
| 0013E1B8 |  |  |  |
| 0013E3F0 |  |  |  |
| 0013E9A8 |  |  |  |
| 0013EBD0 |  |  |  |
| 0013EC40 |  |  |  |
| 0013ED58 |  |  |  |
| 0013EE40 |  |  |  |
| 0013EFB8 |  |  |  |
| 0013F0D8 |  |  |  |
| 0013F1B8 |  |  |  |
| 0013F6A0 |  |  |  |
| 0013F7D8 |  |  |  |
| 0013F8F0 |  |  |  |
| 0013FAE8 |  |  |  |
| 0013FCB8 |  |  |  |
| 0013FF88 |  |  |  |
| 00140140 |  |  |  |
| 00140280 |  |  |  |
| 00140328 |  |  |  |
| 00140450 |  |  |  |
| 00140650 |  |  |  |
| 00140738 |  |  |  |
| 00140878 |  |  |  |
| 001408B0 |  |  |  |
| 001409D0 |  |  |  |
| 00140AC0 |  |  |  |
| 00140CC0 |  |  |  |
| 00140E88 |  |  |  |
| 001411B0 |  |  |  |
| 00141320 |  |  |  |
| 00141518 |  |  |  |
| 001416B8 |  |  |  |
| 00141A18 |  |  |  |
| 00141AF0 |  |  |  |
| 00141C38 |  |  |  |
| 00141D90 |  |  |  |
| 00141F88 |  |  |  |
| 00142080 |  |  |  |
| 001423A8 |  |  |  |
| 001427C8 |  |  |  |
| 00142A20 |  |  |  |
| 00142D28 |  |  |  |
| 00142EF8 |  |  |  |
| 00143230 |  |  |  |
| 001433F8 |  |  |  |
| 001435B0 |  |  |  |
| 00143850 |  |  |  |
| 00143A68 |  |  |  |
| 00143C28 |  |  |  |
| 00143C78 |  |  |  |
| 00143E70 |  |  |  |
| 00143FE8 |  |  |  |
| 00144400 |  |  |  |
| 00144758 |  |  |  |
| 001449F8 |  |  |  |
| 00144BF0 |  |  |  |
| 00144D38 |  |  |  |
| 00145080 |  |  |  |
| 00145430 |  |  |  |
| 00145620 |  |  |  |
| 001457A0 |  |  |  |
| 001458F0 |  |  |  |
| 00145BE8 |  |  |  |
| 00145D68 |  |  |  |
| 00145EA8 |  |  |  |
| 00146158 |  | moves a flying entity: turn, pitch down or up while unkC8 asks for it, pick the animation | src comment |
| 00146388 |  |  |  |
| 00146580 |  |  |  |
| 00146750 |  |  |  |
| 00146948 |  |  |  |
| 00146AF0 |  | turns two model parts towards the target: pitch from the player's height, yaw from the bearing | src comment |
| 00146CD0 |  |  |  |
| 00146E40 |  |  |  |
| 001474D0 |  |  |  |
| 00147678 |  |  |  |
| 00147870 |  |  |  |
| 00147A20 |  |  |  |
| 00147B00 |  |  |  |
| 00147B98 |  |  |  |
| 00147D40 |  |  |  |
| 00148100 |  |  |  |
| 001483F8 |  |  |  |
| 00148688 |  |  |  |
| 001488A8 |  |  |  |
| 001488C0 |  |  |  |
| 00148990 |  |  |  |
| 00148C30 |  |  |  |
| 00148DD8 |  |  |  |
| 00148FE8 |  |  |  |
| 001491C0 |  |  |  |
| 00149290 |  |  |  |
| 00149420 |  |  |  |
| 00149610 |  |  |  |
| 001499F8 |  |  |  |
| 00149CA8 |  |  |  |
| 00149ED8 |  |  |  |
| 0014A058 |  |  |  |
| 0014A1F8 |  |  |  |
| 0014A3F8 |  |  |  |
| 0014A6D0 |  |  |  |
| 0014AAE0 |  |  |  |
| 0014AB18 |  |  |  |
| 0014ACD0 |  |  |  |
| 0014B178 |  |  |  |
| 0014B270 |  |  |  |
| 0014B450 |  |  |  |
| 0014B690 |  |  |  |
| 0014B888 |  |  |  |
| 0014B988 |  |  |  |
| 0014BA98 |  |  |  |
| 0014BCF0 |  |  |  |
| 0014BF80 |  |  |  |
| 0014C1C0 |  |  |  |
| 0014C400 |  |  |  |
| 0014C5B0 |  |  |  |
| 0014C958 |  |  |  |
| 0014CAF8 |  |  |  |
| 0014CCF8 |  |  |  |
| 0014D090 |  |  |  |
| 0014D170 |  |  |  |
| 0014D368 |  |  |  |
| 0014D560 |  |  |  |
| 0014D740 |  |  |  |
| 0014D808 |  |  |  |
| 0014D950 |  |  |  |
| 0014DBC0 |  |  |  |
| 0014DE70 |  |  |  |
| 0014E068 |  |  |  |
| 0014E0E0 |  |  |  |
| 0014E208 |  |  |  |
| 0014E590 |  |  |  |
| 0014E7F8 |  |  |  |
| 0014EA70 |  |  |  |
| 0014EBA8 |  |  |  |
| 0014EC80 |  |  |  |
| 0014ECE8 |  |  |  |
| 0014EFB0 |  |  |  |
| 0014F240 |  |  |  |
| 0014F3F8 |  |  |  |
| 0014F5A8 |  |  |  |
| 0014F6E8 |  |  |  |
| 0014FA28 |  |  |  |
| 0014FCC0 |  |  |  |
| 0014FF68 |  |  |  |

## Per-kind enemy AI: init, update, action choice, movement, attacks, hit reactions, death (0x150200-0x17FFC0)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00150200 |  |  |  |
| 001503A8 |  |  |  |
| 00150478 |  |  |  |
| 00150588 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind150588 | kf_entity.h |
| 00150768 |  |  |  |
| 00150818 |  |  |  |
| 00150A98 |  |  |  |
| 00150D40 |  |  |  |
| 00150E40 |  |  |  |
| 001511D8 |  |  |  |
| 001513E8 |  |  |  |
| 001515A8 |  |  |  |
| 00151910 |  |  |  |
| 00151B60 |  |  |  |
| 00151D30 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind151D30 | kf_entity.h |
| 00151F90 |  |  |  |
| 00152158 |  |  |  |
| 001523D0 |  |  |  |
| 00152710 |  |  |  |
| 00152920 |  |  |  |
| 00152D60 |  |  |  |
| 00153040 |  |  |  |
| 001532C0 |  |  |  |
| 00153350 |  |  |  |
| 00153570 |  |  |  |
| 001537B8 |  |  |  |
| 00153A18 |  |  |  |
| 00153C58 |  |  |  |
| 00153F60 |  |  |  |
| 00154060 |  |  |  |
| 00154250 |  |  |  |
| 001542B0 |  |  |  |
| 00154388 |  |  |  |
| 00154580 |  |  |  |
| 00154710 |  |  |  |
| 00154C58 |  |  |  |
| 00154CD0 |  |  |  |
| 00154DB8 |  |  |  |
| 00154FB0 |  |  |  |
| 00155128 |  |  |  |
| 00155678 |  |  |  |
| 00155760 |  |  |  |
| 00155B58 |  |  |  |
| 00155DF0 |  |  |  |
| 00155FE8 |  |  |  |
| 001566A0 |  |  |  |
| 001569E8 |  |  |  |
| 00156E88 |  | walks through one of three doorways (map objects 0xA0-0xA2): unk7A says which. It is put just inside the doorway facing through it, opens the door on an animation frame and walks to a point on the far side. | src comment |
| 00157428 |  |  |  |
| 001574C0 |  |  |  |
| 00157898 | KF_Entity_CheckAttackPartInFrames | Runs the hit check func_00195BD8 for one part of an attack record unless the animation frames [frame_from, frame_to] lie wholly outside the part's active frames (unk24..unk26, 0 = unused); parts 3, 4, 5 take their window from the four frame marks at params+0x434 and run as part 1, 0, 1. Sibling of func_00195990. | g4 |
| 00157A00 |  |  |  |
| 00157BD0 |  |  |  |
| 00157E70 |  |  |  |
| 00158380 |  |  |  |
| 001583D8 |  |  |  |
| 001587A8 |  |  |  |
| 00158948 |  |  |  |
| 00158C50 |  |  |  |
| 00158E28 |  |  |  |
| 00158ED8 |  |  |  |
| 00159180 |  |  |  |
| 001593E8 |  |  |  |
| 00159620 |  |  |  |
| 00159820 |  |  |  |
| 001599D8 |  |  |  |
| 00159D28 |  |  |  |
| 0015A3D8 |  |  |  |
| 0015A538 |  |  |  |
| 0015A790 |  |  |  |
| 0015A8F8 |  |  |  |
| 0015AA00 |  |  |  |
| 0015ACA8 |  |  |  |
| 0015ADD0 |  |  |  |
| 0015AE88 |  |  |  |
| 0015B190 |  |  |  |
| 0015B340 |  |  |  |
| 0015B428 |  | walk back to the spawn point, turning away from whatever is pushing the entity | src comment |
| 0015B620 |  |  |  |
| 0015B868 |  |  |  |
| 0015BA70 |  |  |  |
| 0015BC58 |  |  |  |
| 0015C3B8 |  |  |  |
| 0015C518 |  |  |  |
| 0015C688 |  |  |  |
| 0015C7A8 |  |  |  |
| 0015C958 |  |  |  |
| 0015CA40 |  |  |  |
| 0015CB68 |  |  |  |
| 0015CC20 |  |  |  |
| 0015CFD0 |  |  |  |
| 0015D218 |  |  |  |
| 0015D410 |  |  |  |
| 0015D598 |  |  |  |
| 0015DA28 |  |  |  |
| 0015DB88 |  |  |  |
| 0015DE30 |  |  |  |
| 0015E000 |  |  |  |
| 0015E170 |  |  |  |
| 0015E298 |  |  |  |
| 0015E350 |  |  |  |
| 0015E628 |  |  |  |
| 0015E820 |  |  |  |
| 0015EA70 |  |  |  |
| 0015EF60 |  |  |  |
| 0015F0C0 |  |  |  |
| 0015F298 |  |  |  |
| 0015F408 |  |  |  |
| 0015F530 |  |  |  |
| 0015F5E8 |  |  |  |
| 0015F8C0 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind15F8C0 | kf_entity.h |
| 0015FAC8 |  |  |  |
| 0015FB98 |  |  |  |
| 0015FD38 |  |  |  |
| 001601B0 |  |  |  |
| 00160630 |  |  |  |
| 00160B30 |  |  |  |
| 001610C0 |  |  |  |
| 00161170 |  |  |  |
| 00161550 |  |  |  |
| 001616D8 |  |  |  |
| 00161860 |  |  |  |
| 00161A48 |  |  |  |
| 00161E20 |  |  |  |
| 001620D8 |  |  |  |
| 00162410 |  |  |  |
| 001625A0 |  |  |  |
| 00162738 |  |  |  |
| 00162898 |  |  |  |
| 00162A48 |  |  |  |
| 00162B48 |  | Counts the first six objects whose flags low nibble is 3, then stores (count + 2) / 3. | src comment |
| 00162BC8 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind162BC8 | kf_entity.h |
| 00162DC0 |  |  |  |
| 00162E38 |  |  |  |
| 00163310 |  |  |  |
| 00163848 |  |  |  |
| 001639A8 |  |  |  |
| 00163B28 |  |  |  |
| 001640B8 |  |  |  |
| 00164480 |  |  |  |
| 001646B8 |  |  |  |
| 00164940 |  |  |  |
| 00164950 |  |  |  |
| 00164A60 |  |  |  |
| 00164C88 | KF_Enemy_ChooseAttack | Picks the next attack by weighted chance: three melee (EntityParams.unk240[n].unk00 weights, usable when func_001984B0 says so) and three ranged (records at params+0x3D8, usable when Entity.unkA2 == 0 and func_001992F0 says so), each weight scaled by Entity.unkC0[n] / 64; stores the choice in flag bits 57 (ranged) and 58-59 (which), returns bit 57. | o3 |
| 00164EA8 |  |  |  |
| 001650A0 |  |  |  |
| 00165280 |  |  |  |
| 001658C8 |  |  |  |
| 00165A70 |  |  |  |
| 00165E68 |  |  |  |
| 00166158 |  | the jump at the player: crouch, leap along the arc func_00166BF8 tried out, land; being hit in the air ends it (unk7B: what to go back to afterwards) | src comment |
| 00166800 |  |  |  |
| 00166B38 |  |  |  |
| 00166BF8 | KF_Enemy_CanJumpArc | Jump arc test: simulates the arc of a jump from the entity's position (same arithmetic as move_along_arc in func_00166158), stores the travel in direction[0..2] and asks func_0018AE98 about the straight line to its end; TRUE when nothing is in the way. | o3 |
| 00166D78 |  |  |  |
| 00166F70 |  |  |  |
| 00167168 |  |  |  |
| 00167358 |  |  |  |
| 001673F0 |  |  |  |
| 00167498 |  |  |  |
| 00167608 |  |  |  |
| 00167808 |  |  |  |
| 001679B8 |  |  |  |
| 00167B88 |  |  |  |
| 00167BA0 |  |  |  |
| 00167C70 |  |  |  |
| 00167F08 |  |  |  |
| 00168000 |  |  |  |
| 00168130 |  |  |  |
| 00168160 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind168160 | kf_entity.h |
| 00168368 |  |  |  |
| 00168C10 |  |  |  |
| 00169918 |  | When the state field in flags is 2, resets the entity and registers func_00169990; then sets state 2. | kf_entity_ai.c |
| 0016A450 |  |  |  |
| 0016A530 |  |  |  |
| 0016A788 |  |  |  |
| 0016AA00 |  |  |  |
| 0016ABB8 |  |  |  |
| 0016AE00 |  |  |  |
| 0016AEC0 |  |  |  |
| 0016AFD8 |  |  |  |
| 0016B088 |  |  |  |
| 0016B228 |  |  |  |
| 0016B408 |  |  |  |
| 0016B560 |  |  |  |
| 0016B920 |  |  |  |
| 0016BC28 |  |  |  |
| 0016BF98 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind16BF98 | kf_entity.h |
| 0016C138 |  |  |  |
| 0016C1E0 |  |  |  |
| 0016C3A0 |  |  |  |
| 0016C7F0 |  |  |  |
| 0016C888 |  |  |  |
| 0016CC58 |  |  |  |
| 0016CE08 |  |  |  |
| 0016D210 |  |  |  |
| 0016D4D0 |  |  |  |
| 0016D6C0 |  |  |  |
| 0016D838 |  |  |  |
| 0016D9C0 |  |  |  |
| 0016DCC0 |  |  |  |
| 0016DE50 |  |  |  |
| 0016DFD8 |  |  |  |
| 0016E118 |  |  |  |
| 0016E310 |  |  |  |
| 0016E470 |  |  |  |
| 0016E868 |  |  |  |
| 0016EBD8 |  |  |  |
| 0016EEE0 |  |  |  |
| 0016F140 |  |  |  |
| 0016F348 |  |  |  |
| 0016F5E8 |  |  |  |
| 0016F7B8 |  |  |  |
| 0016F930 |  |  |  |
| 0016FE10 |  |  |  |
| 0016FFE0 |  |  |  |
| 001701D8 |  |  |  |
| 00170280 |  |  |  |
| 001704A8 |  |  |  |
| 00170A08 |  |  |  |
| 00170AB8 |  |  |  |
| 00170EB0 |  |  |  |
| 001712B8 |  |  |  |
| 00171558 |  |  |  |
| 00171808 |  |  |  |
| 001719F8 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind1719F8 | kf_entity.h |
| 00171C48 |  |  |  |
| 00171E30 |  |  |  |
| 00172320 |  |  |  |
| 001726C8 |  |  |  |
| 00172738 |  |  |  |
| 00172BB8 |  | Returns 0 with no target, 1 if the turn to it is within the limit, else 0x101/0x201 by turn direction. | src comment |
| 00172C78 |  |  |  |
| 00172F00 |  |  |  |
| 00173198 |  |  |  |
| 001735C8 |  |  |  |
| 00173AB0 |  | launches both shots of a record at once, whatever the animation frame | src comment |
| 00173D38 |  |  |  |
| 00173E08 |  |  |  |
| 00173E98 |  |  |  |
| 001740A8 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind1740A8 | kf_entity.h |
| 001742E0 |  |  |  |
| 00174530 |  |  |  |
| 001747F8 |  |  |  |
| 001749F0 |  |  |  |
| 00174B68 |  |  |  |
| 00174DF0 |  |  |  |
| 00175110 |  |  |  |
| 001754D0 |  |  |  |
| 001756C8 |  |  |  |
| 00175780 |  |  |  |
| 00175918 |  |  |  |
| 00175930 |  |  |  |
| 00175AE8 |  |  |  |
| 00175D90 |  |  |  |
| 00175F60 |  |  |  |
| 00176050 |  |  |  |
| 001760B8 |  |  |  |
| 00176238 |  |  |  |
| 00176360 |  |  |  |
| 001765C0 |  |  |  |
| 001766F8 |  |  |  |
| 00176830 |  |  |  |
| 00176A08 |  |  |  |
| 00176AF8 |  |  |  |
| 00176BD0 |  |  |  |
| 00176CD8 |  |  |  |
| 00176CE0 |  |  |  |
| 00176E80 |  |  |  |
| 00177040 |  |  |  |
| 00177120 |  |  |  |
| 00177318 |  |  |  |
| 00177520 |  | Aim: turn the angle toward the target into a clamped value, smooth it, and copy it to the part. | src comment |
| 001775F0 |  |  |  |
| 001777B0 |  |  |  |
| 00177AD0 |  |  |  |
| 00177D90 |  |  |  |
| 00178108 |  |  |  |
| 00178358 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind178358 | kf_entity.h |
| 001785D8 |  |  |  |
| 00178698 |  |  |  |
| 001787E0 |  |  |  |
| 00178BC0 |  |  |  |
| 00178E10 |  |  |  |
| 00178F88 |  |  |  |
| 00179118 |  |  |  |
| 00179380 |  |  |  |
| 00179510 |  |  |  |
| 00179778 |  |  |  |
| 001799A0 |  |  |  |
| 00179BB0 |  |  |  |
| 00179C10 |  |  |  |
| 00179DD8 |  |  |  |
| 00179F28 |  |  |  |
| 0017A158 |  | Copy the actor position to its model, relative to the map origin when on another map. | src comment |
| 0017A1E8 |  |  |  |
| 0017A260 |  |  |  |
| 0017A4F8 |  |  |  |
| 0017A540 |  |  |  |
| 0017A738 |  |  |  |
| 0017A880 |  |  |  |
| 0017ABC0 |  |  |  |
| 0017AE10 |  |  |  |
| 0017AF88 |  |  |  |
| 0017B118 |  |  |  |
| 0017B380 |  |  |  |
| 0017B510 |  |  |  |
| 0017B778 |  |  |  |
| 0017B958 |  |  |  |
| 0017BA98 |  |  |  |
| 0017BC98 |  |  |  |
| 0017BE90 |  |  |  |
| 0017BFD8 |  |  |  |
| 0017C2A8 |  |  |  |
| 0017C6B8 |  |  |  |
| 0017C968 |  |  |  |
| 0017CB38 |  |  |  |
| 0017CC08 |  |  |  |
| 0017CE00 |  |  |  |
| 0017CF48 |  |  |  |
| 0017D218 |  |  |  |
| 0017D628 |  |  |  |
| 0017D8D8 |  |  |  |
| 0017DAA0 |  |  |  |
| 0017DCD0 |  |  |  |
| 0017DF10 |  |  |  |
| 0017E220 |  |  |  |
| 0017E2E0 |  |  |  |
| 0017E560 |  |  |  |
| 0017E5D0 |  | moves a flying entity: turn, roll into the turn, pitch towards the level unk79 asks for, then speed | src comment |
| 0017EA98 |  |  |  |
| 0017EB58 |  |  |  |
| 0017EEB0 |  |  |  |
| 0017F0F0 |  | launches both shots of a record at once, whatever the animation frame | src comment |
| 0017F378 |  |  |  |
| 0017F408 |  |  |  |
| 0017F5A8 |  |  |  |
| 0017F790 |  |  |  |
| 0017F998 |  |  |  |
| 0017FC60 |  | Init of an enemy kind; its Entity+0xE0 work area is EntityUnkE0Kind17FC60 | kf_entity.h |
| 0017FFC0 |  |  |  |

## Shared enemy code: per-frame update and default handlers (0x180170-0x185460)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00180170 |  |  |  |
| 00180590 |  |  |  |
| 001807D0 |  |  |  |
| 00180A90 |  | Sibling of func_00164C88 (KF_Enemy_ChooseAttack): the same weighted attack choice, same shape. | o3 |
| 00180CA0 |  |  |  |
| 00180DE8 |  |  |  |
| 00180EA8 |  |  |  |
| 00181038 |  |  |  |
| 00181378 |  |  |  |
| 001814B0 |  |  |  |
| 001816A0 |  |  |  |
| 001818B8 |  |  |  |
| 00181AE0 |  |  |  |
| 00181C60 |  |  |  |
| 00181E48 |  |  |  |
| 00182240 |  |  |  |
| 001824E0 |  | per-frame update of an enemy: run its handlers, count its timers down, apply this frame's move | src comment |
| 00182800 |  |  |  |
| 00182A08 |  |  |  |
| 00182BD0 |  |  |  |
| 00182C80 |  |  |  |
| 00182DE0 |  | walk back to the spawn point, turning away from whatever is pushing the entity | src comment |
| 00182FD8 |  | wander: every so often pick a point near the spawn point to walk to, or stand still | src comment |
| 00183150 |  |  |  |
| 00183240 |  |  |  |
| 00183570 |  | default movement: turn and move as the command bits in Entity.unkC8 say, and pick the animation to go with it | src comment |
| 00183A40 |  |  |  |
| 00183C78 |  |  |  |
| 00183E20 |  |  |  |
| 00183E98 |  |  |  |
| 00183FA8 |  |  |  |
| 00183FC8 |  |  |  |
| 001840B8 |  |  |  |
| 001841B0 |  |  |  |
| 001841E0 |  |  |  |
| 00184240 |  |  |  |
| 00184330 |  | turn on the spot towards Entity.unk68 | src comment |
| 00184550 |  | turn on the spot towards the angle in the spawn info | src comment |
| 00184778 |  |  |  |
| 001849E0 |  |  |  |
| 00184C78 |  | default movement: turn and move as the command bits in Entity.unkC8 say, and pick the animation to go with it | src comment |
| 001850C8 |  |  |  |
| 00185300 |  |  |  |
| 00185390 |  |  |  |
| 00185460 |  |  |  |

## Entity push-out and the collision library (0x185480-0x18B150)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00185480 |  | keep an enemy out of the player and out of the other enemies; what pushed it is left in Entity.unkB0 | src comment |
| 001857E8 |  | cylinder against cylinder: is `position` (a cylinder reaching `height` up from it) inside the other one, and where should it stand instead | src comment |
| 00185A28 |  | cylinder against sphere: is `position` (a cylinder reaching `height` up from it) inside the sphere at `other`, and where should it stand instead | src comment |
| 00185E28 |  | sphere against cylinder: is the sphere at `position` inside the cylinder reaching `other_height` up from `other`, and where should it be instead | src comment |
| 001861F8 |  |  |  |
| 001862F0 | KF_Entity_ApplySurfaceEffect | Applies a surface's side effect on an entity after func_001125A8 classifies the hit surface (1-5; jump table jtbl_0043EB50): starts the unk20 timer (1800 frames) outright or by chance lowered by params->unkEA, or cuts push speed unk2C (by amount or by 30) and clears flag bits 38-39. | f4 |
| 00186478 |  | Collides a walking entity with a model's collision mesh: pushes `position` out of the walls (adding their normals to `normal_sum`) and lowers `*floor_y` to the highest floor under it. Returns TRUE if it found a floor. | src comment |
| 00186B00 |  | Collides a walking entity with the map pieces whose box it is in (its own map's, then the other loaded map's): pushes it out of walls, updates its push (unkA4), and puts it on the floor, lets it fall, or stops it at a ledge. | src comment |
| 001870F8 |  | Tests an upright capsule (a sphere of `radius` swept from `centre` down by `height`) against a triangle: its face at the top and bottom of the capsule first, then the nearest point of each edge. | src comment |
| 00187A20 |  | Pushes the entity's sphere (at `position`, radius unk30) out of every triangle of a model's collision mesh it touches, adding the wall normals to `normal_sum`. Returns TRUE if it touched the floor strips. | src comment |
| 00187E28 |  | Pushes the entity out of the map pieces whose box it is in (its own map's, then the other loaded map's), remembers the piece it stands on in unk14 and updates its push (unkA4). | src comment |
| 00188230 |  |  |  |
| 00188348 |  | Tests a sphere against a point: a hit when `point` is closer to `centre` than `radius`. | src comment |
| 001883E0 |  | Tests a sphere against a triangle: its face first, then its three edges and three corners. | src comment |
| 00188678 |  | Tests a ray (origin, unit direction, length) against a triangle. | src comment |
| 001888C0 |  | Pushes the entity out of the map objects it touches and adds their normals to its push (unkA4). | src comment |
| 00188BE0 |  | Pushes the entity (as a sphere of radius unk30) out of the map objects it touches and adds their normals to its push (unkA4). | src comment |
| 00188F40 |  | Pushes a sphere out of the four walls standing on the owner's points 0-3 (as high as point 0x1F is above point 0), then out of the four corner posts. | src comment |
| 00189210 |  | Pushes a sphere out of the walls standing on the owner's points, then out of the corner posts: one ring of four points, or two rings (0-3 and 4-7) while the model is animating. | src comment |
| 001898E8 |  | Tests a sphere against the two triangles of the owner's four points and pushes `centre` out along the normal. | src comment |
| 00189A38 | KF_Collision_MoveSphereOnMesh | Moves a sphere against the owner's collision mesh: finds the floor under it (floor_y starts at FLT_MAX, .sdata D_00446840) and pushes it out of walls. | o4 |
| 00189D28 |  | Tests a sphere against the four walls standing on the owner's points 0-3, as high as point 0x1F is above point 0. | src comment |
| 00189EC8 |  | Tests a sphere against the walls standing on the owner's points: one ring of four, or two rings (points 0-3 and 4-7) while the model is animating. | src comment |
| 0018A2B0 |  | Tests a sphere against the two triangles of the owner's four points. | src comment |
| 0018A3A8 |  | Tests a sphere against the owner's eight quads (four points each); a quad with a corner at the model's own position ends the test. | src comment |
| 0018A628 |  | Pushes a sphere out of every triangle of the owner's mesh it touches; leaves the summed normals in the result and moves `centre`. | src comment |
| 0018A970 |  | Tests the way from `from` to `destination` against `target`; TRUE when something is hit. | src comment |
| 0018A9F0 |  | How far above `position` the lowest floor of the prop's mesh is (D_00446844 when there is none). | src comment |
| 0018AB40 |  | Finds the nearest triangle of the prop's mesh a ray hits. Returns which of the four strip lists it is in (1-4, 0: none), leaves the distance in the result and copies the point to `out_point`. | src comment |
| 0018AE98 | KF_Map_RayHitsProp | KF_Map_RayHitsProp(origin, direction, out_point, length): ray against the props of the two loaded maps, current map first; returns the strip list hit (1-4) and copies the hit point. nearest stays FLT_MAX (retail bug), so it reports the last prop hit in record order, not the nearest. | g4 |
| 0018B150 |  | TRUE when the ray (origin, unit direction, length) hits a wall (strip list unk18) of any prop of the loaded maps. | src comment |

## Enemy helpers: line of sight, bone tree, look-at, projectile launches, attack choice (0x18B3C0-0x18FD50)

| address | candidate name | what it does | source |
|---|---|---|---|
| 0018B3C0 |  | whether `point` lies within `radius` of the segment start..end (FALSE beyond its ends) | src comment |
| 0018B518 |  | how far above `point` the ground of the loaded maps is (D_00446850[0]: none found) | src comment |
| 0018B6F8 |  | picks a point like func_0018B778, then moves its height by up to +-height_range | src comment |
| 0018B778 |  | picks a random point within `range` of `centre`. The nearer half of the range is twice as likely while `position` is in the outer half, and the other way round. | src comment |
| 0018BA10 |  | func_0018B778 for a caller that already knows how far from `centre` it is | src comment |
| 0018BC78 |  | a point `distance` away from the player, along the player's unk1AC vector turned by the heading | src comment |
| 0018BD18 |  | looks for `target` in the tree of parts under `part` and multiplies `matrix` by the transforms on the way to it; FALSE: not found | src comment |
| 0018BF30 |  |  |  |
| 0018BFA8 |  | builds the rotation matrix that looks from `origin` towards `target`; `up_sign` below 0 takes "up" as down, `side_sign` below 0 mirrors the sideways axis | src comment |
| 0018C0D0 |  | the angles that turn a part of the model to look at `target` (kept within `limits`, if given, of the model's heading); FALSE: the model has no such part | src comment |
| 0018C298 |  | keeps a direction within pitch limits[0]..limits[1] and yaw limits[2]..limits[3] of `heading`; TRUE: it had to be changed | src comment |
| 0018C4B8 |  | takes the three angles (x, y, z) out of a rotation matrix | src comment |
| 0018C728 |  | whether a map object stands between the entity's middle and the player's | src comment |
| 0018CA50 |  |  |  |
| 0018CAF8 |  | whether something stands between the entity and the player: tests the line from the entity's middle to the player's against the entity's own map record and the player's | src comment |
| 0018CC60 |  | tests the line from the entity's middle to the player's with func_0018B150 | src comment |
| 0018CDC0 |  | calls func_001B2C48 on target if the player is near `point` and within max_angle of `heading` | src comment |
| 0018CF30 |  | launches the shots of a record whose frame lies in (frame_from, frame_to], each along its bone, turned towards `target` by at most the two rates | src comment |
| 0018D4E8 |  | launches the shots of a record whose frame lies in (frame_from, frame_to] | src comment |
| 0018D600 |  |  |  |
| 0018D670 |  | starts what animation `animation` has listed for the frames from `frame_from` up to `frame_to` (both in 1/256 frames): each at a bone of the model, pointing along it | src comment |
| 0018D8B8 |  | starts what animation `animation` has listed for the frames from `frame_from` up to `frame_to` (both in 1/256 frames), telling func_001A0CD0 where the entity is as seen from the player | src comment |
| 0018DA38 |  | tells func_001A0CD0 where the entity is as seen from the player | src comment |
| 0018DB58 |  |  |  |
| 0018DC40 |  |  |  |
| 0018DCD8 |  | what every kind does when its entity dies | src comment |
| 0018DDA8 |  | turns the three angles of a model part. mode 1: `angles` towards `goal` (kept within pitch limits[0]..limits[1] and yaw limits[2]..limits[3] if given) and into the part; mode 0: `angles` towards the part's own and into the part | src comment |
| 0018E3D0 |  | func_0018DDA8 for a part that is to look at a point. mode 1: `angles` turn towards the angles func_0018C0D0 gives for `target` (FALSE if it gives none) and go into the part; mode 0 and other modes as in func_0018DDA8. FALSE: no model or no part. | src comment |
| 0018E9C0 |  | Steps the entity four-state timer machine. | src comment |
| 0018EAC0 |  | while the entity is pushed (flags 45, 46, 48) from the front: the heading that turns it a degree past side on to the push, written to `heading_out`. Returns the side it turns to (1, -1), or 0 when there is nothing to do. | src comment |
| 0018EC78 | KF_Entity_PushIsBehind | Returns whether the push the entity is under (flags 45/46 -> unkA4, 48 -> unkB0) points away from heading; FALSE if no push flag is set. | f4 |
| 0018ED98 |  | whether to react (chance params->unkCB / 5 percent) while the player, close enough and in front, is early in a swing | src comment |
| 0018EEE0 |  | like func_0018ED98, with the chance params->unkCA / 5 percent | src comment |
| 0018F030 | KF_Entity_PickAttack | Picks one of the six attacks (three melee EntityParams.unk240[i], three ranged at params+0x3D8) by weighted draw among those the player is in reach of (angle, height band, distance); weights (entity->unkC0[i] * record.unk00) >> 6. Writes flag bits 58-59 and 57 (ranged), calls func_00190B28, returns bit 57; with none in reach melee attack 0 is picked. | o4 |
| 0018F648 | KF_Entity_ChooseAttack | Picks one of the entity's three melee attacks (params unk240) by weighted draw among those whose reach, angle and height window contain the player; stores it in flag bits 58-59, clears or sets bit 57, calls func_00190B28; returns the chosen index. | f4 |
| 0018F9C0 | KF_Entity_ChooseRangedAttack | Twin of func_0018F648 for the three ranged attacks (params unk3D8, 0x1C-byte records): weighted draw among those whose reach, angle and height window contain the player; stores it in flag bits 58-59, bit 57, calls func_00190B28; returns the chosen index. | f4 |
| 0018FD50 | KF_Entity_PickAction | Picks the entity's next action out of the four; reads entity->unkA2 after a copy loop. | o4 |

## Outside our ranges (0x19xxxx), described in passing by reports (0x192C98-0x19E9E0)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00192C98 |  | Switches the actor to table entry idx and resets the owner's channel state; returns 1 if unchanged. | src comment |
| 00194F48 |  | 3D distance check used by the script walk-to command to end within distance | t10 |
| 00194F80 |  | Distance check in the plane (no height) used by the flat script walk-to command | t10 |
| 00195898 |  | Puts an entity in a state (used for state 4) | t10 |
| 00195990 |  | Runs the check of one launch part, unless the animation frame window [frame_min, frame_max] lies outside the part's frames. | src comment |
| 00195BD8 |  | Hit check for one part of an attack record: s32 func_00195BD8(Entity *, void *, s32, f32); called by func_00157898 when the part is active in the frame range. | g4 |
| 001984B0 |  | Says whether one of an enemy's three melee attacks is usable (asked by func_00164C88 before weighting it). | o3 |
| 001992F0 |  | Says whether one of an enemy's three ranged attacks is usable (asked by func_00164C88 before weighting it). | o3 |
| 0019B838 |  | Wraps an angle | t07 |
| 0019B908 |  | Called when both look buttons are held together to start levelling the view (status bit 32) | q11 |
| 0019B9F0 |  | Ease an angle toward target over frames steps along the shortest way round; returns 1 once it has arrived. | src comment |
| 0019BDA0 |  | Sets the display: called as func_0019BDA0(0x3D00, 0x3F00) at the start of the modal screen loops | q17 |
| 0019C7F0 |  | Clamps the level to 0..0x7F and passes it on with the global as the first argument. | src comment |
| 0019CDA0 |  | Stops a sound slot (called with 2) | t09 |
| 0019CDC0 |  | func_0019CDC0(n): plays menu sound n | q23 |
| 0019D020 |  | Plays a sound and returns its handle (footstep sounds 0x20..0x27; handle kept in D_00446958). | q10b |
| 0019D0F0 |  | Plays a sound by id (0x29, the low-health sound, every 27 frames). | q10b |
| 0019D158 |  | Plays a sound: called with 0x38 on landing when status bit 38 (in water) is newly set | q11 |
| 0019D3A0 |  | Plays a sound with random pan ((u8)sound, volume) | t09 |
| 0019D7D0 |  | Sound call used by the script command func_00214870 as ((char)tag, (u8)sound, value) | t09 |
| 0019DAA8 |  | Sound call used by the script commands func_00214700/func_002147A0 as (tag, sound, id, &from, &to, value) | t09 |
| 0019E9E0 |  | Stops the sound playing under a tag ((char)tag) | t09 |

## Positional sound and queued sound loads (0x1A0080-0x1A1608)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001A0080 |  |  |  |
| 001A01E0 |  | Starts a positional sound whose volume falls off with the distance between level and max_level. | src comment |
| 001A0338 |  | Starts a positional sound whose volume falls off with the distance between level and max_level. | src comment |
| 001A04F0 |  |  |  |
| 001A0678 |  |  |  |
| 001A07D0 | SoundPlayFalloffRound | Plays a positioned sound whose volume falls off along a circle; sibling of func_001A0678 / func_001A0988. | u02 |
| 001A0988 |  |  |  |
| 001A0B10 |  | Starts a sound job and counts it in D_00446864 when it was accepted. | src comment |
| 001A0B70 |  | Stops the voice of a job and counts the stop; -1 if the hardware is not ready. | src comment |
| 001A0BD0 |  | Runs the current step of the sound sequence, moving on when it reports done. | src comment |
| 001A0C18 | KF_Sound_LoadBankStep | func_001A0C18(args, index, voice_out): step 2 sound load of func_00101618. Fills job D_004FF4D8 (result = index + 6, address index * 0x11800 + 0x138810) and its spec D_004FF4C0 from the argument block, runs the sequence step func_001A0BD0; when done stores voice index + 6 through voice_out and returns 0, otherwise 1 (try again next frame). | g5 |
| 001A0CD0 |  | Calls handler handler_index of D_0043EE30 with the low byte of handler_arg and value. | src comment |
| 001A0D08 |  |  |  |
| 001A0D20 |  | Starts a sound job; -1 while the previous one is still being cleared. | src comment |
| 001A0D98 |  | Stops the voice of a job and counts the stop; 0 on success, -1 if the hardware is not ready. | src comment |
| 001A0DF8 |  | Runs one sound script step: stops the previous cue and starts the one the script state names. | src comment |
| 001A0F60 |  |  |  |
| 001A0FC8 |  | Counts a finished sound step and sets the flag for the next one. | src comment |
| 001A1000 |  | Runs the current step of the sound sequence, moving on when it reports done. | src comment |
| 001A1048 | SoundLoadMapBankStep | Step 1 of the queued sound loads: loads the sound bank of a map slot; hands MapSlot.unk34 {size_b, buffer_b, size_a, buffer_a} to the sequence at D_0043ED90. | u02 |
| 001A10D8 | SoundLoadMapBank | Blocking version of func_001A1048: loads the sound bank of a map slot (MapSlot.unk34) through the sequence at D_0043ED90. | u02 |
| 001A1160 |  | Builds the two file names for file_id (+0x96 in the name), loads them and starts the effect. | src comment |
| 001A1260 |  | Starts loading two named files with the given number into the buffers past the heap start. | src comment |
| 001A1360 |  | Loads two files into the spec's buffers and starts the effect; 0 if either file will not open. | src comment |
| 001A1438 |  |  |  |
| 001A1458 |  | Loads the two files named in the slot, then flushes the cache. | src comment |
| 001A14D8 |  | Syncs the cached byte D_004470E3 to the current map slot's value and sends it to the sound call. | src comment |
| 001A15B8 |  |  |  |
| 001A15C0 |  |  |  |
| 001A15D0 |  | Forwards its argument to func_001CD7E0; always returns 0. | src comment |
| 001A15F0 |  |  |  |
| 001A15F8 |  |  |  |
| 001A1600 |  |  |  |
| 001A1608 |  |  |  |

## Effect core: slots, per-frame run, model set-up, frees, GS state (0x1A1610-0x1A3E18)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001A1610 | EffectInit | Effect system init: clears both pools (D_003F1750 slots, D_003F3550 actors), lays out the buffers, loads the three effect files (D_0043EEF0 definitions, D_0043EF00, D_0043EF10 offset table), converts the definitions' angles from degrees to radians, moves unkFA ids up by 0x1400. | u02 |
| 001A1868 |  | Appends a GS store-image packet to the DMA buffer, then runs it and waits for the path to drain. | src comment |
| 001A19F0 |  |  |  |
| 001A1B48 |  | Transforms the positions of the live effect slots and effect actors by a matrix. | src comment |
| 001A1C58 | EffectSlotAlloc | Takes a free spawner slot (D_003F1750, 0x60 slots) for an effect id and fills it from D_00502260[id]. | u02 |
| 001A1E28 |  | Spawns effect effect_id at position and rotation; returns its slot, or -1 when none is free. | src comment |
| 001A1E88 | EffectRunAll | Runs spawner slots and effect actors for one frame: (1) each live slot runs func_001A24D8; (2) free actors count reservation unk90 down, live ones copy model position to unk70; (3) live effects count the hit countdown, spawn key-frame follow-ups (def->unk112[0..2]), run D_00402550[def->unk100][1]. With game status bit 0x400 only D_0038CC50.unk40's things and kinds 0x280/0x282/0x284. | u02b |
| 001A2108 |  |  |  |
| 001A2168 |  | func_001A2168(pass): draws all live effects for pass 0 or 1: map record lookup, texture frame choice, part placement (func_0020C300), send model; in pass 0 it frees finished effects. In texture mode 1 (def->unkF6 == 1) record_id is stale after the last frame (retail bug). | g5 |
| 001A24D8 | EffectSlotRun | One frame of a spawner slot: start delay unk27 (from EffectDef.unkFD), then every unkA7 frames makes unkA5 effect actors and counts level down, freeing the slot at 0; a full actor pool loses the firing. | u02 |
| 001A2838 | EffectSpawnAtEffect | Spawns an effect at an effect's own model; twin of func_001A2928. | u02 |
| 001A2928 |  | Allocates an object slot for effect_id and links it to the actor. | src comment |
| 001A2A18 |  | Frees an effect, then the spawner slots and effects that it owns or targets. | src comment |
| 001A2AF8 |  | Frees a spawner slot: clears its live flag and counts one slot down. | src comment |
| 001A2B28 |  | Frees every effect that targets the given effect. | src comment |
| 001A2BC8 |  | Frees every spawner slot that the given effect spawned. | src comment |
| 001A2C68 | EffectModelSetup | Sets up the model of effect actor n (D_0054A260[n]) from the definition's model file D_00447104[def->unkFE], 0x800 bytes of work memory from D_00447100, draw routine from D_003F1710[placement][unkF4]. | u02 |
| 001A2E58 |  | Looks up the record for a record id (asked by the effect draw pass func_001A2168). | g5 |
| 001A2F20 |  | Adds an offset to the position of every live slot whose definition is not flagged to skip it. | src comment |
| 001A3040 | EffectFreeMap | Frees everything the effect system owns for a map slot being unloaded and clears effect pointers (EffectActor.unk58) into its records. | u02 |
| 001A32A8 |  | Frees every spawner slot and effect that the given entity owns. | src comment |
| 001A33A8 |  | Frees the spawner slots and effects that have no effect_param and belong to owner. | src comment |
| 001A34D0 |  | Frees every spawner slot and effect whose effect_param is the given id. | src comment |
| 001A35D0 | EffectFreePlayerKinds | Frees the player's effects of kinds 0x280, 0x282, 0x284. | u02 |
| 001A3748 | EffectFreePlayerMagic | Frees the player's stoppable effects except those of kinds 0x280, 0x282, 0x284. | u02 |
| 001A3918 |  | Frees every live spawner slot whose effect_param is effect_param, while any are live. | src comment |
| 001A39B0 |  | Returns 1 if a live spawner or effect uses an EffectDef from first_def to last_def. | src comment |
| 001A3AC8 |  |  |  |
| 001A3B38 |  | Sets up the GS test/alpha/zbuf state, then draws the packet. | src comment |
| 001A3BA0 |  | Sets the GS state for an effect pass, then draws the effect's model. | src comment |
| 001A3C08 |  | Sets the GS state for an effect pass, then draws the effect's model. | src comment |
| 001A3C70 |  | Sets the GS state for an effect pass, then draws the effect's model. | src comment |
| 001A3CE0 |  | Sets the GS state for an effect pass, then draws the effect's model. | src comment |
| 001A3D48 |  |  |  |
| 001A3DB0 |  |  |  |
| 001A3E18 |  |  |  |

## Effect kinds: start and update handlers (0x1A3E20-0x1AFF78)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001A3E20 |  | Advances the effect's frame counter by one. | src comment |
| 001A3E30 |  | Starts an effect that moves at its definition's speed. | src comment |
| 001A3EA8 |  | Lowers the effect's model 220 units in y after the common update. | src comment |
| 001A3EE8 |  | Sets flag bit 58 on the effect after the common update. | src comment |
| 001A3F20 |  |  |  |
| 001A4078 |  | Starts an effect and stores a random factor in 1 +- def->unkDC in motion.unk10.z. | src comment |
| 001A40E8 |  | Scales the model's unkE0 vector by the follower's scale factor. | src comment |
| 001A4128 |  | Starts an effect at a target's position, or at its own model if it has none. | src comment |
| 001A41F0 |  | Counts the countdown down, and calls func_001AF298 on the frame that matches def->radius_half. | src comment |
| 001A4248 |  | Starts an effect like func_001A4128, then settles the model onto the ground half a height up. | src comment |
| 001A4380 |  | Counts the countdown down, and calls func_001AF298 once it runs out while the flag is clear. | src comment |
| 001A43D0 |  | Starts an effect with no end target. | src comment |
| 001A4410 |  | On the first frame, turns the model to a random heading within +-pi, then runs func_001A4480. | src comment |
| 001A4480 |  | Advances the actor's frame counter and wraps it at the control block's limit. | src comment |
| 001A44D8 |  | Starts a spawner: clears its count and picks the first spawn frame. | src comment |
| 001A4570 |  | Spawner update: spawns an effect when its frame is due, then schedules the next one. | src comment |
| 001A4630 |  | Lifts the model 220 units, aims it at a target found from its position and starts its velocity. | src comment |
| 001A46F0 |  |  |  |
| 001A4A20 |  | Lifts the model 220 units, starts its velocity and aims it at the player. | src comment |
| 001A4AB8 |  | Homes on the entity it was aimed at; on a hit it spawns an effect where it landed. | src comment |
| 001A4C50 |  | Lifts the model by 220 units, starts its motion and stores func_001B0208's result. | src comment |
| 001A4CE8 |  |  |  |
| 001A4DB8 |  | Lowers the model by 220 units and clears the motion's state. | src comment |
| 001A4E38 |  | Falls under gravity and spawns its landing effects on the first hit. | src comment |
| 001A4F78 |  |  |  |
| 001A50E8 |  | Lands the effect next to its target entity, then spawns the end effect once it is close | src comment |
| 001A52B8 |  |  |  |
| 001A5348 |  | Effect kind per-frame update for a falling projectile: falls no faster than 45 a frame, ends on a hit. | u02 |
| 001A5598 |  |  |  |
| 001A56F0 |  | Steers the effect toward the entity it chases and moves it. | src comment |
| 001A5880 |  | Effect kind start handler that sets up the ground-running kind updated by func_001A5988 (GroundRunMotion). | u02 |
| 001A5988 |  | Effect kind per-frame update for the ground-running kind set up by func_001A5880 (GroundRunMotion). | u02 |
| 001A5C80 |  | Starts an effect of this kind: the shared setup, then the second setup step. | src comment |
| 001A5CB8 |  | Holds the effect for 6 frames and while any active slot has a type-0xE owner, then ends it. | src comment |
| 001A5D20 |  | Per-frame update for an actor: refresh the sub-state at +0x10 and reset its counters. | src comment |
| 001A5DA8 |  | Per-frame update of a timed effect that spawns its children at intervals. | src comment |
| 001A6028 |  |  |  |
| 001A6118 |  | Per-frame update of a swaying effect that drops and bounces. | src comment |
| 001A63C0 |  |  |  |
| 001A6498 |  | Spawns an effect at a point offset from the model by the heading rotated by angle. | src comment |
| 001A6538 |  | Effect update: ground runner; every odd frame spawns def->radius_half at +unkF0 and def->interval at -unkF2 sideways (func_001A6498). Sibling of func_001A6118. | u03 |
| 001A6748 |  | Starts an effect that falls toward the ground in front of it. | src comment |
| 001A6818 |  | Advances the effect's frame counter. | src comment |
| 001A6828 |  | EffectActor.flags bit 58, set when the effect spawns | src comment |
| 001A68C8 |  | Falls under gravity while inside max_frames; after frame 20 it ends on a hit, spawning one or two effects. | src comment |
| 001A6A20 |  | Starts an effect that turns at the speed in its definition and clears its flag. | src comment |
| 001A6A90 |  | Effect update: leading effect that flies until it comes within 150 of an entity or outlives radius_half frames (then sets flag14); pair with func_001A6C98. | u03 |
| 001A6B88 |  | Starts a turning effect, turning at the speed in its definition. | src comment |
| 001A6BF8 |  | Aims a motion at its target if the target is within reach; returns 1 if it is. | src comment |
| 001A6C98 |  | Effect update: trails effect->target (the leader func_001A6A90); when the leader's flag14 is set, aims at the ground under it (func_001A6BF8) and spawns the end effect within 50 of the aim. | u03 |
| 001A6EE8 | EffectPickTargetsStart | Effect start: fills PickedMotion (entity, distance, list index) with the four nearest entities more than 10 above the effect and inside a 45 degree cone; func_001A7148 reads it. | u03 |
| 001A7100 |  | Advances the effect's frame counter. | src comment |
| 001A7110 |  | Turns the vector from one position to another into angles at angles_out. | src comment |
| 001A7148 |  | Starts an effect that aims at the entity its caller's slot picked, or at a random point around it. | src comment |
| 001A72F0 |  | Scales the model each frame, and spawns the end effect when the frame reaches the definition's radius. | src comment |
| 001A73B0 |  | Starts an effect that homes on an entity picked from its target's setup. | src comment |
| 001A7548 |  |  |  |
| 001A7650 |  |  |  |
| 001A77D0 |  | Drifts an effect toward its end position, then advances its frame counter. | src comment |
| 001A7880 |  | Launches the effect along its model's heading turned by the caller's step count, with random offsets. | src comment |
| 001A7A48 |  | Once the countdown has run out, sets the countdown of every pooled effect of the same kind, then spawns effect 0xDF. | src comment |
| 001A7AC8 |  | Starts the effect's ending: spawns the end effect, then flags the motion as ended. | src comment |
| 001A7B28 |  | Effect update: bobbing effect (+-0.01 per frame up to bob_limit); at frame max_frames takes the nearest entity (func_001AFF78) and homes on it with a turn growing 0.1 per frame up to def->unkE0; on ending gives every live effect with the same target end_countdown = distance / 20. | u03 |
| 001A7E98 |  | Spawns an effect that flies along its model's heading and spins at a random rate. | src comment |
| 001A7F88 |  | Effect update: speeds up along its heading while bobbing (+-0.01 per frame up to bob_limit; BobbingGrowMotion). | u03 |
| 001A80E8 |  | Starts an effect at a random point inside the box the definition describes, and lifts the model 220 units. | src comment |
| 001A8218 |  |  |  |
| 001A8408 |  | Per-frame update: clears the motion's 0x24 flag and sets pool flag bit 43. | src comment |
| 001A8468 |  | Advances the effect a frame: before its radius frame it aims a point at the model, from then on it moves the model by its velocity. | src comment |
| 001A8550 |  | Per-frame update: turns the model toward its target and adds a random tilt. | src comment |
| 001A8678 |  |  |  |
| 001A8768 |  | Per-frame update: aims the model along D_00413F80 and sets its turn angle. | src comment |
| 001A88A8 |  | Puts a cooldown on the pool effects of the same kind; once cooled down, spawns effect 0x47. | src comment |
| 001A8928 |  | Per-frame update: turns the model's spawn point round its rotation, then ends the effect at end_frame. | src comment |
| 001A8BA8 |  |  |  |
| 001A8CF0 |  | Steers toward the entity it chases, ending the chase once that entity has flag bits 2-3 set. | src comment |
| 001A8E48 |  | EffectMotion as func_001A8E48 uses it: the start of the velocity and its per-kind state with a 16-bit count and a byte flag at 0x20 | src comment |
| 001A8ED0 |  |  |  |
| 001A8FF0 |  |  |  |
| 001A90A0 |  | Effect start (SweepMotion): sweeping effect aimed at its target or the ground ahead of the player. | u03 |
| 001A92A8 |  | Effect update (SweepMotion, pair with func_001A90A0): spawns the end effect at arrival, then the aim spirals outward. | u03 |
| 001A9578 |  | Starts a falling effect and, if no same-kind effect is already live inside its interval, spawns its follow-up. | src comment |
| 001A96B0 |  |  |  |
| 001A9790 |  | Per-frame update: turns the model's heading by the global counter and wraps it into (-pi, pi]. | src comment |
| 001A98D0 |  |  |  |
| 001A99D8 |  | Start-up helper: stores how far the spawner's level is below the definition's max_level. | src comment |
| 001A9A28 |  | Per-frame update: moves the model's E4 value toward the definition's DC value by the step count. | src comment |
| 001A9AA8 |  | Starts an effect that has no per-kind state: runs the common start helpers. | src comment |
| 001A9AE0 |  | Keeps this effect alive while another effect of kind 0x23 is in use; otherwise ends it. | src comment |
| 001A9B38 |  | Spawns an effect: a random rotation of the model, then a velocity along the rotated radius. | src comment |
| 001A9C90 |  | Spawns an effect: a random change of the model's rotation, then a velocity along the rotated radius. | src comment |
| 001A9E08 |  | Per-frame update: moves the model by its velocity until the definition's frame limit. | src comment |
| 001A9E88 |  | Turning effect launched along its model's heading; its end frame and turn come from the definition. | src comment |
| 001A9F40 |  | Turning effect that copies the definition's unkF2 into its unk20 in modes 3 and 5. | src comment |
| 001A9F98 |  | Starts a sub-effect from the frame that the definition's unkF0 and interval give. | src comment |
| 001AA010 |  | Per-frame update for an effect that only runs the common helpers. | src comment |
| 001AA040 |  | Effect update (RestartMotion): turning effect that starts a sub-effect and restarts (effect->frame = start_frame); update for the starts func_001A9E88 / func_001A9F40. | u03 |
| 001AA230 |  | Starts an effect: copies its collision radius and interval, then spawns its start effects. | src comment |
| 001AA2D0 |  | Pulse effect step: while the frame is in [radius_half, interval) it runs func_001AF298 on the actor. | src comment |
| 001AA330 |  | Per-frame update for an actor: sets the sub-state at +0x10 from its definition at +0x54. | src comment |
| 001AA3E0 |  | Spawn callback: a turning effect that, in game modes 3 and 5, takes the definition's interval. | src comment |
| 001AA438 |  | Turning effect like func_001AA3E0 that also sets flag bit 0x8000 << 43. | src comment |
| 001AA470 |  | Turns toward a point and spawns def->unkF0 every def->interval frames. | src comment |
| 001AA590 |  | Moves by its velocity; once its frame passes end_frame it spawns its effect, or earlier does so on a hit. | src comment |
| 001AA728 |  | Follower effect: copies its target's position and rotation into its own motion. | src comment |
| 001AA770 |  | Copies the model's and the target's position and rotation into the motion, then counts a frame. | src comment |
| 001AA820 |  | Follower effect that copies its target's position into its own motion. | src comment |
| 001AA860 |  | Update: runs the three keyed channels, copies the positions across, then advances the frame. | src comment |
| 001AA8E0 |  | Spawn: starts the velocity from the definition's unkDC and unkE0. | src comment |
| 001AA940 |  | Copies its source entity's pose and scales itself by how far the source is from the camera. | src comment |
| 001AAA60 |  | Starts an effect with the definition's initial velocity.x and velocity.y. | src comment |
| 001AAAC0 |  | Update: fades the effect's scale by the player's distance inside the velocity band. | src comment |
| 001AABA0 |  | Starts a spray effect with random velocity and spin around the definition's ranges. | src comment |
| 001AAD40 |  | Per-frame update for a spray effect: applies gravity, moves the model and turns it by its spin. | src comment |
| 001AAEA0 |  | Spawn: launches along the model's rotation at the definition's speed, then sets the radius. | src comment |
| 001AAF20 |  | Starts an effect with flag bit 43 set, after the common setup. | src comment |
| 001AAF58 |  |  |  |
| 001AB018 |  | Spawn: moves along the model's heading at the definition's speed. | src comment |
| 001AB080 |  | Falls under gravity and spawns its end effect when it reaches the ground. | src comment |
| 001AB188 |  | Follows its target and counts down, then ends at the target's frame count. | src comment |
| 001AB2C0 |  | Bobs toward its target, then ends once it is grounded or a check fails. | src comment |
| 001AB470 |  | Spawn: moves along the model's heading at the definition's speed. | src comment |
| 001AB4D8 |  | Homing effect: steers its velocity toward the player and moves while frame < max_frames. | src comment |
| 001AB5C8 |  | Effect start (TiltFallMotion): tilting, falling "leaf" effect; pair with update func_001AB670. | u03 |
| 001AB670 |  | Effect update (TiltFallMotion, pair with func_001AB5C8): rotation.x swings between +-def->unkE0, falls with fall_speed squared after radius_half frames. | u03 |
| 001AB870 |  | Sinks the model by its definition's float, then sets a random countdown to the next check. | src comment |
| 001AB940 |  | Spawner: each frame, spawns effect def->unkF0 from the model until spawn_count passes the frame. | src comment |
| 001AB9C0 |  | Spins the model's heading, then sets the actor's motion from the spin and the definition's radius. | src comment |
| 001ABAF0 |  | Drifting effect: moves by its velocity, which grows by unk10.y each frame, until def->unkF0. | src comment |
| 001ABBE0 |  |  |  |
| 001ABC40 |  | Moves its model: copies its start position, aims its velocity at radius_half, then adds the velocity. | src comment |
| 001ABCF8 |  | Turns the model's heading by the caster's level, then moves the model along its velocity scaled by the same level. | src comment |
| 001ABE20 |  | Moves the model by its velocity scaled up each frame, then advances the frame counter. | src comment |
| 001ABEF8 |  | Probe effect: sets its velocity from the model's rotation and measures the distance to a point. | src comment |
| 001ABFD0 |  | Probe effect: turns its model toward the player and measures the distance to a point. | src comment |
| 001AC0E8 |  | At frame == def->radius_half, adds the velocity to the model position scale_per_mille / 100 times, each time calling func_001AF298; then counts a frame. | src comment |
| 001AC198 |  | Copies the target's unk20 into this actor's motion, after the common update. | src comment |
| 001AC1F8 |  | Advances the frame counter and scales the model's float at +0xE8 by scale_per_mille / 1000. | src comment |
| 001AC270 |  | Common update, then clears the motion's int at 0x10. | src comment |
| 001AC2C0 |  | Spins the heading, drifts the model along the rotated x axis, and copies the target's position. | src comment |
| 001AC400 |  | Sets the velocity from the heading and speed, and flags the effect. | src comment |
| 001AC478 |  | Steers toward the player for the first interval frames, then flies straight until it hits. | src comment |
| 001AC588 |  | Drops the model by its interval and starts the motion from the heading and speed. | src comment |
| 001AC668 |  | Falls under the definition's gravity, moving the model along the velocity. | src comment |
| 001AC6F0 |  | Launches the effect in a random spread around the player's heading, aimed back at the player. | src comment |
| 001AC8E8 |  | Advances a chasing effect toward the player and frees it after a fixed window past its end frame. | src comment |
| 001ACA28 |  | Effect start: one link of a lightning-like chain (ChainMotion): random length 0..200, links_left = def->interval on the first link else the previous link's count less one; spawns def->radius_half at the far end. | u03 |
| 001ACCA0 |  | Advances the frame counter and scales the model's float at +0xE8 by scale_per_mille / 1000. | src comment |
| 001ACD18 |  | Drops the model onto the entity it is tied to, and clears the motion's flag while it is in reach. | src comment |
| 001ACDA8 |  | Ends the effect when its flag byte is set; otherwise runs one frame of its channels. | src comment |
| 001ACE08 |  | Spawns the effect: keeps its argument, then starts it from its two stored positions. | src comment |
| 001ACE80 |  | Holds its start pose, then flies off along the anchor model's heading. | src comment |
| 001ACF70 |  | Starts an effect that waits for its state flag to be set. | src comment |
| 001ACFC0 |  | Holds the model still for a few frames after a hit, then ends; otherwise copies its start pose and reacts to the player. | src comment |
| 001AD130 |  | Starts an effect that follows its target, taking the target's speed. | src comment |
| 001AD1B8 |  |  |  |
| 001AD278 |  | Starts an effect that follows its target, taking the target's speed. | src comment |
| 001AD318 |  | Steers the effect toward the entity it chases (pulled by gravity after radius_half) and moves it. | src comment |
| 001AD4B8 |  | Starts an effect that turns toward a point, lowering the model by 220 units first. | src comment |
| 001AD558 |  |  |  |
| 001AD618 |  | Spawns the effect: stores a value from the model and the constant D_0043EF60, then aims the velocity at the target. | src comment |
| 001AD6A8 |  | Copies the motion vector into the model position, then the target's model position into the motion. | src comment |
| 001AD730 |  | Spawns the effect: stores a value from the model and the constant D_0043EF60. | src comment |
| 001AD7A0 |  |  |  |
| 001AD7F8 |  | Sets up a spiral's start angle, per-frame steps and a random start phase. | src comment |
| 001AD8E8 |  |  |  |
| 001ADA60 |  | Effect start: player position + def->offset_base turned by the player's heading, random spread; probes the ground from 1500 above to 1500 below and sits radius_half above it; with no ground the effect is freed at once. | u03 |
| 001ADC48 |  | Per-frame update of a drifting effect: moves its model along its velocity. | src comment |
| 001ADCA8 |  | Drops the effect's model by a probe vector and picks the landing frame from the fall distance. | src comment |
| 001ADDD0 |  | Falls under EffectDef.gravity and spawns an effect when it lands. | src comment |
| 001ADE98 |  | Starts an effect with no per-kind state. | src comment |
| 001ADED0 |  | Advances the frame counter and scales the model's unkE0 and unkE4 by 10 * rotation.x. | src comment |
| 001ADF40 |  | Converts the model's heading from radians to degrees and stores it in the motion's first words. | src comment |
| 001ADFC0 |  | Counts down, and every reload_frames frames spawns an effect once frame reaches keyframe_end. | src comment |
| 001AE060 |  | Starts an effect that flies off at the definition's speed in a random direction. | src comment |
| 001AE118 |  | Per-frame update of a spinning effect: turns its model by the spin angle. | src comment |
| 001AE278 |  | Re-seeds the motion from the model and definition, then sets bit 58 of the flags. | src comment |
| 001AE2F0 |  | Per-frame update of a tumbling effect: turns its velocity by a random angle, bounces it off the floor. | src comment |
| 001AE428 |  | Starts an effect that follows its target: copies the target's position and rotation. | src comment |
| 001AE498 |  | Scales the effect's unk0C by its target's, copies the model pose to and from the target, and wraps the frame at keyframe_end. | src comment |
| 001AE570 |  | Starts an effect whose int at motion+0x10 starts at zero. | src comment |
| 001AE5C0 |  | Per-frame update of a tilting effect: tilts its axis by a random angle and moves its model along the target. | src comment |
| 001AE730 |  | Starts an effect at rest, with its model's position and a fixed 0.1 at +0x20 and +0x24. | src comment |
| 001AE7B0 |  |  |  |
| 001AE910 |  |  |  |
| 001AE970 |  |  |  |
| 001AEA68 |  | Clears the effect's velocity word and its target. | src comment |
| 001AEAB0 |  | Advances the frame counter and wraps it at radius_half until the loop is stopped. | src comment |
| 001AEB30 |  |  |  |
| 001AEBF8 |  |  |  |
| 001AEC48 |  | Sets the model's vector at 0xE0 from the stage vectors; returns the stage, or -1 while blending between two. | src comment |
| 001AED58 |  | Adds the stage rotation to the model's rotation, wrapping each angle; returns the stage, or -1 while blending between two. | src comment |
| 001AF0B0 | EffectUpdateColor | Shared effect helper: third keyframed channel (like func_001AEC48 / func_001AED58); writes the four colour floats at EffectActor+0 from colour_keys (bytes), blended by func_001AFC08. | u03 |
| 001AF298 |  | Spawns the end effect once the hit check passes; *state records that it is spawned. | src comment |
| 001AF358 |  | Like func_001AF298, but returns 0, 1 or 2 by the hit flags from func_001B4F90. | src comment |
| 001AF468 |  | Spawns the end effect and sets *state; if *state is already 1, calls func_001A2A18 instead. | src comment |
| 001AF4E0 |  | Returns 1 when the effect has a hit check and func_001B4F90 reports a hit, else 0. | src comment |
| 001AF538 |  | Sets out to a vector of the given length along z, turned by angles (x pitch, y heading). | src comment |
| 001AF5B0 |  | Writes the pitch and heading angles of a direction vector into angles[0] and angles[1]. | src comment |
| 001AF640 | EffectSetStartPosition | Shared: places a new effect's model: offset = def->offset_base + random * offset_spread, turned by pitch (def flag 0x40000) and heading, added to the origin (start_position with flag 0x10000, else the slot's position); result also kept in EffectActor.unk70. | u03 |
| 001AF928 | EffectSetStartRotation | Shared: sets a new effect's model rotation from start_rotation (flag 0x10000) or the slot; x cleared unless flag 0x40000, z cleared if flag 0x800000; adds rotation_base + random * rotation_spread, each angle wrapped. | u03 |
| 001AFBA8 |  | Index of the keyframe segment the effect's frame falls in (or the first empty one), or -1 after three segments. | src comment |
| 001AFC08 |  | Progress from min to max, eased by mode (1: squared, 2: ease out), written as 1 - progress. | src comment |
| 001AFC88 |  | Turns velocity toward direction by turn, then sets its length to speed. | src comment |
| 001AFD08 | EffectFindTargetInCone | Shared: (effect, position, reference) walks the entity list for the entity nearest to a direction: within def->interval, pitch/heading difference under def->unkE4 / unkE8; lowest distance * (pitch + 1) * (heading + 1) wins. | u03 |
| 001AFF78 |  | The nearest entity within the effect's range of position, or NULL. | src comment |

## Targeting helpers and debug menus (0x1B0040-0x1B1030)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001B0040 | KF_Entity_IsUntargetable | Returns whether an entity cannot be targeted: not live, dying, or flag bit 16 or bit 21 set. | q01 |
| 001B00A0 |  | Nonzero if the point is within radius of any live entity centre. | src comment |
| 001B0178 | KF_Vector_Distance | Distance between two points (VU0). | q01 |
| 001B01B0 |  |  |  |
| 001B0208 | KF_Effect_FramesToImpact | Effect helper: casts the path forward and returns the frames until within radius of where it lands ((distance - radius) / speed). | q01 |
| 001B02D8 |  |  |  |
| 001B0390 | KF_Debug_EditInt | Dead debug value editor for an s32; reads the controller report through D_00447108, a pointer nothing in the game sets. | q01 |
| 001B04F0 | KF_Debug_EditU16 | Dead debug value editor for a u16; reads the controller report through D_00447108, a pointer nothing in the game sets. | q01 |
| 001B0658 | KF_Debug_EditU8 | Dead debug value editor for a u8; reads the controller report through D_00447108, a pointer nothing in the game sets. | q01 |
| 001B07C0 | KF_Debug_EditFloat | Dead debug value editor for an f32 (steps 1000/100/10); reads the controller report through D_00447108, a pointer nothing in the game sets. | q01 |
| 001B0950 |  |  |  |
| 001B0970 | KF_Vector_Dot3 | Dot product of two 3D vectors. | q01 |
| 001B09A0 |  | Wraps an angle in radians into [-pi, pi]. | src comment |
| 001B0A00 | KF_Debug_StartMenuLoop | Outer loop of the debug start menu (dead debug code, called by nothing). | q01 |
| 001B0A58 |  |  |  |
| 001B0A60 |  |  |  |
| 001B0A68 |  |  |  |
| 001B0A70 |  |  |  |
| 001B0A78 | KF_Debug_StartMenu | Debug start menu: "PUSH START" screen, then a four-line menu ("NEW GAME" and three more); texts from D_0043EF70, D_0043EF90, D_0043EFD8. Dead debug code. | q01 |
| 001B0E50 | KF_Debug_PickMap | Debug map picker ("MAP %3.3d", 0..254); saves, blanks and restores D_00432E20[0],[30] and D_00433050[0],[30] around its loop. Dead debug code. | q01 |
| 001B1028 |  |  |  |
| 001B1030 | KF_Debug_EditNumber | Debug four-digit decimal editor ("INI:"); saves, blanks and restores D_00432E20[0],[30] and D_00433050[0],[30] around its loop. Dead debug code. | q01 |

## Hit system and damage (0x1B1340-0x1B4EC8)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001B1340 | KF_Shot_ResetRings | Empties both rings of 0x30-byte shot slots and sets D_00446928 (g_dmgLassBossCnt) to 3. | q01 |
| 001B1390 |  | Claims the next 0x30-byte slot from a ring of slots; the index wraps at 0x100. | src comment |
| 001B1400 | KF_Shot_ClaimSecond | Claims the next slot of the second shot ring (ring D_00577348, index D_00447110); twin of func_001B1390. | q01 |
| 001B1470 | KF_Hit_Run | Runs an attack request (Unk1B1470Request): sets the scratchpad result kind to 6, dispatches on (u8)flags & 3: shape 0 func_001B1510, 3 func_001B1B90, 1 and 2 the stubs func_001B1B80/func_001B1B88. | q01 |
| 001B1510 | KF_Hit_RunSphere | Hit test shape 0 (sphere) per target mask byte 0x4D: map objects, player, entities, map meshes via D_00402D88 handlers. Returns 1 player, 2 entity, 4 map object, 8/0x10/0x20 map, 0 nothing. | q01 |
| 001B1B80 |  | Stub for hit shape 1 in func_001B1470: only returns 0. | q01 |
| 001B1B88 |  | Stub for hit shape 2 in func_001B1470: only returns 0. | q01 |
| 001B1B90 | KF_Hit_RunSwept | Hit test shape 3 (swept): player, entities; when the owner is the player uses the longer tests func_0020FCF8/func_00210678 and tests map objects and the map along the weapon edges D_00414780/D_004147A0 arm points. | q01 |
| 001B21F8 |  | Hit sphere against a map model's strip lists 0, 1, 2, 5 (D_0043F1F0): places the mesh points in the world (inlined func_001C2D08), then every triangle through func_0020DF38. D_00402D88[0]. Name family KF_Hit_TestMapModel... | q01 |
| 001B24B0 |  | Hit sphere against a map model's strip lists 1 and 5 (D_00446918): places the mesh points in the world, then every triangle through func_0020DF38; twin of func_001B21F8. D_00402D88[1]. Name family KF_Hit_TestMapModel... | q01 |
| 001B2740 |  | Hit sphere against a map model's strip lists 0 and 2 (D_00446920): places the mesh points in the world, then every triangle through func_0020DF38; twin of func_001B21F8. D_00402D88[2]. Name family KF_Hit_TestMapModel... | q01 |
| 001B29C8 | KF_Hit_LandOnPlayer | A hit lands on the player: marks the attacking entity (flag bit 41 weapon, 42 otherwise), nine func_001B4AA8 damage terms against the player's defence (D_00413F80.unk90), func_001BB260(damage), ailments, reactions. | q01 |
| 001B2C48 | KF_Player_RollAilments | A hit may start the player's six timed ailments: six chances against PlayerState.unkDC[i] (min 5%), skipped while flag_timers[i] runs; sets timed_flags[i] to 1800/150/3600/5400/1800/18000 frames. | q01 |
| 001B2F70 | KF_Entity_ApplyHit | (request, extra, entity, f32 scale): a hit lands on an entity: nine strengths (func_001B3ED8) against EntityParams.unkD4..unkE4 resistances via func_001B49F8, double/kill chances, unk2C (HP) -= damage, unk80 = 15 cooldown, flag bits 36-39, knock-back, life/mana steal, ailments (func_001B3908). Type 99 (last boss) takes no damage and counts D_00446928 down. (alt name: KF_Combat_DamageEntity) | q10 |
| 001B3908 | KF_Entity_RollAilments | A hit may start an entity's four ailments: chances 0, 2, 4, 5 against EntityParams.unkEA..unkED; sets Entity.unk20=1800, unk24=3600, unk28=1800, unk2A=3600 and Entity.unk26=1..4 (tint). | q01 |
| 001B3AC0 | KF_Hit_ApplyToMapObject | A hit lands on a map object: damage from the nine strengths against MapObjectUnk04.unk52[9], breaks it (func_0011E228) when it reaches the object's toughness; else the player's weapon bounces off. Sibling of func_001B2F70. | q02 |
| 001B3E78 |  |  |  |
| 001B3ED8 | KF_Combat_GetHitStrengths | (out, request, extra): the nine effective strengths of a hit. Player: raised by extra->unk20 (weapon, first three) or unk22 in fiftieths, magic adds unk0E[i] / 50; entity with kind 0: copied; otherwise physical three zeroed and six magic ones times EntityParams.unk238[i] tenths (or 10). Rounded up when over 0.001 remains. | q10 |
| 001B49F8 |  | Damage term of a hit on an entity: (strength, raw strength, bonus, resistance); summed for each resistance below 100 by func_001B2F70. | q10 |
| 001B4AA8 |  | Damage term: attack strength against the defender's defence value; called nine times by func_001B29C8. | q01 |
| 001B4B10 | KF_Player_AddWeaponExp | Adds experience to the weapon in hand (LevelRecord of D_003679A0); level counts down to 0x32. | q02 |
| 001B4BC8 | KF_Player_AddArmorExpOnHit | Player was hit: shield/helmet/armor/gloves/boots gain experience by damage*10/max HP; double without a shield, four times for the piece hit (Request.unk4E or random 1-4). | q02 |
| 001B4DF8 |  | Adds experience to a record, rolling over into levels (levels below 0x33 reset to 0x32). | src comment |
| 001B4EC8 |  | Adds amount to the player's running total at 0x2A0. | src comment |

## Effect table and effect hit checks (0x1B4EE0-0x1B4F90)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001B4EE0 | KF_Effect_LoadTable | Loads the effect table D_00402DA8 from file D_0043F248. | q02 |
| 001B4F30 |  | Launches an effect: func_001B4F30(0x7C, shot, player, rotation, map slot) for worn item 0x32. | q10b |
| 001B4F90 | KF_Effect_CheckHit | Hit check of an effect actor: builds an Unk1B1470Request from D_00402DA8[effect_param].hit_checks[def->unk10B] (shape, radius, strengths, flag bits) or from PlayerShot.unk04 for a weapon's shot, runs func_001B1470 and returns its result; on a player hit the row's unk24 goes to the actor's countdown. | q02b |

## Pad input and rumble (0x1B5390-0x1B5808)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001B5390 |  |  |  |
| 001B53E8 |  | Store a fixed set of 14 bit-mask constants into consecutive globals. | src comment |
| 001B5460 | KF_Pad_Update | Per frame: reads pad 0 (func_001B5590), copies D_00413DB8[0] to D_00413DA8, runs the rumble request. | q02 |
| 001B5590 | KF_Pad_ReadPort | Reads one controller port into PadState; right stick up/down become buttons 0x200/0x100, left stick becomes the d-pad bits. | q02 |
| 001B5778 | KF_Pad_Reset | Clears the rumble request and updates the pad. | q02 |
| 001B57A0 | KF_Pad_Rumble | Requests a rumble (strength, duration, fade flag) if the option D_00349686 is on. | q02 |
| 001B57D8 | KF_Pad_MuteRumble | Sets D_0044694E once and stops the actuators. | q02 |
| 001B5800 |  |  |  |
| 001B5808 |  |  |  |

## Player: init, per-frame update, actions, movement, status (0x1B5810-0x1B96C8)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001B5810 | KF_Player_Init | New game: clears PlayerState D_00413F80 (0x300 bytes), its copy D_00414280 and the screen tint, sets the starting values. | q02 |
| 001B5A10 |  | Clears PlayerState unk264/unkBC/unkC0/unk1A4/unk1A6 and picks unk264 again (func_001BB5E8). | q02 |
| 001B5A48 | KF_Player_Update | The player's frame, called once per frame from func_001001C0: map record index check, pad read (D_00413DA8) and masking, copy to D_00414280, magic button D_00446944 (cast via func_001BC9E0), running (status bit 35, button D_00446940), movement callees, stride unk1C0 and head bob unk1C4, footstep sounds by unk210, low-health sound 0x29, func_001B6FA0, func_001B69D0, timers, item 0x32 random shot, func_001BB978. | q10b |
| 001B6800 | KF_Player_DrawScreenTints | Draws the colours laid over the screen (PlayerScreenTint, D_00414580). | q02 |
| 001B69D0 | KF_Player_UpdateScreenTints | Per frame: works out PlayerScreenTint (D_00414580): newest timed flag's colour, fade in/out, cycling colour of flag 5 (D_00413EE8), map glow MapDrawState.unk2A90 for magic 0x3D-0x3F or item 0x15F, dark tint for a cursed weapon (item 0x2B without the full set). | q02b |
| 001B6FA0 | KF_Player_UpdateActions | The player's actions for a frame: item handler D_00413F50[item.unk63](held, pressed), then the magic button (func_001BC9E0 / func_001BD208). | q02 |
| 001B70F8 | KF_Player_SwingWeapon | Item handler of a swung weapon (D_00413F50 family): button starts the swing (fills PlayerShot PlayerState.unk268), between motion frames def->unk78..unk7A sends a shape-3 request to func_001B1470, plays the swing sound, casts the weapon's two magics (def->unk42, def->unk52) in their frame windows. | q02b |
| 001B7788 |  |  |  |
| 001B77E0 |  | Item handler (D_00413F50 family): on the button, starts motion 0 and launches the item's shot (PlayerItemDef.unk52) if D_00369830[def.unk62] is nonzero. | q02 |
| 001B7918 |  | Item handler of a drawn weapon (bow): strength unkBC builds while held (max 5000), letting go fills an Unk1B1470Request in ring D_0057A350 and plays motion 1, anything else lowers the weapon; uses one of D_00369830[def.unk62] (ammunition). | q02 |
| 001B7C90 |  | Calls func_0019CE00(1), sets PlayerState.unkBC = 0 and increments unk258. | q02 |
| 001B7CC8 |  | Animates the weapon model until the motion ends, then clears PlayerState unk298 and unk258. | q02 |
| 001B7D38 |  | Animates the weapon model; when the blend is over calls func_001A35D0 and clears D_004141D8. | q02 |
| 001B7D80 |  |  |  |
| 001B7DF8 |  |  |  |
| 001B7EE0 | KF_Player_Move | One frame of player movement from the button mask (u16 buttons): carry by map object under the player, movement constants from tables D_00413DE0..D_00413ED0 by movement mode, carry-weight and status slow-downs, knock-back along aim, walk/turn/look from button masks D_0044692C..D_0044693A, gravity, 25-unit collision sub-steps, landing sound/fall damage/view dip, conveyor push from the MapRecord, death when unk1AC > 1000 (fell out of the world) | q11 |
| 001B9078 | KF_Player_ApplyGroundEffect | What the ground under the player does (PlayerState.unk1E4 1..5): poison by chance (20%/50% less resistance unkDC[0], min 5%, every 60th frame), poison at once, 10 damage, or 1 damage plus 1 experience to everything worn; unk1EC kills. | q02 |
| 001B92C0 |  | Fall damage: called by KF_Player_Move on landing with a fall speed of 66.67 or more | q11 |
| 001B93B8 |  |  |  |
| 001B9478 |  | Clears PlayerState unk1B0..unk1C4 and status bit 32. | q02 |
| 001B94C0 |  |  |  |
| 001B9588 |  | Counts six countdown timers down to zero, clearing each matching flag on the way. | src comment |
| 001B9610 |  | Run by KF_Player_Move after HP and its bar are set to 0 when the player fell out of the world (unk1AC > 1000) | q11 |
| 001B96C8 | KF_Player_UpdateStatus | Per-frame player bookkeeping on PlayerState D_00413F80: heal/damage over time (unk2B0/unk2B4), 30 timed stat-boost timers (0x14C-0x186), HP/MP regen and drain (unkF0-unkFC), status-ailment timed_flags wear-off and poison ticks via func_001BB2E8, HP/MP clamp to unk58/unk5C, weapon/magic gauges unkBC/unkC0 (0..5000) refill, eased HUD bars unk1A4/unk1A6, item 0x24 curse roll every 600 frames | q11 |

## Player stats, inventory, items, magic, experience, ladders (0x1BA310-0x1BDD38)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001BA310 | KF_Player_UpdateStats | Recomputes the player's derived statistics: max HP/MP, carrying capacity unkA8 = (unkAC * 3 + 80) * 2.2046, worn weight unkA4 (LevelDef.unk3E over nine worn things), nine attack values unk60, six weapon rates unk72, worn-item totals unk7E/unk90 via func_001BB728, unkDC, special cases for item numbers in unk41/unk43/unk44/unk3F, caps (999 for unk58/unk5C, 100 for unk72 and unkDC) | q12 |
| 001BAE88 | KF_Player_SumItemBonuses | Clears PlayerState unkDC..unk10C, adds every worn item's part (func_001BB020), turns the four rates unkF0..unkFC into frame counts (150 / rate, at least 1). | q02 |
| 001BB020 | KF_Player_AddItemBonus | Adds what wearing definition index gives: two (kind, amount) pairs switched over 12 kinds, six u16 added to PlayerState.unkDC, items 0x15A / 0x15F set unk108 / unk10C; kinds 7 and 8 assign unk100/unk104 instead of adding. | q02 |
| 001BB260 |  | Applies damage to the player (called with the computed damage by func_001B29C8). | q01 |
| 001BB2E8 |  | Called by KF_Player_UpdateStatus with 3 every 30 (or 90) frames while timed_flags[0] is set (poison damage, a guess) and with 1 every 30 frames while unkEC != 0 or timed_flags[3] is set (together with MP unkB8--) | q11 |
| 001BB308 |  | Takes amount from the counter, clamped at zero; fails for a non-positive amount or while the block is locked. | src comment |
| 001BB360 |  | Sets the screen tint D_00414580 = 10 unless a script is running. | q03 |
| 001BB390 |  |  |  |
| 001BB468 |  | Sets the player aim vector (+0x220) to the flattened, normalised direction from target, and its speed. | src comment |
| 001BB4D0 |  |  |  |
| 001BB590 | KF_Player_StopHeldSound | Stops the sound slot held in D_00446958. | q03 |
| 001BB5C0 |  | Clears PlayerState unk1A4 and unkB4 and calls func_001B9610, unless unk1A8 is set. | q03 |
| 001BB5E8 | KF_Player_PickMagic | Picks the selected magic PlayerState.unk264: forced unk3D, else unk46, else the equipped item's own magic, else the first learned slot of D_00367980 whose effect is usable. | q03 |
| 001BB728 | KF_Player_AddItemStats | Adds an item's nine stats (base plus gained, times its level in percent) to a totals array. | q03 |
| 001BB908 | KF_Player_UpdateMapArea | Finds the map area the player stands in, stores PlayerState map_index and unk250. | q03 |
| 001BB978 |  |  |  |
| 001BBA58 |  | Fades the screen toward white (D_00413F80.unk2A8 > 0) or black (< 0) by that amount. | src comment |
| 001BBB60 | KF_Player_AddExperience | Adds experience to the player: doubles the amount when PlayerState.unk44 is 0x94, caps at 999999, takes every level reached from the const table D_0043E130 (PlayerLevelStep) and shows message 0x2D for each. | q03b |
| 001BBC88 | KF_Player_CountMagicUse | Counts a use of a magic; at 50 and 150 uses its level rises with message 0x2D; every 100th cast raises PlayerState.unk2C. | q03 |
| 001BBEE0 |  |  |  |
| 001BBFC8 |  |  |  |
| 001BC058 |  | Kick off a request once the flag at 0x261 is clear and the table entry at 0x63 is not 2. | src comment |
| 001BC0E0 |  | Sets PlayerState.unk262 once, with sound 0x23 if asked. | q03 |
| 001BC128 |  | Sets PlayerState.unk263 = 1 while unk258 runs. | q03 |
| 001BC148 |  | Tells whether the inventory has room for an item (checked before taking a picked-up item). | q07 |
| 001BC1C0 | KF_Inventory_Add | Adds an item to the inventory D_00369830 (1 at most for unique ones, 99 otherwise; message 0x1E when full). | q03 |
| 001BC2A0 | KF_Inventory_Remove | Takes one of an item from the inventory D_00369830. | q03 |
| 001BC2D0 | KF_Player_UseItem | Uses the item GameStatus.unk20 names: scripts of the entity or map object in front first, then func_001BC6E8, then by type (0 restores unkB4/unkB8 and cures timed flags, 1/4/5 on the object in front, 2 map change on a kind 12 object, 6 func_0011A438, 8 "cannot use" message 0xA); use_mode says taken or exchanged. | q03 |
| 001BC6E8 | KF_Player_ApplyItem | Item effects by item id: 0x15/0x16 start the 1800-frame timers unk19C/unk1A0, 0x1B/0x1C raise unk2C/unk28, 0x46-0x52 raise unk24 and learn a magic slot through func_001BC990. | q03 |
| 001BC968 |  | Item 0x12: starts PlayerState timed_flags[0] for 1800 frames. | q03 |
| 001BC990 |  | Marks the first free of a row's five magic slots (learning a magic). | q03 |
| 001BC9E0 | KF_Player_CastMagic | Casts magic magic_id: refuses on id 0, deep water, unk40 == 0x49 with magics 1-15, magic gauge unkC0 < 5000 or MP unkB8 < FxTableEntry.unkFC (cost); takes the cost; magics 7-9 arm 30-frame timer D_00447158 and a slot of ring D_0057B3A0; unk102 == 0 launches a shot (func_001B1390 + func_001B4F30); others heal, cure, set timed values unk110..unk14A and special timers, start the effect (func_001A1E28); returns 1 when something was cast | q12 |
| 001BD208 | KF_Player_HoldMagic | Keeps a held magic (7 to 9) firing: one unkB8 every third frame after the first 30 frames; shot leaves along the player's raw rotation via func_001B4F30. | q03 |
| 001BD420 | KF_Ladder_Start | Gets on a ladder (given a map object): end points D_0057B370/D_0057B380, walk to the foot D_0057B350/D_0057B360 in D_00447134 frames of 12 units, D_0044713C rungs, D_00447148 frames per rung. | q03 |
| 001BD6E0 |  | Ladder: gets off (clears player status bit 36). | q03 |
| 001BD710 |  | Dispatches the ladder state handler: table D_004145C8 indexed by state D_00447128. | q03 |
| 001BD778 |  | Ladder state: walks to the foot and turns to face the ladder. | q03 |
| 001BD808 |  | Ladder state: waits on a rung; up and down pick the next state, other buttons or the ends get off. | q03 |
| 001BD970 |  | Ladder state: climbs one rung up along a sine ease and plays func_001BDD38's sound; twin of func_001BDB58 (down). | q03 |
| 001BDB58 |  | Ladder state: climbs one rung down along a sine ease and plays func_001BDD38's sound; twin of func_001BD970 (up). | q03 |
| 001BDD38 |  | Plays the sound for the kind of object D_0044712C refers to. | src comment |

## Player against the map, arm models, weapon trail, floor and wall probes (0x1BDDC0-0x1C1858)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001BDDC0 | KF_Player_CollideWithMap | Player against the collision meshes of both loaded maps within 100 units: surface triangles (unk1E0/unk1E4 or unk1EC = death), walls (pushed 100 units out), water surfaces (status bits 38/39, unk1DC), floors (sets map_index, unk250, unk210, current map); then stands the player on the floor, starts a step-down (bit 33) or lets him fall by unk1AC; counts frames on the same surface in unk1E8; calls func_001152B8; returns found (int) | q12 |
| 001BE760 | KF_Player_PushOutOfWalls | Pushes the player 100 units out of wall triangles. | q03 |
| 001BEA50 | KF_Player_FindFloorKind | Finds the floor triangle under the player and stores what it is made of in PlayerState.unk210. | q03 |
| 001BECA8 | KF_Player_CollideWithEntities | Player against every entity: with climb, first put on top of any entity low enough to step onto, then pushed out of every entity touched (max 50 a frame); kinds 0x11/0x14/0x97/0x9B use up to four bodies (EntityParams.unk20). Step over 16.67 sets status bit 33 and unk1D0/unk1D4. | q04 |
| 001BF350 | KF_Player_CollideWithMapObjects | Player against map objects (through func_001C2A38 / func_001C2758), with optional climb; a newly touched object whose definition unk52 is 1..6 starts that timed flag (1800/150/3600/5400/1800/18000 frames). Step over 16.67 sets status bit 33. | q04 |
| 001BF5F8 |  |  |  |
| 001BF650 |  |  |  |
| 001BF6E8 |  |  |  |
| 001BF788 |  |  |  |
| 001BF898 | KF_Player_LoadArmModels | Loads the held item's model (D_0057B6A0) and the arm's (D_0057B860) when PlayerState.unk3C (item) or unk41 (arm) changed: reads item_arm/i%04d.arm (fallbacks i1001.arm, i5001.arm), copies it, sets up model, motion and textures. | q04 |
| 001BFCC8 | KF_Player_UpdateArmModels | Each frame: places the item's model in front of the camera, poses it, copies the pose to the arm's model, keeps model points 30 and 31 in D_00414780 (previous frame's in D_004147A0). | q04 |
| 001C00D8 | KF_WeaponTrail_Update | Each frame: ages the weapon trail and queues the two ends of the weapon (model points 0 and 1) projected to the screen. | q04 |
| 001C0268 | KF_Player_DrawArm | Draws the held item's and the arm's models, then the weapon trail as one shaded, blended triangle strip whose alpha fades with age. | q04 |
| 001C06E8 | KF_WeaponTrail_PickKind | Picks the weapon trail kind D_00447188 (row of D_00414660): the largest of six PlayerState.unk18E values whose unk122 is set, else by unk43 0x87 to 0x8A. | q04 |
| 001C0840 |  |  |  |
| 001C0890 |  |  |  |
| 001C08A0 |  | Returns a weapon trail point's position (bounds test index < count); sibling of func_001C0A28. | q04 |
| 001C08F0 | KF_WeaponTrail_Age | Ages the weapon trail ring (D_0057DB40 points, D_0057DF40 frames left) and drops expired points. (report writes the name as _Age) | q04 |
| 001C0988 | KF_WeaponTrail_Push | Queues one screen point into the weapon trail ring (D_0057DB40, write index D_00447180). (report writes the name as _Push) | q04 |
| 001C0A28 | KF_WeaponTrail_GetLife | Returns a weapon trail point's frames left (D_0057DF40); bounds test is index <= count. (report writes the name as _GetLife) | q04 |
| 001C0A80 |  |  |  |
| 001C0A88 | KF_Collision_DropOnTriangle | Vertical line through point against a triangle (a, b, c, point); on a hit the scratchpad result gets the height and the rise. | q04 |
| 001C0C50 |  | Vertical line through point against a triangle with a limit (above the point, by no more than reach); variant of func_001C0A88, no caller known. | q04 |
| 001C0E40 | KF_Collision_FindFloorAbove | (reach, point): the lowest floor (CollisionMesh.floor_strips) of the loaded maps' records above point within reach; returns TRUE and leaves the hit in the scratchpad. | q04 |
| 001C1138 |  | Twin of func_001C0E40 for the CollisionMesh unk2C strips: lowest such surface of the loaded maps' records above point within reach; returns TRUE and leaves the hit. | q04 |
| 001C1430 | KF_Collision_SweepPoint | (scale, position, direction): sweeps a point through the loaded maps' records and leaves the nearest hit; tests the four strip lists picked by D_004147C0 = {0, 1, 2, 5}. | q04 |
| 001C1858 |  | Variant of func_001C1430 taking a bit mask of strip lists (bits 0 to 6) instead of the fixed four: sweeps a point through the loaded maps' records, nearest hit; no caller known. | q04 |

## Collision against map objects and map records (0x1C1C80-0x1C48A8)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001C1C80 | KF_Collision_HitRayMap | Ray (segment) from position to target against the map records of both loaded maps, nearest hit; a record is only tested when the start point is within one unit of its box. | q05 |
| 001C2128 | KF_Collision_HitRayObj | Ray test for map objects that have a collision mesh or kind 1/2/6/0x11 | q19, q05 |
| 001C21C0 | KF_Collision_HitSphereObj | Sphere vs one map object: reach test, then dispatch on the object's kind (MapObjectUnk04.kind: 1 box, 2 two boxes, 17 quad, 18 pitfall, else mesh strips). | q05 |
| 001C2300 | KF_Collision_HitSpherePushObj | Sphere vs one map object with two float parameters (radius, push): reach test, then dispatch on the object's kind. | q05 |
| 001C24C0 | KF_Collision_HitFanObj | Fan (sector) vs one map object: reach test, computes the fan's two edge points from heading +- spread, then dispatch on the object's kind. | q05 |
| 001C2758 | KF_Collision_HitCircleAllObjs | Circle vs every map object of both loaded maps; stores the distance in MapObject.unk18 (FLT_MAX when out of reach). | q05 |
| 001C2A38 | KF_Collision_FindFloorObj | Point vs every map object of both maps that can be stood on; returns TRUE/FALSE and for the player stores the object in PlayerState.unk208. | q05 |
| 001C2D08 | KF_Collision_TransformMeshPoints | VU0: multiplies a model's collision points by the model matrix into the scratchpad (COLLISION_WORLD_POINTS 0x70000100); returns nothing. | q05 |
| 001C2E48 | HitRayMeshObj | Ray vs a map object of any other kind: the model's collision strips. | q05 |
| 001C2F30 | HitRayBoxObj | Ray vs map object kind 1 (box: corner points 0-3, point 0x1F gives the height); does not check the model points for NULL. | q05 |
| 001C31E8 | HitRayDoorObj | Ray vs map object kind 2 (two boxes, points 0-3 and 4-7; one box 0,1,6,7 while animation 4 is on frame 1). | q05 |
| 001C3910 | HitRayQuadObj | Draws the first three points of a model, and a second triangle unless the scratch flag is set. | src comment, q05 |
| 001C3A18 | HitRayPitfallObj | Ray vs map object kind 18 (pitfall). | q05 |
| 001C3D00 | HitSphereMeshObj | Sphere vs a map object of any other kind: the model's collision strips. | q05 |
| 001C3DF8 | HitSphereBoxObj | Sphere vs map object kind 1 (box); returns the hit flag. | q05 |
| 001C3FA0 | HitSphereDoorObj | Sphere vs map object kind 2 (two boxes; single box 0,1,6,7 while animation 4 is on frame 0 or 1); returns the hit flag. | q05 |
| 001C43A8 | HitSphereQuadObj | Sphere vs map object kind 17 (quad of points 0-3). | q05 |
| 001C4498 | HitSpherePitfallObj | Sphere vs map object kind 18 (pitfall); name certain from its debug print "Hit HitSpherePitfallObj()" (D_0043F5E8) on every hit. | q05 |
| 001C47A0 |  |  |  |
| 001C48A8 | HitSpherePushBoxObj | Sphere with push vs map object kind 1 (box): four walls, then four corner posts; returns the hit flag. | q05 |

## Map object collision shapes and push-out, dropped items (0x1C4B10-0x1C8F18)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001C4B10 | KF_MapObject_TestCircleOutline | Circle against the walls and corner posts of a map object's outline (points 0,1,6,7 while animation 4 is on frame 0/1, else quads 0-3 and 4-7); stops at first hit, no push; result in the scratchpad CollisionResult. | q06 |
| 001C5170 |  | Draws up to two segments through three or four model points. | src comment |
| 001C5278 |  | Empty function: pitfall (kind 18) case of the sphere-with-push family, still called with full arguments. | q05 |
| 001C5280 | KF_MapObject_TestCapsuleMeshInWedge | Capsule against every mesh triangle of a map object, culling triangles outside the wedge origin->(first)/(second); stores the surface id in result.unk18. | q06 |
| 001C5538 | KF_MapObject_TestCapsuleWallsInWedge | Kind 1 map object: capsule against the four outline walls with the same wedge culling as func_001C5280; returns early on a stale CollisionResult hit. | q06 |
| 001C58F0 | KF_MapObject_CollideDoorInWedge | Per-kind collision test for a double door/gate MapObject (two quads of model points 0-3 and 4-7, height from point 0x1F): for each outline edge not wholly outside the wedge origin->(first_x,first_z) / origin->(second_x,second_z), calls func_00210258(point_a, point_b, height, origin, radius, push); returns as soon as COLLISION_RESULT->hit is set; closed outline 0,1,6,7 while anims[4] frame param < 2 | q13 |
| 001C6360 | KF_MapObject_TestCapsuleFaceInWedge | Kind 17 map object: capsule against the two triangles of the face 1,2,0,3 with wedge culling (side bits not shifted/masked in the loop, retail bug). | q06 |
| 001C65E8 |  | Empty function: pitfall (kind 18) case of the fan family, still called with full arguments. | q05 |
| 001C65F0 | KF_MapObject_PushOutOfMeshWalls | Pushes a capsule out of a map object's mesh wall strips; for kind 10 and the player it records the touched object in PlayerState.unk20C. | q06 |
| 001C68B0 | KF_MapObject_PushOutOfOutline | Pushes out of a map object's four outline walls (by push) or the first corner post (push + 0.1). | q06 |
| 001C6B88 | KF_MapObject_PushOutOfDoor | Pushes a circle (centre, radius, push) out of a double door/gate MapObject: calls model->unk1B4, then walls (func_00210088) and corner posts (func_0020F0D8) of the outline, moving centre x/z after each hit without stopping at the first; closed outline while param == 1; with leaves open walls off the gaps 2-5 and 4-3 when narrow (<= push * 2.25) or, for the player, when object->unk60 bit 0 is set | q13 |
| 001C7960 | KF_MapObject_PushOutOfSide | Pushes out of the upright side point0-point3 of a map object; skipped when the face normal is within 20 degrees of vertical (abs n.y > 0.93969). | q06 |
| 001C7AF0 |  | Empty function: pitfall (kind 18) case of the circle family, still called with full arguments. | q05 |
| 001C7AF8 | KF_MapObject_DropOnMeshFloor | Drops a ball on a map object's mesh floor: y = lowest hit - 1. | q06 |
| 001C7C60 | KF_MapObject_DropOnFace | Drops a ball on a map object's face: y = lowest hit - 1. | q06 |
| 001C7E48 | KF_MapObject_DropOnFaces | Drops a ball on a map object: the mesh floor while animation 4 rests, else the eight quads from 32 model points; y = lowest hit - 1. | q06 |
| 001C8238 |  | Resets the model node lists and marks every unset item record state as 1. | src comment |
| 001C82C0 | KF_Item_SetUpMap | (map): per-map item set-up after a map is loaded: D_00447190[map] = header->item_count, loads model/texture blocks per item kind into ItemModelSource D_0057E040[map][32] (ids D_004469C0[map]), indexes fixed items in D_005B5EC0[map][def->id], builds their ItemNode and model D_005816C0[map][i], then re-lays the 16 saved DroppedItemRecords with func_001C8F18. | q06b |
| 001C87D8 | KF_Item_RollMapItems | Rolls a map's fixed items: 16 group rolls vs D_004469C8[map] thresholds, then each item's own chance. | q06 |
| 001C8938 |  |  |  |
| 001C8990 | KF_Item_MoveMapItems | (map, offset): shifts the item defs, models and dropped nodes of a map. | q06 |
| 001C8AF0 |  | Returns the node at the current map slot for slot, or NULL when the index is invalid or the node is unused. | src comment |
| 001C8B58 | KF_Item_PlaceMapItem | (map, index): places a map's fixed item by index; tail-calls the void func_001CA500 on success, so returns nothing defined there. | q06 |
| 001C8C80 | KF_Item_PlaceMapItemById | (map, id): places a map's fixed item by id; returns func_001C8B58's value. | q06 |
| 001C8CB0 | KF_Item_DropFromEntity | (entity): drops the spawn record's item when flag bit 62 is set (priority 1), else the first/second drop of the entity kind by percent chances; drops at the floor found by func_001C0E40(2000, pos). | q06b |
| 001C8F18 | KF_Item_LayDropped | (map, item, priority, position, floor_y, offset, rotation, scale, order): lays down a dropped item node. | q06 |

## Items lying in maps and gold piles (0x1C9340-0x1CB710)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001C9340 | KF_Item_SpawnForObject | (map, item, position, rotation, scale, object): takes one of the 32 item nodes at D_005807C0[map] for a map object (chest contents); returns the node, or NULL if the player may not get the item, the object already has one out, no node is free, or the item's model is not loaded. | q07 |
| 001C9580 | KF_Item_FindForObject | (object): returns the item node whose unk08 is the given map object. | q07 |
| 001C95B0 |  |  |  |
| 001C9650 | KF_Item_DrawAll | (with_picked): draws the items lying in the maps (ItemNode lists at D_004469D4). | q07 |
| 001C97E8 | KF_Item_DrawPicked | (hidden): draws the item being picked up (D_004469D8) on top, every part's offset and turn scaled by D_0044719C (1 lying, 0 held up). | q07 |
| 001C9AB0 | KF_Item_StartPickup | (node): starts picking up an item node; returns int. | q07 |
| 001C9B00 |  |  |  |
| 001C9B58 |  |  |  |
| 001C9C00 | KF_Item_UpdatePickup | Runs the current item pick-up step: handler table D_004147E0 indexed by D_004471A4. | q07 |
| 001C9C40 |  | Item pick-up step 0: remembers where it lay (D_005B60C0/D0), aims at a point 150 in front of the player (D_005B60E0), per-frame step D_005B60F0, frame count D_004471AC/B0; sets GameStatus.unk30 = 0x87F. | q07 |
| 001C9EB0 |  | Counts down a blend: steps the actor's vectors toward a target, resetting when done. | src comment |
| 001C9FB0 |  | Item pick-up step 2: turns the item in front of the player; pad 0x4000 takes it (if func_001BC148 says there is room, else message 0x1E), pad 0x2000 puts it back. | q07 |
| 001CA0E0 |  | Item pick-up step 3: flies to the player, func_001BC1C0(item), marks where it came from (D_00356928 counter, func_0011E608 for an object, or clears its DroppedItemRecord), frees the node. | q07 |
| 001CA1E8 |  | Item pick-up step 4: flies the item back to where it lay and lets go. | q07 |
| 001CA338 | KF_Item_ClearMap | (map): clears a map's item nodes and tables. | q07 |
| 001CA498 |  | Scans the slot's list of up to 32 ids for key; returns the matching 8-byte entry, or NULL. | src comment |
| 001CA500 |  | Pushes a node onto the front of the list at D_004469D4. | src comment |
| 001CA520 |  | Unlinks node from the model node list and returns the node that followed it. | src comment |
| 001CA578 |  | Sets GS state for an item model (the draw callback of item models). | q07 |
| 001CA5B0 | KF_Item_NodeIndexForId | (map, id): looks up a fixed item's node index in D_005B5EC0. | q07 |
| 001CA5E0 |  |  |  |
| 001CA630 |  | Tells whether none of the five tables after D_00369830 (D_003699B0.., each +0x180) holds a one-of-a-kind copy of this item. | q07 |
| 001CA788 |  |  |  |
| 001CA818 | KF_Gold_Init | Initialises the gold piles (MapActor, 16 per map at D_005B6100, in-use list D_004469E0, model item/i0000.om2). | q07 |
| 001CA920 | KF_Gold_InitMap | (map): sets up the map's gold pile models, then re-drops what the map's ObjectMarks recorded and clears them. | q07 |
| 001CACA0 | KF_Gold_UnlinkMap | (map): unlinks a map's gold piles from the in-use list D_004469E0. | q07 |
| 001CACF8 | KF_Gold_MoveMap | (map, offset): shifts a map's gold piles by the map offset. | q07 |
| 001CAD80 | KF_Gold_Update | Per frame: fades gold piles in/out, finds the nearest in reach within 200; pick-up adds amount to PlayerState.unk4C (gold, capped at 999999) and shows message 0x14 with the amount. | q07 |
| 001CB008 | KF_Gold_Draw | (list): draws the gold piles of a list. | q07 |
| 001CB0D0 | KF_Gold_Drop | (position, rotation, map, amount, object_id, on_ground, other_map): drops a gold pile; returns TRUE when dropped. | q07 |
| 001CB2C8 | KF_Gold_DropFromEntity | (entity): drops the entity's gold (EntityParams.gold at 0x230) as a pile via func_001CB0D0; nothing when it has none. | q07 |
| 001CB460 |  | Returns the first free gold pile actor (MapActor). | q07 |
| 001CB4A8 |  | Returns the oldest gold pile actor not owned by an object. | q07 |
| 001CB508 |  | Links a gold pile actor into the in-use list. | q07 |
| 001CB530 |  |  |  |
| 001CB588 |  |  |  |
| 001CB690 |  |  |  |
| 001CB710 |  |  |  |

## Sound driver front end (0x1CB778-0x1CDE28)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001CB778 | KF_Sound_Init | Binds RPC service 0x1010 (retrying; spins forever if bind fails), points D_004471D4 at D_005BF380, asks the driver for its buffers (commands 0x10, 0x30, 0xA0), resets every voice, bank, the 126-entry table at SoundState 0x240 and the 4 effect slots. | q08 |
| 001CB9A0 |  |  |  |
| 001CBA58 |  | Polls the SIF RPC status until it is no longer 1, giving up after 0x989680 tries. | src comment |
| 001CBAC0 | KF_Sound_IsRpcBusy | sceSifCheckStatRpc on the sound RPC client. | q08 |
| 001CBAE0 | KF_Sound_Command | The game's own sceSdRemote (int blocking, ...): packs a command and up to 8 words into D_005BF280, waits for the client (func_001CBA58), calls the IOP, returns the driver's answer. Bit 0x80000: reply to buffer in arg 0; bit 0x40000: send buffer is arg 0. | q08 |
| 001CBC30 |  | Starts an upload of data to the sound driver (continued by func_001CBD20 in 0x4000-byte pieces). | q08 |
| 001CBD20 | KF_Sound_UploadStep | Continues the upload func_001CBC30 started: when the DMA piece has landed, command 0xB0 writes it, then the next 0x4000-byte piece is sent; returns 1 when done or idle, 0 while busy. | q08 |
| 001CBE60 | KF_Sound_Enable | Sound output on (command 0x40), tracked in SoundState.active. | q08 |
| 001CBEA0 | KF_Sound_Disable | Sound output off (command 0x50), tracked in SoundState.active. | q08 |
| 001CBED8 | KF_Sound_SetMono | Selects the second (unk04) header block for every voice and bank: flag at SoundState 0x4D4, command 0xC0, voice->unk1C, then rebinds voices (0x1010) and banks (0x2070). Probably the mono/stereo output switch (name is a guess). | q08 |
| 001CC018 | KF_Sound_SetStereo | Selects the first (unk00) header block for every voice and bank: flag at SoundState 0x4D4, command 0xC0, voice->unk1C, then rebinds voices (0x1010) and banks (0x2070); twin of func_001CBED8 (name is a guess; report writes SetStereo). | q08 |
| 001CC160 | KF_Sound_FindFreeVoice | Returns the first free voice index or -1. | q08 |
| 001CC1A8 | KF_Sound_LoadVoiceFiles | Loads a voice from files (header file, optional sample file). | q08 |
| 001CC400 | KF_Sound_LoadVoice | Loads a voice from memory (header data + sample data, both through func_001CBC30); accepts ids 0..15. | q08 |
| 001CC6A0 | KF_Sound_CloneVoice | New voice with its own header but the sample data of an existing voice. | q08 |
| 001CC888 | KF_Sound_LoadVoiceChunked | Loads a voice from memory like func_001CC400, the sample data sent through a temporary IOP block in pieces (commands 0x80/0x90). | q08 |
| 001CCC08 |  | Step function of the staged sound load func_001CD020 (table D_0043F670): gives the voice up. Name family KF_Sound_LoadStep_*. | q08 |
| 001CCC28 |  | Step 0 of the staged sound load func_001CD020 (table D_0043F670): clears the request, allocates, uploads the header. Name family KF_Sound_LoadStep_*. | q08 |
| 001CCD78 |  | Rounds the request size up to 16, then allocates the largest IOP heap block it can (halving from 1 MB). | src comment |
| 001CCDF0 |  |  |  |
| 001CCEB8 |  | Step function of the staged sound load func_001CD020 (table D_0043F670): waits for a sample piece and writes it. Name family KF_Sound_LoadStep_*. | q08 |
| 001CCF80 |  |  |  |
| 001CD020 |  | Runs the handler for the current step with a packed argument block, then advances the step. | src comment |
| 001CD0E0 | KF_Sound_LoadVoiceAt | Like func_001CC888 (chunked voice load) into a given voice and a given sound memory address. | q08 |
| 001CD3D0 | KF_Sound_AdoptVoice | Makes a voice from blocks already on the IOP; leaves the voice taken when it fails. | q08 |
| 001CD578 |  |  |  |
| 001CD5E0 |  |  |  |
| 001CD6B8 |  | Sends sound driver command 0x80 with one argument. | q08 |
| 001CD6E0 |  | Releases a voice; called by the voice loaders on failure. | q08 |
| 001CD7E0 |  | Release the IOP heap block of a voice; returns -1 if none was held or the free failed. | src comment |
| 001CD840 | KF_Sound_FreeVoiceHeaders | Frees a voice's two header blocks on the IOP. | q08 |
| 001CD8C8 | KF_Sound_Defragment | Compacts sound memory: shell-sorts the voices by sample address, moves each down over the gaps (commands 0x80/0x90), rebinds voices and banks. | q08 |
| 001CDB58 | KF_Sound_SetVoiceAddress | Sets a voice's sample address. | q08 |
| 001CDB80 |  | Sends sound driver command 0x1040 with the voice id and a byte. | q08 |
| 001CDBA0 |  | Sends sound driver command 0x1050 with the voice id and a byte. | q08 |
| 001CDBC0 |  |  |  |
| 001CDC18 |  |  |  |
| 001CDD20 |  |  |  |
| 001CDE28 | KF_Sound_LoadBank | Loads a bank file to the IOP and binds it to a voice (commands 0x1010, 0x2000). | q08 |

## Sound bank loads and note commands (0x1CDF90-0x1CF2E8)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001CDF90 | KF_Sound_LoadBankStep | Stepped bank load: the work of func_001CE170 in three steps kept in D_004469F8; returns -1 while working (also -1 for a bad bank id, with D_004471C8/D_004471C4 already overwritten). | q09 |
| 001CE170 | KF_Sound_LoadBank | (bank_id, voice_index, data, size): blocking bank load: first free of the 4 banks, sceSifAllocIopHeap, upload (func_001CBC30 then busy-wait on func_001CBD20), driver commands 0x1010 and 0x2000; frees through func_001CE408 on failure. | q09 |
| 001CE300 | KF_Sound_BindBank | (bank_id, voice_index, iop_heap, size): binds a bank for data already in IOP memory into the first free of the 4 banks; -2 when none is free, -1 when iop_heap is 0. | q09 |
| 001CE408 |  | Frees a bank's IOP memory (used by the bank loaders on failure). | q09 |
| 001CE468 |  | Bank driver command sibling of func_001CE4D0; shares the off-by-one bank index check (>= 5 with four banks). | q09 |
| 001CE4D0 |  | Sends sound driver command 0x2010 for a bank (banks[n].unk0C, the driver's id for the bank); index check is >= 5 though there are four banks. | q09 |
| 001CE510 |  | Bank driver command sibling of func_001CE4D0; shares the off-by-one bank index check (>= 5 with four banks). | q09 |
| 001CE568 |  | Sends sound driver command 0x2050 for a bank that holds memory (banks[n].unk0C); index check is >= 5 though there are four banks. | q09 |
| 001CE5C0 |  | Sends bank bank_index's second word to the driver if the bank holds IOP memory; -1 if not. | src comment |
| 001CE620 | KF_Sound_SetBankVolume | Sends sound driver command 0x1060 (bank's driver id banks[n].unk0C, s16 value); func_0019C7D0 calls it with a level 0..0x7F, so the name is "likely". | q09 |
| 001CE658 |  | Sends sound driver command 0x2080 (bank's driver id banks[n].unk0C, u16 value). | q09 |
| 001CE690 | KF_Sound_NoteOn | (slot, voice_index, channel, program, note, volume, bend, pan): queues a MIDI-like byte stream (program/pan/bend/volume/note) for the IOP in the uncached buffer at dma_dest (limit 0x3FC); pan forced to 0x3F when the u16 at SoundState 0x4D4 or the voice's unk1C is 0. | q09 |
| 001CE8B0 |  | Sound note-off of the NoteOn family (queues FD 10 ... 00 00); matched earlier. | q09 |
| 001CE9A0 | KF_Sound_NoteControl | (kind, slot, value): queues one 10-byte controller command for a sounding note (FF voice-id 07 FD kind ch note slot ...); kind 0 = value, 1 = pan, 2 = bend as two 7-bit halves. | q09 |
| 001CEB38 |  |  |  |
| 001CEBE0 |  | Starts a DMA transfer with a 16-byte aligned size and waits for it, then clears the uncached word. | src comment |
| 001CECB8 |  |  |  |
| 001CED78 |  |  |  |
| 001CEE30 |  | Sends sound driver stream command 0x30C0 (stream, two values); returns the stream or -1. | q09 |
| 001CEE78 |  |  |  |
| 001CEF48 |  |  |  |
| 001CEFA0 |  |  |  |
| 001CF008 |  | Copy entry index (0-4) of the table into the current record, then pass it to func_001CBAE0. | src comment |
| 001CF070 |  | Opens the file (retrying until it works), takes its size, and rounds it up to a multiple of 64. | src comment |
| 001CF110 |  | Always returns 0 (the load whose failure func_001CC1A8 checks). | q08 |
| 001CF168 |  |  |  |
| 001CF1F0 |  | Sends slot voice_index's two words with param; the reply is re-packed into *status (some fields inverted). | src comment |
| 001CF2E8 |  | Send voice's two words with value, and status with its bit fields re-packed (some inverted). | src comment |

## On-screen message module (0x1CF378-0x1CFFA0)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001CF378 | KF_Message_Init | Loads the two text files, links the six message slots into a ring (slot 5's text is the sixth 0x400 buffer of D_005BF8C8, D_005C0CC8), clears the debug log, calls func_0020B2E8(D_005C50C8, 0x200) and sets the two font TEX0 register writes in D_005C58C8 (TBP0 0x3B40, 512x512 PSMT4, CBP 0x3FE0 / 0x3FE1). | g4 |
| 001CF4D0 | KF_Message_Update | Per frame: calls func_0020D088; if the message box (MessageBox D_005C58E8) is active (D_004471E0) types it out every delay frames, pages on a button press, closes over 8 frames at the 0xFFFF end mark; else if the ring is active (D_004471E1) ages the shown slot and moves to the next. Always ends in func_001CF708. | q09b |
| 001CF708 | KF_Message_Draw | Builds and queues the GS packet for the on-screen text when box, ring or debug log has something: font texture, count * 7 + 13 quadwords, register blocks, glyph GIF tag; then func_001CFFA0 for the box or ring slot, func_001CFCA0 for the debug log; clears D_004471DC and D_004471EC. | q09b |
| 001CF980 |  |  |  |
| 001CF988 |  | Opens a message box: fills the state D_005C58E8 (has its own local Ctx struct for it). | q09b |
| 001CFA70 | KF_Message_Show | (x, y, font, kind, frames, message_id, ...): formats a message into the ring of 6 MessageSlots D_005C5940: func_001D07C8 gets the format, func_001CFDB8 expands it, func_001D05E0 measures it; position x + 320 - width / 2, y + 400. Kind 1 overwrites the slot on screen, else next free slot. | q09 |
| 001CFBB0 |  | Formats a message into the next 0x108-byte debug log slot. | src comment |
| 001CFC20 |  | Starts the next debug log slot (0x108-byte lines at D_005C5A18, count D_004471EC) with one character: a line that is one colour byte. | q09 |
| 001CFC48 |  |  |  |
| 001CFC58 |  | func_001CFC58(D_00446F38): returns the text table; item description is text number item + 0x1F5 | q28b |
| 001CFC68 |  | func_001CFC68((code & 0xFFF) >> 8): applies a text colour change for a glyph code with 0x4000 set | t01b |
| 001CFC80 |  | Makes D_004471E4's slot the current one and clears its unk04. | src comment |
| 001CFC98 |  |  |  |
| 001CFCA0 |  | Draws the debug log text into the message GS packet. | q09b |
| 001CFDB8 |  | Message formatter, the vsprintf of the glyph stream: int (u16 *format, u16 *out, va_list); expands number codes (& 0x3000) into digit glyphs until the 0xFFFF end; returns glyphs written, control codes (& 0xF000) not counted. | q09b |
| 001CFFA0 | KF_Message_DrawGlyphs | (kind, font, left, top, text, count): fills the per-glyph alpha table D_005C50C8 (kind 1 ring: one fading alpha; kind 0 box: 0x80 typed, ramp behind the cursor) and draws count glyphs through func_001D0298; 0x8000 new line, 0x4000 colour, 0x800 half-width glyph; metrics D_00420BE0[font]. | q09b |

## Text glyphs, HUD, facing target and play clock (0x1D0298-0x1D25C8)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001D0298 |  | Adds one glyph to the text packet at D_004471D8 (PRIM 0x56, the texture page's register pair, colour, two corners) (unmatched) | g6 |
| 001D0588 |  | Width of one glyph (summed by KF_Text_GetWidth) | q14 |
| 001D05E0 | KF_Text_GetWidth | Width of a text string ended by -1, summing func_001D0588 per glyph | q14 |
| 001D0658 |  | Measures each line of a text stream and stores its centering offset. | src comment |
| 001D0758 |  | Produces a number's digits in reverse order into a buffer (used by the message formatter func_001CFDB8). | q09 |
| 001D07C8 | KF_Text_GetMessage | Address of message n: offset table entry n+1 of the message file at D_00446F38, then the text | q14 |
| 001D0800 | KF_Event_UpdateViewRay | Per frame: keeps last frame's view ray in the event runner (unk20/unk30/unk44), then rebuilds it from the player's eyes (y - 220 + unk1F0 + camera unk4C, pitch clamped to +-58 degrees) or from the camera when GameStatus.unk03 is set | q14 |
| 001D0970 | KF_Player_FindFacingTarget | Once a frame, finds what is in front of the player: casts the view ray against the map (func_001C1858, 2000 units), every living entity, every map object in use and every ground item; leaves in EventRunner D_00420D00 unk40 (view reach), unk48 (nearest entity distance), unk4C (picked distance) and one of unk54 (Entity), current (MapObject), unk58 (ItemNode); if GameStatus.bit1 is set and scripts are not skipped, starts that thing's script (func_002109C8) or picks the item up (func_001C9AB0) and clears bit1 | q19 |
| 001D1210 |  |  |  |
| 001D1260 |  | Distance from pos to target if within radius on every axis and overall, else D_00446A0C. | src comment |
| 001D1378 | KF_Sprite_Set | Fills a ScreenSprite: prim, tex0, tex1, colour, box in pixels, texture box | q14 |
| 001D1450 | KF_Hud_Init | Loads two files after the data loaded so far (names D_0043F6B8, D_0043F6C8; second is a texture table, uploaded at once), sets up the ten HUD sprites D_005CDE18[10] and the sprite D_005CE098 | q14 |
| 001D19D0 |  |  |  |
| 001D19D8 | KF_Hud_Draw | Draws the HUD from copies of the HUD sprites: frame, two bars filling from the left (PlayerState.unkBC, unkC0 out of 5000), one from the bottom (unk1A4 out of 49), a faded overlay (unk1A6 * 255 / unk5C), a bar unkCC out of unkD0; selects texture table D_00446F6C | q14 |
| 001D2020 |  | Selects texture table D_004471F0 and queues sprite D_005CE098 (the eleventh HUD sprite) | q14 |
| 001D2048 | KF_Clock_Resume | Restarts the play clock from a saved play time (base D_00446A18) and reads the wall clock into D_00446A10 | q14 |
| 001D20A0 | KF_Clock_Start | Starts the play clock from zero; returns 0 | q14 |
| 001D20E0 |  | Refreshes the play clock (called before the ready-to-collect list is built) | q28 |
| 001D2100 |  | Seconds between clock start D_00446A10 and last wall-clock read D_00446A20 | q14 |
| 001D2138 |  | Minutes between clock start D_00446A10 and last wall-clock read D_00446A20 | q14 |
| 001D2170 |  | Seconds from a given play time to the current play time D_00446A28 | q14 |
| 001D21B8 |  | Minutes from a given play time to the current play time D_00446A28 | q14 |
| 001D2200 |  |  |  |
| 001D2258 |  | Seconds from one play time to another | q14 |
| 001D2298 |  | Reads the wall clock into D_00446A20; returns sceCdReadClock's result (via func_001D25C8) | q14 |
| 001D22B8 |  |  |  |
| 001D2350 |  | Converts a play time to seconds: sec + 60 * (min + 60 * (hour + 99 * mday)) | q14 |
| 001D2390 |  |  |  |
| 001D23F0 |  | Seconds from clock start to clock end (same century). | src comment |
| 001D24A8 |  |  |  |
| 001D2550 |  | Byte-identical copy of func_001D2350: converts a play time to seconds (sec + 60 * (min + 60 * (hour + 99 * mday))) | q14 |
| 001D2590 |  | Converts a play time to minutes | q14 |
| 001D25C8 |  | Wall-clock read wrapper around sceCdReadClock (func_001D2298 returns its result) | q14 |

## Menu item rings and menu model loading (0x1D2680-0x1D5A20)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001D2680 | KF_Menu_BuildItemRings | rebuilds every ring the menus walk from what the player owns: the items (page mark on the seventh), everything not locked and not in use, usable items plus castable spells, one ring per equipment group with the list put on the piece worn, the spells known, the spells that can be cast | g7 |
| 001D3608 |  | Upload queue: runs the current link node's callback with the node, or steps to the next node when it has none or is finished | q14 |
| 001D3650 |  | Upload queue: moves the current upload node into the spare node D_005CE8B0, resets the original, cuts the chain after it, gives the spare's pool node back | q14 |
| 001D3730 |  | Upload queue: copies the current upload node into the spare node D_005CE8B0 and cuts the chain after it | q14 |
| 001D37F8 | KF_Menu_RotateModelsNextCategory | "Next category" of the five-item carousel: node 0 moves to node 2, node 1 to node 0, nodes 1, 3 and 4 are reset, the five are rechained and the file names rewritten; passes its own first argument to func_001D3730 / func_001D3650, which take none. | o5 |
| 001D39F0 | KF_Upload_RotateNodes | Upload handler (int, void *): saves the current node into the spare (func_001D3730 copies or func_001D3650 moves), frees node 1's request, copies node 0 into node 1 and node 2 into node 0, makes nodes 2, 3, 4 fresh, appends the first unfinished node to the current chain, relinks 0-4; sibling of func_001D3BE8. (report writes the name as KF_Upload_RotateNodes...) | f5 |
| 001D3BE8 | KF_Upload_RotateNodes... | Rotation of the five upload nodes at D_00426470: saves the current node into the spare, frees a request, copies node 0 into node 3 and node 4 into node 0, resets node 4 (unk20 = 0, unk1C = 0, next = NULL, unk08 = -1), appends the first unfinished node to the current chain, relinks in order 0, 3, 4, 1, 2; signature (int, void *), called from func_001DFFF8 (report gives the name as a stem "KF_Upload_RotateNodes..." for three variants) | q14b |
| 001D3DB0 | KF_Menu_RotateItemNodesPrev | Rotates the five menu item load nodes one step, the other direction than q14's func_001D3BE8; queues what is not loaded; ends in func_001D46F0 (report: "KF_Menu_RotateItemNodesPrev (5 nodes)") | q15b |
| 001D3F78 | KF_MenuLoad_OpenModel | Menu model load stage 0: opens node->name (or the fallback file D_0043F6F8, unchecked) and starts reading 0x25800 bytes | q15 |
| 001D3FF8 |  |  |  |
| 001D4070 |  |  |  |
| 001D40C0 | KF_MenuLoad_RunStage | LinkNode callback (takes the node): runs stage unk1C of the three-stage table D_0043F710 (open, wait for read, hand over) unless the node is done | q15 |
| 001D4118 |  |  |  |
| 001D41B0 |  |  |  |
| 001D4228 | KF_MenuLoad_FinishSlot | Menu load last stage: points slot target->slot (D_00420DF0) at the file's two parts, swaps buffers with the target node, calls func_002032E0 with the per-slot value from D_0043F720, goes to the next waiting node | q15 |
| 001D4360 |  | Call the stage handler selected by upload->stage unless upload->done is set. | src comment |
| 001D43B8 | KF_MenuLoad_OpenPackage | Stage 0 of the big menu loader: opens the file and starts reading 0x1A2000 bytes (handle stored to D_00446AD0); on open failure clears done/stage but carries on with a NULL node (retail bug) | q15 |
| 001D4428 | KF_MenuLoad_WaitRead | Menu load stage 1: polls the read, closes the file, goes to the next stage; on error starts over | q15 |
| 001D44A0 |  | Walk every chunk of the loaded package, hand each payload to func_002007C0, then flush. | src comment |
| 001D4540 |  | Runs the next stage of a chunk upload until it is done. | src comment |
| 001D4598 | KF_MenuLoad_TakeBuffer | Takes a buffer node off the free list D_004472B0 for a load node; returns nothing | q15 |
| 001D45B8 |  | Pushes the request's node back onto the menu free list. | src comment |
| 001D45D0 |  | Called before going back to func_001E6AB8 on cancel; takes no argument (though passed 0) | q27 |
| 001D4690 | KF_Menu_FormatItemModelName | sprintf(buffer, "...%d", definition[item->unk0C].file_number): writes a menu item's model file name; identical copy of func_001D46C8 | q15 |
| 001D46B8 |  | Formats a menu item's value into buffer. | src comment |
| 001D46C8 | KF_Menu_FormatItemModelName | sprintf(buffer, "...%d", definition[item->unk0C].file_number): writes a menu item's model file name; identical copy of func_001D4690 | q15 |
| 001D46F0 | KF_Menu_NameShownItems | Writes the model file names of the five menu items on show through the per-group handler table D_0043F780 (MenuNameHandlers, 12 entries) | q15 |
| 001D4840 | KF_Menu_NameEquipmentModels | Writes the model file names of the player's weapon and eight worn things into the nine equipment load nodes | q15 |
| 001D49D0 | KF_Menu_QueueEquipmentLoads | Ten func_001D4598 calls, then sets slots/callbacks/chain of the nine equipment load nodes behind D_005CED40 and the five item nodes; leaves D_00446C90 = &D_005CED40 | q15b |
| 001D4C60 | KF_Menu_NameListItems | Writes model file names for a long menu list: the current item, its neighbours, and the items seven steps away either way | q15 |
| 001D4E58 |  | Opener of main menu line 5, called through func_001D8260 | q16 |
| 001D4F50 |  |  |  |
| 001D4FD0 |  | Builds two node lists: 470 -> 4C0 -> 510 and 560 -> 5B0. | src comment |
| 001D50C0 | KF_Menu_QueueKeyItemLoads | Sets up and chains the twelve key item load nodes (D_005CEF30); tables from D_0043F7C8, D_0043F7F8 | q15 |
| 001D52D8 | KF_Menu_RotateItemNodesNext | Rotates the three menu item load nodes one step forward and queues what is not loaded (report writes the names as "KF_Menu_RotateItemNodesNext / Prev (3 nodes)") | q15b |
| 001D5460 | KF_Menu_RotateItemNodesPrev | Rotates the three menu item load nodes one step back and queues what is not loaded (report writes the names as "KF_Menu_RotateItemNodesNext / Prev (3 nodes)") | q15b |
| 001D55E8 |  | Builds the free list of 16 pool nodes with 0x25800-byte buffers and sets up their owner. | src comment |
| 001D56B0 | KF_Menu_LoadPartFile | Loads a file of 0x60-byte records behind a 0x10 header; gives back count and pointer | q15 |
| 001D5740 | KF_Menu_LoadAllParts | 36 calls of func_001D56B0: loads every menu screen's part list (counts D_00446A30.., pointers D_0044722C..) | q15 |
| 001D5A20 | KF_Menu_Init | Menu start-up: resets the GS, sets up the menus' frame buffers, loads menu/menu.tmx, menu/font.tmr and the twenty menu/equip/a%03d.mi2 files, fills every MenuItem table (D_004210D0, D_00421A30, D_004231F0 ... D_00423FB0, D_00424050, D_00424600: definition pointer, running number, group) and links the eleven item lists D_005CE770..D_005CE810 into a ring | q19 |

## Main menu and its screens: items, equipment, key items, options, memory card (0x1D62C8-0x1E4068)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001D62C8 | KF_Menu_Run | Runs a whole menu (s8 mode, menu number stored in D_00446C94): sync, sound/pad setup, reads the clock (unused), then every frame func_001CEBE0() and handler D_0043F6E0[D_00446AC4] until it gives 0, with end-of-frame (func_001D72F0), pad read, memory card state (func_002179F8) and a 20-frame input lock on change; afterwards an 11-quadword packet (menu 0 only), teardown, two more frames; menu 1 returns 1 when D_005CF2F0.unk16 was set | q15b |
| 001D67A0 | KF_Menu_Draw...Sprite | Sends a fixed 15-quadword GS packet (settings + one sprite); sibling of func_001D6A00 and func_001D6C30 | q15 |
| 001D6A00 | KF_Menu_Draw...Sprite | Sends a fixed 15-quadword GS packet (settings + one sprite); sibling of func_001D67A0 and func_001D6C30 | q15 |
| 001D6C30 | KF_Menu_Draw...Sprite | Sends a fixed 15-quadword GS packet (settings + one sprite); sibling of func_001D67A0 and func_001D6A00 | q15 |
| 001D6E60 | KF_Menu_DrawItemModel | Sends a GS settings packet, draws the model of load node D_00426470 with the matrix given, restores the view-screen matrix; returns nothing | q15 |
| 001D6FB0 | KF_Menu_DrawBackdrop | Step 0 of the menu handler table D_0043F6E0: on menu 0 sends a 26-quadword packet copying the shown buffer into the drawn one in two half-brightness passes; D_00446AC4++; returns 1 | q15b |
| 001D72E8 |  |  |  |
| 001D72F0 | KF_Menu_EndFrame | End of a menu frame: brightness sprite, vsync, half-line offset, swap buffers, run option handler; returns the field | q15 |
| 001D7460 | MenuDrawBackdrop | Only on menu screen 0 (D_00446C94): sends a 5-quadword textured sprite packet, syncs, then a 3-quadword flat rectangle in the colour {0x10,0x10,0x10} (from D_00446B18) | q16 |
| 001D76D0 | MenuDrawKind2Sprites | Draws every SpritePacket of D_00447248 whose unk3E is 2; twin of func_001D7760 (which does it for D_00446BFC) | q16 |
| 001D7760 |  | Twin of func_001D76D0: draws every SpritePacket of D_00446BFC whose unk3E is 2 | q16 |
| 001D77F0 |  |  |  |
| 001D7840 | MenuStartScreen | Menu start-up step: texture table, the loaded blocks (D_0042A2A0[D_00446DEC]), then picks the handler D_00446B20 for screen D_00446C94 (0 func_001D7C30, 1 func_001E4F58, 2 func_001E1D70, 3 func_001E2750 after func_001E2558, 4 func_001E37C8, 5 func_001E4098), resets the three MenuColors D_00446C78/80/88 to grey, advances D_00446AC8; returns 1 | q16 |
| 001D7AB0 | MenuRunHandler | Calls the current menu screen handler D_00446B20; returns 1 | q16 |
| 001D7AD8 | MenuWaitLoads | Menu screen 0: while the load chain D_00446C90 is busy cuts its links and returns 1; once empty copies the ten list heads D_005CE6B0..740 to D_005CE770..800, loads model file D_005CE740[0]->unk0C, closes file D_00446AD0, resets D_00446AC8 | q16 |
| 001D7C30 |  | Screen the magic and item menus go back to on cancel (main menu) | q20 |
| 001D7C90 | MainMenuUpdate | The seven-line main menu (cursor D_004472CD, state D_004472C4): fades out and opens the chosen screen through func_001D8260 (line 0 func_001E4568, 1 func_001D4FD0, 2 func_001D49D0, 4 queues the upload D_005CEEE0, 5 func_001D4E58); lines 5 and 1 are refused (sound 3) when func_001F92D0 / func_001F93A8 return 0 | q16 |
| 001D8260 |  | Opens the screen chosen in the main menu (called with the screen's opener after the fade out) | q16 |
| 001D8298 | MenuStepBlink | D_00446B39 = (D_00446B39 + 1) % 7 | q16 |
| 001D82C8 | MenuItemMoveAfter | Unlinks a MenuItem and links it in after another | q16 |
| 001D8300 |  | Steps value toward `target` by (target - start) / 60 per call, bouncing between the two ends. | src comment |
| 001D8398 | KF_Menu_UpdateItemList | Item menu screen handler, as in q20 (nothing new about behaviour): list D_00426110 / D_00426120 / D_00426130, names from D_004147F8 via func_001FDB88 + func_001FDD28, uses the item on 0x4000; a dead load of D_00426120[0] at the top is a leftover shared with the magic menu | q20b |
| 001D8C90 | KF_Menu_UpdateMagicList | Magic menu screen handler: moves the cursor (func_001DFEF8) while state D_004472C8 < 2, draws the parts (D_00447234[D_00446A38], alpha in D_00425510[]), the cursor bar (alpha pulsing 16..32 via func_001D8300), seven magic names (func_001FE9A8(&D_00402DA8[n], 0x1A)), the description (func_00200068), the model of D_00426470; 0x4000 confirms if MP unkB8 >= cost unkFC and unkC0 >= 5000, storing the number in GameStatus.unk28 (else sound 3); 0x2000 cancels; states 0 fade in, 1 open, 2 wait for D_00446C90, 3 fade out | q20 |
| 001D94F8 | KF_Menu_EquipScreen | Equipment screen run every frame through D_00446B20 (description unchanged from q21): category/slot stepping, item and worn-weapon models, per-group detail panel, worn list from D_00420D60[0..10], equip on 0x4000, screen step D_004472C6 and panel fade step D_004472C7; category node D_005CE840[0] holds a MenuItem pointer in its first word | q21b |
| 001DBDB0 | MenuPartsScreenUpdate | Menu screen that only shows its parts (D_0044724C, count D_00446A50): fades in, and on button 0x2000 fades out to the main menu (state 3) or to func_001DC2A8 (state 2) | q16 |
| 001DC0E0 | KeyItemSlotsClear | Sets the twelve key item slots D_00421050[12] to -1 | q16 |
| 001DC120 | KeyItemSlotsAdd | Puts the index of a key item (LevelDef numbers 0x2F..0x3B, in D_0043F7C8 order) into the first free slot of D_00421050 | q16 |
| 001DC2A8 | KeyItemScreenUpdate | Key item screen: six of the twelve slots at a time (row D_00446BB8), each with its loaded model (uploads D_005CEF30[slot], texture pages patched into its TEX0) and, when D_00369830[number] is set, its name; fades in and out, leaves through func_001D8398 | q16 |
| 001DCA80 | KF_Menu_UpdateListScreen | Menu screen handler in D_0043FC28[] (0x0043FC48): list screen of 50 pages (D_00446BC0), entries bit-mapped in D_0034C350[50][8], counts D_0034C4E0[50]; cursor D_004472D8 in a 7-row window; 0x4000 opens func_001DD738 with text number page*50+entry+3000; probably a bestiary or record list | q22 |
| 001DD738 | EntryPageUpdate | The page of entry context->unk04 (3000 and up, 50 to a page): parts, the page's model out of the package D_005CEEE0, and three texts; button 0x2000 goes back to func_001DCA80 | q16 |
| 001DDB10 | KF_Menu_ItemScreenUpdate | per-frame handler of the item (equipment) screen: the list of seven under a cursor bar, the turning model, the chosen item's text, the detail panel with its values and the fades; confirm puts the item on or takes it off, cancel leaves through func_001D2680 | g7 |
| 001DF308 | KF_Menu_UpdateTitleScreen | Five-line title/top menu handler (as in q23): cursor D_004472E8, yes/no D_00446BD8, screen step D_00446C70, dispatches to the chosen line's screen handler; draws its cursor bar with func_001FC5E8 | q23b |
| 001DFC60 | ItemListMoveCursor | Moves the item list cursor on lists D_00426110/20/30 (with a lock argument): one row with 0x10/0x40, one page of 7 with 0x20/0x80 when the list has 8 or more; returns 0/1; twin of func_001E0088 | q16 |
| 001DFEF8 |  | Steps the selected list node forward (0x10) or back (0x40). | src comment |
| 001DFFF8 |  | Moves the menu cursor inside the current category (called by the equipment screen when left/right is not pressed) | q21 |
| 001E0088 | ItemListMoveCursor | Moves the item list cursor on lists D_005CE850/60/70: one row with 0x10/0x40, one page of 7 with 0x20/0x80 when the list has 8 or more; twin of func_001DFC60 | q16 |
| 001E02F8 | KF_MemCard_UpdateSlotScreen | Memory-card screen that picks card slot 1 or 2 (D_005CF2F0.unk0C), shows the card messages (unk12: 4/5 no card, 9/10 errors, -1 none), starts the card check (unk14) and moves on to the save list func_001E0D98 or back to func_001DF308 | q23 |
| 001E0D98 | KF_MemCard_UpdateSaveList | List of three saves on the chosen card: copies each save's numbers into the 36 text parameters D_00446DF4..D_00446E80, draws three name lines, moves cursor (unk0D) over existing saves only, asks yes/no before overwriting (unk13 8, cursor unk0E), starts the transfer (func_001FA4F0, 0x20000-byte buffer D_0036A130) and shows its result (unk13 1, 3, 10, 23) | q23 |
| 001E1D70 | OptionsScreenUpdate | Options screen handler (MenuScreenContext *): context->unk04 is the cursor line 0-6 (up 0x10, down 0x40); edits the options block D_00349680 ([0] toggle applied next frame via func_001CBED8 / func_001CC018 with state D_00446BE4 and old value D_00446BE0, [1] volume applied by func_0019C7F0 + func_0019C8C0, [2] volume, [5]/[6]/[9] toggles, [8] 0..2); draws parts D_0044725C (count D_00446A68) via func_00207478; fade state D_00446C70; on leaving sets D_00446B20 = func_001DF308 or D_00446AC8++ | q16b |
| 001E2558 | PadConfigFillTables | For the 12 button slots D_00440200[i] (pointers to pad bit masks) stores the bit's index 0..11 into D_004210B0[i] and D_004210C0[i] | q16 |
| 001E2750 | KF_Menu_UpdateButtonConfigScreen | Per-frame handler of the button configuration screen: twelve actions plus a thirteenth line "defaults" (choice_count 12); cancel (0x2000) fades out if no two actions share a button and the last action's button is not shared, otherwise shows prompt 1 (D_00446BF0); every part is drawn with func_00207478(&part, kind + 1000, ...) | q24b |
| 001E37C8 | MenuOpenScreen4 | Menu screen 4 opener: if func_001F94C0() >= 0, resets MenuScreenState fields and makes func_001E3858 the handler | q16 |
| 001E3858 | KF_Menu_UpdateCardCheckScreen | Per-frame handler of the screen func_001E37C8 opens (looks like the memory card check before load or save): MenuScreenState.unk17 steps 0-3 probe both card slots into unk19/unk1A, build unk04 and pick message unk12; from step 4 draws the parts, a yes/no cursor bar at row unk0C, takes 0x4000 confirm and 0x50 toggle, fades in and out (D_00446C70) | q24 |
| 001E4068 | MenuScreenBusy | Returns D_005CF2F0.unk19 > 0 or unk1A > 0 | q16 |

## Trade screens and remaining menu handlers (0x1E4098-0x1EEE28)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001E4098 | KF_Menu_SliderScreen | Slider screen handler: pad 0x20 / 0x80 change D_00349680[10] by 4 between 0x20 and 0xFC (wrapping), 0x4000 confirms, 0x2000 cancels and restores the value saved in D_004472CF; slider x = (value - 32) * 402 / 220; fade state D_00446C70; afterwards steps D_00446AC8 (when D_00446C94 == 5) or returns to func_001DF308; brightness or volume setting by the look of it | q17 |
| 001E4478 |  |  |  |
| 001E44D0 |  | Looks up the table entry for index in the current node, and if it is set, sets TEST_1 and submits it. | src comment |
| 001E4568 |  | Sets up the trade lists with D_00446C1C/20/34 when the select screen's fade ends | q17b |
| 001E4660 | KF_Menu_BuildLinkNodeNames | Builds the five link node model file names for the trade screen D_00426110 lists: sprintf of GameDefinitionBits.file_number for the selected item, its prev and next, and the two items found walking from the window mark D_00426120 to the list end marks D_00446C40 / D_00446C44; twin of func_001ED540 | q17 |
| 001E4858 | KF_Menu_ShiftLinkNodesNext | Link nodes D_00426470.., cursor one item forward: 470's model moves to the neighbour it came from, the other neighbour's model moves into 470, the freed node and the two far nodes are reset, the first node still needing load is appended to the queue, the five are rechained (report writes the pair as "KF_Menu_ShiftLinkNodesNext/Prev") | q17 |
| 001E4A48 | KF_Menu_ShiftLinkNodesPrev | Link nodes D_00426470.., cursor one item back: 470's model moves to the neighbour it came from, the other neighbour's model moves into 470, the freed node and the two far nodes are reset, the first node still needing load is queued, the five are rechained (report writes the pair as "KF_Menu_ShiftLinkNodesNext/Prev") | q17 |
| 001E4C38 | KF_Menu_ShiftLinkNodesPage... | Link nodes D_00426470.., page step: node 470 takes the model of a far node (5B0 or 560), all others are reset; twin of func_001E4DC8 (other direction) | q17 |
| 001E4DC8 | KF_Menu_ShiftLinkNodesPage... | Link nodes D_00426470.., page step the other way: node 470 takes the model of a far node (5B0 or 560), all others are reset; twin of func_001E4C38 | q17 |
| 001E4F58 | KF_Menu_OpenScreenE02F8 | Resets the menu screen state D_005CF2F0, calls func_001F94C0, func_001FBB68(0), func_001FBB68(1), func_001F9740(1) (memory-card style calls) and installs handler func_001E02F8 | q17 |
| 001E4FE0 | KF_Trade_IsNotGroupBase | func_001E4FE0(value, step): value != base, where base is 0, 30, 55, 80, 105, 130, 140, 150 for steps 3-9 (group bases of the item table); steps 0-2 and 11 test value != 0; step 10 is always FALSE | q17 |
| 001E5070 | KF_Menu_DrawGreySprite | Sends a five-quadword GS packet with one textured sprite, RGB = level, alpha 0x80, FRAME from D_00446F78, on D2 at once | q17 |
| 001E5210 | KF_Gs_SetMenuBlend | Sets alpha test and blend for menu parts by u8 mode 0-3: TEST_1 = 0x30000 (mode 3: 3) and ALPHA_1 A/B/C/D = 0/1/0/1, 0/2/0/1, 2/0/0/1, 0/2/0/0; returns func_00207320's result; twin of func_001E53A8 | q17 |
| 001E53A8 |  | Sets the blend mode of ALPHA_1 from mode and writes it; the value is never seeded. | src comment |
| 001E54A8 | KF_Trade_ClearEquipSlot | Empties equipment slot (s8 slot): *D_00420D60[slot] = first item of that slot's list and the player's unk3C..unk45 set to the slot's "nothing" number (0, 0, 0, 30, 55, 80, 105, 130, 140, 150); slot 11 clears unk46 / unk48 (magic slot); slot 10 does nothing | q17 |
| 001E5660 | KF_Trade_IsEquipped | func_001E5660(slot, kind, value): TRUE when item number value is what the player has in slot (unk3C + 150, unk3D, unk3E..unk45 + 220); slot 10 compares unk46 depending on kind and unk48 | q17 |
| 001E57A0 | KF_Trade_BuildItemRings | for trade table `table`: builds the ring of what the player owns and the table gives a value for (count to D_00446C34) and the ring of what is in stock (count to D_00446C38); copies the tables D_00440300 / D_00440318 from read-only data to the stack on every call | g7 |
| 001E6180 | KF_Trade_Run | Complete modal screen loop: sync, set the display (func_0019BDA0(0x3D00, 0x3F00)), then calls D_004402E0[D_00446CA8](value) until it returns 0, reading the pad each frame (func_001D72F0(1) -> D_00447328, func_001B5460) and watching the controller type (func_002179F8(0, 0)): after a change pad word D_00446DE8 is held at 0 for 20 frames | q17 |
| 001E62D0 | KF_Trade_CopyFrameToTexture | Screen grab for the trade screens' background: copies the shown frame buffer into a work buffer and draws it back into both frame buffers (26-quadword packet on D2), steps D_00446CA8; twin of func_001EEA90. | f5 |
| 001E6600 |  | Calls the handler selected by D_00446CA4 with the context at D_005CF330, storing value in it first. | src comment |
| 001E6668 |  | Start-up step (step 0 of the D_004402E8 table) of the trade screens: loads textures and blocks, builds the lists of trade table context->arg (func_001E57A0), installs func_001E6AB8 (or for table 4 the handler in D_004402F8 with the link nodes started, func_001E6928), clears the three alpha tables, sets the two menu colours, D_00446CA4++ | q17 |
| 001E68D0 |  | Trade screen step 1: calls the installed handler in D_00446CA0 with the same context; returns 1 | q17 |
| 001E68F8 | KF_Menu_CutLinkChain | Cuts the link node chain: node->next = NULL for the queue head (D_00446C90) and for head->unk10; with no queue clears the screen step D_00446CA4; twin of func_001EEE50 | q17 |
| 001E6928 |  | Sets up the trade lists with D_00446C24/28/38 when the select screen's fade ends | q17b |
| 001E6A20 | KF_Menu_PulseValue | func_001E6A20(low, high, value): ping-pong of a float between two bounds, moving by (high - low) / 60 per call, up until it passes high then down until it passes low (direction in D_00446C09) | q17 |
| 001E6AB8 | KF_Trade_SelectMenu | Two-row first screen of the trade (HandlerContext *): pad 0x10 / 0x40 move context->unk04 (modulo 2); draws the D_00446BFC parts and the cursor bar (alpha pulsing via func_001E6A20 while D_00447311 == 1); 0x4000 takes the row (row 0 needs D_005CF380[0], row 1 D_00426110[0], sound 3 when empty), 0x2000 leaves; D_00447311 is the fade state; when the fade ends installs handler D_004402F8[row] and sets up the lists (func_001E6928 or func_001E4568) or steps D_00446CA4; rows being buy/sell is a guess | q17b |
| 001E7010 | KF_Trade_UpdateStockList | Trade (shop) screen per-frame handler while the STOCK list is shown (D_00446CA0 points at it): seven item names with take_value in D_00446E64..D_00446E7C, cursor bar, turning model, detail panel on 0x1000 (D_00446C9C, D_00447312); 0x4000 goes to amount screen func_001E88A0, 0x2000 cancels to func_001E6AB8; D_00447311 is the screen step | q25 |
| 001E88A0 | KF_Trade_UpdateAmountScreen | func_001E88A0(HandlerContext *context): trade (shop) screen while the amount to buy is chosen (context->arg trade table 0-4, context->unk04 amount); shows owned/stock/amount/price in D_00446E44..D_00446E50; confirm needs enough gold (D_00413F80.unk4C) or paying item (table 4), adds to D_00369830[item], reduces stock, rebuilds lists (func_001E57A0) (alt name: TradeBuyNumSelect) | q26 |
| 001EA568 | KF_Trade_UpdateOwnedList | Trade (shop) screen handler while the list of what the player owns is shown (ring D_00426110[0]): cursor via func_001DFC60, seven names with give values in D_00446E64..D_00446E7C, model, details panel on 0x1000 (D_00446C9C); 0x4000 goes on to func_001EBB98, 0x2000 back to func_001E6AB8; D_00447311 is the screen state (alt name: TradeMenuGiveList) | q27 |
| 001EBB98 | KF_Trade_UpdateGiveAmount | func_001EBB98(HandlerContext *context): trade (smith/shop) screen while the amount to GIVE is chosen, twin of func_001E88A0: amount context->unk04 between 1 and owned D_00369830[item]; confirm (0x4000) adds amount * give_value to money D_00413F80.unk4C (cap 999999), removes the items (func_001E54A8 if a worn one runs out), adds to the trader's stock, rebuilds lists (func_001E57A0) | q28b |
| 001ED540 | KF_Menu_BuildLinkNodeNames | Twin of func_001E4660 for the D_005CF380 lists: sprintf the model file numbers of the selected item, its prev and next, and the two items found walking from the window mark D_005CF360 into the five link nodes | q17 |
| 001ED738 | KF_Menu_MoveListCursor | List cursor movement for the D_005CF360/370/380 lists (twin of func_001E0088): pad 0x10 / 0x40 one row, 0x20 / 0x80 one page (lists of 8 and more), then rebuilds the names (func_001ED540); returns 0/1 | q17 |
| 001ED9A8 | KF_Trade_BuildItemRings | KF_Trade_BuildItemRings(mode): builds the two MenuItem rings of the trade screen: items that can be handed in (owned D_00369830[n] != 0, !(def.state & 2), record unk18 == 0; mode 0 also below level 100, mode 1 adds runs 350/360/370) with heads D_00426110 / D_00426120, seventh D_00426130, count D_00446C34; then items handed in earlier whose time is up (func_001D2258(record.unk19, D_00446A28) >= def.unk36 * 60) into D_005CF390.., count D_00446C38. | g6 |
| 001EE730 | KF_..._Run | Complete modal screen loop, twin of func_001E6180: sync, set the display, calls D_004404C0[D_00446CCC](0) until it returns 0, reading the pad each frame and holding it at 0 for 20 frames after a controller type change | q17 |
| 001EE880 |  | Start-up step for the D_004404C8 screens (same as func_001E6668 for the trade screens): installs func_001EEF78 as handler, D_00446CC8++ | q17 |
| 001EEA90 | KF_Menu_CopyFrameToTexture | Screen grab: a 26-quadword packet draws the shown frame buffer (TBP 0 or 0x1180 by D_00446F78) into a work buffer at FBP 0x192 and that buffer back into both frame buffers, waits for the path and steps the screen counter D_00446CCC. Same body as func_001D6FB0 without its D_00446C94 guard. (alt name: KF_Trade_CopyFrameToTexture) | o5 |
| 001EEDC0 |  | Calls the handler selected by D_00446CC8 with the context at D_005CF340, storing value in it first. | src comment |
| 001EEE28 |  | Step 1 of the D_004404C8 screens: calls the installed handler in D_00446CC4 with the same context; returns 1 | q17 |

## Second selection menu and smith list (0x1EEE50-0x1EF840)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001EEE50 | MenuSelect2CutModelChain | If the link-node chain D_00446C90 exists, clears next of its head and of the node at head->unk10 and returns 1; otherwise resets the handler index D_00446CC8 to 0 and returns 0 | q18 |
| 001EEE80 | MenuSelect2LoadModels | The func_001E4568 / func_001E6928 of the select2 list: if D_005CF3B0[0] (selected item) is set, func_001EF648() writes the five file names, then the five LinkNodes at D_00426470 are reset and chained with func_001D40C0 as callback | q18 |
| 001EEF78 | MenuSelect2Main | HandlerContext handler of the three-line "select2" menu (context->unk04 = cursor line 0-2): moves the cursor (pad 0x10 / 0x40), draws parts D_00446C4C[0..D_00446AA0) with alpha from D_00425510[], the pulsing cursor bar, the D_00446C64 parts while D_00446CD0 is 0; confirm 0x4000 builds the list (func_001ED9A8(line)), cancel 0x2000; fade state D_00447316; after fade-out opens the chosen list (D_00446CC4 = D_004404D8[line], func_001E4568 or func_001EEE80) or steps D_00446CC8 | q18 |
| 001EF648 | MenuSelect2SetModelNames | sprintf's "menu/select2/i%04d.mod" (D_004404A8) into the five link nodes (name at node+0x28): the selected item, its prev, its next, and the two items just outside the seven-row window (walking from D_005CF390[0] up to 7 steps until D_00446C40 / D_00446C44; fewer than 8 items (D_00446C48): both D_00446C40); number from D_004147F8[item->unk0C] low 16 bits of the word at 0x30 | q18 |
| 001EF840 | KF_Menu_UpdateUpgradeList | Item-upgrade (smith) list screen per-frame update (unchanged from q29); the current level D_00446E34 and the confirm test (level < 100) read D_003679A0[number - 150].level for numbers below 220, else D_003682F0[number - 220].level | q29b |

## Outside our ranges (0x1Fxxxx), described in passing by reports (0x1F0D90-0x1FF9F0)

| address | candidate name | what it does | source |
|---|---|---|---|
| 001F0D90 |  | Screen the upgrade list goes on to after confirm and fade-out (context->unk04 set to 1) | q29 |
| 001F91F8 |  | Caller of KF_Menu_Run: passes menu numbers 1, 2 and 3 and reads the result only for 1 | q15b |
| 001F92D0 |  | Main menu line 5 is refused (sound 3) when this returns 0 | q16 |
| 001F93A8 |  | Main menu line 1 is refused (sound 3) when this returns 0 | q16 |
| 001F9648 |  | Returns the memory-card operation result (below 0 = failed) | q23 |
| 001FA4F0 |  | Starts the memory-card save transfer (called by the save list with the 0x20000-byte buffer D_0036A130) | q23 |
| 001FB948 |  | Returns the 0x180-byte save record (CardSaveInfo: unk04..unk28, unk179..unk17B) | q23 |
| 001FB980 |  | func_001FB980(i): tells whether save i exists on the card | q23 |
| 001FBBE0 |  | func_001FBBE0(slot): tells whether a memory card is present in the slot | q23 |
| 001FBE70 |  | Returns the scratchpad packet buffer that the three PoolPart draw functions (func_00201518, func_002008E8, func_002020A0) write into. | t17 |
| 001FC0D8 |  | Draws a kind-1 menu object (MenuGlyphSprite, 0x60 bytes: two GsPackedWord corners, colour, kind at 0x31, glyph at 0x32) | q21 |
| 001FC5E8 |  | Draws the cursor bar from a 0x60-byte MenuCursorBar (two corners, four colour bytes); also copies the float at bar+0x24 into the packet | q23b |
| 001FC820 |  | Gives the current value of the number a status sprite stands for (SpritePacket.unk34) | t02 |
| 001FDB88 |  | Takes a GsPackedWord (text position word) and stores it in D_00446D60 | q20 |
| 001FDC28 |  | Takes a GsPackedWord, stores it in D_00446D68 (like func_001FDB88 does D_00446D60) and returns 1 | q20 |
| 001FDD28 |  | Sibling of func_00200068 with the same glyph-drawing code (one of five, func_001FDD28 .. func_001FF9F0) | t01b |
| 001FE9A8 |  | Draws a magic's name: func_001FE9A8(&D_00402DA8[n], 0x1A) | q20 |
| 001FF428 |  | Draws the item's name text on the equipment screen | q21 |
| 001FF9F0 |  | Sibling of func_00200068 with the same glyph-drawing code (last of the five func_001FDD28 .. func_001FF9F0) | t01b |

## Model-part slot pool, node files and TM2 loading (0x200068-0x204D60)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00200068 | DrawWrappedText | DrawWrappedText(text, limit): draws 16-bit glyph codes at the pen D_00446D60 with glyph size D_00446D68 and colour D_00446D70 into a GS packet (colour change, half-width glyphs 50 a row, full-width 25 a row / 625 a page, spaces, word breaks, 0xFFFF end); wraps at limit glyphs a word at a time; returns count (unmatched) | g6 |
| 002007C0 |  |  |  |
| 00200820 | RelocateNodeFile | Turns the file offsets in a loaded node file (RelocFile: entries with layers, keys unk30, child unk38, sibling unk3C) into pointers | t01 |
| 002008E8 | DrawPoolPartTinted | (PoolPart *part, matrix, RenderSlotArg color): builds the part's matrix, transforms points and normals into the scratchpad work buffer, then per strip sends one packet with the strip's registers, each point's ST, the flat colour (the argument's four bytes) and screen point; if strip unk10 is not 0, a second pass with that TEX0 and normals as texture coordinates; ends with ALPHA_1 = 0x44. | f6 |
| 00201518 | DrawPoolPart | (PoolPart *part, matrix): draws one PoolPart (rigid triangle-strip mesh: menu key items, loaded models) straight to the GS via DMA channel 2 from the scratchpad; builds the part matrix, times view-screen D_0042B7D0, screen points into D_00446D40 + 0x5000, one packet per strip with per-point colours (context 2), then ALPHA_1 = 0x44. | t17 |
| 00201E10 | DrawLoadedModel | Sets a loaded model's texture pages (TBP0 0x2840, CBP table indexed by byte D_00446D80), uploads the texture, then draws every part with func_002008E8 | t01 |
| 00201F58 | DrawLoadedModel | Twin of func_00201E10: sets a loaded model's texture pages, uploads the texture, draws every part with func_00201518 (index byte D_00446D81) | t01 |
| 002020A0 | DrawPoolSlot | (RenderSlot *slot, RenderSlotArg unused): draws one pool slot's part straight to the GS using the slot's own matrix (first 0x40 bytes); nothing if slot->part is NULL; with a record (slot->unk48) the first record->unk00 points and record->unk04 normals use the parent slot's matrix (slot->unk40); per-strip packet with TEX0 = slot->unk58, optional second pass for unk10; GS context 1; ends with ALPHA_1 = 0x44. | t17b |
| 00202EB0 |  |  |  |
| 00202F00 | DrawPartSlots20 | Calls func_002020A0 (draw one slot) over model part slots 0-19 of D_0042A8F0; sibling of func_00202EB0/2F50/2FD0 | t01 |
| 00202F50 |  |  |  |
| 00202FD0 |  | Runs func_002020A0 on slots 0-14 and 16-19 of the slot array. | src comment |
| 00203050 | SetAllPartSlots | Gives the first 21 pool slots (D_0042A8F0, the 21-part figure) the part file's parts in order | t01 |
| 00203090 | SetPartSlotGroup | Gives one group of pool slots (groups of 7, 4, 2, 6 and singles 15, 20) its parts; group 5 loses its part | t01 |
| 00203290 |  |  |  |
| 002032E0 | LoadPartSlotGroup | Sets a group's texture pages, uploads the texture and wires the group's pool slots (part, TEX0 pair, record, parent slot) | t01 |
| 00203770 | ClearPartSlots | Clears the part pointer of all 32 pool slots D_0042A8F0[32]; returns 1 | t01 |
| 002037A8 |  | Blends two points and their tangents with the cubic Hermite weights for phase. | src comment |
| 00203820 | PoseNodeTree | Recursive skeleton pose over RelocEntry nodes: interpolates the node's keys (func_002037A8), builds the local matrix, multiplies it into the node's pool slot, recurses into child and sibling | t01 |
| 00203C88 | ExpandPartFile | Copies a part file with every strip point expanded from point/normal numbers to the vectors themselves (0x40 bytes a point); the strips go right behind the copy's parts (alt name: FlattenPartFile) | t01b |
| 00203F28 |  | Builds an object with the handler in slot index, then dispatches it by kind. | src comment |
| 00203FC8 |  |  |  |
| 00204030 | ReadWholeFile | Reads a whole file (sceOpen/sceLseek/sceRead/sceClose); returns the buffer or NULL | t01 |
| 002040C0 |  |  |  |
| 00204128 |  |  |  |
| 00204180 | LoadTm2Split | Texture upload: the image (in two halves when over 0x7FFFF nibbles), then the CLUT; compare func_0020D0E8 "load_tm2()" | t01 |
| 00204580 | LoadTm2Strips | Texture upload a strip of rows at a time, then the CLUT; writes header->tex0 through D_00420DC0; never called in retail | t01 |
| 002048D8 | LoadTm2StripsTwoCluts | Strip-by-strip TM2 texture upload with two CLUTs: stores header->tex0 through tex0_out when not NULL, image data follows the TextureHeader, psm switch (jtbl_00440E80, 59 entries). Never called in retail. | g2 |
| 00204D60 |  | Copies out a texture's TEX0 word and, for a palettized PSM, the word after its CLUT. | src comment |

## Menu sprite handlers and menu pad stepping (0x204DE8-0x207570)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00204DE8 |  | Returns its argument | t01 |
| 00204DF0 |  |  |  |
| 00204DF8 |  |  |  |
| 00204E50 |  |  |  |
| 00204EC8 |  | Copies the packet to D_005DD4C0 with both corners moved left 0xF0, if its slot matches. | src comment |
| 00204F40 |  |  |  |
| 00204FB8 |  |  |  |
| 00205058 |  | Menu sprite slide-in from the right: copies a SpritePacket to D_005DD5E0, shifted right by amount/unk41 of 320 | t01 |
| 002050F8 |  | Menu sprite slide up: copies a SpritePacket to D_005DD640, shifted up by amount/unk41 of 224 (no working clamp at 0) | t01 |
| 002051A0 |  | Menu sprite slide sibling of func_002050F8; clamps at 0xFFF0 correctly | t01 |
| 00205270 |  | Menu sprite fade: copies a SpritePacket to D_005DD700 and sets the copy's alpha | t01 |
| 002052C0 |  |  |  |
| 00205360 |  |  |  |
| 00205368 |  |  |  |
| 00205370 |  |  |  |
| 00205378 | TintStatAgainstItem | Copies a SpritePacket to D_005DD7C0 and tints it blue/red by comparing the number it stands for with what item D_00446E28 gives (cases 0x2C..0x34); same code as func_00206450 with other case numbers and target | t01b |
| 002057A8 | TintStatAgainstWornItem | Copies a SpritePacket to D_005DD820 and tints it blue/red against a worn item `index` using (LevelRecord.unk00[n] + LevelRecordDef.unk00[n]) * level / 100 from D_003682F0 / D_0041E118 | t01b |
| 00205AF0 | StatusSprite_TintMagicStat | Sprite handler (table D_00440B70): copies packet to D_005DD880; current = func_001FC820(packet->unk34); for unk34 0x2C..0x33 base is D_00402DA8[PlayerState.unk3D].hit_checks[0].strengths.unk00[unk34-0x2C], else 0; tints blue (0x40,0x40,0xFF) if current above base, red (0xFF,0x20,0x20) if below | t02 |
| 00205D00 |  |  |  |
| 00205D08 |  | Copies the packet, opaque (alpha 0x80) when its slot's bit is set in the mask, else clear. | src comment |
| 00205D80 | OptionSprite_ShowIfSelected | Sprite handler: copies packet to D_005DD940 (unused), then by packet->slot group compares with option bytes D_00349680/85/86/89/88; returns the original packet only when slot minus group's first slot equals the stored choice, else NULL (only the selected option sprite is drawn) | t02 |
| 00205E90 |  |  |  |
| 00205F38 |  |  |  |
| 00206018 | Sprite_SetStatColour | Sprite handler: colour = D_00446DF8; 0x80FF00 points D_00447344 at D_00446DB0, 0xFF800000 at D_00446DB8, 0 returns NULL; copies packet to D_005DDA60 with red/green/blue from bits 24/16/8 (via an inline colour-unpack helper) | t02b |
| 002060D8 |  |  |  |
| 002061B8 |  |  |  |
| 002062A0 | Sprite_SetAlphaBlendNormal | Sprite handler used for its side effect: writes GS ALPHA_1 = (A 0, B 2, C 0, D 1, FIX 0) through func_00207320 and returns the packet unchanged; unused bits of the register value are uninitialised | t02 |
| 00206318 |  | Copies the packet and greys its colour to 0x40 when its slot matches. | src comment |
| 00206380 | Sprite_SetColumn | Sprite handler: copies packet to D_005DDBE0, sets xyz0.X = column << 4 and xyz1.X = (column << 4) + 0x180 (a 24-pixel wide sprite at pixel column) | t02 |
| 002063E0 |  |  |  |
| 00206450 | StatusSprite_TintItemStat | Sprite handler: copies packet to D_005DDCA0; for unk34 0x2D..0x35 compares current stat with base from item index D_00446E28, tables D_003679A0 (LevelRecord) and D_0041B9B8 (PlayerItemDef), scaled by record.level/100; tints blue above base, red below; twin of func_00205378 | t02 |
| 00206880 | StatusSprite_TintWornStat | Sprite handler: like func_00206450 with the item index as second argument; copies packet to D_005DDD00, base from tables D_003682F0 (LevelRecord) and D_0041E118 (LevelRecordDef) scaled by level/100; tints blue above base, red below; twin of func_002057A8 | t02 |
| 00206BC8 |  | Copies the packet and tints it red while the player's 0xA8 value is below 0xA4. | src comment |
| 00206C40 | MenuPad_Update | Menu input for the frame: D_00446DE8 = (pressed & 0xFF0F) or (held & 0xF00), ORs in 0x20/0x80/0x10/0x40 repeat steps from func_00206DF8/func_00206F58; in analog mode (func_002179F8 == 1) turns left stick into directions via func_002070B8/func_002071F0; mode 0xFF clears it; returns D_00446DE8 as s16 | t02 |
| 00206DF8 | MenuPad_StepHorizontal | Left/right auto-repeat from held directional buttons (0x20 right, 0x80 left): hold counter D_00446DC4, repeating flag D_00446DC0, idle counter D_00446DC1; returns 1 or -1 on a tap or after more than ten frames held, then repeats from repeat_start; clears state and returns 0 while controller mode (func_002179F8) is 0xFF | t02 |
| 00206F58 | MenuPad_StepVertical | Up/down auto-repeat from held directional buttons (0x10 up, 0x40 down): hold counter D_00446DD0, repeating flag D_00446DCC, idle counter D_00446DC8; returns 1 or -1 on a tap or after more than ten frames held, then repeats from repeat_start; clears state and returns 0 while controller mode is 0xFF | t02 |
| 002070B8 | MenuPad_StepHorizontalFrom | Twin of func_00206DF8 on a button word passed in (repeat_start, buttons): left/right auto-repeat with its own state D_00446DD4/DD5/DD8; returns 1, -1 or 0 | t02 |
| 002071F0 | MenuPad_StepVerticalFrom | Twin of func_00206F58 on a button word passed in (repeat_start, buttons): up/down auto-repeat with its own state D_00446DDC/DE0/DE4; returns 1, -1 or 0 | t02 |
| 00207320 |  | Called by func_002062A0 to write the GS ALPHA_1 register value | t02 |
| 00207428 | Sprite_SetAlpha | Sprite copy with alpha: copies the 0x60-byte packet to D_005DDE40 with alpha = second argument and returns the copy; func_00207478 calls it for entry ids of 1000 and up | t02 |
| 00207478 |  | Builds an object with the handler for entry_id % 1000, then dispatches it by kind. | src comment |
| 00207570 |  |  |  |

## VU0 helpers and the start-up settings file reader (ini/NNNN.ini) (0x2075C0-0x2080B0)

| address | candidate name | what it does | source |
|---|---|---|---|
| 002075C0 |  | VU0 lighting helper: normal -> max(light matrix vf5..vf7 * normal, 0) | t02 |
| 002075F8 |  | VU0 lighting helper: (colour matrix vf8..vf11 * factors + vf11) * colour, converted with ftoi0 | t02 |
| 00207620 |  | VU0 lighting helper: colour with w taken from the alpha source's w, converted with ftoi0 | t02 |
| 00207640 |  | VU0 helper: ftoi0 of the second argument, first argument unused (possibly an unlit variant with the same signature as func_002075F8/func_00207620) | t02 |
| 00207650 |  | VU0 matrix load: loads a 4x4 matrix into vf8..vf11 (identical copy of func_002076A0) | t02 |
| 00207668 |  | VU0 matrix load: loads three rows into vf5..vf7 (identical copy of func_00207690) | t02 |
| 00207678 |  | VU0 matrix load: loads a 4x4 matrix into vf1..vf4 (identical copy of func_002076D0) | t02 |
| 00207690 |  | VU0 matrix load: loads three rows into vf5..vf7 (identical copy of func_00207668) | t02 |
| 002076A0 |  | VU0 matrix load: loads a 4x4 matrix into vf8..vf11 (identical copy of func_00207650) | t02 |
| 002076B8 |  |  |  |
| 002076D0 |  | VU0 matrix load: loads a 4x4 matrix into vf1..vf4 (identical copy of func_00207678) | t02 |
| 002076E8 |  | VU0 texture-coordinate helper: sets vf20 = 2 and vf21 = (0, 0, 0, 0.5) | t02 |
| 00207700 |  | VU0 texture-coordinate helper: func_00207720 without the load; transforms the vector already in vf16 by vf12..vf14, (xy + 1) * vf21.w, z = 1, stores it (texture coordinate from a normal: environment mapping) | t02 |
| 00207720 |  | VU0 helper: loads a vector, transforms by vf12..vf14, adds 1 to x/y, scales by vf21.w, z = 1, stores it (texture coordinate from a normal) | t03 |
| 00207748 |  | VU0 helper, twin of func_00207720 with arguments swapped (src, dst): transforms by vf12..vf14, adds 1 to x/y, scales by vf21.w, z = 1 | t03 |
| 00207770 |  |  |  |
| 002077F0 | KF_Ini_Load | Reader of start-up settings file (debug/test start state), called from func_001000C8: opens "ini/%4.4d.ini" (D_004410A8), falls back to ini/9999.ini, reads it to D_00249680, for each line (func_002080B0) runs the handler of every matching key in D_0042B810 with the text after "=" (alt name: LoadIniFile) | t03 |
| 00207970 |  | Ini key handler for "hp=": sets PlayerState.unk20 (HP) from the number | t03 |
| 00207998 |  | Ini key handler for "mp=": sets PlayerState.unk24 (MP) from the number | t03 |
| 002079C0 |  | Ini key handler for "power=": sets PlayerState.unk28 from the number | t03 |
| 002079E8 |  | Ini key handler for "magic=": sets PlayerState.unk2C from the number | t03 |
| 00207A10 |  | Ini key handler for "level=": sets PlayerState.unk54 from the number | t03 |
| 00207A38 |  | Ini key handler for "exp=": sets PlayerState.unk50 from the number | t03 |
| 00207A60 |  | Ini key handler for "gold=": sets PlayerState.unk4C from the number | t03 |
| 00207A88 |  | Ini key handler for "equip_weapon=": sets PlayerState.unk3C = n + 106, or 0 if n < 0 | t03 |
| 00207AC8 |  | Ini key handler for "equip_magic=": sets PlayerState.unk3D = max(n, 0) | t03 |
| 00207AF8 |  | Ini key handler for "equip_shield=": sets PlayerState.unk3E = n + 36, or 0 if n < 0 | t03 |
| 00207B38 |  | Ini key handler for "equip_helm=": sets PlayerState.unk3F = n + 36, or 30 (nothing worn) if n < 0 | t03 |
| 00207B78 |  | Ini key handler for "equip_armor=": sets PlayerState.unk40 = n + 36, or 55 (nothing worn) if n < 0 | t03 |
| 00207BB8 |  | Ini key handler for "equip_guntlet=" (sic, gauntlet): sets PlayerState.unk41 = n + 36, or 80 (nothing worn) if n < 0 | t03 |
| 00207BF8 |  | Ini key handler for "equip_boot=": sets PlayerState.unk42 = n + 36, or 105 (nothing worn) if n < 0 | t03 |
| 00207C38 |  | Ini key handler for "equip_ring=": sets PlayerState.unk43 = n + 36, or 130 (nothing worn) if n < 0 | t03 |
| 00207C78 |  | Ini key handler for "equip_bracelet=": sets PlayerState.unk44 = n + 36, or 140 (nothing worn) if n < 0 | t03 |
| 00207CB8 |  | Ini key handler for "equip_amulet=": sets PlayerState.unk45 = n + 36, or 150 (nothing worn) if n < 0 | t03 |
| 00207CF8 |  | Ini key handler for "equip_shortcut_type=": sets PlayerState.unk48 = n if 0..2, else 0 | t03 |
| 00207D38 |  | Ini key handler for "equip_shortcut=": sets PlayerState.unk46 = max(n, 0) | t03 |
| 00207D68 | IniSetItem | Ini key handler for "item_NNN=count,level,time": D_00369830[NNN] = count; weapons 150..219 set LevelRecord.level = clamp(level, 50, 100) in D_003679A0[NNN-150], unk16 = clamp(time*20, 0, 480) and unk14 = 2/1/0; items 220..319 set only the level in D_003682F0[NNN-220] | t03 |
| 00207F70 |  | Ini key handler for "magic_fire_": calls func_00207FD0(0, text) (magic element 0) | t03 |
| 00207F80 |  | Ini key handler for "magic_earth": calls func_00207FD0(1, text) (magic element 1) | t03 |
| 00207F90 |  | Ini key handler for "magic_wind": calls func_00207FD0(2, text) (magic element 2) | t03 |
| 00207FA0 |  | Config (ini) key parser: calls func_00207FD0 with table row 3 and the text | t04 |
| 00207FB0 |  | Config (ini) key parser: calls func_00207FD0 with table row 4 and the text | t04 |
| 00207FC0 |  | Config (ini) key parser: calls func_00207FD0 with table row 5 and the text | t04 |
| 00207FD0 |  | Common worker of the config key parsers func_00207FA0/FB0/FC0, called with a table row and the text | t04 |
| 00208088 |  | Ini key handler for "debug_auto_recv": empty in retail | t03 |
| 00208090 |  | Ini key handler for "debug_dash": empty in retail | t03 |
| 00208098 |  | Ini key handler for "debug_magic_rapid_fire": empty in retail | t03 |
| 002080A0 |  |  |  |
| 002080B0 | Config_ReadLine | Reads the next line of the config text (cursor D_00447350, bytes left D_00447354) into D_005F9700, dropping CR LF; returns NULL when no bytes are left | t04b |

## Splash image, item-use effect, title screens (0x208158-0x20A290)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00208158 |  |  |  |
| 00208168 |  | Returns the splash image state (0 = idle). | src comment |
| 00208170 |  | Starts splash image splash_id with default hold, fade and skip settings. | src comment |
| 00208198 |  | Starts splash image splash_id: hold_frames (<0 = forever), skippable, fade-in and fade-out frames. Returns 1 if started. | src comment |
| 00208288 |  | Aborts the splash image: jumps to the close state (6) with a short end delay. | src comment |
| 002082A8 |  | Run the handler for the splash image's current state. | src comment |
| 002082E0 |  |  |  |
| 002082E8 |  | State 1: count down the start delay, then advance to loading the image. | src comment |
| 00208320 |  | Loads the splash image file into D_00249680 and sets up its display. | src comment |
| 00208400 |  |  |  |
| 00208450 |  | State 4: hold the image until the hold time runs out or a skippable button is pressed. | src comment |
| 002084A0 |  | State 5: fade the image out over the fade-out frames, then advance to closing. | src comment |
| 002084F8 |  | Counts down the end delay, then releases the splash and closes its file. | src comment |
| 00208540 |  | Draw the splash image while it is on screen (states 3..5). | src comment |
| 00208670 |  | Build the splash sprite packet D_005F9B00 for the image at D_00249680. | src comment |
| 002087D0 | Splash_BuildLoadPacket | Splash_BuildLoadPacket(tbp, cbp): builds at D_00446E84 the DMA packet that uploads the splash image file D_00249680 to GS memory: DMA cnt tag, VIF DIRECT, TEXFLUSH / BITBLTBUF / TRXPOS / TRXREG / TRXDIR, image GIF tag, pixels padded to 16 bytes, and for 4- and 8-bit images the same header and data for the palette. D_0042C028[psm & 7] is bits per pixel. | o6 |
| 00208BC0 | ItemEffect_Load | Item use effect (items 0x46-0x52): loads the two models item_ms/ms_s.omd and ms_b.omd, their motions (.mix) and seven textures i0040..i0046 (item_ms/i%4.4d.tx2) into D_00447394/D_00447398 and D_005F9B40[7] | t04 |
| 00208FF8 | ItemEffect_Reset | Clears the item use effect state (D_00447388 active etc.) | t04 |
| 00209018 | ItemEffect_Start | Starts the item use effect (kind, marked): picks the small (0x46-0x4C) or big (0x4D-0x52) model into D_0044739C, sets its texture, puts it 75 units ahead of the camera | t04 |
| 00209218 | ItemEffect_Update | Per-frame update of the item use effect: runs step D_0042C190[D_00447390], starts effects, poses the model | t04 |
| 00209408 |  | Item use effect step 0 (table D_0042C190): asks the game for status 0xC5F and frees spawner 0x5D | t04 |
| 00209448 |  | Item use effect step 1 (table D_0042C190): waits for D_004141D8[0] == 0 | t04 |
| 00209480 |  | Item use effect step 2 (table D_0042C190): plays the model's motion to the kind's frame | t04 |
| 002094C8 |  | Item use effect step 3 (table D_0042C190): fades out | t04 |
| 002095A0 | ItemEffect_Draw | Draws the item use effect model while the effect runs | t04 |
| 002095D0 |  | The item use effect model's draw handler: sets textures and TEST_1 = 0x5010D | t04 |
| 00209608 | Title_Run | Runs a title screen (screen): calls its init (table D_0042C1B0), then calls the screen's update (D_004473C4, from table D_0042C1A0) and flips until it returns nonzero | t04 |
| 00209688 | Title_Init | Title screens: display setup and loading of the thirteen title textures (title\logo.tm2, title_menu1..5.tm2, copy.tm2, ...) | t04 |
| 002097E0 | Title_LoadTexture | Loads one title texture file (name, index) into D_005FAB60[index] | t04 |
| 00209868 | Title_SendTexture | Uploads title texture (index) to the GS, and its palette for PSMT8 | t04 |
| 00209A28 | Title_SetSprite | Fills a BootSprite (texture, position, size, frame, track) | t04 |
| 00209A48 | Title_DrawSprite | Draws a title sprite (scale, sprite, blend) with brightness interpolated from its key track (BootSpriteTrack); returns 0 | t04 |
| 00209CD8 | Title_SendPacket | Sends a packet (packet, qwords) on the GIF channel and waits | t04 |
| 00209D68 |  | Swaps the display buffers (frame flip) in the boot/title screens | t05 |
| 00209DE0 |  |  |  |
| 00209E48 |  | Sends two sound-driver commands (0x3050, 0x3030): the stop that pairs with func_00209DE0 | t04 |
| 00209E78 |  | Init of the first title screen (table D_0042C1B0) | t04 |
| 00209EE8 |  | Update of the first title screen (table D_0042C1A0) | t04 |
| 00209FC8 |  | Init of the second title screen (table D_0042C1B0); returns 0 | t04 |
| 0020A108 |  | Update of the second title screen: runs 3600 frames with 30-frame fades at each end (D_004473C8 countdown, D_004473D0 fading out); start skips ahead; returns 0 until done | t04 |
| 0020A290 |  | Init of the third title screen: four choices, the one func_001E4068 returns pulsing (D_004473D4 selected choice) | t04 |

## Boot menus, GS double buffers, memory and IOP set-up (0x20A490-0x20B8A8)

| address | candidate name | what it does | source |
|---|---|---|---|
| 0020A490 | KF_Boot_UpdateSelect4 | One frame of a four-choice boot menu: left stick y and pad 0x10/0x40 move the choice (D_004473D4, wraps & 3), 0x4008 confirms, then a 30-frame fade; returns choice + 1 when done; draws sprites D_005FADA0..D_005FAE20, D_005FAE80[4], D_005FAE60 | t05 |
| 0020A6D8 | KF_Boot_RunSelect2 | Runs the two-choice boot menu: calls func_0020A710, then loops func_0020A8C8 + func_00209D68 (swap) until the update returns nonzero | t05 |
| 0020A710 | KF_Boot_InitSelect2 | Init of the two-choice boot menu: sceGsResetGraph, sceGsSetDefDBuff(D_0042C4C0, 640x224), loads four textures (0x10, 0x11, 0x12, 0x16) to D_00446F50, sets up sprites D_005FAEF0[2], D_005FAF28, D_005FAF48; returns 0 | t05 |
| 0020A8C8 | KF_Boot_UpdateSelect2 | One frame of the two-choice boot menu (choice D_004473DC, & 1), two-choice version of func_0020A490, with sounds func_0019CDC0(0) on confirm and (1) on move; probably the language or 50/60Hz style choice | t05 |
| 0020AAE0 | main |  | repo name |
| 0020AD78 |  |  |  |
| 0020AE68 |  |  |  |
| 0020AF18 |  |  |  |
| 0020AF28 |  |  |  |
| 0020AF38 |  | The game's system init; the only game caller of libdev (calls sceDevVif0Reset func_0022B5B8 and sceDevVif1Reset func_0022B8E8). | u01 |
| 0020AFA8 | KF_Gs_InitDoubleBuffers | sceGsSetDefDBuff for D_00432D20 and D_00432F50 (640x448); retargets the second buffer's packets to GS context 2; keeps DISPFB/DISPLAY in D_00433180/D_00433190; unit matrices into D_00432320 and D_004323A0; projection D_00432560 = func_0020B5F8(512, 1, 65536) | t05 |
| 0020B208 |  | Sets the sizes of the memory areas (D_00446F0C..) that func_0020B3D0 lays out | t05 |
| 0020B278 | KF_Mem_ClearBytes | Clears byte_count bytes at dest (dest, byte_count) by doublewords; retail bug: when the size is not a multiple of 8 only the first leftover byte is cleared | t05 |
| 0020B2E8 | KF_Mem_ClearWords | Clears word_count 32-bit words at dest (dest, word_count) | t05 |
| 0020B320 | KF_Mem_CopyWords | Copies word_count 32-bit words (dest, src, word_count) | t05 |
| 0020B358 |  |  |  |
| 0020B3D0 | KF_Mem_LayoutAreas | Places the memory areas back to back from D_005FAF80 using the sizes func_0020B208 set (pointers D_00446F50..D_00446F74, sizes D_00446F0C..D_00446F28); D_00446F68 = D_00446F58; D_00446F08 = D_004321F0 | t05 |
| 0020B458 | KF_Iop_RebootAndLoadModules | sceSifInitRpc, func_00102150, sceSifRebootIop(D_00441350) until it succeeds, sceSifSyncIop, sceFsReset, then nine sceSifLoadModule (D_00441368..D_00441458, args D_00446F90) | t05 |
| 0020B570 |  |  |  |
| 0020B5F8 | KF_Matrix_SetProjection | Builds a projection matrix (matrix, screen_scale, near, far): unit matrix with [0][0] = [1][1] = scale/1024, [2][2] = (far+near)/(far-near), [3][2] = -2*far*near/(far-near), [2][3] = 1, [3][3] = 0 | t05 |
| 0020B6A0 | KF_Mem_Init | Memory init: calls func_0020B208, D_00446F38 = D_01BFAF80, ClockControl D_00432820.unk208..20A = 0, unk20B = 2 | t05 |
| 0020B6E8 |  |  |  |
| 0020B6F0 |  |  |  |
| 0020B6F8 |  |  |  |
| 0020B700 |  |  |  |
| 0020B708 |  |  |  |
| 0020B780 |  | Swaps both double buffers, waiting for vsync first unless allow_late_swap is set and the frame is already late. | src comment |
| 0020B840 |  | Wait for vsync, then swap both double buffers. | src comment |
| 0020B8A8 |  |  |  |

## Model and matrix helpers, vectors, timer, texture packets (0x20B8B0-0x20DBB8)

| address | candidate name | what it does | source |
|---|---|---|---|
| 0020B8B0 | KF_ModelPart_BuildMatrix | Builds a model part's matrix (model, part, dest): scale (unk60), RotX(unk50), RotZ(unk58), RotY(unk54), translate (unk40); angles wrapped to -pi..pi | t05 |
| 0020B9E0 | KF_ModelPart_BuildRotation | Builds a model part's rotation matrix (model, part, matrix, with_offset): same rotations as func_0020B8B0, no scale, translation optional | t05 |
| 0020BB18 | KF_Model_BuildMatrix | Builds a model's matrix (model): RotX, RotY, RotZ, translate; models with unk102 & 0x300 (billboards) only RotZ then multiply by D_00432620 | t05 |
| 0020BCB8 |  |  |  |
| 0020BD40 |  |  |  |
| 0020BD48 |  | Local matrix from the model's rotation x and z, row 1 zeroed, RotX(-pi/2), translate (0, 0, 16), then func_00216B48(model, D_004322E0, matrix); looks like a shadow or a marker under the model | t05 |
| 0020BE70 |  | Scale * RotX * RotY * RotZ * translate into model->unk40, then a copy with row 1 = (-0.707, 0, -0.707, 0) multiplied by D_004323E0 into the model; probably a flattened (shadow) projection | t05 |
| 0020BFF0 | KF_Model_BuildScreenMatrixXYZ | Scratchpad matrix = scale, rotations x, y, z, translate into model->unk40; model matrix = D_004323E0 * that | t05 |
| 0020C178 | KF_Model_BuildScreenMatrixZYX | Twin of func_0020BFF0 with rotation order z, y, x: scratchpad matrix = scale, rotations, translate into model->unk40; model matrix = D_004323E0 * that | t05 |
| 0020C300 | KF_Model_BuildMatrix | General model matrix (model, position, rotation, scale): unk102 & 0x300 billboards toward the camera (D_004325A0.position; 0x100 adds pitch, 0x200 adds yaw); 0x400 turns about z only; otherwise RotZ, RotX, RotY | t05 |
| 0020C6A0 |  |  |  |
| 0020C7A8 |  | Flat mirrored model matrix (scratch[10] = -1): turn about z, offset D_00441490 whose y gets + z * sin(camera pitch), then D_004322A0 (view to screen); a screen-space overlay, perhaps a lens flare or sun | t05 |
| 0020C8D0 | KF_Matrix_FromEuler | Closed-form 3x3 rotation matrix from three angles (matrix, angles) | t05 |
| 0020CB18 | KF_Matrix_MulRotation | VU0: three rows of matrix times the 3x3 of rotation (dest, rotation, matrix); only xyz of the rows, fourth row not written | t05 |
| 0020CB70 |  |  |  |
| 0020CBC0 | KF_Model_MakeDrawMatrices | (model, normal_src, light_src, normal_dest, light_dest): first matrix with rows divided by the model's scale; second, for unk100 & 0x400, zeroed 3x3 and row 3 = colour (unk150..158)/255 * alpha/255 | t05 |
| 0020CD00 |  |  |  |
| 0020CD78 | KF_Clock_AdvanceTo | Adds to GameClock.ticks (D_00431CE8) the seconds until the clock next shows (hour, minute, second); 86400-second day, hour = (ticks/3600 + 12) % 24 so tick 0 is noon | t05 |
| 0020CE28 | KF_Vector_Distance | Distance between two points (a, b) | t06 |
| 0020CE90 |  | Horizontal (XZ-plane) distance between two points. | src comment |
| 0020CEF0 | KF_Vector_Heading | atan2f(dx, dz) of a - b: the heading from b to a (a, b) | t06 |
| 0020CF20 |  |  |  |
| 0020CF88 | KF_Angle_StepTowards | Moves *angle by 0.01 towards target (f32 *angle, f32 target) | t06 |
| 0020CFC8 | KF_Vu0_WorldToScreen | Loads the world-to-screen matrix D_004323E0 and projects one point (point, screen) | t06 |
| 0020D008 |  | Returns the index of the largest of seven s16 values behind a pointer at +0x70 of its argument; dead code (no caller) | t06 |
| 0020D050 | KF_Timer_OverflowHandler | Timer 0 overflow interrupt handler: D_00446F84++ (overflow count) and acknowledges the overflow flag in T0_MODE; installed by func_00209FC8 through func_00216D50 | t06 |
| 0020D078 | KF_Timer_Reset | D_00446F84 = 0; *T0_COUNT = 0 | t06 |
| 0020D088 | KF_Timer_GetTicks | Returns D_00446F84 * 60000 + *T0_COUNT: ticks since the reset (also the seed given to srand) | t06 |
| 0020D0A8 |  |  |  |
| 0020D0B0 |  | Calls load_tm2 (func_0020D0E8) on a texture chunk when upload is not 0; returns the address after the chunk (chunk, upload) | t06 |
| 0020D0E8 | load_tm2 | Uploads a texture (TextureHeader *) with sceGsSetDefLoadImage/sceGsExecLoadImage using TBP0, TBW, PSM, sizes from the header's TEX0, waits for the GIF, then uploads the CLUT (16x16 for PSMT8/8H, 8x2 for PSMT4 kinds); prints "load_tm2() tbp:0 !!!!" and does nothing when TBP0 is 0 | t06 |
| 0020D348 | KF_Texture_BuildPacketTable | Builds a table of GS upload packets from a texture table (char **textures, char **packets): copies the table header and entries with recomputed offsets, calls func_0020D478 per texture, leaves *packets after the last packet and the total size in the new header's first word | t06 |
| 0020D470 |  |  |  |
| 0020D478 | KF_Texture_BuildLoadPacket | Builds the DMA/VIF/GIF packet that uploads one texture at *packet (char **texture, char **packet): BITBLTBUF/TRXPOS/TRXREG/TRXDIR plus IMAGE data, then the same for the CLUT when 8 bits per pixel or fewer; prints every TEX0 field ("load_tm2()" debug dump); stores the packet end in *packet; switch on pixel format | t06 |
| 0020D9A8 |  |  |  |
| 0020DAA0 |  |  |  |
| 0020DAA8 |  | Walks a chunk table, handing each entry's chunk to func_0020D0B0; returns the read cursor. | src comment |
| 0020DB48 |  | Adds the deltas to two 14-bit fields packed in header->tex0; returns the data base. | src comment |
| 0020DBB8 | KF_Texture_RelocateTable | Calls func_0020DB48 (moves TBP0 and CBP) for every texture of a table (table, delta_a, delta_b) | t06 |

## Collision primitives (0x20DC68-0x210678)

| address | candidate name | what it does | source |
|---|---|---|---|
| 0020DC68 | HitSphereCapsule | Collision: point against a capsule (point, base, axis, reach_squared, length), line from base along unit axis for length; writes scratchpad CollisionResult; retail bug: far cap tested around axis + axis*length so it practically never hits | t06 |
| 0020DE48 |  |  |  |
| 0020DF38 | HitSphereTriangle | Collision: point against a triangle (a, b, c, point, reach): distance to the plane under reach and the foot inside the triangle (barycentric test); writes scratchpad CollisionResult | t06 |
| 0020E1F8 | HitCylinderTriangle | Collision: upright capsule against a triangle (a, b, c, centre, radius, height): face first through func_0020E918, then the three edges seen from above; older sibling of func_001870F8; writes scratchpad CollisionResult | t06 |
| 0020E918 | HitRayTriangle | Collision: ray against a triangle (a, b, c, origin, direction, reach); writes scratchpad CollisionResult | t06 |
| 0020EBB8 | HitRayPlane | Collision: a move against the plane of a triangle, no inside test (a, b, c, origin, direction, length); leaves the point, the plane normal and (length * t) squared in CollisionResult; its parallel test is buggy (sums components instead of a dot product) | t06 |
| 0020EE58 | HitVerticalTriangle | Collision: drops the upright line through point onto a triangle (a, b, c, point), at most 8192 away; leaves the height in CollisionResult (a ground-height probe) | t06 |
| 0020F058 |  | 1 if point lies outside the circle of radius around the model in x/z, else 0 | src comment |
| 0020F0D8 | HitCylinderCylinder | Collision: upright cylinder against upright cylinder (base, radius, height, other, other_radius, other_height); writes scratchpad CollisionResult | t06 |
| 0020F230 | HitPointCylinder | Collision: point inside an upright cylinder (point, base, radius_squared, height); writes scratchpad CollisionResult | t06 |
| 0020F2B8 | HitRayCylinder | Collision: ray against an upright cylinder (radius, height, length, base, origin, direction); writes scratchpad CollisionResult | t06 |
| 0020F820 | HitSphereCylinder | Collision: sphere against an upright cylinder (base, centre, radius, height, reach); writes scratchpad CollisionResult | t06 |
| 0020FA58 | HitArcCylinder | Collision: cylinder against the arc of a swing, a sector of a cylinder (base, other, radius, height, other_radius, other_height, half_angle, facing); writes CollisionResult; hit height is buggy (levels divided instead of averaged) | t06 |
| 0020FCF8 | HitArcPitchCylinder | Collision: func_0020FA58's swing-arc against cylinder test with a pitch window on top (other_low, other_high, pitch_range, pitch); writes CollisionResult; hit height comes from an uninitialised local | t06 |
| 00210018 | HitSphereEdge | Collision wrapper: raises the point by the radius, calls func_00210088 with 2*radius as the cylinder height, then puts the point's own height back in the scratchpad CollisionResult | t07 |
| 00210088 | HitCylinderEdge | Collision: upright cylinder against a wall edge (start, end, height): axis on the front side, beside the edge (0 <= t <= 1) and within reach; leaves nearest point and a flat normal in CollisionResult | t07 |
| 00210258 | HitCylinderEdgeRounded | Collision: func_00210088 (cylinder against wall edge) with the edge extended by reach/length at both ends | t07 |
| 00210488 | HitSphereSwing | Collision: ball against an attack swing sector: height slab below the origin, width of the ball where the slab cuts it, range, half angle around facing (func_0019B838 wraps the angle); result in CollisionResult | t07 |
| 00210678 | HitSphereSwingPlayer | Collision: ball against an attack swing sector like func_00210488 but with explicit low/high heights and a pitch window | t07 |

## Script interpreter (0x2108F8-0x211BC0)

| address | candidate name | what it does | source |
|---|---|---|---|
| 002108F8 | KF_Script_FillWords | Fills count words at dest with value (dest, count, value) | t07 |
| 00210930 |  |  |  |
| 002109C8 | KF_Script_Start | Starts a script (script_id, argument): returns 0 if already running (func_00210C48); takes the first free thread of D_01DFB3B0[24] (error flag D_00432920 if none), stores argument/id/cursor = D_004473E8, walks words with func_00210CD0 to label 0x190<<16 or script_id, clears the 16 commands' active words, returns the slot index; 0 at end label 0x7FFE | t07b |
| 00210AF8 | KF_Script_StartAlways | Starts a script like func_002109C8 but without the already-running test and without the end-label test: claims a free thread of D_01DFB3B0, finds the script's label in D_004473E8, clears its commands, returns the slot index | t07b |
| 00210C28 | KF_Script_IsSlotBusy | Returns whether script thread slot (index) of D_01DFB3B0 is in use | t07 |
| 00210C48 | KF_Script_IsRunning | Returns whether a script with script_id is running in any of the 24 threads at D_01DFB3B0 | t07 |
| 00210C88 | KF_Script_FindFreeCommand | Returns the index of a free command among a thread's 16; when all are busy sets error flag D_00432921 and returns 0 | t07 |
| 00210CD0 | KF_Script_SkipWord | Returns the cursor after one script word (cursor), by the length of each control word: 0x192 "if" = (n >> 8) * 5 + 1 more words, 0x196 "set" = n, 0x19D = n + 1, commands = 2 * n | t07 |
| 00210D48 | KF_Script_UpdateAll | Per-frame update of all script threads: func_00210E88 steps the running commands, func_002110A0 reads on; returns D_004473E4 (commands live this frame) | t07 |
| 00210DE8 |  | Walks the 24 entries of 0xC4C bytes; each live one is handed to the two step callbacks. | src comment |
| 00210E88 |  | Steps the running commands of one script thread (called per frame by func_00210D48) | t07 |
| 00210F88 |  | Variant of func_00210E88 (steps a thread's commands) for commands with bit 0x8000, used by func_00210DE8; calls handlers through table D_00433650 (index bug for such commands) | t07 |
| 002110A0 | KF_Script_Advance | Reads on in a thread's script (thread): control words 0x190 label, 0x191 end (frees the thread), 0x192 if, 0x194 begin no-wait block, 0x195 end block and yield, 0x196 set, 0x197 jump, 0x19D (broken: cursor = 0); anything else starts a command through func_00211C38 | t07 |
| 002111F0 | KF_Script_EvalIf | Evaluates a script "if": up to 32 comparisons (==, !=, >, <, >=, <=; float when either side has type 6) joined by "or" (byte 0 == 0) or "and"; returns the words to skip when false | t07 |
| 002114D8 | KF_Script_EvalSet | Evaluates a script "set": target variable, then operand/operator words (+ - * / % << >> & or; float only + - * /) until the 0xFF word; writes back by the target's kind (1 game variable, 2 thread local, 3 shared) | t07 |
| 00211810 | KF_Script_ReadArg | Reads one script argument (thread, words, result): kind 0 constant, 1 game variable, 2 thread local, 3 shared variable, 0x11-0x13 passed through as output arguments (resolved later by func_00211CF8 / func_00211D90) | t07 |
| 002118D8 |  | Script variable reader: game variable from block table D_00433340[low byte] + offset, by value type (0 s8, 1 u8, 2 s16, 3 u16, 4-6 32 bits, 6 = float) | t07 |
| 00211958 |  | Script variable reader: shared variable from the 0x400 bytes at D_01DFAFB0, by value type; offset > 0x3FC sets error flag D_0043292A but still reads | t07 |
| 002119D8 |  | Script variable reader: thread local (ScriptThread.locals), by value type; offset > 0x1FC sets error flag D_0043292B but still reads | t07 |
| 00211A58 |  | Script variable writer (ScriptArg *, u32, int), first of the three for the current thread D_00446FD0 (the order of the readers suggests game variable), by value type | t07 |
| 00211AD0 |  | Script variable writer (ScriptArg *, u32, int), second of the three for the current thread D_00446FD0 (the order of the readers suggests shared variable); out-of-range offset sets an error flag but still writes | t07 |
| 00211B48 |  | Script variable writer (ScriptArg *, u32, int), third of the three for the current thread D_00446FD0 (the order of the readers suggests thread local); out-of-range offset sets an error flag but still writes | t07 |
| 00211BC0 |  | Script variable writer for a given thread (thread local), by value type; out-of-range offset sets an error flag but still writes | t07 |

## Script commands (0x211C38-0x2166F8)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00211C38 | ScriptStartCommand | Script interpreter (thread, cursor): reads a script word (number << 16 or argument count), takes a free command (func_00210C88), stores the number, reads the arguments with func_00211810 (2 words each), clears the 8 work words at command+0x84, returns the new cursor | t08 |
| 00211CF0 |  | Stub that takes nothing and returns 0 (called for script control word 0x19D) | t07 |
| 00211CF8 |  | Dispatch on the low byte of command->opcode (0x11-0x13) with the int as the operand value. | src comment |
| 00211D90 |  | Dispatch on the low byte of command[0] (0x11-0x13) with the float as the operand value. | src comment |
| 00211E30 |  |  |  |
| 00211E38 | ScriptCmdWait | Script command handler: counts argument 1 down each frame, finishes (clears command->active) at <= 0 | t08 |
| 00211E58 |  | Starts a memory-card request once, then clears the done flag when it finishes. | src comment |
| 00211EC0 |  |  |  |
| 00211EC8 |  |  |  |
| 00211ED0 | ScriptCmdStopAllScripts | Script command handler: sets script_id = 0 and all 16 commands' active = 0 for all 24 threads of D_01DFB3B0 | t08 |
| 00211F48 |  |  |  |
| 00211F50 |  |  |  |
| 00211F58 |  |  |  |
| 00211F60 |  | Script command handler (handler table entry at 0x004331D4), identical to func_00211F90: stores an unset value through the result argument at +0x3C via func_00211CF8; looks like a stubbed-out command | t08 |
| 00211F90 |  | Script command handler (handler table entry at 0x004331D8), identical to func_00211F60: stores an unset value through the result argument at +0x3C via func_00211CF8; looks like a stubbed-out command | t08 |
| 00211FC0 |  |  |  |
| 00211FC8 |  |  |  |
| 00211FD0 |  |  |  |
| 00211FD8 |  |  |  |
| 00211FE0 |  |  |  |
| 00211FE8 |  |  |  |
| 00211FF0 |  | Script command handler: D_004325E8 (a float global) = float argument | t08 |
| 00212008 |  |  |  |
| 00212040 | ScriptCmdGetAreaCounter | Script command handler: result = D_0034C4E0[input] for input < 50, else -1 | t08 |
| 002120A0 | ScriptCmdSetAreaCounter | Script command handler: D_0034C4E0[first] = second when first < 50 and 0 <= second < 4 | t08 |
| 002120E0 |  |  |  |
| 00212138 |  |  |  |
| 002121C0 |  |  |  |
| 00212218 |  | Script command handler: func_00103990(first, second), moves area state first to second (D_0034C4E0 / D_0034C350 rows) | t08 |
| 00212248 |  | Script command handler: calls func_00103858(value) | t08 |
| 00212278 | ScriptCmdRequestSoundLoad | Script command handler: SoundLoadQueue D_0038A130: if unk00 != value then unk00 = value, unk04 = 1 | t08 |
| 002122A0 | ScriptCmdSetVolume | Script command handler: for value <= 64 calls func_0019C7F0(D_00349681 * value / 64) | t08 |
| 002122F8 |  | Script command handler: calls func_001BB5C0() | t08 |
| 00212320 |  |  |  |
| 00212388 |  |  |  |
| 00212428 |  | Script command handler: PlayerState D_00413F80.unkC0 = unkBC = 0 (the two gauges that refill) | t08 |
| 00212440 |  |  |  |
| 00212448 |  |  |  |
| 00212450 |  | Script command handler: func_001C8C80(current map, value) (item) | t08 |
| 00212480 |  | Script command handler: calls func_001C9B58(first, second) | t08 |
| 002124B0 |  |  |  |
| 002125B8 |  |  |  |
| 00212690 |  | Run func_001C8CB0 on the looked-up object with its spawn_info->unk38 temporarily set, then clear the command. | src comment |
| 002126F8 |  |  |  |
| 00212778 |  | Turns the model of the owner by one step of the motion, then counts down. | src comment |
| 00212820 |  | copies the current position of the target object into the command's x, y, z | src comment |
| 002128C8 |  |  |  |
| 00212970 |  | Script command handler: map object lookup func_0011E1F0(D_00446770, id), then func_0011E0C8 on it | t08 |
| 002129B0 |  | Script command handler: map object lookup func_0011E1F0(D_00446770, id), then func_0011E140 on it | t08 |
| 002129F0 |  |  |  |
| 00212B00 |  | Eases an object's position toward a goal over frames.value frames, then stops. | src comment |
| 00212BD8 |  | Script command handler: map object lookup func_0011E1F0(D_00446770, id), then func_0011E228 on it | t08 |
| 00212C18 |  |  |  |
| 00212C70 |  | Script command handler: func_0011E758(map object, first), which sets bit 20 and resets the object's state bits | t08 |
| 00212CB8 |  | Starts the target map object on the two arguments of the command, then deactivates it. | src comment |
| 00212D18 | ScriptCmdWaitMapObjectMotion | Script command handler: stays active while the map object exists, its model has unk100 bit 0, object bit 22 is clear and MODEL_CHANNEL(model)->param (s16 at +0x82) is nonzero | t08 |
| 00212D90 |  | Script command: query form of func_00212D18's test (is the map object's motion still running) | t08 |
| 00212E18 |  |  |  |
| 00212E90 | ScriptCmdSetMapObjectPartHidden | Script command handler: MODEL_PART_FLAG_BITS(&model->parts[first])->bit40 = (second == 0) for a valid part index of a map object's model | t08 |
| 00212F28 |  | Script command handler: func_0011D660(map object, first), the object kind's handler from D_003BE330 | t08 |
| 00212F70 |  |  |  |
| 00213010 |  |  |  |
| 00213090 |  | Add this command's per-frame offset to the target object's position; the command ends when its step count runs out. | src comment |
| 00213110 |  | Turns the map object of the owner by one step of the motion, then counts down. | src comment |
| 002131C0 |  | Script command handler: writes the map object's position to three script results; twin of func_00213270 | t08 |
| 00213270 | ScriptCmdGetMapObjectRotation | Script command handler: writes the map object's model->rotation x, y, z to three script results through func_00211D90; twin of func_002131C0 (position) | t08 |
| 00213320 |  | Script command handler: func_0011E810(map object, 0); twin of func_00213380 | t08 |
| 00213380 |  | Script command handler: func_0011E810(map object, 1); twin of func_00213320, which passes 0 | t08 |
| 002133C0 | ScriptCmd_MapObjectClearBit21 | Script command handler: func_0011E810(map object id, 0): clears bit 21 of a waiting map object | t09 |
| 00213400 |  |  |  |
| 002134F8 |  | Eases two of the command's floats toward the shared target each frame until the count runs out. | src comment |
| 002135A0 |  |  |  |
| 00213698 |  |  |  |
| 002136F8 | ScriptCmd_AddPlayerHp | Script command handler: PlayerState.unkB4 += amount, clamped to 0..unk58 (HP and max HP, it seems) | t09 |
| 00213738 |  |  |  |
| 00213778 | ScriptCmd_SetPlayerCanRun | Script command handler: GameStatus.unk06 = (arg == 0); 0 lets the player run | t09 |
| 00213790 | ScriptCmd_GetPlayerStep | Script command handler: stores PlayerState.unk258 (step counter) in a script variable | t09 |
| 002137C8 | ScriptCmd_GetPlayerHp | Script command handler: stores PlayerState.unkB4 and unk58 (HP and max HP, it seems) in two script variables | t09 |
| 00213818 | ScriptCmd_GetPlayerMp | Script command handler: stores PlayerState.unkB8 and unk5C (MP and max MP, it seems) in two script variables | t09 |
| 00213868 |  |  |  |
| 00213910 | ScriptCmd_FlashScreenTint | Script command handler: calls func_001BB360() (PlayerScreenTint.unk00 = 10) | t09 |
| 00213938 | ScriptCmd_StartEffectAtPlayer | Script command handler: func_001A1C58(effect, &player.position, &player.rotation, &player) | t09 |
| 00213978 | ScriptCmd_LaunchShot | Script command handler: takes a PlayerShot slot (unk00 = 1, unk20 = unk22 = 0) and launches shot id from a point with angles, owner = the current MapSlot entry | t09 |
| 00213A40 | ScriptCmd_SetCameraLookMode | Script command handler: GameStatus.unk03 = arg when arg is 0 or 1 (the camera look-at mode func_00213A60 tests) | t09 |
| 00213A60 |  | Tests the camera look-at mode GameStatus.unk03 | t09 |
| 00213AB8 |  |  |  |
| 00213BA8 |  | Eases three of the command's floats toward the shared target each frame until the count runs out. | src comment |
| 00213C78 | ScriptCmd_ShakeCamera | Script command handler: CameraState.unk70 = reach, unk74 = frames (camera shake) | t09 |
| 00213C98 |  |  |  |
| 00213CE8 | ScriptCmd_LoadMap | Script command handler: when no background map load runs and no request is pending, fills D_0038D640 (map = arg, offset 0) and starts the load (request, timer 2); otherwise waits; arg >= 0x80 ends at once | t09 |
| 00213D58 | ScriptCmd_CommitMapLoad | Script command handler: D_00446794 = 1 when a background map load is running and a request is pending | t09 |
| 00213D88 |  |  |  |
| 00213DF8 |  |  |  |
| 00213F08 | ScriptCmd_PropTurnTo | Script command handler: turns a prop to rotation x/y/z over frames (func_0019B9F0 per axis); ends when there or when frames run out | t09 |
| 00213FF8 | ScriptCmd_PropMoveBy | Script command handler: moves a prop by x/y/z every frame for frames frames (func_00115550) | t09 |
| 00214080 |  | Step the target prop's x/y/z position by this command's per-frame deltas, ending the command when its step count runs out. | src comment |
| 00214130 |  | Feed the target prop's x/y/z position to three func_00211D90 commands, then clear the command. | src comment |
| 002141E0 |  |  |  |
| 00214290 |  |  |  |
| 00214358 | ScriptCmd_SetMapLight | Script command handler: sets light (0..2) of light block (< 32) of the current map (MapSlot.unk10 block): direction column and colour/255 | t09 |
| 00214480 |  |  |  |
| 00214510 |  |  |  |
| 00214580 |  |  |  |
| 002145F0 |  |  |  |
| 00214660 |  |  |  |
| 002146D0 | ScriptCmd_StopSoundTag | Script command handler: func_0019E9E0((char)arg), stops the sound playing under that tag | t09 |
| 00214700 | ScriptCmd_PlaySoundBetween | Script command handler: func_0019DAA8(tag, sound, 0, &from, &to, value); sibling of func_002147A0, which passes a third id | t09 |
| 002147A0 |  | Script command handler: sibling of func_00214700 that passes a third id to func_0019DAA8 | t09 |
| 00214840 | ScriptCmd_PlaySound | Script command handler: func_0019D3A0((u8)sound, volume), plays a sound with random pan | t09 |
| 00214870 | ScriptCmd_PlaySoundTagged | Script command handler: func_0019D7D0((char)tag, (u8)sound, value) | t09 |
| 002148A8 |  |  |  |
| 002148F8 |  |  |  |
| 00214950 |  |  |  |
| 002149A0 |  |  |  |
| 002149F0 | ScriptCmd_ChangeMapTag | Script command handler: func_001187F0(arg), starts the transition to map tag arg | t09 |
| 00214A20 | ScriptCmd_ExitMainLoop | Script command handler: sets game status bits 9 and 10, bit 11 = (arg != 0); bit 9 leaves the main loop | t09 |
| 00214A58 |  |  |  |
| 00214AB8 | ScriptCmd_WaitStatusBit14 | Script command handler, three steps in work[0]: func_0019CDA0(2) (stop sound slot 2); set game status bit 14; wait until bit 14 is cleared | t09 |
| 00214B70 | ScriptCmd_SetDrawUnk68 | Script command handler: MapDrawState.unk68 = arg | t09 |
| 00214B88 | ScriptCmd_SetScreenOverlay | Script command handler: screen overlay colour unk6C..unk78 = red, green, blue, alpha; blend mode (MapDrawStateOverlays.unk80) from arg 5, but the switch has no breaks so it always ends as 0x44 | t09 |
| 00214C20 | KF_Script_CmdPadShake | Script command handler: func_001B57A0(strength, duration, delay) (u8 each) | t10 |
| 00214C58 |  | Script command handler: result = func_00210C48(input), i.e. whether the script with that id is running | t10 |
| 00214C90 |  |  |  |
| 00214C98 |  |  |  |
| 00214CA0 |  |  |  |
| 00214CA8 |  |  |  |
| 00214CB0 |  |  |  |
| 00214CB8 |  |  |  |
| 00214CC0 |  |  |  |
| 00214CC8 |  |  |  |
| 00214CD0 |  |  |  |
| 00214CD8 |  |  |  |
| 00214CE0 |  |  |  |
| 00214CE8 |  |  |  |
| 00214CF0 |  |  |  |
| 00214CF8 |  |  |  |
| 00214D00 |  |  |  |
| 00214D08 |  |  |  |
| 00214D10 |  |  |  |
| 00214D18 |  |  |  |
| 00214D20 |  |  |  |
| 00214DC0 |  | Converts the angle to a table index and passes its cosine (sine table + 0x400) on. | src comment |
| 00214E60 |  |  |  |
| 00214EE8 |  | Rotates the vector at D_00413F80+0x1AC about Y, scales it, and stores the three components as ints. | src comment |
| 00214FF0 |  |  |  |
| 00215040 | KF_Script_CmdRandom | Script command handler: result = rand() % input | t10 |
| 00215088 |  |  |  |
| 002150A0 |  |  |  |
| 002150B0 |  | Starts the one-time setup for the task, then clears active once func_001CF980 reports done. | src comment |
| 00215118 | KF_Script_CmdShowMessage | Script command handler: func_001CFA70(0, 0, font, kind, frames, message_id) | t10 |
| 00215158 | KF_Script_CmdEntityMotion | Script command handler: func_00192C98(entity, first, 0, second, 1) on a live entity | t10 |
| 002151C0 |  | Each frame, run the callback on the target entity until the frame count runs out. | src comment |
| 00215230 |  |  |  |
| 00215298 |  |  |  |
| 00215310 | KF_Script_CmdEntityState4Wait | Script command handler: puts the entity in state 4 with func_00195898 (skipped when already in state 5; kinds with params flag 0x80 must also face the player within params->unk438 degrees), then counts frames down and waits for state 5 | t10 |
| 00215488 |  | Script command handler: puts the entity in state 6 once, then waits until it is back in state 0 | t10 |
| 00215520 | KF_Script_CmdEntityWalkTo | Script command handler: per frame sets movement command bits in Entity.unkC8 (0x100/0x200 turn, 0x10/0x20 down/up, 1 walk, 0x40 run) toward a point, flag bits 11-13 = mode; ends within distance (func_00194F48, 3D) | t10 |
| 002156F0 |  | Script command handler: twin of func_00215520 in the plane: walks the entity toward (x, z) via Entity.unkC8 bits, ends within distance (func_00194F80, no height) | t10 |
| 00215850 |  | Per frame, shift the target entity's rotation by the command's deltas (wrapped), then count down. | src comment |
| 00215920 |  | Per frame, turn the target entity's heading a step toward facing the point (x, z), then count down. | src comment |
| 00215A18 |  | Copy bit 0 of the request into flag bit 5 of every entity in the current map's list, depending on the request kind. | src comment |
| 00215AF8 | KF_Script_CmdEntityIsFacing | Script command handler: result = whether abs(wrap(entity->unk68 - model rotation y)) in degrees <= range / 2 | t10 |
| 00215BA0 | KF_Script_CmdEntitySetAnimSpeed | Script command handler: Entity.unkD6 = value; model->unk15C = (float)unkD6 | t10 |
| 00215BF0 |  |  |  |
| 00215CC8 |  |  |  |
| 00215DA0 |  |  |  |
| 00215E78 |  |  |  |
| 00215F50 |  |  |  |
| 00215FE0 |  |  |  |
| 00216088 | KF_Script_CmdEntitySetFlags | Script command handler: for each of 16 mask bits copies that bit of value to an entity flag: mask bits 0..15 -> flag bits 1, 2-3 (both), 5, 6, 7, 18, 22, 23, 24, 25, 26, 27, 28, 14, 9, 29 | t10 |
| 00216408 | KF_Script_CmdEntityGetFlags | Script command handler: reverse of func_00216088: packs the same 16 entity flags into the result (bit 1 set when flag bits 2-3 are 2 or 3) | t10 |
| 00216580 |  | Move the looked-up object to the command's x/y/z, then clear the command. | src comment |
| 002165D0 |  |  |  |
| 00216660 | KF_Script_CmdEntityTrySetFlag18 | Script command handler: sets entity flag bit 18 if (flags & 3) == 3, unk82 == 0 and bits 2, 3, 36, 37 are clear; result = whether it did; func_002166F8 is the clearing twin | t10 |
| 002166F8 |  | Script command handler: clears entity flag bit 18 (clearing twin of func_00216660) | t10 |

## VU0 vector helpers (0x216778-0x216D20)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00216778 | KF_Vu0_CopyVector | Copies one 16-byte vector (lq/sq, like libvu0's sceVu0CopyVector); byte-identical copy of func_00216788 | t10 |
| 00216788 | KF_Vu0_CopyVector | Copies one 16-byte vector (lq/sq, like libvu0's sceVu0CopyVector); byte-identical copy of func_00216778 | t10 |
| 00216798 | KF_Vu0_LoadMatrix |  | repo name |
| 002167B0 |  | VU0 helper: loads three rows into vf5-vf7 | t10 |
| 002167C0 |  | VU0 helper: loads a matrix into vf8-vf11 | t10 |
| 002167D8 |  | VU0 helper: loads two vectors into vf12, vf13 (setup for func_00216850 and friends) | t10 |
| 002167E8 | KF_Vu0_ProjectToScreen |  | repo name |
| 00216850 |  | VU0 routine that uses vf12/vf13 and vf1-vf4 set up by the loader helpers | t10 |
| 002168B8 |  |  |  |
| 00216960 |  |  |  |
| 00216A10 |  |  |  |
| 00216A88 |  |  |  |
| 00216B48 |  |  |  |
| 00216BC0 | KF_Vu0_ApplyMatrix | VU0: dst = matrix * vector (xyzw) | t10 |
| 00216BF0 | KF_Vector_MultiplyComponents |  | repo name |
| 00216C08 |  | VU0: vftoi4 on one vector | t10 |
| 00216C18 |  | VU0: vftoi0 on one vector | t10 |
| 00216C28 |  | VU0: vitof4 on one vector | t10 |
| 00216C38 |  | VU0: vitof0 on one vector | t10 |
| 00216C48 | KF_Vu0_AddVectorXYZ | VU0: adds two vectors, xyz only | t10 |
| 00216C60 | Vu0AddVector | VU0: dst = a + b (xyzw) | t11 |
| 00216C78 | KF_Vector_Subtract |  | repo name |
| 00216C90 | Vu0SubVector | VU0: dst = a - b | t11 |
| 00216CA8 | KF_Vector_Scale |  | repo name |
| 00216CC0 | Vu0ScaleVector | VU0: dst = src * float scale | t11 |
| 00216CD8 | Vu0OuterProduct | VU0: dst = a x b, cross product (vopmula/vopmsub) | t11 |
| 00216CF0 | Vu0ApplyMatrixRows | VU0: dst = row0*x + row1*y + row2*z + row3*w, the four matrix rows passed separately | t11 |
| 00216D20 |  |  |  |

## Timer, pad slots, debug text (0x216D50-0x217F98)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00216D50 | TimerSetup | Sets Tn_MODE and Tn_COMP of an EE timer and adds the INTC handler for timers 0 and 1; no callers per this report; register address bug (timer * 0x2000) makes it right for timer 0 only | t11 |
| 00216DD0 |  | Reads the pad report for a slot (called by the pad update in the settled phases) | t11 |
| 00216EA0 |  | Runs the pad auto-repeat for a slot (called by the pad update) | t11 |
| 00216F50 |  |  |  |
| 00216FB8 | PadPortOpen | Marks the controller slot open (PadSystem D_01E0DB00 open_slots) and calls scePadPortOpen with the slot's 0x100-byte work area | t11 |
| 00217008 | PadUpdate | Per-frame pad update: set-up state machine per open slot (PadSlot.phase: detect type, switch digital/analog/pressure, scePadSetActAlign, settled 0xFD/0xFE/0xFF), then reads the pad (func_00216DD0), computes pressed, runs auto-repeat (func_00216EA0) and sends rumble with scePadSetActDirect | t11 |
| 00217710 | PadGetState | Returns the scePadGetState value saved by the pad update for (port, slot) (PadSlot.state) | t11 |
| 00217738 | PadGetReqState | Returns the scePadGetReqState value saved by the pad update for (port, slot) (PadSlot.request_state) | t11 |
| 00217760 | PadGetData | Returns a pointer to the slot's report (status, id, buttons, analog bytes, pressed, repeated) for (port, slot) | t11 |
| 00217780 | PadGetButtons | Returns the held buttons of (port, slot) (PadSlot.buttons) | t11 |
| 002177A8 | PadGetTrigger | Returns the newly pressed buttons of (port, slot) (PadSlot.pressed) | t11 |
| 002177D0 | PadGetRepeat | Returns the auto-repeated buttons of (port, slot) (PadSlot.repeated) | t11 |
| 002177F8 |  | Sets auto-repeat for a button mask (pair of func_00217880) | t11 |
| 00217880 | PadClearRepeat | Clears auto-repeat for a button mask (pair of func_002177F8, which sets it) | t11 |
| 002178B8 |  |  |  |
| 00217908 | PadSetVibration | Rumble: actuators[0] = power != 0, actuators[1] = power for the slot | t11 |
| 00217948 | PadRequestDigital | Requests digital mode for the slot (mode_lock = arg, mode_request = 0) | t11 |
| 00217970 | PadRequestAnalog | Requests analog mode if the pad can switch; returns 0/1 | t11 |
| 002179B8 |  |  |  |
| 002179F8 | PadGetMode | Returns the slot's current mode (0 digital, 1 analog, 2 analog + pressure, 0xFF unknown) | t11 |
| 00217A20 | DebugFontLoad | Uploads the debug font (D_00433820, 256x128 PSMT4) and its CLUT (D_004337E0) to GS memory; debug text log, never called by retail code | t11 |
| 00217C20 | DebugTextSetWindow | Debug text log (TextLog D_01E0E9C0): sets the text window, white colour, empties the log; no callers | t11 |
| 00217CB8 | DebugTextPutChar | Debug text log: queues one character as a 16x8 sprite (UV, RGBAQ, XYZ2) in the packet D_01E0F210 | t11 |
| 00217DA0 | DebugTextSetGsRegs | Debug text log: sends TEX0/TEX1/ALPHA etc. for the font through the GIF channel | t11 |
| 00217EE0 |  | printf-style append to the text log, cut off at its 0x800 bytes. | src comment |
| 00217F60 | DebugPrintf | Debug text log: variadic wrapper of func_00217EE0 (on-screen printf); never called by retail code | t11 |
| 00217F98 | DebugTextFlush | Debug text log: draws the queued text with wrapping, sends the packet, empties the log | t11 |

## Text log colour, memory card requests (0x218160-0x21B728)

| address | candidate name | what it does | source |
|---|---|---|---|
| 00218160 | KF_TextLog_SetColor | Copies three ints (int *rgb) to TextLog red/green/blue | t12 |
| 00218188 | KF_Mc_CheckCardStep | Memory card (port, slot, &type, &free, &format, step): calls sceMcGetInfo three times (steps 0, 2, 4) and waits for each (1, 3, 5); returns the next step, or the last answer (<= 0) after step 5, -100 when sceMcGetInfo keeps failing; stores the answer in MC_CARD_INFO->result[port][slot] | t12 |
| 00218270 | KF_Mc_MessageForCheck | Memory card: message code for the current port/slot from the check result, the card type and the format flag (0 usable card, 1 no/unknown card, 2, 3 unformatted, 4, 100) | t12 |
| 00218340 |  |  |  |
| 002183C0 | KF_Mc_StepGetInfo | Memory card request 3 step: one sceMcGetInfo + sync; returns 1 when a usable card is there, else -10 | t12 |
| 00218570 | KF_Mc_StepGetDir | Memory card request 4 step: sceMcGetDir(name, flags, max, table); entry count to McState.unk94 | t12 |
| 002186E0 | McStepWriteFile | Memory card "write a file" step function (jump table jtbl_00441870): returns 0 while running, 1 when written, negative on error; step in D_01E4F278.unk8C; step 23 closes the file, step 100 is the error path (sync, close, message 0x3D). | t18b |
| 00218F68 | KF_Mc_StepLoadChecked | Memory card request 6 step: reads back a file written by func_002186E0: 4-byte data length, end mark check in the last 0x20 bytes, then 0x400-byte blocks each ending in a check byte (sum of the inverted bytes); bad check byte gives error -399 | t12 |
| 00219700 | KF_Mc_StepSaveRaw | Memory card request 7 step: checks free space, opens (create when new), writes McState.unkB8 0x400 bytes at a time, closes; errors -4, -6, -11, -101, -999 | t12 |
| 00219CD8 | KF_Mc_StepCopyFile | Memory card request 8 step: like func_00219700 but the data comes from a file opened with sceOpen (unkA4 path), read with sceRead 0x400 bytes at a time; how the save icon is copied from the disc to the card | t12 |
| 0021A368 | KF_Mc_StepLoadRaw | Memory card request 12 step: card check, open, read 0x400 bytes, copy to McState.unkB4, close (delivers at most 0x400 bytes correctly) | t12 |
| 0021A7F8 |  |  |  |
| 0021A910 | KF_Mc_StepChdir | Memory card request 10 step (change directory), stepped once a frame | t12 |
| 0021AA40 | KF_Mc_StepMkdir | Memory card request 11 step (make directory), stepped once a frame | t12 |
| 0021AB58 | KF_Mc_StepFileExists | Memory card request 13 step: sceMcGetDir of one name; message 0x2E found, 0x2D not found | t12 |
| 0021ACB8 | KF_Mc_RequestGetInfo | Starts memory card request 3 (get info) for (port, slot); returns 1 = busy, -1 = bad argument | t12 |
| 0021AD08 | KF_Mc_RequestGetDir | Starts memory card request 4 (get directory) (port, slot, name, flags, max, table); returns 1 = busy, -1 = bad argument | t12 |
| 0021AD68 |  |  |  |
| 0021ADD0 |  | Start a request unless one is active; returns 1 if busy, -1 on a bad argument, 0 on success. | src comment |
| 0021AE38 |  | Start a request unless one is active; returns 1 if busy, -1 on a bad argument, 0 on success. | src comment |
| 0021AEA0 |  | Start a memory card request: kind 0..2, slot 0..4. Returns 1 while a request is already running. | src comment |
| 0021AF08 | KF_Mc_RequestLoadRaw | Starts memory card request 12 (raw load, message 0x46) (port, slot, name, buffer, size); returns 1 = busy, -1 = bad argument | t12 |
| 0021AF70 |  | Start a memory card request: kind 0..2, slot 0..4. Returns 1 while a request is already running. | src comment |
| 0021AFC8 | KF_Mc_RequestChdir | Starts memory card request 10 (chdir) (port, slot, name, pwd); returns 1 = busy, -1 = bad argument | t12 |
| 0021B020 |  |  |  |
| 0021B070 |  | Start a memory card request. Fails while another request is in progress or the arguments are out of range. | src comment |
| 0021B0C0 | KF_Mc_Update | Steps the memory card request in progress (McState D_01E4F278.state 3..13): returns 0 = idle or just finished, the request number while it runs, minus the request number when it failed | t12 |
| 0021B208 | KF_Mc_ClassifyCard | Memory card (port, slot): sets the message from card type and check result; returns the negated result (0 unchanged card, 1 new card, 2 new unformatted...), or -1, -2, -5, -6 for unusable cards | t12 |
| 0021B338 |  | Init the memory card library, then clear the card state block if it succeeded. | src comment |
| 0021B3A0 | KF_Mc_GetCheckResult | Returns the card check result for (port, slot) | t12 |
| 0021B3C0 | KF_Mc_GetFreeClusters | Returns the card's free clusters less two for (port, slot), -1 when no card was seen; message 0x1E for too little space is overwritten with 0 (retail bug) | t12 |
| 0021B438 | KF_Mc_GetCardType | Returns the card type for (port, slot) | t12 |
| 0021B458 |  |  |  |
| 0021B468 |  |  |  |
| 0021B478 |  |  |  |
| 0021B4D0 | KF_Movie_HeapInit | Movie player bump allocator init (start, size) over the block the caller lends the player (MovieThreadState D_00437850 heap_start/heap_next/heap_end) | t12 |
| 0021B4F0 | KF_Movie_HeapAlloc | Movie player bump allocator (size, alignment): returns an aligned block from the lent heap, NULL when full | t12 |
| 0021B540 | KF_Movie_Play | Plays a movie (path, heap, heap_size, skip_requested, volume?, s16, s16, s16): allocates the eleven buffers (stacks, mpeg work area, audio, stream ring, five output pictures and tags, read buffer), sets up the 640x448 display, plays; returns 0, or -1 when the block is too small | t12 |
| 0021B728 | KF_Movie_PlayFile | Plays a movie file (path, skip_requested): func_0021BA20 (create), func_0021B778 (play loop), func_0021BD18 (destroy); returns what the play loop returned | t12 |

## Movie player (SDK sample based): read loop, audio, video decoder, display, file access (0x21B778-0x21FD68)

| address | candidate name | what it does | source |
|---|---|---|---|
| 0021B778 | readMpeg | Movie main play loop (Sony mpegstr sample): file -> read ring -> sceMpegDemuxPssRing; starts picture and sound when both are buffered; then flushes; KF4 addition: 4th argument is a callback polled from the 11th picture on, nonzero = skip the movie (aborts the decoder) | t13 |
| 0021B9E0 | switchThread | Movie player: RotateThreadReadyQueue(1), yields to the other thread | t13 |
| 0021B9E8 | isAudioOK | Movie player: returns whether the sound preset state is reached, or TRUE when the movie has no sound (D_00437820 == 0) | t13 |
| 0021BA20 | initAll | Movie player init: DMA controller, read ring, video and sound decoders, stream callbacks, picture ring, the two threads, opens the file D_0043789C (retries forever, printing), installs vblank and GIF DMA handlers | t13 |
| 0021BD18 |  | Movie player: destroy | t12 |
| 0021BDC0 | defMain | Movie player idle thread: yields forever | t13 |
| 0021BDD8 |  | Empty debug print stub in retail (takes a message, does nothing) | t13 |
| 0021BDE0 | videoCallback | Movie demux callback: copies video stream data from the read ring into the stream ring and queues the time stamp | t13 |
| 0021BF00 |  |  |  |
| 0021BFD8 | copy2area | Movie player: memcpy of two source stretches into two destination stretches (ring buffer copy) | t13 |
| 0021C110 | readBufCreate | Movie read ring (MovieReadBuffer): empties it (size 0x50000); returns 0 | t13 |
| 0021C130 |  |  |  |
| 0021C138 | readBufBeginPut | Movie read ring (MovieReadBuffer) function of Sony's mpegstr sample readbuf.c | t13 |
| 0021C168 | readBufEndPut | Movie read ring (MovieReadBuffer) function of Sony's mpegstr sample readbuf.c | t13 |
| 0021C1B0 | readBufBeginGet | Movie read ring (MovieReadBuffer) function of Sony's mpegstr sample readbuf.c | t13 |
| 0021C1F8 | readBufEndGet | Movie read ring (MovieReadBuffer) function of Sony's mpegstr sample readbuf.c | t13 |
| 0021C220 |  |  |  |
| 0021C2D8 |  |  |  |
| 0021C350 |  |  |  |
| 0021C3D8 |  |  |  |
| 0021C448 | audioDecStart | Movie audio (MovieBuffers D_004379F0): SPU2 mixer setup, starts the looping block transfer, input volume 0x7FFF, master volume from D_00437850.unkD0, state = PLAY | t13 |
| 0021C5A8 | audioDecReset | Movie audio: all volumes 0, stops both block transfers, clears the counters, state = INIT | t13 |
| 0021C6F0 | audioDecBeginPut | Movie audio: gives the area to put data into: the header area while state is INIT, then the free part of the PCM ring | t13 |
| 0021C790 |  |  |  |
| 0021C810 |  |  |  |
| 0021C828 | audioDecSendToIOP | Movie audio: sends whole 1024-byte blocks to the free part of the IOP buffer | t13 |
| 0021C9E0 |  |  |  |
| 0021CA88 | sendToIOP2area | Movie audio: like copy2area but through sceSifSetDma (func_0021CBE8), truncating what does not fit | t13 |
| 0021CBE8 |  | Movie audio: sceSifSetDma wrapper used by func_0021CA88 to send data to the IOP | t13 |
| 0021CC60 | changeMasterVolume | Movie audio: sets SPU2 master volume on both cores, left and right | t13 |
| 0021CCC8 | changeInputVolume | Movie audio: sets core 0 input volume, left and right | t13 |
| 0021CD10 | getFIFOindex | Movie video input buffer: converts a DMA address to the block index of the stream ring | t13 |
| 0021CD50 | setD3_CHCR | Movie video input buffer: sets the D3_CHCR DMA register (func_0021CDB8 is the D4 twin) | t13 |
| 0021CDB8 | setD4_CHCR | Movie video input buffer: sets the D4_CHCR DMA register (twin of func_0021CD50) | t13 |
| 0021CE20 | scTag2 | Movie video input buffer: builds a source-chain DMA tag | t13 |
| 0021CE48 | viBufCreate | Movie video input buffer (MovieStreamBuffer): creates it, with viBufReset written out inline, plus the time stamp ring (stamps set to -1) | t13 |
| 0021CFD0 |  |  |  |
| 0021D0D0 | viBufBeginPut | Movie video input buffer (MovieStreamBuffer) function of Sony's sample vibuf.c; takes stream->sema, computes the free space of the stream ring from dma_count + 2 reserved blocks | t13 |
| 0021D1B8 |  |  |  |
| 0021D208 | viBufAddDMA | Movie video input buffer function of Sony's sample vibuf.c; takes stream->sema; when is_active is 0 returns FALSE without releasing the semaphore (bug inherited from Sony's sample) | t13 |
| 0021D3F8 | viBufStopDMA | Movie video input buffer: called by the mpegStopDMA callback; saves the D4/D3/IPU registers (MovieDmaRegisters order) in stream->saved | t14 |
| 0021D4F8 | viBufRestartDMA | Movie video input buffer: recomputes D4 MADR/QWC/TADR/CHCR from the saved registers (backing up over the IPU FIFO contents, with ring wrap-around), restarts D3, re-issues BCLR with the saved bit position, restarts D4 | t14 |
| 0021D818 | viBufDelete | Resets DMA channel 4 registers, then deletes the semaphore kept at +0x40. | src comment, t14 |
| 0021D870 | viBufIsActive | Movie video input buffer: returns MovieStreamBuffer.is_active, read under the semaphore | t14 |
| 0021D8B0 | viBufCount | Movie video input buffer: called by videoDecInputCount (func_0021E028) on the stream | t14 |
| 0021D900 | viBufFlush | Movie video input buffer: rounds read_bytes up to a whole 0x800 block | t14 |
| 0021D950 |  | Movie video input buffer (game addition): returns whether any queued time stamp covers stream position stop | t14 |
| 0021D9F0 | viBufModifyPts | Movie video input buffer: shortens or drops queued time stamps whose data a new stamp overwrites; prints via func_0021BDD8 when it drops a live one | t14 |
| 0021DBB8 |  |  |  |
| 0021DC90 | viBufGetTs | Movie video input buffer: pops the time stamp for the data the IPU is decoding now | t14 |
| 0021DE28 |  |  |  |
| 0021DF40 |  |  |  |
| 0021DF48 | videoDecSetStream | Movie video decoder: sceMpegAddStrCallback; returns 1 | t14 |
| 0021DF68 | videoDecBeginPut | Movie video decoder: viBufBeginPut on decoder->stream | t14 |
| 0021DF70 | videoDecEndPut | Movie video decoder: viBufEndPut on decoder->stream | t14 |
| 0021DF78 |  |  |  |
| 0021DF88 | videoDecDelete | Movie video decoder: viBufDelete (func_0021D818) + sceMpegDelete; returns 1 | t14 |
| 0021DFC0 |  |  |  |
| 0021DFD0 | videoDecGetState | Movie video decoder: returns decoder->state | t14 |
| 0021DFD8 |  | Movie video decoder: sets decoder->state (called as (decoder, 3)) | t14 |
| 0021DFE8 | videoDecPutTs | Movie video decoder: builds a time stamp (pos = start - stream.data) and queues it into D_00437980 | t14 |
| 0021E028 | videoDecInputCount | Movie video decoder: viBufCount (func_0021D8B0) of the stream ring | t14 |
| 0021E048 | videoDecInputSpaceCount | Movie video decoder: returns the free bytes in the stream ring | t14 |
| 0021E080 |  |  |  |
| 0021E150 | videoDecIsFlushed | Movie video decoder: returns whether input count == 0 and sceMpegIsRefBuffEmpty | t14 |
| 0021E198 | videoDecReset | Movie video decoder: viBufReset, voBufReset, decodes the rest (func_0021E208), busy-waits until the picture ring is empty, state = 3 | t14 |
| 0021E208 |  | Movie video decoder: decodes the rest of the buffered input (called by videoDecReset) | t14 |
| 0021E2D0 | mpegError | sceMpeg callback type 0 (error): returns 1 | t14 |
| 0021E2D8 | mpegNodata | sceMpeg callback type 1 (no data): reads more (func_0021E500), yields (func_0021B9E0), viBufAddDMA | t14 |
| 0021E310 | mpegStopDMA | sceMpeg callback type 2: viBufStopDMA (func_0021D3F8) | t14 |
| 0021E338 | mpegRestartDMA | sceMpeg callback type 3: viBufRestartDMA (func_0021D4F8) | t14 |
| 0021E360 | mpegBackground | sceMpeg callback type 4: reads more (func_0021E500) | t14 |
| 0021E380 | mpegTS | sceMpeg callback type 5: viBufGetTs into the callback record (pts, dts) | t14 |
| 0021E3C8 | copy2area | Movie player: copies a two-part source into a two-part destination, returns 0 if it does not fit | t14 |
| 0021E500 |  | Movie file read pump: calls func_0021C828(&D_004379F0) (audio send to IOP), returns 0 | t14 |
| 0021E528 | voBufCreate | Movie player: sets up the decoded-picture ring (voBuf D_00437838) and clears every tag's status | t14 |
| 0021E570 | voBufDelete | Movie picture ring: the SDK sample's delete routine for the ring. | t15 |
| 0021E578 | voBufReset | Resets the movie picture ring (MovieOutBuffer D_00437838, SDK mpegstr vobuf.c); returns 0. | t15 |
| 0021E588 | voBufIsFull | Movie picture ring: tests whether the ring (MovieOutBuffer) is full. | t15 |
| 0021E5A0 | voBufIncCount | Increments the picture ring's count (MovieOutBuffer.count) with interrupts disabled (DI/EI around it). | t15 |
| 0021E610 | voBufGetData | Returns the MovieOutPicture slot to write the next decoded picture into; NULL when the ring is full. | t15 |
| 0021E658 | voBufIsEmpty | Movie picture ring: tests whether the ring (MovieOutBuffer) is empty. | t15 |
| 0021E668 | voBufGetTag | Returns the MovieOutTag of the oldest picture in the ring; NULL when the ring is empty. | t15 |
| 0021E6D0 | voBufDecCount | Decrements the picture ring's count if it is above 0 (drops the oldest picture). | t15 |
| 0021E6F0 | KF_Movie_InitDisplay | (display, screen_x, screen_y, width, height, picture_width, picture_height): clears vblank variables D_00437824..D_00437834, sceGsResetGraph with video mode D_00437850.unkD4, sets the display env, sends PRMODECONT/FRAME_1/TEST_1/XYOFFSET_1/SCISSOR_1/CLAMP_1 from D_01E4FFC0 over GIF DMA. | t15 |
| 0021E958 |  |  |  |
| 0021E960 | KF_Movie_BuildFieldPackets | (display, packet, picture, field): builds the DMA chain stored in a MovieOutTag; field 0 also uploads the picture per 16x16 macroblock (BITBLTBUF/TRXREG/TRXPOS/TRXDIR, IMAGE tag, DMA ref); both fields end with TEXFLUSH/TEX0/TEX1 and a sprite to the screen rect. | t15 |
| 0021ED10 | KF_Movie_ShowField | (display, second): sets DISPFB.FBP = frame_base[second != 0] and calls sceGsPutDispEnv. | t15 |
| 0021ED48 | KF_Movie_ClearFrames | (display, color): opens the scissor to (width, height*2), draws one sprite of color over both frame buffers (func_0021EF38), restores the scissor; packet sent from D_01E50100. | t15 |
| 0021EE78 |  | Sends the corner points of two rects (x, y, width, height). | src comment |
| 0021EF38 |  | Sends a colored primitive for the rect (x, y, width, height), color as RGBA bytes. | src comment |
| 0021EFE8 | KF_GsPacket_DmaTag | (packet, spr, addr, irq, id, pce, qwc): writes a DMA tag (low doubleword only) at the packet address and returns the address advanced by 16. | t15 |
| 0021F040 | KF_GsPacket_GifTag | (packet, regs, nreg, flg, prim, pre, eop, nloop): writes a GIF tag at the packet address and returns the next address. | t15 |
| 0021F098 | KF_GsPacket_AddReg | (packet, reg, value): writes one A+D register/value pair as four words and returns the next packet address. | t15 |
| 0021F0C8 |  | GS packet writer: appends a TEXFLUSH register write; returns the next packet address. | t15 |
| 0021F0E8 |  | GS packet writer: appends TEX1_1. | t15 |
| 0021F150 |  | GS packet writer: appends TEX0_1. | t15 |
| 0021F1F0 |  | GS packet writer: appends PRIM (prim, iip, tme, fge, abe, aa1, fst, ctxt, fix); returns the next packet address. | t15 |
| 0021F278 |  | GS packet writer: appends UV; returns the next packet address. | t15 |
| 0021F2A8 |  | GS packet writer: appends RGBAQ. | t15 |
| 0021F2F8 |  | GS packet writer: appends XYZ2; returns the next packet address. | t15 |
| 0021F330 |  | GS packet writer: appends FRAME_1. | t15 |
| 0021F378 |  | GS packet writer: appends TEST_1 (ate, atst, aref, afail, date, datm, zte, ztst); returns the next packet address. | t15 |
| 0021F3F0 |  | GS packet writer: appends SCISSOR_1. | t15 |
| 0021F430 |  | GS packet writer: appends XYOFFSET_1; returns the next packet address. | t15 |
| 0021F460 |  | GS packet writer: appends PRMODECONT; returns the next packet address. | t15 |
| 0021F488 |  | GS packet writer: appends PRMODE; returns the next packet address. | t15 |
| 0021F500 |  | Packs six fields into one 64-bit word and sends it to func_0021F098 with a count of 8. | src comment |
| 0021F560 |  | GS packet writer: appends BITBLTBUF (dbp, dbw, dpsm only); returns the next packet address. | t15 |
| 0021F590 |  | GS packet writer: appends TRXPOS (dir, dsax, dsay); returns the next packet address. | t15 |
| 0021F5C0 |  | GS packet writer: appends TRXREG; returns the next packet address. | t15 |
| 0021F5F0 |  | GS packet writer: appends TRXDIR; returns the next packet address. | t15 |
| 0021F618 | KF_Movie_SendChain | (tadr): starts the GIF DMA channel on a chain: D2_TADR = addr & 0x0FFFFFFF, D2_QWC = 0, D2_CHCR = 0x105. | t15 |
| 0021F650 | KF_Movie_VblankHandler | Movie player vblank interrupt handler (SDK sample: vblankHandler): if enabled (D_00437824) and GS paths idle, takes the oldest picture's tag; even field + status 2 shows frame buffer 1 and DMAs field_packets[0]; odd field + status 1 shows buffer 0, DMAs field_packets[1], sets D_0043782C = 1. | t16 |
| 0021F790 |  | Movie player path-3 (GIF) DMA end handler: sets D_0043782C ("a picture was shown completely"), then drops the picture from the ring with func_0021E6D0. | t16 |
| 0021F7E8 | KF_Movie_StartDisplay | Waits for the other field, then enables the movie vblank handler (D_00437824 = 1, D_00437828 = 0) (SDK sample: startDisplay). | t16 |
| 0021F830 |  |  |  |
| 0021F848 | KF_Movie_LoadModule | Loads an IOP module with no arguments; Exit(1) on failure (SDK sample: loadModule). | t16 |
| 0021F880 |  |  |  |
| 0021F998 | KF_Movie_FileOpen | Opens the movie file into a MovieFile (SDK sample: strFileOpen): "cdrom0:" upper-cases the name, appends ";1", allocates a 0x28010-byte IOP stream ring, sceCdStInit, finds the file and starts the stream; other devices use sceOpen + sceLseek for size; no device means "host0:" + path. | t16 |
| 0021FCE8 | KF_Movie_FileClose | Closes the movie file (MovieFile; SDK sample: strFileClose). | t16 |
| 0021FD68 | KF_Movie_FileRead | Reads from the movie file (SDK sample: strFileRead): disc stream uses sceCdStRead(size >> 11) retrying on errors (error 1 restarts the stream at lsn, others wait for the disc and seek), returns sectors << 11; host file uses sceRead until it does not fail. | t16 |

## Sony libdev (not game code) and end-of-text padding (0x22AE10-0x249660)

| address | candidate name | what it does | source |
|---|---|---|---|
| 0022AE10 | sceDevVu0GetCnd | Sony libdev (sceDevVu0GetCnd-like): copies VU0 status/MAC/clip/R/I/Q and VI0-15 into the caller's block; returns FALSE if VU0 is running. Not called by the game. (alt name: sceDevVu0GetVi, the header's name) | u01 |
| 0022AEF0 | sceDevVu0PutCnd | Sony libdev (sceDevVu0PutCnd-like): stores VF0-31 into the caller's block (likely an SDK bug), then loads VU0 control and integer registers (status/MAC/clip/R/I/Q, VI0-15) from it. Not called by the game. (alt name: sceDevVu0GetVf, the header's name) | u01 |
| 0022B060 | sceDevVu0ChkIdle | Sony libdev (header name sceDevVu0ChkIdle): really "VU0 or VU1 is running" (VPU_STAT & 0xF == 1 or & 0xF00 == 0x100). | u01 |
| 0022B090 | sceDevVu0PutMem | Sony libdev: sceDevVu0PutMem(u16 address, quadword by value): writes one quadword into VU0 data memory. | u01 |
| 0022B2E0 | sceDevVu1GetCnd | Sony libdev (sceDevVu1GetCnd-like): reads VU1's registers through VU0 data memory 0x400.. into a LibdevVu1Registers block. (alt name: sceDevVu0GetCtx, the header's name) | u01 |
| 0022B470 | sceDevVu1PutCnd | Sony libdev (sceDevVu1PutCnd-like): writes VU1's registers back from the block with sceDevVu0PutMem. (alt name: sceDevVu0PutCtx, the header's name) | u01 |
| 0022B5B8 | sceDevVif0Reset | Sony libdev: resets VIF0 (FBRST = 1, ERR = 6); returns nothing. Called by the game's system init func_0020AF38. | u01 |
| 0022B5E0 | sceDevVif0Pause | Sony libdev: sceDevVif0Pause(mode): 0 = force break, 1 = stop. | u01 |
| 0022B648 | sceDevVif0Continue | Sony libdev: resumes VIF0 after a pause. | u01 |
| 0022B688 | sceDevVif0PutErr | Sony libdev: sceDevVif0PutErr(interrupt, dmatag, vifcode masks): writes VIF0 ERR; returns the value written. | u01 |
| 0022B6B8 | sceDevVif0GetStat | Sony libdev: returns VIF0 state: 0 idle, 1 stopped, 2 waiting for VU, 3 error stall, 4 busy. | u01 |
| 0022B708 | sceDevVif0GetCnd | Sony libdev (sceDevVif0GetCnd-like): takes a snapshot of the VIF0 registers into a LibdevVif0Registers block. (alt name: sceDevVif0GetRegs, the header's name) | u01 |
| 0022B888 | sceDevVif0PutFifo | Sony libdev: sceDevVif0PutFifo(source, quadword count): writes quadwords to the VIF0 FIFO; its "FIFO full" test (VIF0_STAT & 0x10000000) can never stop the loop. | u01 |
| 0022B8E8 | sceDevVif1Reset | Sony libdev: resets VIF1; returns nothing. Called by the game's system init func_0020AF38. | u01 |
| 0022B910 | sceDevVif1Pause | Sony libdev: sceDevVif1Pause(mode): VIF1 twin of func_0022B5E0 (0 = force break, 1 = stop). | u01 |
| 0022B978 | sceDevVif1Continue | Sony libdev: resumes VIF1 after a pause (VIF1 twin of func_0022B648). | u01 |
| 0022B9B8 | sceDevVif1PutErr | Sony libdev: writes VIF1 ERR from interrupt/dmatag/vifcode masks; returns the value written (VIF1 twin of func_0022B688). | u01 |
| 0022B9E8 | sceDevVif1GetStat | Sony libdev: returns VIF1 state (VIF1 twin of func_0022B6B8: 0 idle, 1 stopped, 2 waiting for VU, 3 error stall, 4 busy). | u01 |
| 0022BA38 | sceDevVif1GetCnd | Sony libdev (sceDevVif1GetCnd-like): takes a snapshot of the VIF1 registers into a LibdevVif1Registers block. (alt name: sceDevVif1GetRegs, the header's name) | u01 |
| 0022BC10 | sceDevVif1PutFifo | Sony libdev: sceDevVif1PutFifo(source, quadword count): writes quadwords to the VIF1 FIFO, stopping when VIF1_STAT & 0x10000000 (FIFO full). | u01 |
| 0022BC70 | sceDevVif1GetFifo | Sony libdev: reads quadwords from the VIF1 FIFO. | u01 |
| 00249660 |  | Not a function: linker/section alignment padding, the first of 0x20 bytes of zero fill at the end of .text after libc's _read_r; nothing references it. (unmatched) | u01b |

## Conflicts

Where reports disagree the table keeps the later report (or the one that matched the function); the other reading is noted here.

- 0011CF88: f3/g3 say it saves four flags of every object of both maps (KF_MapObject_SaveFlagsAll); o1 only notes that the save writer calls it first and supposed it updates the play time. Kept g3.
- 00117AA8: f2 listed a 0x63/0x15 tag pair; g2 (matched) says that was misread, the pairs are 4/0x63, 0x62/0x63, 0x15/0x1B and 0x1A/0x1B. Kept g2.
- 0018AE98: f4 says it returns the prop hit by the ray; g4 (matched) adds that it returns the last prop hit in record order, not the nearest (retail bug). Kept g4.
- 001B4B10: q02 says it adds experience to the weapon in hand; q10 lists its call among the "sounds/effects" of a hit. Kept q02 (the report that matched it).
- 001B5A48: q10's notes before the match had pad [0] = pressed and status bit 35 = guard/aim; q10b (matched) says [0] = held, [1] = pressed, bit 35 = running. q02 put the magic button at D_0044693E, q10b at D_00446944. Kept q10b.
- 001C1C80: q05 guessed its callers were camera code; q01/q04 show they are the weapon's swept hit test.
- 001D37F8 / 001D39F0 / 001D3BE8: q14b and f5 call the D_00426470 nodes an upload queue (KF_Upload_RotateNodes...); q15/q15b/q17/o5 say they are the menu item model nodes (o5: KF_Menu_RotateModelsNextCategory for 001D37F8). The rows for 39F0 and 3BE8 still carry the q14b/f5 name.
- 001D3DB0 / 001D5460: q15b gives both the name KF_Menu_RotateItemNodesPrev (five nodes / three nodes).
- 001D4598: q14b says it releases a node; q15 (matched) says it takes a buffer node off the free list D_004472B0. Kept q15.
- 001DDB10: KF_Menu_UpdateItemScreen (q22) vs KF_Menu_ItemScreenUpdate (g7). Kept g7.
- 001E4F58: q16 lists it as the handler of menu screen 1; q17 (matched) says it is the opener that installs func_001E02F8. Kept q17.
- 001E57A0 / 001ED9A8: both end up as KF_Trade_BuildItemRings (g7 for 001E57A0, g6 for 001ED9A8). Earlier names: KF_Trade_BuildItemLists (q24) for 001E57A0; KF_Menu_BuildItemLists (q28), KF_Trade_BuildGiveLists (q28b) for 001ED9A8, which q29 places in the smith screen.
- 001E6AB8 / 001EA568: q25 calls 001E6AB8 the owned-list handler of the trade screen; q27 gives that role to 001EA568 and makes 001E6AB8 the screen before it. Kept q27 / q17b.
- 001EBB98: q28 (before the match) read D_00446E48 as a level and the lists as built by func_001ED9A8; q28b (matched) says it is the trader's stock and the lists come from func_001E57A0. Kept q28b.
- 001EEA90: KF_Trade_CopyFrameToTexture (q17b) vs KF_Menu_CopyFrameToTexture (o5). 001EEE50: KF_Menu_CutLinkChain (q17, in passing) vs MenuSelect2CutModelChain (q18, matched).
- 002179F8: q15b took it for the memory card state; q17 and t11 (matched) say controller mode (PadGetMode). Kept t11.
- 00216D50: t06 says func_00209FC8 installs the timer handler through it; t11 (matched it, TimerSetup) says it has no callers.
- PlayerState fields: the ini keys (t03) map "hp=" to unk20 and "mp=" to unk24, while q10/t09 show unkB4 / unk58 as current and maximum HP; probably base versus current values, no report settles it.
- One name on two functions: KF_Sound_LoadBank (001CDE28 q08, 001CE170 q09); KF_Sound_LoadBankStep (001A0C18 g5, 001CDF90 q09); KF_Vector_Distance (001B0178 q01, 0020CE28 t06); KF_Model_BuildMatrix (0020BB18, 0020C300, t05); DrawLoadedModel (00201E10, 00201F58, t01); ItemListMoveCursor (001DFC60, 001E0088, q16); KF_Vu0_CopyVector (00216778, 00216788, byte-identical, t10); copy2area (0021BFD8 t13, 0021E3C8 t14: the SDK sample has one in each file).
- Two names offered by one report (first kept, second in the description): 001B2F70, 001D94F8, 001E88A0, 001EA568, 001EF840, 00203C88, 002186E0, 00103C18, 0010EA08, 002077F0.
- 0022AE10..0022BA38: u01's inferred libdev names (sceDevVu0GetCnd, ...) differ from the names in the repo's libdev_internal.h for six functions; u01 says two of the header's Vu0 names are really VU1 functions.
- 00249660 is not a function (zero padding at the end of .text, u01b). 001017A8: g1's report never filled in its result; the row uses f1.
- Naming style differs by report: most use KF_Area_Verb; q16, q18, t01, t02, t11, u02, u03 use unprefixed names; t08 ScriptCmdX, t09 ScriptCmd_X, t10 KF_Script_CmdX; t04 Title_* and t05 KF_Boot_* for the same family; t13-t15 use the SDK movie sample's names.
