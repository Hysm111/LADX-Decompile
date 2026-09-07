#include "bank1/world_handler.h"
#include "bank1/room_transition.h"
#include "bank1/world_map.h"
#include "constants/gfx.h"
#include "constants/audio.h"
#include "constants/gameplay.h"
#include "constants/link.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "home/audio.h"
#include "home/clear_memory.h"
#include "home/entities.h"
#include "home/gameplay.h"
#include "home/room.h"

/* Minimap layout table (01:4385 - 01:4394) */
const uint8_t MinimapLayoutTable[16] = {
    0x00, /* Tail Cave */
    0x00, /* Bottle Grotto */
    0x00, /* Key Cavern */
    0x00, /* Angler's Tunnel */
    0x00, /* Catfish's Maw */
    0x00, /* Face Shrine */
    0x30, /* Eagle's Tower (four floors) */
    0x00, /* Turtle Rock */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00  /* Color Dungeon */
};

/* Data tables for minimap construction */
const uint8_t Data_001_53D8[16] = {
    0x9D, 0x9D, 0x9D, 0xFF, 0x9D, 0x9D, 0x9D, 0xFF,
    0x9D, 0x9D, 0x9C, 0xFF, 0x9D, 0x9D, 0x9C, 0xFF
};

const uint8_t Data_001_53E8[16] = {
    0x32, 0x32, 0x09, 0xFF, 0x2E, 0x2E, 0x09, 0xFF,
    0x8A, 0x32, 0xE9, 0xFF, 0x8A, 0x2E, 0xE9, 0xFF
};

const uint8_t Data_001_53F8[16] = {
    0xC8, 0xC8, 0x00, 0xFF, 0xC8, 0xC8, 0x00, 0xFF,
    0x48, 0xC8, 0x00, 0xFF, 0x48, 0xC8, 0x00, 0xFF
};

const uint8_t Data_001_5408[16] = {
    0x7F, 0x7F, 0xBA, 0xFF, 0x7F, 0x7F, 0xBA, 0xFF,
    0x7F, 0x7F, 0xBA, 0xFF, 0x7F, 0x7F, 0xBA, 0xFF
};

const uint8_t Data_001_5418[67] = {
    0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF,
    0x9D, 0x9D, 0xFF, 0x00, 0x9D, 0x9D, 0x9D, 0xFF,
    0x9D, 0x9C, 0xFF, 0x00, 0x9D, 0x9C, 0x9C, 0xFF,
    0x9D, 0x9D, 0x9C, 0x9C, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x9D, 0x9D, 0x9C, 0x9C, 0x9C,
    0x9C, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x9D, 0x9D,
    0x9C, 0x9C, 0x9D, 0x9D, 0x9C, 0x9C, 0xFF, 0x00,
    0x00, 0x9D, 0x9D, 0x9C, 0x9C, 0x9D, 0x9D, 0x9C,
    0x9C, 0x9C, 0x9C
};

const uint8_t Data_001_545C[67] = {
    0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF,
    0x0D, 0x12, 0xFF, 0x00, 0x0D, 0x11, 0x12, 0xFF,
    0x92, 0xF2, 0xFF, 0x00, 0x92, 0xF1, 0xF2, 0xFF,
    0x8D, 0x92, 0xED, 0xF2, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x8D, 0x92, 0xED, 0xF2, 0xF1,
    0xF2, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x92,
    0xED, 0xF2, 0x91, 0x92, 0xF1, 0xF2, 0xFF, 0x00,
    0x00, 0x8D, 0x92, 0xED, 0xF2, 0x91, 0x92, 0xEC,
    0xED, 0xF1, 0xF2
};

const uint8_t Data_001_54A0[67] = {
    0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF,
    0xE8, 0xE9, 0xFF, 0x00, 0xE8, 0xEC, 0xE8, 0xFF,
    0xE8, 0xE9, 0xFF, 0x00, 0xE8, 0xEC, 0xE8, 0xFF,
    0xE8, 0xEA, 0xE9, 0xEB, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xE8, 0xEA, 0xE9, 0xEB, 0xEC,
    0xE8, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xE8, 0xEA,
    0xE9, 0xEB, 0xEC, 0xE8, 0xEC, 0xE9, 0xFF, 0x00,
    0x00, 0xE8, 0xEA, 0xE9, 0xEB, 0xEC, 0xE8, 0xEC,
    0xEA, 0xEC, 0xE9
};

const uint8_t Data_001_54E4[2] = { 0x9D, 0x9C };
const uint8_t Data_001_54E6[2] = { 0x0A, 0xEA };

