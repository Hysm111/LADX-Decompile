#include "test_bank2.h"

#include "gb.h"
#include "bank2/audio.h"
#include "constants/audio.h"
#include "constants/gameplay.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/rooms.h"

#include <assert.h>

void test_bank2_audio(void) {
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

}
