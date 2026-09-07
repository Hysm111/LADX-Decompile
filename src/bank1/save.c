#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/link.h"
#include "constants/maps.h"
#include "constants/gfx.h"
#include "bank1/room_transition.h"
#include "bank1/save.h"
#include "home/gameplay.h"
#include "constants/memory.h"
#include "constants/joypad.h"
#include "constants/audio.h"
#include "constants/sfx.h"
#include "home/entities.h"
#include "home/link.h"
#include "home/animated_tiles.h"
#include "home/clear_memory.h"
#include "home/audio.h"
#include "bank1/world_map.h"
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

#include "constants/gfx.h"
#include "constants/audio.h"
#include "constants/sfx.h"
#include "constants/gameplay.h"
#include "bank1/room_transition.h"
#include "bank1/world_map.h"
#include "home/link.h"
#include "home/clear_memory.h"
#include "home/audio.h"
#include "home/animated_tiles.h"
#include "home/gameplay.h"

const uint8_t DebugSaveFileData[67] = {
    0x02, /* B: shield */
    0x01, /* A: sword */
    0x03, /* bombs */
    0x04, /* power bracelet */
    0x05, /* bow */
    0x06, /* hookshot */
    0x07, /* magic rod */
    0x08, /* pegasus boots */
    0x09, /* ocarina */
    0x0A, /* rocs feather */
    0x0B, /* shovel */
    0x0C, /* magic powder */
    0x01, /* flippers */
    0x01, /* medicine */
    0x01, /* trading item */
    0x00, /* seashells */
    0x00,
    0x01, /* tail key */
    0x01, /* angler key */
    0x01, /* face key */
    0x01, /* bird key */
    0x00, /* leaves */
    /* Dungeon flags (map, compass, stone beak, nightmare key, small keys) */
    1, 1, 1, 1, 1, /* Tail Cave */
    1, 1, 1, 1, 2, /* Bottle Grotto */
    1, 1, 1, 1, 3, /* Key Cavern */
    1, 1, 1, 1, 4, /* Angler's Tunnel */
    1, 1, 1, 1, 5, /* Catfish's Maw */
    1, 1, 1, 1, 6, /* Face Shrine */
    1, 1, 1, 1, 7, /* Eagle's Tower */
    1, 1, 1, 1, 8, /* Turtle Rock */
    1, 1, 1, 1, 9  /* Color Dungeon */
};

const uint8_t Data_001_4128[2] = {
    0x38 + 0x10, /* 0x48 */
    0x48 + 0x10  /* 0x58 */
};

void func_001_4794(GBState *gb, uint16_t de) {
    if (!gb) return;
    uint16_t prefix_addr = (uint16_t)(0xA100 + de);
    uint8_t c = 1;
    bool valid = true;

    for (uint8_t b = 0; b < SAVE_PREFIX_SIZE; b++) {
        EnableSRAM(gb);
        if (gb_read(gb, (uint16_t)(prefix_addr + b)) != c) {
            valid = false;
            break;
        }
        c += 2;
    }

    if (valid) {
        return;
    }

    /* Wipe corrupted save data */
    uint16_t main_addr = (uint16_t)(0xA105 + de);
    EnableSRAM(gb);
    for (uint16_t i = 0; i < 0x03A8; i++) {
        gb_write(gb, (uint16_t)(main_addr + i), 0x00);
    }

    /* Set valid prefix (1, 3, 5, 7, 9) */
    uint8_t val = 1;
    for (uint8_t i = 0; i < SAVE_PREFIX_SIZE; i++) {
        EnableSRAM(gb);
        gb_write(gb, (uint16_t)(prefix_addr + i), val);
        val += 2;
    }
}