const uint8_t Data_001_54E8[41] = {
    0x9C, 0xE9, 0x49, 0x7F, 0x9D, 0x09, 0x49, 0x7F,
    0x9D, 0x29, 0x49, 0x7F, 0x9D, 0x49, 0x49, 0x7F,
    0x9D, 0x69, 0x49, 0x7F, 0x9D, 0x89, 0x49, 0x7F,
    0x9D, 0xA9, 0x49, 0x7F, 0x9D, 0xC9, 0x49, 0x7F,
    0x9D, 0xE9, 0x49, 0x7F, 0x9E, 0x09, 0x49, 0x7F,
    0x00
};

void WorldHandlerEntryPoint(GBState *gb) {
    if (!gb) return;

    uint8_t subtype = gb_read(gb, wGameplaySubtype);
    switch (subtype) {
        case 0:
            GameplayWorldLoad0Handler(gb);
            break;
        case 1:
            GameplayWorldLoadRoomHandler(gb);
            break;
        case 2:
            GameplayWorldSelectTilesetHandler(gb);
            break;
        case 3:
            GameplayWorldLoadRoomTilemapHandler(gb);
            break;
        case 4:
            GameplayWorldLoadHeartsAndRupeesHandler(gb);
            break;
        case 5:
            GameplayWorldLoadABButtonsHandler(gb);
            break;
        case 6:
            GameplayWorldLoad6Handler(gb);
            break;
        default:
            break;
    }
}

void GameplayWorldLoad0Handler(GBState *gb) {
    if (!gb) return;

    func_27F2(gb, NULL);
    IncrementGameplaySubtype(gb);

    if (gb_read(gb, ROM_DebugTool2) != 0) {
        gb_write(gb, wBGMapToLoad, TILEMAP_INVENTORY_DEBUG);
        return;
    }

    if (gb_read(gb, wIsIndoor) == 0) {
        /* Overworld inventory */
        gb_write(gb, wBGMapToLoad, TILEMAP_INVENTORY);
        uint8_t rnd = GetRandomByte(gb);
        uint8_t seq = (rnd | gb_read(gb, hFrameCounter)) & 0x03;
        gb_write(gb, hTileGlintSequence, seq);
        return;
    }

    /* Indoors */
    uint8_t map = gb_read(gb, hMapId);
    uint16_t src_addr;
    if (map == MAP_COLOR_DUNGEON) {
        src_addr = wColorDungeonItemFlags;
    } else {
        src_addr = (uint16_t)(wDungeonItemFlags + (uint16_t)(map * 5));
    }

    uint16_t dst_addr = wHasDungeonMap;
    for (uint8_t c = 0; c < 5; c++) {
        uint8_t val = 0;
        if (map == MAP_COLOR_DUNGEON || (map < MAP_CAVE_B && map != MAP_WINDFISHS_EGG)) {
            val = gb_read(gb, src_addr++);
        }
        gb_write(gb, dst_addr++, val);
    }

    uint8_t layout_idx = map;
    if (layout_idx == MAP_COLOR_DUNGEON) {
        layout_idx = 0x0F;
    }
    if (layout_idx < sizeof(MinimapLayoutTable)) {
        gb_write(gb, wMinimapLayout, MinimapLayoutTable[layout_idx]);
    }

    if (map == MAP_COLOR_DUNGEON) {
        CreateMinimapTilemap(gb);
        gb_write(gb, wBGMapToLoad, TILEMAP_MINIMAP);
        return;
    }

    if (map == MAP_WINDFISHS_EGG || map >= MAP_CAVE_B) {
        gb_write(gb, wBGMapToLoad, TILEMAP_EAGLES_TOWER_CLOUDS);
        return;
    }

    if (map == MAP_EAGLES_TOWER && gb_read(gb, hIsSideScrolling) != 0) {
        gb_write(gb, wBGMapToLoad, TILEMAP_EAGLES_TOWER_CLOUDS);
        return;
    }

    CreateMinimapTilemap(gb);
    gb_write(gb, wBGMapToLoad, TILEMAP_MINIMAP);
}

void GameplayWorldLoadRoomHandler(GBState *gb) {
    GameplayWorldLoadRoomHandlerWithHooks(gb, NULL);
}

