#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/link.h"
#include "constants/maps.h"
#include "constants/gfx.h"
#include "bank1/room_transition.h"
#include "bank1/save.h"
#include "home/gameplay.h"
#include "constants/memory.h"
#include <string.h>

const uint16_t SaveGameTable[3] = {
    0xA105, /* SaveGame1.main */
    0xA4B2, /* SaveGame2.main */
    0xA85F  /* SaveGame3.main */
};

const uint8_t MaxHeartsToStartingHealthTable[15] = {
    24, 24, 24, 24, 24, 24, /* 0 - 5 containers: 3 full hearts (24 health) */
    40, 40, 40, 40,         /* 6 - 9 containers: 5 full hearts (40 health) */
    56, 56, 56, 56,         /* 10 - 13 containers: 7 full hearts (56 health) */
    80                      /* 14 containers: 10 full hearts (80 health) */
};

static const uint8_t SaveSlot1HeartsDrawData[21] = {
    0x98, 0xCB, 0x06, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E,
    0x98, 0xEB, 0x06, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E,
    0x00
};

static const uint8_t SaveSlot2HeartsDrawData[21] = {
    0x99, 0x2B, 0x06, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E,
    0x99, 0x4B, 0x06, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E,
    0x00
};

static const uint8_t SaveSlot3HeartsDrawData[21] = {
    0x99, 0x8B, 0x06, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E,
    0x99, 0xAB, 0x06, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E,
    0x00
};

void BuildSaveSlotHeartsDrawCommand(GBState *gb) {
    if (!gb) return;

    uint8_t cmd_size = gb_read(gb, wDrawCommandsSize);
    uint8_t de = cmd_size;
    gb_write(gb, wDrawCommandsSize, (uint8_t)(cmd_size + 0x14));

    const uint8_t *src_data;
    uint8_t slot = gb_read(gb, hMultiPurpose4);
    if (slot == 0) {
        src_data = SaveSlot1HeartsDrawData;
    } else if (slot == 1) {
        src_data = SaveSlot2HeartsDrawData;
    } else {
        src_data = SaveSlot3HeartsDrawData;
    }

    uint16_t dest_addr = (uint16_t)(wDrawCommand + de);
    for (uint8_t i = 0; i < 0x15; i++) {
        gb_write(gb, dest_addr + i, src_data[i]);
    }

    uint16_t hl = (uint16_t)(wDrawCommand + de + 3); /* wDrawCommand.data */
    uint8_t c = 0;
    uint8_t health = gb_read(gb, hMultiPurpose2);
    if (health == 0) {
        goto drawEmptyHeartsLoop;
    }

    gb_write(gb, hMultiPurpose0, health);

drawFullHeartsLoop: {
        uint8_t mp0 = gb_read(gb, hMultiPurpose0);
        if (mp0 < 8) {
            goto drawHalfHeart;
        }
        mp0 -= 8;
        gb_write(gb, hMultiPurpose0, mp0);

        gb_write(gb, hl++, 0xAE);
        c++;
        if (c == 7) {
            hl += 3;
        }
        goto drawFullHeartsLoop;
    }

drawHalfHeart: {
        uint8_t mp0 = gb_read(gb, hMultiPurpose0);
        if (mp0 == 0) {
            goto drawEmptyHeartsLoop;
        }
        gb_write(gb, hl++, 0xAE);
        c++;
        if (c == 7) {
            hl += 3;
        }
        goto drawEmptyHeartsLoop;
    }

drawEmptyHeartsLoop: {
        uint8_t max_hearts = gb_read(gb, hMultiPurpose3);
        if (c == max_hearts) {
            return;
        }
        gb_write(gb, hl++, 0xAE);
        c++;
        if (c == 7) {
            hl += 3;
        }
        goto drawEmptyHeartsLoop;
    }
}

void func_5DC0(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wSaveFilesCount, 0);
    uint8_t b = 1;
    for (uint8_t c = 0; c < 15; c++) {
        uint8_t ch = gb_read(gb, (uint16_t)(wSaveSlotNames + c));
        if (ch != 0) {
            uint8_t count = gb_read(gb, wSaveFilesCount);
            gb_write(gb, wSaveFilesCount, (uint8_t)(count | b));
        }
        if (c + 1 == 5) {
            b = 2;
        } else if (c + 1 == 10) {
            b = 4;
        }
    }
}

