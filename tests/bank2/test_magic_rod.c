#include "test_bank2.h"

#include "gb.h"
#include "bank2/items.h"
#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/memory.h"

#include <assert.h>
#include <stdint.h>

void test_bank2_magic_rod(void) {
    /* Test 58: Bank 2 Magic Rod Visuals & OAM Buffers (02:52E0-02:5382, label_002_5310) */
    {
        /* Verify static tables */
        assert(LinkDirectionToMagicRodXOffset[0] == 0x0D);
        assert(LinkDirectionToMagicRodXOffset[1] == (int8_t)0xF3);
        assert(LinkDirectionToMagicRodXOffset[2] == 0x00);
        assert(LinkDirectionToMagicRodXOffset[3] == (int8_t)0xFF);
        assert(LinkDirectionToMagicRodXOffset[4] == 0x08);
        assert(LinkDirectionToMagicRodXOffset[5] == (int8_t)0xF8);
        assert(LinkDirectionToMagicRodXOffset[6] == 0x0C);
        assert(LinkDirectionToMagicRodXOffset[7] == (int8_t)0xF5);

        assert(LinkDirectionToMagicRodYOffset[0] == 0x00);
        assert(LinkDirectionToMagicRodYOffset[1] == 0x00);
        assert(LinkDirectionToMagicRodYOffset[2] == (int8_t)0xF3);
        assert(LinkDirectionToMagicRodYOffset[3] == 0x0E);
        assert(LinkDirectionToMagicRodYOffset[4] == (int8_t)0xF3);
        assert(LinkDirectionToMagicRodYOffset[5] == (int8_t)0xF3);
        assert(LinkDirectionToMagicRodYOffset[6] == (int8_t)0xFC);
        assert(LinkDirectionToMagicRodYOffset[7] == 0x00);

        assert(LinkDirectionToMagicRodTiles[0] == 0x06);
        assert(LinkDirectionToMagicRodTiles[4] == 0x04);
        assert(LinkDirectionToMagicRodTiles[5] == 0xFF);
        assert(LinkDirectionToMagicRodTiles[6] == 0xFF);
        assert(LinkDirectionToMagicRodTiles[7] == 0x04);

        assert(LinkDirectionToMagicRodOAMAttributes[0] == 0x02);
        assert(LinkDirectionToMagicRodOAMAttributes[2] == 0x22);
        assert(LinkDirectionToMagicRodOAMAttributes[7] == 0x42);

        /* 1. Forward swing (countdown < 8), Direction Right (0) */
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wLinkAttackStepAnimationCountdown, 0x05);
        gb_write_hram(&gb, hLinkDirection, DIRECTION_RIGHT);
        gb_write(&gb, wC145, 0x10);
        gb_write(&gb, wC13B, 0x20);
        gb_write_hram(&gb, hLinkPositionX, 0x50);

        label_002_5310(&gb);

        /* y = 0x10 + 0x20 + 0x00 = 0x30 */
        /* tile0 = 0x06 != 0xFF -> wLinkOAMBuffer[0x10] = 0x30 */
        /* tile1 = 0x08 != 0xFF -> wLinkOAMBuffer[0x14] = 0x30 */
        assert(gb_read(&gb, wLinkOAMBuffer + 0x10) == 0x30);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x14) == 0x30);
        /* x_offset = 0x0D -> pos_x = 0x50 + 0x0D = 0x5D */
        assert(gb_read(&gb, wLinkOAMBuffer + 0x11) == 0x5D);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x15) == 0x65);
        /* tiles */
        assert(gb_read(&gb, wLinkOAMBuffer + 0x12) == 0x06);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x16) == 0x08);
        /* attributes */
        assert(gb_read(&gb, wLinkOAMBuffer + 0x13) == 0x02);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x17) == 0x02);

        /* 2. Side swing (countdown >= 8), Direction Up (2 -> offset = 2 + 4 = 6) */
        gb_init(&gb);
        gb_write(&gb, wLinkAttackStepAnimationCountdown, 0x0A);
        gb_write_hram(&gb, hLinkDirection, DIRECTION_UP);
        gb_write(&gb, wC145, 0x04);
        gb_write(&gb, wC13B, 0x10);
        gb_write_hram(&gb, hLinkPositionX, 0x40);

        label_002_5310(&gb);

        /* offset 6: y_offset = (int8_t)0xFC (-4). final_y = 0x04 + 0x10 - 4 = 0x10 */
        /* tile0 = 0x06, tile1 = 0x08 */
        assert(gb_read(&gb, wLinkOAMBuffer + 0x10) == 0x10);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x14) == 0x10);
        /* offset 6: x_offset = 0x0C (+12). pos_x = 0x40 + 0x0C = 0x4C */
        assert(gb_read(&gb, wLinkOAMBuffer + 0x11) == 0x4C);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x15) == 0x54);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x12) == 0x06);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x16) == 0x08);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x13) == 0x02);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x17) == 0x02);

        /* 3. Forward swing, Direction Up (2): tile1 is 0xFF, so wLinkOAMBuffer + 0x14 should NOT be overwritten */
        gb_init(&gb);
        gb_write(&gb, wLinkAttackStepAnimationCountdown, 0x04);
        gb_write_hram(&gb, hLinkDirection, DIRECTION_UP);
        gb_write(&gb, wC145, 0x00);
        gb_write(&gb, wC13B, 0x20);
        gb_write_hram(&gb, hLinkPositionX, 0x40);
        gb_write(&gb, wLinkOAMBuffer + 0x14, 0xAA); /* sentinel */

        label_002_5310(&gb);

        /* offset 2: tile0 = 0x04, tile1 = 0xFF */
        /* y_offset = 0xF3 (-13). final_y = 0x20 - 13 = 0x13 */
        assert(gb_read(&gb, wLinkOAMBuffer + 0x10) == 0x13);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x14) == 0xAA); /* untouched because tile1 == 0xFF */
        assert(gb_read(&gb, wLinkOAMBuffer + 0x12) == 0x04);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x16) == 0xFF);
    }

    /* Test 59: Bank 2 Magic Rod Projectile Init (02:5383-02:53AF, label_002_538B / entity) */
    {
        /* Tables check */
        assert(LinkDirectionToEntitiesPositionX[0] == 4);
        assert(LinkDirectionToEntitiesPositionX[1] == -4);
        assert(LinkDirectionToEntitiesPositionX[2] == -4);
        assert(LinkDirectionToEntitiesPositionX[3] == 4);

        assert(LinkDirectionToEntitiesPositionY[0] == 4);
        assert(LinkDirectionToEntitiesPositionY[1] == 4);
        assert(LinkDirectionToEntitiesPositionY[2] == -4);
        assert(LinkDirectionToEntitiesPositionY[3] == 4);

        /* 1. Normal projectile spawn without Piece of Power */
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hLinkDirection, DIRECTION_LEFT);
        gb_write_hram(&gb, hLinkPositionX, 0x60);
        gb_write_hram(&gb, hLinkPositionY, 0x70);
        gb_write(&gb, wActivePowerUp, 0x00);

        label_002_538B_entity(&gb, 3);

        /* dir = 1: X pos = 0x60 - 4 = 0x5C, Y pos = 0x70 + 4 = 0x74 */
        assert(gb_read(&gb, wEntitiesPosXTable + 3) == 0x5C);
        assert(gb_read(&gb, wEntitiesPosYTable + 3) == 0x74);
        assert(gb_read(&gb, wEntitiesSpriteVariantTable + 3) == 0x00);
        /* speed normal: data_13AD[1] = 0xD0 (-48), data_13B5[1] = 0 */
        assert(gb_read(&gb, wEntitiesSpeedXTable + 3) == 0xD0);
        assert(gb_read(&gb, wEntitiesSpeedYTable + 3) == 0x00);

        /* 2. Projectile spawn with Piece of Power */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkDirection, DIRECTION_DOWN);
        gb_write_hram(&gb, hLinkPositionX, 0x40);
        gb_write_hram(&gb, hLinkPositionY, 0x50);
        gb_write(&gb, wActivePowerUp, POWER_UP_PIECE_OF_POWER);

        label_002_538B_entity(&gb, 5);

        /* dir = 3: X pos = 0x40 + 4 = 0x44, Y pos = 0x50 + 4 = 0x54 */
        assert(gb_read(&gb, wEntitiesPosXTable + 5) == 0x44);
        assert(gb_read(&gb, wEntitiesPosYTable + 5) == 0x54);
        assert(gb_read(&gb, wEntitiesSpriteVariantTable + 5) == 0x00);
        /* speed with piece of power: offset = 3 + 4 = 7: data_13AD[7] = 0, data_13B5[7] = 0x40 */
        assert(gb_read(&gb, wEntitiesSpeedXTable + 5) == 0x00);
        assert(gb_read(&gb, wEntitiesSpeedYTable + 5) == 0x40);

        /* 3. label_002_538B default slot 0 wrapper */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkDirection, DIRECTION_RIGHT);
        gb_write_hram(&gb, hLinkPositionX, 0x30);
        gb_write_hram(&gb, hLinkPositionY, 0x20);
        gb_write(&gb, wActivePowerUp, 0x00);

        label_002_538B(&gb);

        assert(gb_read(&gb, wEntitiesPosXTable + 0) == 0x34);
        assert(gb_read(&gb, wEntitiesPosYTable + 0) == 0x24);
        assert(gb_read(&gb, wEntitiesSpriteVariantTable + 0) == 0x00);
        assert(gb_read(&gb, wEntitiesSpeedXTable + 0) == 0x30);
        assert(gb_read(&gb, wEntitiesSpeedYTable + 0) == 0x00);
    }

}