void GameplayWorldLoadRoomHandlerWithHooks(GBState *gb, uint8_t (*get_chests_status)(GBState *gb, uint8_t d, uint8_t e)) {
    if (!gb) return;

    ClearLowerWRAM(gb);
    gb_write(gb, wLinkMotionState, 0);
    IncrementGameplaySubtype(gb);

    uint8_t entry_x = gb_read(gb, wMapEntrancePositionX);
    gb_write(gb, hLinkPositionX, entry_x);
    gb_write(gb, wLinkMapEntryPositionX, entry_x);

    uint8_t entry_y = gb_read(gb, wMapEntrancePositionY);
    gb_write(gb, hLinkPositionY, entry_y);
    gb_write(gb, wLinkMapEntryPositionY, entry_y);

    uint8_t entry_z = gb_read(gb, wMapEntrancePositionZ);
    gb_write(gb, hLinkPositionZ, entry_z);
    if (entry_z != 0) {
        gb_write(gb, wIsLinkInTheAir, 2);
    }

    gb_write(gb, wRoomTransitionDirection, 4);
    LoadRoom(gb, NULL);
    LoadRoomEntities(gb, NULL, NULL, NULL, NULL);
    CreateFollowingNpcEntity(gb, NULL);
    gb_write(gb, hAnimatedTilesFrameCount, 0xFF);

    if (gb_read(gb, wIsIndoor) != 0) {
        uint8_t d = gb_read(gb, wIsIndoor);
        uint8_t map = gb_read(gb, hMapId);
        if (map == MAP_COLOR_DUNGEON) {
            d = 0;
        } else if (map < 0x1A && map >= 0x06) {
            d++;
        }
        uint8_t e = gb_read(gb, hMapRoom);

        uint8_t chest_status = 0;
        if (get_chests_status) {
            chest_status = get_chests_status(gb, d, e);
        } else {
            /* If no hook passed, run trampoline */
            GetChestsStatusForRoom_trampoline(gb, NULL);
        }

        uint8_t event_effect = gb_read(gb, wRoomEvent) & EVENT_EFFECT_MASK;

        if (chest_status == 0x1A || chest_status == 0x19 || event_effect == EFFECT_DROP_KEY) {
            if (gb_read(gb, wHasDungeonCompass) != 0) {
                if (!(gb_read(gb, hRoomStatus) & ROOM_STATUS_EVENT_1)) {
                    gb_write(gb, wCompassSfxCountdown, 0x0C);
                }
            }
        }
    }

    if (gb_read(gb, wIsIndoor) != 0) {
        gb_write(gb, wTilesetToLoad, 0x06);
    } else {
        gb_write(gb, wTilesetToLoad, 0x07);
    }
}

void GameplayWorldSelectTilesetHandler(GBState *gb) {
    if (!gb) return;

    gb_write(gb, hWorldTileset, 0x0F);
    if (gb_read(gb, hIsGBC) != 0) {
        gb_write(gb, rSVBK, 3);
        gb_write(gb, wIsFileSelectionArrowShifted, 0);
        gb_write(gb, rSVBK, 0);
    }

    SelectRoomTilesets(gb);
    gb_write(gb, hNeedsUpdatingBGTiles, 0);
    gb_write(gb, hNeedsUpdatingEntityTilesA, 0);
    gb_write(gb, wTilesetToLoad, TILESET_ROOM_SPECIFIC);

    IncrementGameplaySubtype(gb);
}

void GameplayWorldLoadRoomTilemapHandler(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wTilesetToLoad, TILESET_ROOM_TILEMAP);

    if (gb_read(gb, wRoomSwitchableObject) != 0) {
        gb_write(gb, wSwitchableObjectAnimationStage, 5);
        if (gb_read(gb, wSwitchButtonPressed) != 0) {
            gb_write(gb, hReplaceTiles, REPLACE_TILES_BUTTON_PRESSED);
        }
    }

    IncrementGameplaySubtype(gb);
}

void GameplayWorldLoadHeartsAndRupeesHandler(GBState *gb) {
    if (!gb) return;

    LoadHeartsAndRupeesCount(gb, NULL, NULL);
    IncrementGameplaySubtype(gb);
}

void GameplayWorldLoadABButtonsHandler(GBState *gb) {
    if (!gb) return;

    DrawABButtonSlots(gb, NULL);
    IncrementGameplaySubtype(gb);
}

void GameplayWorldLoad6Handler(GBState *gb) {
    if (!gb) return;

    InitializeInventoryBar(gb);

    uint8_t lcdc = (uint8_t)(gb_read(gb, rLCDC) | LCDCF_WINON);
    gb_write(gb, wLCDControl, lcdc);
    gb_write(gb, rLCDC, lcdc);

    IncrementGameplaySubtype(gb);

    gb_write(gb, wD463, gb_read(gb, wLinkMotionState));
    gb_write(gb, wLinkMotionState, LINK_MOTION_MAP_FADE_IN);
    gb_write(gb, wTransitionSequenceCounter, 0);
    gb_write(gb, wC16C, 0);

    if (gb_read(gb, wObjectAffectingBGPalette) != 0) {
        gb_write(gb, wBGPalette, gb_read(gb, wC5AD));
        gb_write(gb, wOBJ0Palette, 0x1C);
        gb_write(gb, wOBJ1Palette, 0xE4);
        if (gb_read(gb, hIsGBC) == 0) {
            gb_write(gb, wTransitionSequenceCounter, 4);
        }
    }

    SelectMusicTrackAfterTransition_trampoline(gb, NULL);
}