void InitSaveFiles(GBState *gb) {
    if (!gb) return;

    func_001_4794(gb, 0);
    func_001_4794(gb, 0x03AD);
    func_001_4794(gb, 0x075A);

    if (gb_read(gb, 0x0003) == 0) {
        return;
    }

    /* Debug save file creation */
    EnableSRAM(gb);
    for (uint8_t i = 0; i < 67; i++) {
        gb_write(gb, (uint16_t)(0xA405 + i), DebugSaveFileData[i]);
    }
    gb_write(gb, 0xA453, 0x01);
    gb_write(gb, 0xA449, 0x01); /* Shield level 1 */
    gb_write(gb, 0xA448, 0x02); /* Bracelet level 2 */
    for (uint8_t i = 0; i < 9; i++) {
        gb_write(gb, (uint16_t)(0xA46A + i), 0x02); /* Boss flags */
    }
    gb_write(gb, 0xA452, 0x60); /* 60 bombs */
    gb_write(gb, 0xA47D, 0x60); /* 60 max arrows */
    gb_write(gb, 0xA47C, 0x60); /* 60 max bombs */
    gb_write(gb, 0xA44A, 0x60); /* 60 arrows */
    gb_write(gb, 0xA47B, 0x40); /* 40 max magic powder */
    gb_write(gb, 0xA451, 0x40); /* 40 magic powder */
    gb_write(gb, 0xA44C, 0x89);
    gb_write(gb, 0xA414, 0x00); /* 0 seashells */
    gb_write(gb, 0xA44E, 0x07); /* 3 songs */
    gb_write(gb, 0xA462, 0x05); /* 5xx rupees */
    gb_write(gb, 0xA463, 0x09); /* x09 rupees */
    gb_write(gb, 0xA44D, 0x01); /* Tarin flag */
    gb_write(gb, 0xA45F, 0x50); /* 10 hearts health */
    gb_write(gb, 0xA460, 0x0A); /* 10 heart containers */

    if (gb_read(gb, wGameplayType) != GAMEPLAY_FILE_NEW) {
        static const uint8_t zelda_name[5] = { 0x34, 0x1F, 0x26, 0x1E, 0x1B };
        for (uint8_t i = 0; i < 5; i++) {
            gb_write(gb, (uint16_t)(0xA454 + i), zelda_name[i]);
        }
    }

    gb_write(gb, 0xA45C, 0); /* death count */
    gb_write(gb, 0xA45D, 0);
    gb_write(gb, 0xA45B, 0); /* bowwow */
    gb_write(gb, 0xA464, 0); /* overworld */
    gb_write(gb, 0xA465, 0); /* submap */
    gb_write(gb, 0xA466, 0x92); /* room 0x92 */
    gb_write(gb, 0xA467, 0x48); /* pos Y */
    gb_write(gb, 0xA468, 0x62); /* pos X */

    for (uint16_t i = 0; i < 256; i++) {
        gb_write(gb, (uint16_t)(0xA105 + i), 0x80); /* all rooms visited */
    }

    gb_write(gb, wColorDungeonItemFlags + 0, 1);
    gb_write(gb, wColorDungeonItemFlags + 1, 1);
    gb_write(gb, wColorDungeonItemFlags + 2, 1);
    gb_write(gb, wColorDungeonItemFlags + 3, 1);
    gb_write(gb, wColorDungeonItemFlags + 4, 1);
    gb_write(gb, wPhotos1, 0xFF);
    gb_write(gb, wPhotos2, 0x0F);
}

void FileSaveInitial(GBState *gb) {
    if (!gb) return;
    IncrementGameplaySubtype(gb);

    if (gb_read(gb, hIsGBC)) {
        if (!gb_read(gb, wIsFileSelectionArrowShifted)) {
            gb_write(gb, wIsFileSelectionArrowShifted, 1);
        }
    }
}

void FileSaveMapFadeOut(GBState *gb) {
    if (!gb) return;
    DrawLinkSprite(gb);
    AnimateEntitiesAndRestoreBank01(gb, NULL);
    func_1A22(gb, NULL, NULL);

    if (gb_read(gb, wTransitionSequenceCounter) == 4) {
        gb_write(gb, hVolumeRight, 3);
        gb_write(gb, hVolumeLeft, 0x30);
        IncrementGameplaySubtype(gb);
        gb_write(gb, wScrollXOffset, 0);
        gb_write(gb, wInventoryAppearing, 0);
        gb_write(gb, wOcarinaMenuOpening, 0);
        gb_write(gb, wOcarinaMenuClosing, 0);
        gb_write(gb, wOcarinaMenuOpen, 0);
        gb_write(gb, wTilesetToLoad, TILESET_0F);
    }
}

