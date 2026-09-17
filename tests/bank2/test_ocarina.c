#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/ocarina.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/joypad.h"
#include "constants/memory.h"
#include "constants/sfx.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void test_bank2_ocarina_use(void) {
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

}

void test_bank2_ocarina_playing(void) {
    /* Test 37: LinkPlayingOcarinaHandler */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Countdown == 0 -> returns early */
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0x00);
        LinkPlayingOcarinaHandler(&gb, NULL, NULL);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 0x00);

        /* 2. Countdown == 0xFF: increments wD210/wD211 */
        gb_init(&gb);
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0xFF);
        gb_write(&gb, wD210, 0x20);
        gb_write(&gb, wD211, 0x01);

        LinkPlayingOcarinaHandler(&gb, NULL, NULL);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 0x02);
        assert(gb_read(&gb, wD210) == 0x21);
        assert(gb_read(&gb, wD211) == 0x01);

        /* 3. Countdown == 0xFF: reaches 0x08D0 triggers termination and reload track */
        gb_init(&gb);
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0xFF);
        gb_write(&gb, wD210, 0xCF);
        gb_write(&gb, wD211, 0x08);
        gb_write(&gb, wD465, 0x00);
        gb_write(&gb, wD461, 0x03);
        gb_write(&gb, (uint16_t)(wEntitiesStateTable + 3), 0x05);
        g_mock_select_music_calls = 0;

        LinkPlayingOcarinaHandler(&gb, mock_select_music, NULL);
        assert(gb_read(&gb, wLinkPlayingOcarinaCountdown) == 0x00);
        assert(gb_read(&gb, wC167) == 0x00);
        assert(gb_read(&gb, wC5A3) == 0x03);
        assert(gb_read(&gb, (uint16_t)(wEntitiesStateTable + 3)) == 0x00);
        assert(g_mock_select_music_calls == 1);

        /* 4. Countdown == 0xFF: A button cancels and reloads track */
        gb_init(&gb);
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0xFF);
        gb_write(&gb, wD210, 0x00);
        gb_write(&gb, wD211, 0x00);
        gb_write(&gb, wD465, 0x00);
        gb_write(&gb, wD461, 0x02);
        gb_write(&gb, (uint16_t)(wEntitiesStateTable + 2), 0x05);
        gb_write_hram(&gb, hJoypadState, J_A);
        g_mock_select_music_calls = 0;

        LinkPlayingOcarinaHandler(&gb, mock_select_music, NULL);
        assert(gb_read(&gb, wLinkPlayingOcarinaCountdown) == 0x00);
        assert(gb_read(&gb, wC5A3) == 0x03);
        assert(gb_read(&gb, (uint16_t)(wEntitiesStateTable + 2)) == 0x00);
        assert(g_mock_select_music_calls == 1);

        /* 5. Countdown decrements towards 0 */
        gb_init(&gb);
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0x15);
        gb_write(&gb, wC5A4, 0x00);
        LinkPlayingOcarinaHandler(&gb, NULL, NULL);
        assert(gb_read(&gb, wLinkPlayingOcarinaCountdown) == 0x14);
        assert(gb_read(&gb, wC5A4) == 0x01);
        assert(gb_read(&gb, wC167) == 0x02);
        assert(gb_read(&gb, wC111) == 0x02);

        /* 6. Reaching 0 with Marin following (outdoor, song != 1) triggers Dialog277 (0x277) */
        gb_init(&gb);
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0x01);
        gb_write(&gb, wIsMarinFollowingLink, 0x01);
        gb_write(&gb, wSelectedSongIndex, 0x00);
        gb_write(&gb, wIsIndoor, 0x00);

        LinkPlayingOcarinaHandler(&gb, NULL, NULL);
        assert(gb_read(&gb, wLinkPlayingOcarinaCountdown) == 0x00);
        assert(gb_read(&gb, wDialogIndex) == 0x77);
        assert(gb_read(&gb, wDialogIndexHi) == 0x02);

        /* 7. Reaching 0 with no Marin and no songs known triggers Dialog08E (0x8E) */
        gb_init(&gb);
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0x01);
        gb_write(&gb, wIsMarinFollowingLink, 0x00);
        gb_write(&gb, wOcarinaSongFlags, 0x00);

        LinkPlayingOcarinaHandler(&gb, NULL, NULL);
        assert(gb_read(&gb, wLinkPlayingOcarinaCountdown) == 0x00);
        assert(gb_read(&gb, wDialogIndex) == 0x8E);
        assert(gb_read(&gb, wDialogIndexHi) == 0x00);

        /* 8. Song 1 (Manbo's Mambo) triggers warp transition and jingle */
        gb_init(&gb);
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0x01);
        gb_write(&gb, wOcarinaSongFlags, 0x02);
        gb_write(&gb, wSelectedSongIndex, 0x01);
        gb_write_hram(&gb, hJingle, 0x00);

        LinkPlayingOcarinaHandler(&gb, NULL, NULL);
        assert(gb_read(&gb, wLinkPlayingOcarinaCountdown) == 0x00);
        assert(gb_read(&gb, wTransitionGfx) == TRANSITION_GFX_MANBO_IN);
        assert(gb_read_hram(&gb, hJingle) == JINGLE_MANBO_WARP);

        /* 9. Musical note entity spawning: countdown >= 0x10 and wC5A4 reaches 0x14 */
        gb_init(&gb);
        gb_write(&gb, wLinkPlayingOcarinaCountdown, 0x20);
        gb_write(&gb, wC5A4, 0x13); /* will increment to 0x14 */
        gb_write(&gb, wC5A5, 0x00);
        gb_write_hram(&gb, hLinkPositionX, 0x50);
        gb_write_hram(&gb, hLinkPositionY, 0x60);

        LinkPlayingOcarinaHandler(&gb, NULL, mock_spawn_entity);
        /* Musical note entity (ENTITY_MUSICAL_NOTE = 0xC9) spawned at entity slot 15 */
        assert(gb_read(&gb, (uint16_t)(wEntitiesTypeTable + 15)) == ENTITY_MUSICAL_NOTE);
        /* note_y = 0x60 - 8 = 0x58 */
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosYTable + 15)) == 0x58);
        /* note_x = 0x50 + Data_002_4A12[0] (8) = 0x58 */
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosXTable + 15)) == 0x58);
        assert(gb_read(&gb, (uint16_t)(wEntitiesSpeedXTable + 15)) == 0x06);
        assert(gb_read(&gb, (uint16_t)(wEntitiesSpeedYTable + 15)) == 0xFC);
        assert(gb_read(&gb, (uint16_t)(wEntitiesInertiaTable + 15)) == 0x40);
    }

}
