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