void FileSaveDelay1(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wTilesetToLoad, TILESET_SAVE_MENU);
    gb_write(gb, wC13F, 0);
    IncrementGameplaySubtype(gb);
}

void FileSaveDelay2(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wBGMapToLoad, TILEMAP_MENU_FILE_SAVE);
    gb_write(gb, wWindowY, 0xFF);
    gb_write(gb, hBaseScrollX, 0);
    gb_write(gb, hBaseScrollY, 0);
    gb_write(gb, wTransitionSequenceCounter, 0);
    gb_write(gb, wC16C, 0);
    gb_write(gb, wPaletteUnknownE, 1);
    IncrementGameplaySubtype(gb);
}

void FileSaveVisible(GBState *gb) {
    if (!gb) return;
    func_1A39(gb, NULL, NULL);
    if (gb_read(gb, wTransitionSequenceCounter) == 4) {
        IncrementGameplaySubtype(gb);
    }
}

void func_001_412A(GBState *gb) {
    if (!gb) return;

    MoveSelect(gb);
    uint8_t joypad = gb_read(gb, hJoypadState);
    if (joypad & (J_UP | J_DOWN)) {
        uint8_t val = (gb_read(gb, wC13F) + 1) & 0x01;
        gb_write(gb, wC13F, val);
    }

    uint8_t opt = gb_read(gb, wC13F) & 0x01;
    gb_write(gb, wOAMBuffer + 0x18, Data_001_4128[opt]);
    gb_write(gb, wOAMBuffer + 0x19, 0x24);
    gb_write(gb, wOAMBuffer + 0x1A, 0xBE);
    gb_write(gb, wOAMBuffer + 0x1B, 0x00);
}

void LCDOn(GBState *gb) {
    if (!gb) return;
    gb_write(gb, rLCDC, 0xC7);
    gb_write(gb, wLCDControl, 0xC7);
    gb_write(gb, rWX, 0x07);
    gb_write(gb, wWindowY, 0x80);
    gb_write(gb, rWY, 0x80);
    gb_write(gb, hVolumeRight, 0x07);
    gb_write(gb, hVolumeLeft, 0x70);
}

void FileSaveInteractive(GBState *gb) {
    if (!gb) return;

    func_001_412A(gb);

    uint8_t joypad = gb_read(gb, hJoypadState);
    if (!(joypad & (J_A | J_B | J_START))) {
        return;
    }

    gb_write(gb, hJingle, JINGLE_VALIDATE);

    if (gb_read(gb, wC13F) == 1) {
        /* Save and Quit */
        label_27F2(gb);
        SaveGameToFile(gb);
        ClearWRAMAndLowerHRAM(gb);
        gb_write(gb, hActiveEntityTilesOffset, 0);
        gb_write(gb, wCurrentBank, 1);
        func_001_6162(gb, NULL);
        LCDOn(gb);
        return;
    }

    /* Return to Game */
    IncrementGameplaySubtype(gb);
    gb_write(gb, wTransitionSequenceCounter, 0);
    gb_write(gb, wC16C, 0);

    if (gb_read(gb, wIsIndoor)) {
        gb_write(gb, wBlockItemUsage, 0);
        gb_write(gb, wC116, 0);
    }
}

void FileSaveEntryPoint(GBState *gb) {
    if (!gb) return;

    uint8_t subtype = gb_read(gb, wGameplaySubtype);
    switch (subtype) {
        case 0: FileSaveInitial(gb); break;
        case 1: FileSaveMapFadeOut(gb); break;
        case 2: FileSaveDelay1(gb); break;
        case 3: FileSaveDelay2(gb); break;
        case 4: FileSaveVisible(gb); break;
        case 5: FileSaveInteractive(gb); break;
        case 6: FileSaveFadeOut(gb); break;
        default: break;
    }
}
