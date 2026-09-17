#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/room_transition.h"
#include "bank2/vfx.h"
#include "constants/gfx.h"
#include "constants/memory.h"
#include "constants/sfx.h"
#include "constants/vfx.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void test_bank2_vfx(void) {
    /* Test 66: renderTranscientVFXs (02:54E4) */
    {
        GBState gb;

        /* 1. VFX iteration: calls callback for non-zero slots */
        gb_init(&gb);
        gb_write(&gb, (uint16_t)(wTranscientVfxTypeTable + 3), 0x02);
        gb_write(&gb, (uint16_t)(wTranscientVfxTypeTable + 7), 0x05);
        g_mock_transient_vfx_calls = 0;

        renderTranscientVFXs(&gb, mock_render_transient_vfx);

        assert(g_mock_transient_vfx_calls == 2);
        assert(gb_read(&gb, wActiveEntityIndex) == 0x00);

        /* 2. Inactive staircase, Link still on top (dx < 12 && dy < 12) -> stays inactive */
        gb_init(&gb);
        gb_write_hram(&gb, hStaircase, STAIRCASE_INACTIVE);
        gb_write_hram(&gb, hLinkPositionX, 0x40);
        gb_write_hram(&gb, hStaircasePosX, 0x40);
        gb_write_hram(&gb, hLinkPositionY, 0x40);
        gb_write_hram(&gb, hStaircasePosY, 0x40);

        renderTranscientVFXs(&gb, NULL);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_INACTIVE);

        /* 3. Inactive staircase, Link leaves in X (dx >= 12, Link=0x50, Staircase=0x40 -> 0x10 + 6 = 22 >= 12) -> becomes active */
        gb_write_hram(&gb, hLinkPositionX, 0x50);
        renderTranscientVFXs(&gb, NULL);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_ACTIVE);

        /* 4. Inactive staircase, Link leaves in Y (dy >= 12, Link=0x50, Staircase=0x40) -> becomes active */
        gb_init(&gb);
        gb_write_hram(&gb, hStaircase, STAIRCASE_INACTIVE);
        gb_write_hram(&gb, hLinkPositionX, 0x40);
        gb_write_hram(&gb, hStaircasePosX, 0x40);
        gb_write_hram(&gb, hLinkPositionY, 0x50);
        gb_write_hram(&gb, hStaircasePosY, 0x40);

        renderTranscientVFXs(&gb, NULL);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_ACTIVE);

        /* 5. Room transition active -> early returns, staircase untouched */
        gb_init(&gb);
        gb_write(&gb, wRoomTransitionState, 1);
        gb_write_hram(&gb, hStaircase, STAIRCASE_INACTIVE);
        gb_write_hram(&gb, hLinkPositionX, 0x50);
        gb_write_hram(&gb, hStaircasePosX, 0x40);

        renderTranscientVFXs(&gb, NULL);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_INACTIVE);
    }

    /* Test 67: label_002_5487 (02:5487) */
    {
        GBState gb;
        gb_init(&gb);

        gb_write(&gb, wIndoorARoomStatus, 0x55);
        gb_write(&gb, wIndoorBRoomStatus, 0xAA);
        gb_write(&gb, wDialogCooldown, 0x05);
        gb_write(&gb, wPhotoAlbumCooldown, 0x08);

        label_002_5487(&gb, NULL);

        assert(gb_read(&gb, wIndoorARoomStatus) == 0x00);
        assert(gb_read(&gb, wIndoorBRoomStatus) == 0x00);
        assert(gb_read(&gb, wDialogCooldown) == 0x04);
        assert(gb_read(&gb, wPhotoAlbumCooldown) == 0x07);

        /* Cooldowns at 0 do not underflow */
        gb_write(&gb, wDialogCooldown, 0x00);
        gb_write(&gb, wPhotoAlbumCooldown, 0x00);
        label_002_5487(&gb, NULL);
        assert(gb_read(&gb, wDialogCooldown) == 0x00);
        assert(gb_read(&gb, wPhotoAlbumCooldown) == 0x00);
    }

    /* Test 68: ClearTranscientVfx and func_002_58D0 (02:58E6, 02:58D0) */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. ClearTranscientVfx sets slot type to 0 */
        gb_write(&gb, (uint16_t)(wTranscientVfxTypeTable + 3), 0x05);
        ClearTranscientVfx(&gb, 3);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxTypeTable + 3)) == 0x00);

        /* 2. func_002_58D0: Y >= 0x88 -> clears VFX, updates hMultiPurpose1 */
        gb_write(&gb, (uint16_t)(wTranscientVfxTypeTable + 1), 0x02);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 1), 0x88);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 1), 0x20);
        func_002_58D0(&gb, 1);
        assert(gb_read_hram(&gb, hMultiPurpose1) == 0x88);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxTypeTable + 1)) == 0x00);

        /* 3. func_002_58D0: Y < 0x88, X >= 0xA8 -> clears VFX, updates hMultiPurpose1 and 2 */
        gb_write(&gb, (uint16_t)(wTranscientVfxTypeTable + 2), 0x07);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 2), 0x40);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 2), 0xA8);
        func_002_58D0(&gb, 2);
        assert(gb_read_hram(&gb, hMultiPurpose1) == 0x40);
        assert(gb_read_hram(&gb, hMultiPurpose2) == 0xA8);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxTypeTable + 2)) == 0x00);

        /* 4. func_002_58D0: within bounds -> keeps VFX active */
        gb_write(&gb, (uint16_t)(wTranscientVfxTypeTable + 4), 0x09);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 4), 0x45);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 4), 0x60);
        func_002_58D0(&gb, 4);
        assert(gb_read_hram(&gb, hMultiPurpose1) == 0x45);
        assert(gb_read_hram(&gb, hMultiPurpose2) == 0x60);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxTypeTable + 4)) == 0x09);
    }

    /* Test 69: label_002_5854, label_002_58F5, and label_002_583A (02:5854, 02:58F5, 02:583A) */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. label_002_5854 writes single sprite and advances pointers */
        gb_write_hram(&gb, hMultiPurpose1, 0x10);
        gb_write_hram(&gb, hMultiPurpose2, 0x20);
        static const uint8_t test_sprite[4] = { 0x02, 0x05, 0x44, 0x10 };
        const uint8_t *src_ptr = test_sprite;
        uint16_t dst_addr = 0xC000;
        label_002_5854(&gb, &src_ptr, &dst_addr);
        assert(gb_read(&gb, 0xC000) == 0x12);
        assert(gb_read(&gb, 0xC001) == 0x25);
        assert(gb_read(&gb, 0xC002) == 0x44);
        assert(gb_read(&gb, 0xC003) == 0x10);
        assert(src_ptr == test_sprite + 4);
        assert(dst_addr == 0xC004);

        /* 2. label_002_58F5 advances slot by count*4 and wraps at 0x60 */
        gb_write(&gb, wOAMNextAvailableSlot, 0x58);
        gb_write(&gb, wC3C1, 0x10);
        label_002_58F5(&gb, 2); /* 2*4 = 8 bytes -> 0x58 + 8 = 0x60 -> wraps to 0x00 */
        assert(gb_read(&gb, wOAMNextAvailableSlot) == 0x00);
        assert(gb_read(&gb, wC3C1) == 0x18);

        /* When wC3C1 >= 0x60, resets slot from Data_002_58ED */
        gb_write(&gb, wOAMNextAvailableSlot, 0x10);
        gb_write(&gb, wC3C1, 0x5C);
        gb_write_hram(&gb, hFrameCounter, 0x03);
        gb_write(&gb, wActiveEntityIndex, 0x02); /* (3 + 2) & 7 = 5 -> Data_002_58ED[5] = 0x28 */
        label_002_58F5(&gb, 2); /* +8 -> wC3C1 = 0x64 >= 0x60 */
        assert(gb_read(&gb, wOAMNextAvailableSlot) == 0x28);

        /* 3. label_002_583A emits 2 sprites and advances slot */
        gb_init(&gb);
        gb_write_hram(&gb, hMultiPurpose1, 0x30);
        gb_write_hram(&gb, hMultiPurpose2, 0x40);
        gb_write(&gb, wOAMNextAvailableSlot, 0x00);
        label_002_583A(&gb, Data_002_5736, 0);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 0)) == 0x30 + Data_002_5736[0]);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 1)) == 0x40 + Data_002_5736[1]);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 2)) == Data_002_5736[2]);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 3)) == Data_002_5736[3]);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 4)) == 0x30 + Data_002_5736[4]);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 5)) == 0x40 + Data_002_5736[5]);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 6)) == Data_002_5736[6]);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 7)) == Data_002_5736[7]);
        assert(gb_read(&gb, wOAMNextAvailableSlot) == 0x08);
    }

    /* Test 70: RenderTranscientWaterSplash and RenderTranscientPegasusSplash (02:5825, 02:581D) */
    {
        GBState gb;
        gb_init(&gb);

        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 0), 0x30);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 0), 0x40);
        gb_write_hram(&gb, hMultiPurpose0, 0x00);
        gb_write(&gb, wOAMNextAvailableSlot, 0x00);

        /* 1. Deep water splash uses Data_002_57FD */
        gb_write(&gb, wC1A7, 0x00);
        RenderTranscientWaterSplash(&gb, 0);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 2)) == Data_002_57FD[2]);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 3)) == Data_002_57FD[3]);

        /* 2. Shallow water splash (wC1A7 == 2) uses Data_002_5867 */
        gb_init(&gb);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 0), 0x30);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 0), 0x40);
        gb_write_hram(&gb, hMultiPurpose0, 0x00);
        gb_write(&gb, wOAMNextAvailableSlot, 0x00);
        gb_write(&gb, wC1A7, 0x02);
        RenderTranscientWaterSplash(&gb, 0);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 2)) == Data_002_5867[2]);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 3)) == Data_002_5867[3]);

        /* 3. Pegasus splash uses Data_002_580D */
        gb_init(&gb);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 0), 0x30);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 0), 0x40);
        gb_write_hram(&gb, hMultiPurpose0, 0x00);
        gb_write(&gb, wOAMNextAvailableSlot, 0x00);
        RenderTranscientPegasusSplash(&gb, 0);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 2)) == Data_002_580D[2]);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 3)) == Data_002_580D[3]);
    }

    /* Test 71: RenderTranscientPoof (02:58A4) */
    {
        GBState gb;
        gb_init(&gb);

        g_mock_spawn_chest_calls = 0;
        g_mock_spawn_stairs_calls = 0;

        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 1), 0x20);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 1), 0x30);
        gb_write(&gb, (uint16_t)(wTranscientVfxTypeTable + 1), TRANSCIENT_VFX_CHEST_APPEARS);
        gb_write_hram(&gb, hMultiPurpose0, 0x04);

        /* 1. At countdown == 4 and type == 3, triggers spawn_chest */
        RenderTranscientPoof(&gb, 1, mock_spawn_chest, mock_spawn_stairs);
        assert(g_mock_spawn_chest_calls == 1);
        assert(g_mock_spawn_stairs_calls == 0);

        /* 2. At countdown == 4 and type == 4, triggers spawn_stairs */
        gb_write(&gb, (uint16_t)(wTranscientVfxTypeTable + 1), TRANSCIENT_VFX_STAIRS_APPEARS);
        RenderTranscientPoof(&gb, 1, mock_spawn_chest, mock_spawn_stairs);
        assert(g_mock_spawn_chest_calls == 1);
        assert(g_mock_spawn_stairs_calls == 1);

        /* 3. At countdown != 4, neither callback triggered */
        gb_write_hram(&gb, hMultiPurpose0, 0x03);
        RenderTranscientPoof(&gb, 1, mock_spawn_chest, mock_spawn_stairs);
        assert(g_mock_spawn_chest_calls == 1);
        assert(g_mock_spawn_stairs_calls == 1);
    }

    /* Test 72: Smoke, SwordPoke, LaserBeam, MovingSparkle (02:5746, 02:57ED, 02:57B4, 02:575E) */
    {
        GBState gb;
        gb_init(&gb);

        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 0), 0x25);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 0), 0x35);
        gb_write_hram(&gb, hMultiPurpose0, 0x08);
        gb_write(&gb, wOAMNextAvailableSlot, 0x00);

        /* 1. RenderTranscientSmoke */
        RenderTranscientSmoke(&gb, 0);
        assert(gb_read(&gb, wOAMNextAvailableSlot) == 0x08);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 2)) == Data_002_5736[10]);

        /* 2. RenderTranscientSwordPoke */
        gb_write(&gb, wOAMNextAvailableSlot, 0x00);
        RenderTranscientSwordPoke(&gb, 0);
        assert(gb_read(&gb, wOAMNextAvailableSlot) == 0x08);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 2)) == Data_002_57DD[10]);

        /* 3. RenderTranscientLaserBeam */
        gb_write(&gb, wOAMNextAvailableSlot, 0x00);
        gb_write_hram(&gb, hFrameCounter, 0x01);
        RenderTranscientLaserBeam(&gb, 0); /* 1 ^ 0 = 1 -> attr 0x10 */
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 0)) == 0x25);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 1)) == 0x35);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 2)) == 0x24);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 3)) == 0x10);
        assert(gb_read(&gb, wOAMNextAvailableSlot) == 0x04);

        /* 4. RenderTranscientMovingSparkle */
        gb_init(&gb);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 0), 0x20);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 0), 0x30);
        gb_write(&gb, (uint16_t)(wC590 + 0), 0x00); /* dir 0: dx=+1, dy=+1 */
        gb_write_hram(&gb, hMultiPurpose0, 0x0A); /* >= 0x0A -> moves position */
        gb_write(&gb, wOAMNextAvailableSlot, 0x00);
        RenderTranscientMovingSparkle(&gb, 0);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxPosXTable + 0)) == 0x31);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxPosYTable + 0)) == 0x21);
        assert(gb_read(&gb, wOAMNextAvailableSlot) == 0x08);
    }

    /* Test 73: LavaSplash, PegasusDust, SwordBeam (02:560C, 02:5718, 02:55DC) */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. RenderTranscientLavaSplash emits 4 sprites */
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 0), 0x20);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 0), 0x30);
        gb_write(&gb, wOAMNextAvailableSlot, 0x00);
        RenderTranscientLavaSplash(&gb, 0);
        assert(gb_read(&gb, wOAMNextAvailableSlot) == 0x10); /* 4 sprites * 4 bytes */
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 0)) == 0x20); /* Y */
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 1)) == 0x30 + 0x10); /* X + 16 */
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 2)) == 0x6C); /* tile */
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 3)) == 0x20); /* attr */

        /* 2. RenderTranscientPegasusDust running with boots vs stopped */
        gb_init(&gb);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 0), 0x20);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 0), 0x30);
        gb_write_hram(&gb, hMultiPurpose0, 0x00);
        gb_write(&gb, wIsRunningWithPegasusBoots, 0);
        RenderTranscientPegasusDust(&gb, 0);
        assert(gb_read(&gb, (uint16_t)(wOAMBuffer + 2)) == Data_002_5708[2]); /* writes to wOAMBuffer */

        gb_init(&gb);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 0), 0x20);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 0), 0x30);
        gb_write_hram(&gb, hMultiPurpose0, 0x00);
        gb_write(&gb, wIsRunningWithPegasusBoots, 1);
        gb_write(&gb, wOAMNextAvailableSlot, 0x00);
        RenderTranscientPegasusDust(&gb, 0);
        assert(gb_read(&gb, (uint16_t)(wDynamicOAMBuffer + 2)) == Data_002_5708[2]); /* writes to dynamic OAM */
        assert(gb_read(&gb, wOAMNextAvailableSlot) == 0x08);

        /* 3. RenderTranscientSwordBeam frame counter parity */
        gb_init(&gb);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 0), 0x20);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 0), 0x30);
        gb_write(&gb, wOAMNextAvailableSlot, 0x00);
        gb_write_hram(&gb, hFrameCounter, 0x00);
        RenderTranscientSwordBeam(&gb, 0); /* 0 ^ 0 = 0 -> returns early */
        assert(gb_read(&gb, wOAMNextAvailableSlot) == 0x00);

        gb_write_hram(&gb, hFrameCounter, 0x01); /* 1 ^ 0 = 1 -> renders */
        RenderTranscientSwordBeam(&gb, 0);
        assert(gb_read(&gb, wOAMNextAvailableSlot) == 0x08);
    }

    /* Test 74: RenderTranscientRumble (02:5646) */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Countdown 0xDE triggers door unlocked noise */
        gb_write_hram(&gb, hMultiPurpose0, 0xDE);
        RenderTranscientRumble(&gb, 0);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 0x02);
        assert(gb_read(&gb, wC167) == 0x02);
        assert(gb_read_hram(&gb, hNoiseSfx) == NOISE_SFX_DOOR_UNLOCKED);

        /* 2. Countdown 0xA0 triggers open key cavern noise */
        gb_init(&gb);
        gb_write_hram(&gb, hMultiPurpose0, 0xA0);
        RenderTranscientRumble(&gb, 0);
        assert(gb_read_hram(&gb, hNoiseSfx) == NOISE_SFX_OPEN_KEY_CAVERN);

        /* 3. Countdown 0x0A triggers next world music track countdown */
        gb_init(&gb);
        gb_write_hram(&gb, hMultiPurpose0, 0x0A);
        RenderTranscientRumble(&gb, 0);
        assert(gb_read(&gb, wNextWorldMusicTrackCountdown) == 0x50);

        /* 4. Countdown 0x24 triggers screen shake */
        gb_init(&gb);
        gb_write_hram(&gb, hMultiPurpose0, 0x24); /* 0x24 & 4 != 0 -> 0xFE */
        RenderTranscientRumble(&gb, 0);
        assert(gb_read(&gb, wScreenShakeHorizontal) == 0xFE);

        gb_write_hram(&gb, hMultiPurpose0, 0x20); /* 0x20 & 4 == 0 -> 0x01 */
        RenderTranscientRumble(&gb, 0);
        assert(gb_read(&gb, wScreenShakeHorizontal) == 0x01);

        /* 5. Countdown 0x08 redraws tiles, replaces room object, plays dungeon opened jingle */
        gb_init(&gb);
        gb_write_hram(&gb, hMultiPurpose0, 0x08);
        gb_write(&gb, (uint16_t)(wTranscientVfxCountdownTable + 0), 0x08);
        gb_write(&gb, wDrawCommandsSize, 0x00);
        RenderTranscientRumble(&gb, 0);
        assert(gb_read(&gb, wDrawCommandsSize) == 0x08);
        assert(gb_read(&gb, (uint16_t)(wDrawCommand + 2)) == 0x41);
        assert(gb_read(&gb, (uint16_t)(wDrawCommand + 3)) == Data_002_5642[0]);
        assert(gb_read(&gb, (uint16_t)(wRoomObjectsArea + 0x27)) == 0xE3);
        assert(gb_read_hram(&gb, hJingle) == JINGLE_DUNGEON_OPENED);
    }

    /* Test 75: RenderTranscientVfx and func_002_5926 (02:5567, 02:5926) */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. None type does nothing */
        gb_write(&gb, (uint16_t)(wTranscientVfxTypeTable + 0), TRANSCIENT_VFX_NONE);
        RenderTranscientVfx(&gb, 0, NULL, NULL);

        /* 2. Room transition state clears VFX */
        gb_write(&gb, (uint16_t)(wTranscientVfxTypeTable + 1), TRANSCIENT_VFX_WATER_SPLASH);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 1), 0x20);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 1), 0x30);
        gb_write(&gb, wRoomTransitionState, 1);
        RenderTranscientVfx(&gb, 1, NULL, NULL);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxTypeTable + 1)) == 0x00);

        /* 3. Countdown decrements and clears VFX when reaches 0 */
        gb_init(&gb);
        gb_write(&gb, (uint16_t)(wTranscientVfxTypeTable + 2), TRANSCIENT_VFX_SMOKE);
        gb_write(&gb, (uint16_t)(wTranscientVfxCountdownTable + 2), 0x02);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosYTable + 2), 0x20);
        gb_write(&gb, (uint16_t)(wTranscientVfxPosXTable + 2), 0x30);
        RenderTranscientVfx(&gb, 2, NULL, NULL);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxCountdownTable + 2)) == 0x01);
        assert(gb_read_hram(&gb, hMultiPurpose0) == 0x01);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxTypeTable + 2)) == TRANSCIENT_VFX_SMOKE);

        RenderTranscientVfx(&gb, 2, NULL, NULL);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxCountdownTable + 2)) == 0x00);
        assert(gb_read_hram(&gb, hMultiPurpose0) == 0x00);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxTypeTable + 2)) == 0x00); /* cleared! */

        /* 4. func_002_5926 adds water splash at Link Y */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkPositionY, 0x42);
        gb_write_hram(&gb, hLinkPositionX, 0x55);
        func_002_5926(&gb);
        assert(gb_read_hram(&gb, hJingle) == JINGLE_WATER_SPLASH);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxTypeTable + 15)) == TRANSCIENT_VFX_WATER_SPLASH);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxPosYTable + 15)) == 0x42);
        assert(gb_read(&gb, (uint16_t)(wTranscientVfxPosXTable + 15)) == 0x55);
    }

}
