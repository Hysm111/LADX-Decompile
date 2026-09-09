# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 56.92%
* **Number of Verified Functions**: 683
* **Number of Decompiled Functions**: 506
* **Number Remaining**: ~517 functions
* **Current Subsystem**: ROM Bank 2 (Magic Rod Visuals & Projectiles, 02:52E0-02:53AF)
* **Current Task**: Bank 2 Magic Rod Visuals & Projectiles routines decompiled and verified
* **Last Completed Task**: Decompiled and verified `label_002_5310` and `label_002_538B` (`02:52E0`-`02:53AF`)
* **Next Task**: Decompile and verify Bank 2 Key Door Opening & Key Drop Subsystem (`TryOpenKeyDoor`, `EnqueueDoorUnlockedSfx`, `label_002_5425`, `02:53B0`-`02:5477`)
* **Last Update Timestamp**: 2026-09-09T23:05:00+03:00

---

## Status Table

| Section | Status | Build | Verification | Notes |
| :--- | :--- | :--- | :--- | :--- |
| `label_002_5310` | VERIFIED | PASS | PASS | Emits Magic Rod attack OAM sprites according to direction and swing phase (`02:5310`) |
| `label_002_538B` | VERIFIED | PASS | PASS | Initializes spawned Magic Rod fireball entity position, variant, and velocity with Piece of Power bonus (`02:538B`) |
| `HandleGotItemA` | VERIFIED | PASS | PASS | Got item jingle trigger on countdown 0x2E and updates sprite (`02:51BC`) |
| `HandleGotItemB` | VERIFIED | PASS | PASS | Got item state handler: spin attack reset, air physics, OAM sprite buffer generation (`02:51C7`) |
| `func_002_523A` | VERIFIED | PASS | PASS | Writes Guardian Acorn tile 0xAE and returns OAM flags 0x14 (`02:523A`) |
| `func_002_523F` | VERIFIED | PASS | PASS | Writes default got-item tile 0x8E and returns OAM flags 0x14 (`02:523F`) |
| `func_002_524A` | VERIFIED | PASS | PASS | Writes Magic Rod tile 0x8C and returns OAM flags 0x10 (`02:524A`) |
| `LinkMotionRecoverHandler` | VERIFIED | PASS | PASS | Link recovery from falling/damage: countdown animations, health subtraction, Angler's Tunnel repositioning, map entry respawn (`02:5267`) |
| `LinkMotionFallingDownHandler` | VERIFIED | PASS | PASS | Pit falling handler: animation advancement, overworld warp holes, tractor device, mountain cave waterfall warp, entry respawn (`02:50D4`) |
| `label_002_52B9` | VERIFIED | PASS | PASS | Resets Link coordinates to map entry position, sets invincibility timer 0x40, and returns to default motion (`02:52B9`) |
| `func_002_52D6` | VERIFIED | PASS | PASS | Resets active staircase state to STAIRCASE_INACTIVE (`02:52D6`) |
| `func_002_5928` | VERIFIED | PASS | PASS | Generates water splash transient VFX and triggers JINGLE_WATER_SPLASH (`02:5928`) |
| `LinkMotionSwimmingHandler` | VERIFIED | PASS | PASS | Handles swimming/diving physics, A stroke speed boost, B dive toggle, and underwater heart/warp checks (`02:4F30`) |
| `LinkMotionUnknownHandler` | VERIFIED | PASS | PASS | Unknown / falling motion state 0x0F: blocks input, integrates Z velocity, transitions map on threshold (`02:50A3`) |
| `label_002_4D97` | VERIFIED | PASS | PASS | Replaces room object with 0xAE, queries GBC attributes via func_91D_jp_92E, emits 10-byte draw command (`02:4D97`) |
| `func_002_4DFC` | VERIFIED | PASS | PASS | Copies 8 bytes of object palette 1 from WRAM bank 1 to WRAM bank 2 (`02:4DFC`) |
| `func_002_4E2C` | VERIFIED | PASS | PASS | Loads 8 bytes from Data_002_4E1C into wObjPal8 and flags palette update (`02:4E2C`) |
| `func_002_4E48` | VERIFIED | PASS | PASS | Restores 8 bytes of wObjPal8 from WRAM bank 2 to WRAM bank 1 and flags palette update (`02:4E48`) |
| `LinkMotionRevolvingDoorHandler` | VERIFIED | PASS | PASS | Handles Eagle's Tower revolving door: Link positioning, palette effects, door animation sequence, transition (`02:4E6D`) |
| `func_002_4EDD` | VERIFIED | PASS | PASS | Resets revolving door animation frame, wC167, palette transition effect, wDDD7, returns to LINK_MOTION_DEFAULT (`02:4EDD`) |
| `func_020_4B4A_trampoline` | VERIFIED | PASS | PASS | Switches to ROM Bank $20, executes func_020_4B4A, and restores saved bank (`00:134B`) |
| `func_002_4B49` | VERIFIED | PASS | PASS | Shovel usage state handler: advances digging animation, triggers hole placement, Marin scolding (`02:4B49`) |
| `func_002_4BC8` | VERIFIED | PASS | PASS | Validates facing tile for digging, sets shovel state = 2, and invokes hole/drop placement (`02:4BC8`) |
| `func_002_4BD4` | VERIFIED | PASS | PASS | Prepares DMG draw command buffer for a dug shovel hole tile at intersected object address (`02:4BD4`) |
| `func_002_4C14` | VERIFIED | PASS | PASS | Prepares CGB draw commands in VRAM0 and VRAM1 for a dug shovel hole tile (`02:4C14`) |
| `label_002_4C92` | VERIFIED | PASS | PASS | Places shovel hole in wRoomObjects, backups to RAM2, issues draw commands, rolls random drop (`02:4C92`) |
| `func_002_4D20` | VERIFIED | PASS | PASS | Validates whether the tile in front of Link can be dug with the shovel (`02:4D20`) |
| `ApplyLinkGroundMotion_noChecks` | VERIFIED | PASS | PASS | Air motion & vertical physics integration without air/side-scrolling guards (`02:44FA`) |
| `LinkMotionUnstuckingHandler` | VERIFIED | PASS | PASS | Unstick Link from solid geometry: loops vertical adjustments, calls background collision, updates air physics (`02:4960`) |
| `LinkPlayingOcarinaHandler` | VERIFIED | PASS | PASS | Ocarina playing handler: song countdown, note VFX entities, Marin/dialog triggers, Manbo warp transition (`02:4A16`) |
| `UpdateSpinAttackAnimation` | VERIFIED | PASS | PASS | Spin attack 360-degree rotation animation, motion blocking, 45-degree angle slices, and sword collision box (`02:4709`) |
| `label_002_476B` | VERIFIED | PASS | PASS | Progresses sword swing animation from wC16D timer, sets wC16E = 4, blocks motion, and transitions to SWING_MIDDLE (`02:476B`) |
| `UpdateLinkAnimation` | VERIFIED | PASS | PASS | Top-level Link animation updater: whirlpool rotation (wD475), airborne jumping frames, spin attack, and sword swing advancement (`02:478C`) |
| `label_002_4827` | VERIFIED | PASS | PASS | Computes sword direction, updates link animation state, sets coordinates wC13A..B, calculates collision box wC140..wC143, and triggers static collision check (`02:4827`) |
| `label_002_48B0` | VERIFIED | PASS | PASS | Clears wC1AC, resets sword animation state and spin attack flags unless running with Pegasus boots (`02:48B0`) |
| `LinkMotionDefault` | VERIFIED | PASS | PASS | Default Link motion handler: countdowns, walk physics, animations, spin attack charging and release (`02:4287`) |
| `func_002_436C` | VERIFIED | PASS | PASS | Motion and collision physics dispatcher between overhead walk and side-scrolling physics (`02:436C`) |
| `OverheadWalkPhysics` | VERIFIED | PASS | PASS | Overhead walking physics, Pegasus boots running, turning, piece of power boost, and slow-down throttling (`02:43BA`) |
| `ApplyLinkGroundMotion` | VERIFIED | PASS | PASS | Updates velocity from gravity, joypad movement in air, landing reset, and terrain noise/splash (`02:44ED`) |
| `shallowWaterVfx` | VERIFIED | PASS | PASS | Shallow water splash particle VFX and water splash audio (`02:45AD`) |
| `func_002_44AD` | VERIFIED | PASS | PASS | Checks inventory appearing state, updates final position, and falls through to ground status reset (`02:44AD`) |
| `label_002_44B5` | VERIFIED | PASS | PASS | Copies wLinkGroundStatus to wC130, zeroes ground status, and checks map transition (`02:44B5`) |
| `func_002_44C2` | VERIFIED | PASS | PASS | Decrements ignore collisions countdown, checks collision axis, clears speed X/Y, and calls ApplyLinkMotionState (`02:44C2`) |
| `func_002_4338` | VERIFIED | PASS | PASS | Lifted object state updater and motion blocking (`02:4338`) |
| `func_002_434A` | VERIFIED | PASS | PASS | Decrements attack step animation countdown and updates animation state from direction (`02:434A`) |
| `MoveLinkToPressedButtonDirection` | VERIFIED | PASS | PASS | Applies joypad d-pad directional speed increments (normal / piece of power) (`02:437A`) |
| `func_002_438F` | VERIFIED | PASS | PASS | Smoothly accelerates/nudges Link speed toward target joypad velocity (`02:438F`) |
| `SelectMusicTrackAfterTransition` | VERIFIED | PASS | PASS | Audio selector after screen transition, handles swordless, boss defeat, dungeons, 2D underground, power-up precedence (`02:4146`) |
| `SpawnChestWithItem` | VERIFIED | PASS | PASS | Spawns chest entity with item at intersected object coordinates and sets variant from hMultiPurpose8 (`02:41D0`) |
| `UseOcarina` | VERIFIED | PASS | PASS | Link ocarina action handler, verifies air/hookshot state, resets positions, selects ballad/mambo/frog/offkey SFX (`02:41FC`) |
| `FireHookshot` | VERIFIED | PASS | PASS | Fires hookshot chain projectile, assigns lifetime countdown 0x2A and directional speed vector (`02:4254`) |
| `RenderIntroMarin` | VERIFIED | PASS | PASS | Intro beach scene Marin entity renderer and state machine dispatcher (`01:765F`) |
| `IntroMarinState0` | VERIFIED | PASS | PASS | Marin walking on beach, inertia countdown, and distance check (`01:7681`) |
| `IntroMarinState1` | VERIFIED | PASS | PASS | Marin stops, waits for transition countdown, and spawns Inert Link (`01:76AB`) |
| `IntroMarinState2` | VERIFIED | PASS | PASS | Marin walks toward Link with camera horizontal scroll and beach draw commands (`01:76D6`) |
| `IntroMarinState3` | VERIFIED | PASS | PASS | Marin approaches Link, scrolls to A0, and triggers VBlank interrupt switch (`01:7711`) |
| `IntroMarinState4` | VERIFIED | PASS | PASS | Marin kneeling over Link and looking at Link with blinking/expression variants (`01:7781`) |
| `RenderIntroSparkle` | VERIFIED | PASS | PASS | Title screen DX sparkle entity renderer and lifespan timer (`01:77DD`) |
| `func_001_7A11` | VERIFIED | PASS | PASS | Submits beach tilemap slice 2 draw command to wDrawCommand (`01:7A11`) |
| `func_001_7A16` | VERIFIED | PASS | PASS | Submits beach tilemap slice 1 draw command to wDrawCommand (`01:7A16`) |
| `RenderIntroInertLink` | VERIFIED | PASS | PASS | Unconscious Link on beach entity renderer and state machine dispatcher (`01:7A2F`) |
| `InertLinkState0Handler` | VERIFIED | PASS | PASS | Inert Link initial delay timer countdown (`01:7A52`) |
| `InertLinkState1Handler` | VERIFIED | PASS | PASS | Inert Link delay before vertical camera panning (`01:7A5E`) |
| `InertLinkState2Handler` | VERIFIED | PASS | PASS | Camera vertical panning up to Koholint sky, streaming post-beach tilemap, title music trigger (`01:7A6E`) |
| `InertLinkState3Handler` | VERIFIED | PASS | PASS | Inert Link final timer countdown, advances gameplay subtype and unloads beach entities (`01:7AC4`) |
| `func_7C60` | VERIFIED | PASS | PASS | Streams title screen post-beach tilemap row into wDrawCommand and advances row counter (`01:7C60`) |
| `func_001_7CCB` | VERIFIED | PASS | PASS | Streams GBC title screen post-beach attribute map row command into wDrawCommandVRAM1 (`01:7CCB`) |
| `IntroStage5Handler` | VERIFIED | PASS | PASS | Intro stage 5: sets beach BG map, palette flag, and advances subtype (`01:711A`) |
| `IntroStage6Handler` | VERIFIED | PASS | PASS | Intro stage 6: sea waves sfx, fade timer, palette updates, beach entity setup (`01:7158`) |
| `IntroBeachHandler` | VERIFIED | PASS | PASS | Intro stage 7: renders beach entities (`01:71C3`) |
| `func_001_71C7` | VERIFIED | PASS | PASS | Intro periodic sea waves audio trigger (`01:71C7`) |