void func_001_5619(GBState *gb, uint16_t *hl) {
    if (!gb || !hl) return;
    gb_write(gb, (*hl)++, gb_read(gb, hMultiPurpose0));
    gb_write(gb, (*hl)++, gb_read(gb, hMultiPurpose1));
    gb_write(gb, (*hl)++, gb_read(gb, hMultiPurpose2));
    gb_write(gb, *hl, gb_read(gb, hMultiPurpose3));
}

void CreateMinimapTilemap(GBState *gb) {
    if (!gb) return;

    uint16_t de = (uint16_t)(wMinimapTilemap - 1);
    for (uint8_t c = 0; c < 0x29; c++) {
        de++;
        gb_write(gb, de, Data_001_54E8[c]);
    }

    gb_write(gb, hMultiPurpose0, 0);
    gb_write(gb, hMultiPurpose1, 0);
    gb_write(gb, hMultiPurpose2, 0);
    gb_write(gb, hMultiPurpose3, 0);

    uint8_t layout = gb_read(gb, wMinimapLayout);
    uint8_t e = (uint8_t)((layout >> 4) & 0x03);
    uint16_t bc = 0;
    while (e != 0) {
        bc = (uint16_t)(bc + 4);
        e--;
    }

    uint16_t hl = de;

    while (1) {
        if (bc < sizeof(Data_001_53D8)) {
            gb_write(gb, hMultiPurpose0, Data_001_53D8[bc]);
            gb_write(gb, hMultiPurpose1, Data_001_53E8[bc]);
            gb_write(gb, hMultiPurpose2, Data_001_53F8[bc]);
            gb_write(gb, hMultiPurpose3, Data_001_5408[bc]);
        }
        func_001_5619(gb, &hl);
        bc++;
        hl++;
        if (bc >= sizeof(Data_001_53D8) || Data_001_53D8[bc] == 0xFF) {
            break;
        }
    }

    gb_write(gb, hl, 0);

    gb_write(gb, hMultiPurpose0, 0);
    gb_write(gb, hMultiPurpose1, 0);
    gb_write(gb, hMultiPurpose2, 0);
    gb_write(gb, hMultiPurpose3, 0);

    layout = gb_read(gb, wMinimapLayout);
    e = (uint8_t)((layout >> 4) & 0x03);
    bc = 0;
    if (e != 0) {
        while (e != 0) {
            bc = (uint16_t)(bc + 8);
            e--;
        }

        if ((layout & 0x03) != 0) {
            if ((layout & 0x30) == 0x30) {
                e = (uint8_t)(layout & 0x03);
                while (e != 0) {
                    bc = (uint16_t)(bc + 11);
                    e--;
                }
            } else {
                bc = (uint16_t)(bc + 4);
            }
        }

        while (1) {
            if (bc < sizeof(Data_001_5418)) {
                gb_write(gb, hMultiPurpose0, Data_001_5418[bc]);
                gb_write(gb, hMultiPurpose1, Data_001_545C[bc]);
                gb_write(gb, hMultiPurpose2, 0);
                gb_write(gb, hMultiPurpose3, Data_001_54A0[bc]);
            }
            func_001_5619(gb, &hl);
            bc++;
            hl++;
            if (bc >= sizeof(Data_001_5418) || Data_001_5418[bc] == 0xFF) {
                break;
            }
        }
    }

    /* .jr_001_55E */
    bc = 0;
    layout = gb_read(gb, wMinimapLayout);
    if ((layout & 0x20) != 0) {
        bc++;
    }

    if (bc < sizeof(Data_001_54E4)) {
        gb_write(gb, hMultiPurpose0, Data_001_54E4[bc]);
        gb_write(gb, hMultiPurpose1, Data_001_54E6[bc]);
    }
    gb_write(gb, hMultiPurpose2, 0x01);
    gb_write(gb, hMultiPurpose3, (uint8_t)(gb_read(gb, hMapId) + 0xB1));

    func_001_5619(gb, &hl);
    hl++;
    gb_write(gb, hl++, 0x7F);
    gb_write(gb, hl, 0x00);
}