void SaveGameToFile(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, wHealth) == 0) {
        uint8_t max_hearts = gb_read(gb, wMaxHearts);
        if (max_hearts < 15) {
            gb_write(gb, wHealth, MaxHeartsToStartingHealthTable[max_hearts]);
        }
    }

    SynchronizeDungeonsItemFlags_trampoline(gb, SynchronizeDungeonsItemFlags);

    uint8_t slot = gb_read(gb, wSaveSlot);
    if (slot >= 3) {
        slot = 0;
    }
    uint16_t sram_dest = SaveGameTable[slot];

    /* Save Main */
    for (uint16_t i = 0; i < SAVE_MAIN_SIZE; i++) {
        EnableSRAM(gb);
        uint8_t val = gb_read(gb, (uint16_t)(wOverworldRoomStatus + i));
        EnableSRAM(gb);
        gb_write(gb, (uint16_t)(sram_dest + i), val);
    }
    sram_dest += SAVE_MAIN_SIZE;

    /* Save DX1 */
    for (uint16_t i = 0; i < SAVE_DX1_SIZE; i++) {
        EnableSRAM(gb);
        uint8_t val = gb_read(gb, (uint16_t)(wColorDungeonItemFlags + i));
        EnableSRAM(gb);
        gb_write(gb, (uint16_t)(sram_dest + i), val);
    }
    sram_dest += SAVE_DX1_SIZE;

    /* Save DX2 */
    for (uint16_t i = 0; i < SAVE_DX2_SIZE; i++) {
        EnableSRAM(gb);
        uint8_t val = gb_read(gb, (uint16_t)(wColorDungeonRoomStatus + i));
        EnableSRAM(gb);
        gb_write(gb, (uint16_t)(sram_dest + i), val);
    }
    sram_dest += SAVE_DX2_SIZE;

    /* Save DX3 */
    EnableSRAM(gb);
    uint8_t tunic = gb_read(gb, wTunicType);
    EnableSRAM(gb);
    gb_write(gb, sram_dest++, tunic);

    EnableSRAM(gb);
    uint8_t p1 = gb_read(gb, wPhotos1);
    EnableSRAM(gb);
    gb_write(gb, sram_dest++, p1);

    EnableSRAM(gb);
    uint8_t p2 = gb_read(gb, wPhotos2);
    EnableSRAM(gb);
    gb_write(gb, sram_dest++, p2);
}

