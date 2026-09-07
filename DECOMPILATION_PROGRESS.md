# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 54.00%
* **Number of Verified Functions**: 648
* **Number of Decompiled Functions**: 487
* **Number Remaining**: ~552 functions
* **Current Subsystem**: ROM Bank 2 (Link Motion Default Core Loop & Overhead Walk Physics, 02:4287-02:44AC)
* **Current Task**: Bank 2 Link Motion Default Core Loop & Overhead Walk Physics decompiled and verified
* **Last Completed Task**: Decompiled and verified `LinkMotionDefault`, `func_002_436C`, and `OverheadWalkPhysics` (`02:4287`-`02:44AC`)
* **Next Task**: Decompile and verify Bank 2 Sword Animation, Spin Attack, and Sword Collision routines (`UpdateSpinAttackAnimation`, `UpdateLinkAnimation`, `02:4709`+)
* **Last Update Timestamp**: 2026-09-08T00:15:00+03:00

---

## Status Table

| Section | Status | Build | Verification | Notes |
| :--- | :--- | :--- | :--- | :--- |
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
