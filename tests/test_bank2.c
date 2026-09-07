#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "bank2/audio.h"
#include "bank2/bank2.h"
#include "constants/audio.h"
#include "constants/directions.h"
#include "constants/entities.h"
#include "constants/hardware.h"
#include "constants/link.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/sfx.h"
#include "gb.h"

static uint16_t mock_spawn_entity_slot3(GBState *gb, uint8_t entity_type) {
    (void)entity_type;
    gb_write(gb, (uint16_t)(wEntitiesStatusTable + 3), ENTITY_STATUS_ACTIVE);
    return 3;
}

static uint16_t mock_spawn_entity_fail(GBState *gb, uint8_t entity_type) {
    (void)gb;
    (void)entity_type;
    return 0xFFFF;
}

void run_bank2_tests(void) {
    printf("[*] Running Bank 2 unit tests...\n");

    /* Test 1: Tables size and constant values */
    assert(sizeof(OverworldMusicTracks) == 256);
    assert(sizeof(HouseMusicTracks) == 32);
    assert(sizeof(MusicOverridesPowerUpTrack) == 38);
    assert(sizeof(HookshotChainSpeedX) == 4);
    assert(sizeof(HookshotChainSpeedY) == 4);

    assert(OverworldMusicTracks[0x00] == MUSIC_TAL_TAL_RANGE);
    assert(OverworldMusicTracks[0x20] == MUSIC_OVERWORLD);
    assert(OverworldMusicTracks[0x40] == MUSIC_MYSTERIOUS_FOREST);
    assert(OverworldMusicTracks[0x4C] == MUSIC_RAFT_RIDE_RAPIDS);
    assert(OverworldMusicTracks[0x81] == MUSIC_MABE_VILLAGE);
    assert(OverworldMusicTracks[0xCC] == MUSIC_ANIMAL_VILLAGE);

    assert(HouseMusicTracks[0] == MUSIC_TAIL_CAVE);
    assert(HouseMusicTracks[1] == MUSIC_BOTTLE_GROTTO);
    assert(HouseMusicTracks[6] == MUSIC_EAGLES_TOWER);
    assert(HouseMusicTracks[9] == MUSIC_COLOR_DUNGEON);
    assert(HouseMusicTracks[14] == MUSIC_SHOP);
    assert(HouseMusicTracks[15] == MUSIC_MINIGAME);
    assert(HouseMusicTracks[16] == MUSIC_INSIDE_BUILDING);
    assert(HouseMusicTracks[30] == MUSIC_GHOST_HOUSE);

    assert(MusicOverridesPowerUpTrack[MUSIC_MINIGAME] == 1);
    assert(MusicOverridesPowerUpTrack[MUSIC_MABE_VILLAGE] == 1);
    assert(MusicOverridesPowerUpTrack[MUSIC_SHOP] == 1);
    assert(MusicOverridesPowerUpTrack[MUSIC_OVERWORLD] == 0);

    assert(HookshotChainSpeedX[DIRECTION_RIGHT] == 0x30);
    assert(HookshotChainSpeedX[DIRECTION_LEFT] == -0x30);
    assert(HookshotChainSpeedX[DIRECTION_UP] == 0);
    assert(HookshotChainSpeedX[DIRECTION_DOWN] == 0);

    assert(HookshotChainSpeedY[DIRECTION_RIGHT] == 0);
    assert(HookshotChainSpeedY[DIRECTION_LEFT] == 0);
    assert(HookshotChainSpeedY[DIRECTION_UP] == -0x30);
    assert(HookshotChainSpeedY[DIRECTION_DOWN] == 0x30);

    /* Test 2: SelectMusicTrackAfterTransition - hContinueMusicAfterWarp */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hContinueMusicAfterWarp, 1);
        gb_write_hram(&gb, hDefaultMusicTrack, 0x05);
        SelectMusicTrackAfterTransition(&gb);
        assert(gb_read_hram(&gb, hContinueMusicAfterWarp) == 0);
        assert(gb_read_hram(&gb, hDefaultMusicTrack) == 0x05); /* unmodified */
    }

    /* Test 3: SelectMusicTrackAfterTransition - Swordless Overworld */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wSwordLevel, 0);
        gb_write(&gb, wBossDefeated, 1);
        gb_write(&gb, wActivePowerUp, 0);
        SelectMusicTrackAfterTransition(&gb);
        assert(gb_read(&gb, wBossDefeated) == 0);
        assert(gb_read_hram(&gb, hDefaultMusicTrack) == MUSIC_OVERWORLD_SWORDLESS);
    }

    /* Test 4: SelectMusicTrackAfterTransition - Swordless Overworld with Active PowerUp */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wSwordLevel, 0);
        gb_write(&gb, wActivePowerUp, 1);
        SelectMusicTrackAfterTransition(&gb);
        assert(gb_read_hram(&gb, hDefaultMusicTrack) == MUSIC_OVERWORLD_SWORDLESS);
        /* MUSIC_OVERWORLD_SWORDLESS (0x1D) has MusicOverridesPowerUpTrack[0x1D] == 0,
         * so power-up music takes over */
        assert(gb_read(&gb, wMusicTrackToPlay) == MUSIC_ACTIVE_POWER_UP);
        assert(gb_read_hram(&gb, hDefaultMusicTrackAlt) == MUSIC_ACTIVE_POWER_UP);
        assert(gb_read_hram(&gb, hNextDefaultMusicTrack) == MUSIC_ACTIVE_POWER_UP);
        assert(gb_read(&gb, wC1CF) == 0);
    }

    /* Test 5: SelectMusicTrackAfterTransition - Overworld with Sword */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wSwordLevel, 1);
        gb_write(&gb, wIsIndoor, 0);
        gb_write_hram(&gb, hMapRoom, 0x81); /* Mabe Village */
        gb_write_hram(&gb, hNextMusicTrackToFadeInto, 0x12);
        gb_write(&gb, wBossDefeated, 1);
        gb_write(&gb, wActivePowerUp, 1);
        SelectMusicTrackAfterTransition(&gb);
        assert(gb_read_hram(&gb, hNextMusicTrackToFadeInto) == 0);
        assert(gb_read(&gb, wBossDefeated) == 0);
        assert(gb_read_hram(&gb, hDefaultMusicTrack) == MUSIC_MABE_VILLAGE);
        /* Mabe Village overrides power up music (MusicOverridesPowerUpTrack[4] == 1) */
        assert(gb_read(&gb, wMusicTrackToPlay) != MUSIC_ACTIVE_POWER_UP);
    }

    /* Test 6: SelectMusicTrackAfterTransition - Indoor Boss Defeated */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wSwordLevel, 1);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wBossDefeated, 1);
        gb_write(&gb, wActivePowerUp, 0);
        SelectMusicTrackAfterTransition(&gb);
        assert(gb_read_hram(&gb, hDefaultMusicTrack) == MUSIC_AFTER_BOSS);
    }

    /* Test 7: SelectMusicTrackAfterTransition - Indoor Dungeon & Special Maps */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wSwordLevel, 1);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wBossDefeated, 0);
        gb_write(&gb, wActivePowerUp, 0);

        /* Tail Cave */
        gb_write_hram(&gb, hMapId, 0x00);
        SelectMusicTrackAfterTransition(&gb);
        assert(gb_read_hram(&gb, hDefaultMusicTrack) == MUSIC_TAIL_CAVE);

        /* Color Dungeon */
        gb_write_hram(&gb, hMapId, MAP_COLOR_DUNGEON);
        SelectMusicTrackAfterTransition(&gb);
        assert(gb_read_hram(&gb, hDefaultMusicTrack) == MUSIC_COLOR_DUNGEON);

        /* Camera shop in MAP_HOUSE */
        gb_write_hram(&gb, hMapId, MAP_HOUSE);
        gb_write_hram(&gb, hMapRoom, ROOM_INDOOR_B_CAMERA_SHOP);
        SelectMusicTrackAfterTransition(&gb);
        assert(gb_read_hram(&gb, hDefaultMusicTrack) == MUSIC_MINIGAME);

        /* Regular house in MAP_HOUSE */
        gb_write_hram(&gb, hMapId, MAP_HOUSE);
        gb_write_hram(&gb, hMapRoom, 0x00);
        SelectMusicTrackAfterTransition(&gb);
        assert(gb_read_hram(&gb, hDefaultMusicTrack) == MUSIC_INSIDE_BUILDING);
    }

    /* Test 8: SelectMusicTrackAfterTransition - 2D Side Scrolling Underground */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wSwordLevel, 1);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wBossDefeated, 0);
        gb_write(&gb, wActivePowerUp, 0);
        gb_write_hram(&gb, hMapId, 0x03);
        gb_write_hram(&gb, hIsSideScrolling, 1);
        SelectMusicTrackAfterTransition(&gb);
        assert(gb_read_hram(&gb, hDefaultMusicTrack) == MUSIC_2D_UNDERGROUND);

        /* Map >= 0x0A does not override to 2D underground */
        gb_write_hram(&gb, hMapId, 0x0A);
        SelectMusicTrackAfterTransition(&gb);
        assert(gb_read_hram(&gb, hDefaultMusicTrack) == MUSIC_CAVE);
    }

    /* Test 9: SpawnChestWithItem - Default Slot Allocation */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hIntersectedObjectLeft, 0x44);
        gb_write_hram(&gb, hIntersectedObjectTop, 0x62);
        gb_write_hram(&gb, hMultiPurpose8, 0x2A);

        /* All slots disabled except slot 15 */
        for (int i = 0; i < MAX_ENTITIES; i++) {
            gb_write(&gb, (uint16_t)(wEntitiesStatusTable + i), 0);
        }

        bool ok = SpawnChestWithItem(&gb, NULL);
        assert(ok);
        /* Slot 15 should have been chosen (scanned 15 down to 0) */
        assert(gb_read(&gb, (uint16_t)(wEntitiesStatusTable + 15)) == ENTITY_STATUS_INIT);
        assert(gb_read(&gb, (uint16_t)(wEntitiesTypeTable + 15)) == ENTITY_CHEST_WITH_ITEM);
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosXTable + 15)) == 0x48);
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosYTable + 15)) == 0x70);
        assert(gb_read(&gb, (uint16_t)(wEntitiesSpriteVariantTable + 15)) == 0x2A);
    }

    /* Test 10: SpawnChestWithItem - Entity Table Full */
    {
        GBState gb;
        gb_init(&gb);
        for (int i = 0; i < MAX_ENTITIES; i++) {
            gb_write(&gb, (uint16_t)(wEntitiesStatusTable + i), ENTITY_STATUS_ACTIVE);
        }
        bool ok = SpawnChestWithItem(&gb, NULL);
        assert(!ok);
    }

    /* Test 11: SpawnChestWithItem - Custom Callback */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hIntersectedObjectLeft, 0x20);
        gb_write_hram(&gb, hIntersectedObjectTop, 0x30);
        gb_write_hram(&gb, hMultiPurpose8, 0x05);

        bool ok = SpawnChestWithItem(&gb, mock_spawn_entity_slot3);
        assert(ok);
        assert(gb_read(&gb, (uint16_t)(wEntitiesStatusTable + 3)) == ENTITY_STATUS_ACTIVE - 1);
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosXTable + 3)) == 0x28);
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosYTable + 3)) == 0x40);
        assert(gb_read(&gb, (uint16_t)(wEntitiesSpriteVariantTable + 3)) == 0x05);

        /* Callback failure */
        ok = SpawnChestWithItem(&gb, mock_spawn_entity_fail);
        assert(!ok);
    }

    /* Test 12: UseOcarina - Blocked Conditions */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hWaveSfx, 0);

        /* Link in the air */
        gb_write(&gb, wIsLinkInTheAir, 1);
        UseOcarina(&gb);
        assert(gb_read_hram(&gb, hWaveSfx) == 0);

        /* Link already playing ocarina */
        gb_write(&gb, wIsLinkInTheAir, 0);
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 10);
        UseOcarina(&gb);
        assert(gb_read_hram(&gb, hWaveSfx) == 0);

        /* Link using hookshot */
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0);
        gb_write(&gb, wIsUsingHookshot, 1);
        UseOcarina(&gb);
        assert(gb_read_hram(&gb, hWaveSfx) == 0);
    }

    /* Test 13: UseOcarina - Song Dispatching */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wC5A4, 0xFF);
        gb_write(&gb, wC5A5, 0xFF);

        /* No songs available (wOcarinaSongFlags & 7 == 0) */
        gb_write(&gb, wOcarinaSongFlags, 0x00);
        UseOcarina(&gb);
        assert(gb_read(&gb, wC5A4) == 0);
        assert(gb_read(&gb, wC5A5) == 0);
        assert(gb_read(&gb, wLinkPlayingOcarinaCountdown) == 0xD0);
        assert(gb_read_hram(&gb, hWaveSfx) == WAVE_SFX_OCARINA_NOSONG);

        /* Song 0: Ballad of the Wind Fish */
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0);
        gb_write(&gb, wOcarinaSongFlags, 0x07);
        gb_write(&gb, wSelectedSongIndex, 0);
        UseOcarina(&gb);
        assert(gb_read(&gb, wLinkPlayingOcarinaCountdown) == 0xDC);
        assert(gb_read_hram(&gb, hWaveSfx) == WAVE_SFX_OCARINA_BALLAD);

        /* Song 1: Manbo's Mambo */
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0);
        gb_write(&gb, wSelectedSongIndex, 1);
        UseOcarina(&gb);
        assert(gb_read(&gb, wLinkPlayingOcarinaCountdown) == 0xD0);
        assert(gb_read_hram(&gb, hWaveSfx) == WAVE_SFX_OCARINA_MAMBO);

        /* Song 2: Frog's Song of the Soul */
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0);
        gb_write(&gb, wSelectedSongIndex, 2);
        UseOcarina(&gb);
        assert(gb_read(&gb, wLinkPlayingOcarinaCountdown) == 0xBB);
        assert(gb_read_hram(&gb, hWaveSfx) == WAVE_SFX_OCARINA_FROG);
    }

    /* Test 14: FireHookshot - Blocked when In Air */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wIsLinkInTheAir, 1);
        bool ok = FireHookshot(&gb);
        assert(!ok);
    }

    /* Test 15: FireHookshot - Successful Fire in All 4 Directions */
    {
        const uint8_t dirs[4] = { DIRECTION_RIGHT, DIRECTION_LEFT, DIRECTION_UP, DIRECTION_DOWN };
        const int8_t exp_vx[4] = { 0x30, -0x30, 0x00, 0x00 };
        const int8_t exp_vy[4] = { 0x00, 0x00, -0x30, 0x30 };

        for (int d = 0; d < 4; d++) {
            GBState gb;
            gb_init(&gb);
            gb_write_hram(&gb, hLinkDirection, dirs[d]);

            bool ok = FireHookshot(&gb);
            assert(ok);

            /* Slot 0 was free, so projectile should be in slot 0 */
            assert(gb_read(&gb, (uint16_t)(wEntitiesTypeTable + 0)) == ENTITY_HOOKSHOT_CHAIN);
            assert(gb_read(&gb, (uint16_t)(wEntitiesTransitionCountdownTable + 0)) == 0x2A);
            assert(gb_read(&gb, (uint16_t)(wEntitiesSpriteVariantTable + 0)) == 0x00);
            assert((int8_t)gb_read(&gb, (uint16_t)(wEntitiesSpeedXTable + 0)) == exp_vx[d]);
            assert((int8_t)gb_read(&gb, (uint16_t)(wEntitiesSpeedYTable + 0)) == exp_vy[d]);
        }
    }

    /* Test 16: FireHookshot - Entity Table Full */
    {
        GBState gb;
        gb_init(&gb);
        for (int i = 0; i < MAX_ENTITIES; i++) {
            gb_write(&gb, (uint16_t)(wEntitiesStatusTable + i), ENTITY_STATUS_ACTIVE);
        }
        bool ok = FireHookshot(&gb);
        assert(!ok);
    }

    /* Test 17: DirectionToLinkAnimationState, HorizontalIncrementForLinkPosition,
     * VerticalIncrementForLinkPosition, JoypadToLinkDirection Tables */
    {
        assert(DirectionToLinkAnimationState[DIRECTION_RIGHT] == LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_RIGHT);
        assert(DirectionToLinkAnimationState[DIRECTION_LEFT]  == LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_LEFT);
        assert(DirectionToLinkAnimationState[DIRECTION_UP]    == LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_UP);
        assert(DirectionToLinkAnimationState[DIRECTION_DOWN]  == LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_DOWN);

        assert(sizeof(HorizontalIncrementForLinkPosition) == 32);
        assert(sizeof(VerticalIncrementForLinkPosition) == 32);
        assert(sizeof(JoypadToLinkDirection) == 11);

        assert(JoypadToLinkDirection[0] == DIRECTION_KEEP);
        assert(JoypadToLinkDirection[1] == DIRECTION_RIGHT);
        assert(JoypadToLinkDirection[2] == DIRECTION_LEFT);
        assert(JoypadToLinkDirection[3] == DIRECTION_KEEP);
        assert(JoypadToLinkDirection[4] == DIRECTION_UP);
        assert(JoypadToLinkDirection[8] == DIRECTION_DOWN);
        assert(JoypadToLinkDirection[5] == DIRECTION_KEEP);
    }

    /* Test 18: func_002_4338 - Lifted Object State */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hLinkAnimationState, 0);
        gb_write_hram(&gb, hLinkInteractiveMotionBlocked, 0);

        /* a < 2: no effect */
        gb_write(&gb, wIsCarryingLiftedObject, 0);
        func_002_4338(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == 0);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 0);

        gb_write(&gb, wIsCarryingLiftedObject, 1);
        func_002_4338(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == 0);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 0);

        /* a >= 2: animation state updated and motion blocked */
        gb_write(&gb, wIsCarryingLiftedObject, 2);
        func_002_4338(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == 2);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 1);

        gb_write(&gb, wIsCarryingLiftedObject, 0x38);
        func_002_4338(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == 0x38);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 1);
    }

    /* Test 19: func_002_434A - Attack Step Countdown & Animation State */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hLinkAnimationState, 0);

        /* Countdown mask is zero */
        gb_write(&gb, wLinkAttackStepAnimationCountdown, 0x80);
        func_002_434A(&gb);
        assert(gb_read(&gb, wLinkAttackStepAnimationCountdown) == 0);
        assert(gb_read_hram(&gb, hLinkAnimationState) == 0);

        /* Countdown > 0: decrements and updates state based on direction */
        const uint8_t dirs[4] = { DIRECTION_RIGHT, DIRECTION_LEFT, DIRECTION_UP, DIRECTION_DOWN };
        const uint8_t exp_anim[4] = {
            LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_RIGHT,
            LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_LEFT,
            LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_UP,
            LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_DOWN
        };

        for (int d = 0; d < 4; d++) {
            gb_write(&gb, wLinkAttackStepAnimationCountdown, 0x15);
            gb_write_hram(&gb, hLinkDirection, dirs[d]);
            func_002_434A(&gb);
            assert(gb_read(&gb, wLinkAttackStepAnimationCountdown) == 0x14);
            assert(gb_read_hram(&gb, hLinkAnimationState) == exp_anim[d]);
        }
    }

    /* Test 20: MoveLinkToPressedButtonDirection - Normal & Piece of Power */
    {
        GBState gb;
        gb_init(&gb);

        /* Normal: Right (1) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x01);
        MoveLinkToPressedButtonDirection(&gb, 0);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedX) == 0x10);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Normal: Left (2) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x02);
        MoveLinkToPressedButtonDirection(&gb, 0);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedX) == (int8_t)0xF0);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Normal: Up + Right (5) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x05);
        MoveLinkToPressedButtonDirection(&gb, 0);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedX) == 0x0C);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedY) == (int8_t)0xF4);

        /* Piece of Power (offset 0x10): Right (1) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x01);
        MoveLinkToPressedButtonDirection(&gb, 0x10);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedX) == 0x14);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Piece of Power: Up + Left (6) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x06);
        MoveLinkToPressedButtonDirection(&gb, 0x10);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedX) == (int8_t)0xF1);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedY) == (int8_t)0xF1);
    }

    /* Test 21: func_002_438F - Smooth Acceleration / Speed Nudging */
    {
        GBState gb;
        gb_init(&gb);

        /* Target speed for Right (1) is X=0x10, Y=0x00 */
        gb_write_hram(&gb, hPressedButtonsMask, 0x01);

        /* Case 1: current speed X < target X -> increments speed X */
        gb_write_hram(&gb, hLinkSpeedX, 0x0E);
        gb_write_hram(&gb, hLinkSpeedY, 0x00);
        func_002_438F(&gb, 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x0F);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Case 2: current speed X > target X -> decrements speed X */
        gb_write_hram(&gb, hLinkSpeedX, 0x12);
        gb_write_hram(&gb, hLinkSpeedY, 0x00);
        func_002_438F(&gb, 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x11);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Case 3: current speed X == target X -> unmodified */
        gb_write_hram(&gb, hLinkSpeedX, 0x10);
        gb_write_hram(&gb, hLinkSpeedY, 0x00);
        func_002_438F(&gb, 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x10);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Target speed for Up (4) is X=0x00, Y=0xF0 (-16) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x04);

        /* Case 4: current speed Y is 0xF4 (-12, which is > -16) -> decrements towards 0xF0 */
        gb_write_hram(&gb, hLinkSpeedX, 0x00);
        gb_write_hram(&gb, hLinkSpeedY, 0xF4);
        func_002_438F(&gb, 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x00);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0xF3);

        /* Case 5: current speed Y is 0xEC (-20, which is < -16) -> increments towards 0xF0 */
        gb_write_hram(&gb, hLinkSpeedX, 0x00);
        gb_write_hram(&gb, hLinkSpeedY, 0xEC);
        func_002_438F(&gb, 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x00);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0xED);
    }

    printf("[+] Bank 2 unit tests passed successfully!\n");
}
