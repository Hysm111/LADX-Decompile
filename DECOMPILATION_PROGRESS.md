# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 53.33%
* **Number of Verified Functions**: 640
* **Number of Decompiled Functions**: 479
* **Number Remaining**: ~560 functions
* **Current Subsystem**: ROM Bank 2 (Link Motion Movement Physics & State Helpers, 02:4338-02:43B9)
* **Current Task**: Bank 2 Link Motion Movement Physics & State Helpers decompiled and verified
* **Last Completed Task**: Decompiled and verified Bank 2 Link Motion Helpers: `func_002_4338`, `func_002_434A`, `MoveLinkToPressedButtonDirection`, `func_002_438F`, and lookup tables (`02:4338`-`02:43B9`, `02:48C5`-`02:4910`)
* **Next Task**: Decompile and verify Bank 2 Link Motion Default core physics loop (`LinkMotionDefault`, `ApplyLinkGroundMotion`, `02:4287`+)
* **Last Update Timestamp**: 2026-09-07T23:05:00+03:00

---

## Status Table

| Section | Status | Build | Verification | Notes |
| :--- | :--- | :--- | :--- | :--- |
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