void LoadSavedFile(GBState *gb) {
    if (!gb) return;

    gb_write(gb, hIsSideScrolling, 0);

    if (gb_read(gb, wHealth) == 0) {
        uint8_t max_hearts = gb_read(gb, wMaxHearts);
        if (max_hearts < 15) {
            gb_write(gb, wHealth, MaxHeartsToStartingHealthTable[max_hearts]);
        }
    }

    uint8_t dbd1 = gb_read(gb, wDBD1);
    gb_write(gb, wDBD1, 0);
    if (dbd1 == 0) {
        uint8_t slot = gb_read(gb, wSaveSlot);
        if (slot >= 3) {
            slot = 0;
        }
        uint16_t sram_src = SaveGameTable[slot];

        /* Load Main */
        for (uint16_t i = 0; i < SAVE_MAIN_SIZE; i++) {
            EnableSRAM(gb);
            uint8_t val = gb_read(gb, (uint16_t)(sram_src + i));
            gb_write(gb, (uint16_t)(wOverworldRoomStatus + i), val);
        }
        sram_src += SAVE_MAIN_SIZE;

        /* Load DX1 */
        for (uint16_t i = 0; i < SAVE_DX1_SIZE; i++) {
            EnableSRAM(gb);
            uint8_t val = gb_read(gb, (uint16_t)(sram_src + i));
            gb_write(gb, (uint16_t)(wColorDungeonItemFlags + i), val);
        }
        sram_src += SAVE_DX1_SIZE;

        /* Load DX2 */
        for (uint16_t i = 0; i < SAVE_DX2_SIZE; i++) {
            EnableSRAM(gb);
            uint8_t val = gb_read(gb, (uint16_t)(sram_src + i));
            gb_write(gb, (uint16_t)(wColorDungeonRoomStatus + i), val);
        }
        sram_src += SAVE_DX2_SIZE;

        /* Load DX3 */
        EnableSRAM(gb);
        gb_write(gb, wTunicType, gb_read(gb, sram_src++));
        EnableSRAM(gb);
        gb_write(gb, wPhotos1, gb_read(gb, sram_src++));
        EnableSRAM(gb);
        gb_write(gb, wPhotos2, gb_read(gb, sram_src++));
    }

    /* jr_001_531D */
    gb_write(gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(gb, wGameplaySubtype, 0);
    gb_write(gb, wLinkMotionState, 0);
    gb_write(gb, hLinkPhysicsModifier, 0);
    gb_write(gb, wAddHealthBuffer, 0);
    gb_write(gb, wSubtractHealthBuffer, 0);
    gb_write(gb, wAddRupeeBufferLow, 0);
    gb_write(gb, wAddRupeeBufferHigh, 0);
    gb_write(gb, wSubstractRupeeBufferLow, 0);
    gb_write(gb, wSubstractRupeeBufferHigh, 0);

    if (gb_read(gb, wWreckingBallRoom) == 0) {
        gb_write(gb, wWreckingBallRoom, 0x16);
        gb_write(gb, wWreckingBallPosX, 0x50);
        gb_write(gb, wWreckingBallPosY, 0x27);
    }

    uint8_t spawn_x = gb_read(gb, wSpawnPositionX);
    if (spawn_x == 0) {
        /* initNewGame */
        gb_write(gb, wMaxArrows, 0x30);
        gb_write(gb, wMaxBombs, 0x30);
        gb_write(gb, wMaxMagicPowder, 0x20);
        gb_write(gb, wMapEntranceRoom, ROOM_INDOOR_B_MARIN_HOUSE);
        gb_write(gb, hMapRoom, ROOM_INDOOR_B_MARIN_HOUSE);
        gb_write(gb, wDB54, ROOM_INDOOR_B_MARIN_HOUSE);
        gb_write(gb, wIsIndoor, 1);
        gb_write(gb, hMapId, MAP_HOUSE);
        gb_write(gb, wMapEntrancePositionX, 0x50);
        gb_write(gb, wMapEntrancePositionY, 0x60);
        gb_write(gb, hLinkAnimationState, 0);
        gb_write(gb, hLinkDirection, DIRECTION_DOWN);
        gb_write(gb, wWreckingBallRoom, 0x16);
        gb_write(gb, wWreckingBallPosX, 0x50);
        gb_write(gb, wWreckingBallPosY, 0x27);
    } else {
        gb_write(gb, wMapEntrancePositionX, spawn_x);
        gb_write(gb, wMapEntrancePositionY, gb_read(gb, wSpawnPositionY));
        uint8_t map_room = gb_read(gb, wSpawnMapRoom);
        gb_write(gb, hMapRoom, map_room);
        gb_write(gb, wMapEntranceRoom, map_room);
        gb_write(gb, hMapId, gb_read(gb, wSpawnMapId));
        gb_write(gb, wIndoorRoom, gb_read(gb, wSpawnIndoorRoom));
        gb_write(gb, hIsSideScrolling, 0);
        gb_write(gb, hLinkDirection, DIRECTION_DOWN);

        uint8_t is_indoor = gb_read(gb, wSpawnIsIndoor) & 1;
        gb_write(gb, wIsIndoor, is_indoor);
        if (is_indoor != 0) {
            gb_write(gb, hLinkAnimationState, LINK_ANIMATION_STATE_STANDING_UP);
            gb_write(gb, hLinkDirection, DIRECTION_UP);
        }
    }

    gb_write(gb, wBGMapToLoad, TILEMAP_INVENTORY);
}
