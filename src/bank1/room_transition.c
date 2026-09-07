#include "bank1/room_transition.h"
#include "home/entities.h"
#include "home/link.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/dialog.h"
#include "constants/joypad.h"
#include "constants/sfx.h"

#define OAM_COUNT 40
#define SIZEOF_OAM_ATTRS 4

/* Direction offsets: right (0), left (1), top (2), bottom (3), default (4) */
static const uint8_t EntityPosXOffsetTable[5] = { 0xA0, 0x60, 0x00, 0x00, 0x00 };
static const uint8_t EntityPosXSignTable[5]   = { 0x00, 0xFF, 0x00, 0x00, 0x00 };
static const uint8_t EntityPosYOffsetTable[5] = { 0x00, 0x00, 0x80, 0x80, 0x00 };
static const uint8_t EntityPosYSignTable[5]   = { 0x00, 0x00, 0xFF, 0x00, 0x00 };

void PrepareEntityPositionForRoomTransition(GBState *gb, uint8_t entity_index) {
    if (!gb) return;

    uint8_t order = gb_read(gb, hMultiPurposeD);
    gb_write(gb, (uint16_t)(wEntitiesLoadOrderTable + entity_index), order);
    gb_write(gb, hMultiPurposeD, (uint8_t)(order + 1));

    uint8_t dir = gb_read(gb, wRoomTransitionDirection);
    if (dir > 4) {
        dir = 4;
    }

    uint8_t off_x  = EntityPosXOffsetTable[dir];
    uint8_t sign_x = EntityPosXSignTable[dir];
    uint8_t off_y  = EntityPosYOffsetTable[dir];
    uint8_t sign_y = EntityPosYSignTable[dir];

    gb_write(gb, hMultiPurpose0, off_x);
    gb_write(gb, hMultiPurpose1, sign_x);
    gb_write(gb, hMultiPurpose2, off_y);
    gb_write(gb, hMultiPurpose3, sign_y);

    /* [wEntitiesPosXTable + de] += [hMultiPurpose0] */
    uint16_t x_sum = (uint16_t)(gb_read(gb, (uint16_t)(wEntitiesPosXTable + entity_index)) + off_x);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + entity_index), (uint8_t)x_sum);
    uint8_t carry_x = (x_sum > 0xFF) ? 1 : 0;

    /* [wEntitiesPosXSignTable + de] += [hMultiPurpose1] + carry */
    uint8_t sign_x_res = (uint8_t)(gb_read(gb, (uint16_t)(wEntitiesPosXSignTable + entity_index)) + sign_x + carry_x);
    gb_write(gb, (uint16_t)(wEntitiesPosXSignTable + entity_index), sign_x_res);

    /* [wEntitiesPosYTable + de] += [hMultiPurpose2] */
    uint16_t y_sum = (uint16_t)(gb_read(gb, (uint16_t)(wEntitiesPosYTable + entity_index)) + off_y);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + entity_index), (uint8_t)y_sum);
    uint8_t carry_y = (y_sum > 0xFF) ? 1 : 0;

    /* [wEntitiesPosYSignTable + de] += [hMultiPurpose3] + carry */
    uint8_t sign_y_res = (uint8_t)(gb_read(gb, (uint16_t)(wEntitiesPosYSignTable + entity_index)) + sign_y + carry_y);
    gb_write(gb, (uint16_t)(wEntitiesPosYSignTable + entity_index), sign_y_res);
}

void UpdateRecentRoomsList(GBState *gb) {
    if (!gb) return;

    uint8_t map_room = gb_read(gb, hMapRoom);

    for (uint8_t i = 0; i < 6; i++) {
        if (gb_read(gb, (uint16_t)(wRecentRooms + i)) == map_room) {
            return;
        }
    }

    uint8_t idx = gb_read(gb, wRecentRoomsIndex);
    idx++;
    if (idx >= 6) {
        idx = 0;
    }
    gb_write(gb, wRecentRoomsIndex, idx);

    uint8_t evicted_room = gb_read(gb, (uint16_t)(wRecentRooms + idx));
    gb_write(gb, (uint16_t)(wRecentRooms + idx), map_room);

    gb_write(gb, (uint16_t)(wEntitiesClearedRooms + evicted_room), 0);
}

void HideAllSprites(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, hIsGBC) != 0) {
        gb_write(gb, rRAMG, CART_SRAM_DISABLE);
    } else {
        gb_write(gb, rRAMG, 0xFF);
    }

    for (uint8_t i = 0; i < OAM_COUNT; i++) {
        gb_write(gb, (uint16_t)(wOAMBuffer + i * SIZEOF_OAM_ATTRS), 0xF4);
    }
}

void HideSpritesUnderDialog(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, wWindowY) == 0) return;
    uint8_t dialog_state = gb_read(gb, wDialogState);
    if (dialog_state == DIALOG_CLOSED) return;

    bool is_bottom = (dialog_state & DIALOG_BOX_BOTTOM_FLAG) != 0;
    uint8_t d;
    if (is_bottom) {
        d = 0x58; /* DIALOG_BOX_BOTTOM_Y - (OAM_Y_OFS / 2) */
    } else {
        d = 0x3E; /* DIALOG_BOX_TOP_Y + DIALOG_BOX_HEIGHT + OAM_Y_OFS - 2 */
    }

    for (uint8_t i = 9; i < OAM_COUNT; i++) {
        uint16_t entry = (uint16_t)(wOAMBuffer + i * SIZEOF_OAM_ATTRS);
        uint8_t y = gb_read(gb, entry);

        bool hide;
        if (is_bottom) {
            hide = (y >= d);
        } else {
            hide = (y < d);
        }

        if (hide) {
            uint8_t dlg_low = gb_read(gb, wDialogIndex);
            uint8_t dlg_hi = gb_read(gb, wDialogIndexHi);
            if (dlg_low == Dialog04F && dlg_hi == 0) {
                uint8_t tile = gb_read(gb, (uint16_t)(entry + 2));
                if (tile >= 0x9A && tile < 0xA0) {
                    continue;
                }
            }
            gb_write(gb, entry, 0);
        }
    }
}

void HideSprites(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, wInventoryAppearing) == 0) {
        HideSpritesUnderDialog(gb);
        return;
    }

    uint8_t d = (uint8_t)(gb_read(gb, wWindowY) + 8);
    for (uint8_t i = 0; i < OAM_COUNT; i++) {
        uint16_t entry = (uint16_t)(wOAMBuffer + i * SIZEOF_OAM_ATTRS);
        uint8_t y = gb_read(gb, entry);
        if (y >= d) {
            gb_write(gb, entry, 0);
        }
    }
}

#include "home/audio.h"
#include "home/copy_data.h"
#include "home/dialog.h"
#include "home/entities.h"
#include "constants/maps.h"
#include "constants/rooms.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/sfx.h"
#include "constants/link.h"

static const uint16_t Data_001_618A[16] = {
    0x5700, 0x5710, 0x5720, 0x5730, 0x5740, 0x5750, 0x5760, 0x5770,
    0x5780, 0x5790, 0x5800, 0x5810, 0x5820, 0x5830, 0x5840, 0x5850
};

void SynchronizeDungeonsItemFlags(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wIsIndoor) == 0) return;

    uint8_t map_id = gb_read(gb, hMapId);
    uint16_t dest;
    if (map_id == MAP_COLOR_DUNGEON) {
        dest = wColorDungeonItemFlags;
    } else {
        if (map_id >= MAP_CAVE_B) return;
        dest = (uint16_t)(wDungeonItemFlags + map_id * 5);
    }

    for (uint8_t i = 0; i < 5; i++) {
        gb_write(gb, (uint16_t)(dest + i), gb_read(gb, (uint16_t)(wCurrentDungeonItemFlags + i)));
    }
}

void CreateFollowingNpcEntity(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t entity_type)) {
    if (!gb) return;

    if (gb_read(gb, wIsIndoor) != 0) {
        if (gb_read(gb, hIsSideScrolling) != 0) return;
        uint8_t map_id = gb_read(gb, hMapId);
        if (map_id == MAP_S_FACE_SHRINE ||
            map_id == MAP_KANALET ||
            map_id == MAP_DREAM_SHRINE ||
            map_id < MAP_CAVE_B) {
            return;
        }
        uint8_t map_room = gb_read(gb, hMapRoom);
        if (map_room == ROOM_INDOOR_B_MANBO ||
            map_room == ROOM_INDOOR_B_FISHING_MINIGAME) {
            return;
        }
    }

    /* Rooster entity */
    if (gb_read(gb, wIsRoosterFollowingLink) == 1) {
        for (int i = 0x0F; i >= 0; i--) {
            if (gb_read(gb, (uint16_t)(wEntitiesTypeTable + i)) == ENTITY_ROOSTER) {
                if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + i)) != 0) {
                    gb_write(gb, (uint16_t)(wEntitiesStatusTable + i), 0);
                }
            }
        }
        uint16_t slot = SpawnNewEntity_trampoline(gb, ENTITY_ROOSTER, spawn_new_entity);
        gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), gb_read(gb, hLinkPositionX));
        gb_write(gb, (uint16_t)(wEntitiesPosZTable + slot), gb_read(gb, hLinkPositionZ));
        uint8_t y = (uint8_t)(gb_read(gb, hLinkPositionY) + gb_read(gb, wC13B));
        gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), y);
    }

    /* Ghost entity */
    uint8_t ghost_following = gb_read(gb, wIsGhostFollowingLink);
    if (ghost_following == 1) {
        for (int i = 0x0F; i >= 0; i--) {
            if (gb_read(gb, (uint16_t)(wEntitiesTypeTable + i)) == ENTITY_GHOST) {
                if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + i)) != 0) {
                    gb_write(gb, (uint16_t)(wEntitiesStatusTable + i), 0);
                }
            }
        }
        uint16_t slot = SpawnNewEntity_trampoline(gb, ENTITY_GHOST, spawn_new_entity);
        gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), gb_read(gb, hLinkPositionX));
        uint8_t y = (uint8_t)(gb_read(gb, hLinkPositionY) + gb_read(gb, wC13B));
        gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), y);
        uint8_t st = gb_read(gb, (uint16_t)(wEntitiesPrivateState1Table + slot));
        gb_write(gb, (uint16_t)(wEntitiesPrivateState1Table + slot), (uint8_t)(st + 1));
        gb_write(gb, hJingle, JINGLE_GHOST_PRESENCE);
    } else if (ghost_following == 2) {
        if (gb_read(gb, wIsIndoor) == 0 &&
            gb_read(gb, hMapRoom) >= ROOM_SECTION_OW_GHOST_TRIGGER &&
            (gb_read(gb, wHasInstrument4) & 0x02) != 0) {
            if (gb_read(gb, wPowerBraceletLevel) < 2) {
                gb_write(gb, wIsGhostFollowingLink, 1);
            } else {
                gb_write(gb, wIsGhostFollowingLink, 0);
            }
        }
    }

    /* Marin entity */
    if (gb_read(gb, wIsMarinFollowingLink) != 0) {
        for (int i = 0x0F; i >= 0; i--) {
            if (gb_read(gb, (uint16_t)(wEntitiesTypeTable + i)) == ENTITY_MARIN_AT_THE_SHORE) {
                if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + i)) != 0) {
                    gb_write(gb, (uint16_t)(wEntitiesStatusTable + i), 0);
                }
            }
        }
        uint16_t slot = SpawnNewEntity_trampoline(gb, ENTITY_MARIN_AT_THE_SHORE, spawn_new_entity);
        uint8_t lx = gb_read(gb, hLinkPositionX);
        gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), lx);
        for (uint8_t i = 0; i < 0x10; i++) {
            gb_write(gb, (uint16_t)(wLinkPositionXHistory + i), lx);
        }

        uint8_t ly = (uint8_t)(gb_read(gb, hLinkPositionY) + gb_read(gb, wC13B));
        gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), ly);
        for (uint8_t i = 0; i < 0x10; i++) {
            gb_write(gb, (uint16_t)(wLinkPositionYHistory + i), ly);
        }

        uint8_t lz = gb_read(gb, hLinkPositionZ);
        gb_write(gb, (uint16_t)(wEntitiesPosZTable + slot), lz);
        for (uint8_t i = 0; i < 0x10; i++) {
            gb_write(gb, (uint16_t)(wLinkPositionZHistory + i), lz);
        }

        gb_write(gb, (uint16_t)(wEntitiesPrivateState4Table + slot), 0x01);
        gb_write(gb, (uint16_t)(wEntitiesPrivateCountdown1Table + slot), 0x0C);

        if (gb_read(gb, hMapRoom) == UNKNOWN_ROOM_A4 && gb_read(gb, hMapId) == MAP_CAVE_C) {
            gb_write(gb, hJingle, JINGLE_FALL_DOWN);
            gb_write(gb, wC167, JINGLE_FALL_DOWN);
            gb_write(gb, (uint16_t)(wEntitiesPrivateCountdown2Table + slot), 0x79);
        }

        uint8_t ldir = gb_read(gb, hLinkDirection);
        for (uint8_t i = 0; i < 0x10; i++) {
            gb_write(gb, (uint16_t)(wLinkDirectionHistory + i), ldir);
        }

        if (gb_read(gb, wDB10) != 0) {
            uint8_t px = (uint8_t)(gb_read(gb, (uint16_t)(wEntitiesPosXTable + slot)) + 0x20);
            gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), px);
            uint8_t py = (uint8_t)(gb_read(gb, (uint16_t)(wEntitiesPosYTable + slot)) + 0x10);
            gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), py);
        }
        return;
    }

    /* Bow-Wow entity */
    if (gb_read(gb, hMapRoom) == ROOM_INDOOR_B_MRS_MEOW_MEOW) return;

    if (gb_read(gb, wIsBowWowFollowingLink) == BOW_WOW_FOLLOWING) {
        for (int i = 0x0F; i >= 0; i--) {
            if (gb_read(gb, (uint16_t)(wEntitiesTypeTable + i)) == ENTITY_BOW_WOW) {
                if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + i)) != 0) {
                    gb_write(gb, (uint16_t)(wEntitiesStatusTable + i), 0);
                }
            }
        }
        uint16_t slot = SpawnNewEntity_trampoline(gb, ENTITY_BOW_WOW, spawn_new_entity);
        gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), gb_read(gb, hLinkPositionX));
        gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), gb_read(gb, hLinkPositionY));
        gb_write(gb, (uint16_t)(wEntitiesPosZTable + slot), gb_read(gb, hLinkPositionZ));
    }
}

void func_001_6162(GBState *gb, void (*func_01F_4003)(GBState *)) {
    if (!gb) return;

    func_27F2(gb, func_01F_4003);
    gb_write(gb, wGameplayType, 0);
    gb_write(gb, wGameplaySubtype, 0);
    gb_write(gb, wOBJ0Palette, 0);
    gb_write(gb, wOBJ1Palette, 0);
    gb_write(gb, wBGPalette, 0);
    gb_write(gb, rBGP, 0);
    gb_write(gb, rOBP0, 0);
    gb_write(gb, rOBP1, 0);
    gb_write(gb, hBaseScrollY, 0);
    gb_write(gb, hBaseScrollX, 0);
    gb_write(gb, wSwitchBlocksState, 0);
    gb_write(gb, wSwitchableObjectAnimationStage, 0);
    gb_write(gb, hButtonsInactiveDelay, 0x18);
}

void LoadCounterAnimatedTiles(GBState *gb) {
    if (!gb) return;

    uint8_t dialog_id = gb_read(gb, wTextDebuggerDialogId);
    uint8_t low_nibble = dialog_id & 0x0F;
    uint16_t src1 = Data_001_618A[low_nibble];
    CopyDataFromBank(gb, 0x0F, 0x96D0, src1, 0x10);

    uint8_t high_nibble = (uint8_t)((dialog_id >> 4) & 0x0F);
    uint16_t src2 = Data_001_618A[high_nibble];
    CopyDataFromBank(gb, 0x0F, 0x96C0, src2, 0x10);

    gb_write(gb, 0x9909, 0x6C);
    gb_write(gb, 0x990A, 0x6D);
}

void OpenDungeonNameDialog(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wLinkMotionState) != LINK_MOTION_DEFAULT) return;
    if (gb_read(gb, wFreeMovementMode) != 0) return;

    uint8_t dialog_id = (uint8_t)(gb_read(gb, hMapId) + 0x56);
    OpenDialogInTable0(gb, dialog_id);
}

static const uint8_t DMARoutineBytes[10] = {
    0x3E, 0xC0, 0xE0, 0x46, 0x3E, 0x28, 0x3D, 0x20, 0xFD, 0xC9
};

static const uint16_t MinimapEntrancePosition[16] = {
    vBGMap1 + 0x20B + MINIMAP_ARROW_TAIL_CAVE,
    vBGMap1 + 0x20B + MINIMAP_ARROW_BOTTLE_GROTTO,
    vBGMap1 + 0x20B + MINIMAP_ARROW_KEY_CAVERN,
    vBGMap1 + 0x20B + MINIMAP_ARROW_ANGLERS_TUNNEL,
    vBGMap1 + 0x20B + MINIMAP_ARROW_CATFISHS_MAW,
    vBGMap1 + 0x20B + MINIMAP_ARROW_FACE_SHRINE,
    vBGMap1 + 0x20B + MINIMAP_ARROW_EAGLES_TOWER,
    vBGMap1 + 0x20B + MINIMAP_ARROW_TURTLE_ROCK,
    0, 0, 0, 0, 0, 0, 0,
    vBGMap1 + 0x20B + MINIMAP_ARROW_COLOR_DUNGEON
};

void func_001_6D11(GBState *gb) {
    if (!gb) return;
    uint8_t d = (gb_read(gb, wGameplayType) == GAMEPLAY_WORLD) ? 0x05 : 0x06;
    gb_write(gb, rVBK, 1);
    for (uint16_t i = 0; i < 0x400; i++) {
        gb_write(gb, (uint16_t)(vBGMap0 + i), d);
    }
    gb_write(gb, rVBK, 0);
}

void LoadTileset0F(GBState *gb) {
    if (!gb) return;
    for (uint16_t i = 0; i < 0x400; i++) {
        uint16_t addr = (uint16_t)(vBGMap0 + i);
        uint8_t l = (uint8_t)(addr & 0xFF);
        uint8_t e = (l & 0x20) ? 1 : 0;
        uint8_t d = 0xAE;
        if ((l & 0x01) ^ e) {
            d++;
        }
        if ((l & 0x1F) < 0x14) {
            gb_write(gb, addr, d);
        }
    }
    if (gb_read(gb, hIsGBC) != 0) {
        func_001_6D11(gb);
    }
}

void WriteDMACodeToHRAM(GBState *gb) {
    if (!gb) return;
    for (uint8_t i = 0; i < 10; i++) {
        gb_write(gb, (uint16_t)(hDMARoutine + i), DMARoutineBytes[i]);
    }
}

void UpdateMinimapEntranceArrowAndReturn(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, ROM_DebugTool2) != 0) return;
    if (gb_read(gb, wIsIndoor) == 0) return;

    uint8_t map_id = gb_read(gb, hMapId);
    if (map_id == MAP_COLOR_DUNGEON) {
        map_id = 0x0F;
    } else {
        if (map_id >= 0x08) return;
    }

    uint16_t target_addr = MinimapEntrancePosition[map_id];
    gb_write(gb, target_addr, 0xA3);

    if (gb_read(gb, hIsSideScrolling) != 0) {
        gb_write(gb, target_addr, 0x7F);
    }
}

void IncrementGameplaySubtype(GBState *gb) {
    if (!gb) return;
    uint8_t val = gb_read(gb, wGameplaySubtype);
    gb_write(gb, wGameplaySubtype, (uint8_t)(val + 1));
}

void IncrementGameplaySubtypeAndReturn(GBState *gb) {
    IncrementGameplaySubtype(gb);
}

void func_001_5888(GBState *gb) {
    if (!gb) return;
    for (uint8_t i = 0; i < 0x0C; i++) {
        gb_write(gb, (uint16_t)(wRoomTransitionState + i), 0x00);
    }
}

void InitializeInventoryBar(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wWindowY, 0x80);
    gb_write(gb, rWX, 0x07);
    gb_write(gb, wSubscreenScrollIncrement, 0x08);
    gb_write(gb, wInventoryAppearing, 0x00);
}

void func_001_58A8(GBState *gb) {
    if (!gb) return;
    uint16_t hl = (uint16_t)(wDynamicOAMBuffer + 0x6C);

    uint8_t val = gb_read(gb, wDB54);
    uint8_t y = (uint8_t)(((val >> 1) & 0x78) + 0x18);
    gb_write(gb, hl++, y);

    val = gb_read(gb, wDB54);
    uint8_t swapped = (uint8_t)((val << 4) | (val >> 4));
    uint8_t x = (uint8_t)(((swapped >> 1) & 0x78) + 0x18);
    gb_write(gb, hl++, x);

    gb_write(gb, hl++, 0x3E);

    if (gb_read(gb, hIsGBC) != 0) {
        if ((gb_read(gb, hFrameCounter) & 0x08) != 0) {
            gb_write(gb, hl, 0x03);
        } else {
            gb_write(gb, hl, 0x00);
        }
    } else {
        uint8_t frame = gb_read(gb, hFrameCounter);
        uint8_t attr = (uint8_t)((frame << 1) & 0x10);
        gb_write(gb, hl, attr);
    }
}

void PeachPictureState2Handler(GBState *gb) {
    if (!gb) return;
    uint8_t tileset = TILESET_EAGLES_TOWER_TOP;
    if (gb_read(gb, hMapId) != MAP_EAGLES_TOWER) {
        if (gb_read(gb, hMapRoom) != ROOM_INDOOR_B_SCHULE_HOUSE) {
            tileset = TILESET_CHRISTINE;
        } else {
            tileset = TILESET_SCHULE_PAINTING;
        }
    }
    gb_write(gb, wTilesetToLoad, tileset);
    gb_write(gb, wC13F, 0x00);
    IncrementGameplaySubtype(gb);
}

void PeachPictureState3Handler(GBState *gb) {
    if (!gb) return;
    uint8_t tilemap = TILEMAP_EAGLES_TOWER_COLLAPSE;
    if (gb_read(gb, hMapId) != MAP_EAGLES_TOWER) {
        if (gb_read(gb, hMapRoom) != ROOM_INDOOR_B_SCHULE_HOUSE) {
            tilemap = TILEMAP_PEACH;
        } else {
            tilemap = TILEMAP_SCHULE_PAINTING;
        }
    }
    gb_write(gb, wBGMapToLoad, tilemap);
    gb_write(gb, wWindowY, 0xFF);
    gb_write(gb, hBaseScrollX, 0x00);
    gb_write(gb, hBaseScrollY, 0x00);
    gb_write(gb, wTransitionSequenceCounter, 0x00);
    gb_write(gb, wC16C, 0x00);
    for (uint8_t i = 0; i < 8; i++) {
        gb_write(gb, (uint16_t)(wD210 + i), 0x00);
    }
    gb_write(gb, wPaletteUnknownE, 0x01);
    IncrementGameplaySubtype(gb);
}

static const uint8_t Data_001_6976[6] = {
    0x14, 0x14, 0x10, 0x10, 0x0C, 0x0C
};

static const uint8_t Data_001_697C[6] = {
    0x17, 0x17, 0x13, 0x13, 0x0F, 0x0F
};

static const uint8_t Data_6982[] = {
    0x00, 0x00, 0xCC, 0x10,
    0x00, 0x08, 0xCE, 0x10,
    0x00, 0x10, 0xDC, 0x10,
    0x00, 0x18, 0xCC, 0x30,
    0x10, 0x00, 0xDE, 0x10,
    0x10, 0x08, 0xE0, 0x10,
    0x10, 0x10, 0xE2, 0x10,
    0x10, 0x18, 0xDE, 0x30,
    0x20, 0x00, 0xE4, 0x10,
    0x20, 0x08, 0xE6, 0x10,
    0x20, 0x10, 0xE8, 0x10,
    0x20, 0x18, 0xE4, 0x30,
    0x30, 0x00, 0xDE, 0x10,
    0x30, 0x08, 0xE0, 0x10,
    0x30, 0x10, 0xE0, 0x30,
    0x30, 0x18, 0xDE, 0x30,
    0x40, 0x00, 0xDE, 0x10,
    0x40, 0x08, 0xE0, 0x10,
    0x40, 0x10, 0xE0, 0x30,
    0x40, 0x18, 0xDE, 0x30
};

static const uint8_t Data_69D2[] = {
    0x00, 0x10, 0xDC, 0x16,
    0x10, 0x10, 0xE2, 0x16,
    0x20, 0x10, 0xE8, 0x16,
    0x00, 0x00, 0xCC, 0x15,
    0x00, 0x08, 0xCE, 0x15,
    0x00, 0x10, 0xCE, 0x35,
    0x00, 0x18, 0xCC, 0x35,
    0x10, 0x00, 0xDE, 0x15,
    0x10, 0x08, 0xE0, 0x15,
    0x10, 0x10, 0xE0, 0x35,
    0x10, 0x18, 0xDE, 0x35,
    0x20, 0x00, 0xE4, 0x15,
    0x20, 0x08, 0xE6, 0x15,
    0x20, 0x10, 0xE6, 0x35,
    0x20, 0x18, 0xE4, 0x35,
    0x30, 0x00, 0xDE, 0x15,
    0x30, 0x08, 0xE0, 0x15,
    0x30, 0x10, 0xE0, 0x35,
    0x30, 0x18, 0xDE, 0x35,
    0x40, 0x00, 0xDE, 0x15,
    0x40, 0x08, 0xE0, 0x15,
    0x40, 0x10, 0xE0, 0x35,
    0x40, 0x18, 0xDE, 0x35
};

static const uint8_t Data_6A2E[] = {
    0x48, 0x08, 0xF0, 0x07,
    0x48, 0x10, 0xF2, 0x07,
    0x48, 0x18, 0xF4, 0x07,
    0x48, 0x20, 0xF4, 0x27,
    0x48, 0x28, 0xF2, 0x27,
    0x48, 0x30, 0xF0, 0x27
};

static const uint8_t Data_6A46[] = {
    0x48, 0x08, 0xF6, 0x07,
    0x48, 0x10, 0xF8, 0x07,
    0x48, 0x18, 0xFA, 0x07,
    0x48, 0x20, 0xFA, 0x27,
    0x48, 0x28, 0xF8, 0x27,
    0x48, 0x30, 0xF6, 0x27
};

static const uint8_t Data_6A5E[] = {
    0x48, 0x08, 0xFC, 0x07,
    0x48, 0x10, 0xFE, 0x07,
    0x48, 0x18, 0xEE, 0x07,
    0x48, 0x20, 0xEE, 0x27,
    0x48, 0x28, 0xFE, 0x27,
    0x48, 0x30, 0xFC, 0x27
};

static const uint8_t *const Data_6A76[3] = {
    Data_6A2E,
    Data_6A46,
    Data_6A5E
};

void func_001_695B(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wScreenShakeVertical, 0);
    uint8_t d215 = gb_read(gb, wD215);
    if (d215 == 0) return;
    d215--;
    gb_write(gb, wD215, d215);
    uint8_t e = (d215 & 0x04) ? 0x00 : 0xFE;
    gb_write(gb, wScreenShakeVertical, e);
}

void func_6A7C(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, hMapId) != MAP_EAGLES_TOWER) return;

    gb_write(gb, hActiveEntitySpriteVariant, 0);
    gb_write(gb, hActiveEntityFlipAttribute, 0);
    gb_write(gb, hActiveEntityTilesOffset, 0);
    gb_write(gb, hActiveEntityPosX, 0x38);

    uint8_t shake = gb_read(gb, wScreenShakeVertical);
    gb_write(gb, hActiveEntityVisualPosY, (uint8_t)(0x20 - shake));

    uint8_t d214 = gb_read(gb, wD214);
    if (d214 != 0) {
        d214--;
        gb_write(gb, wD214, d214);

        uint8_t d212 = gb_read(gb, wD212);
        if ((gb_read(gb, hFrameCounter) & 0x07) == 0) {
            d212++;
            if (d212 >= 3) {
                d212 = 0;
            }
        }
        gb_write(gb, wD212, d212);

        const uint8_t *list = Data_6A76[d212 % 3];
        RenderActiveEntitySpritesRectUsingAllOAM(gb, list, 6, NULL);
    }

    gb_write(gb, hActiveEntityPosX, 0x48);
    uint8_t d211 = gb_read(gb, wD211);
    gb_write(gb, hActiveEntityVisualPosY, (uint8_t)(d211 + 0x20 - shake));

    uint8_t d213 = gb_read(gb, wD213);
    uint8_t is_gbc = (gb_read(gb, hIsGBC) != 0);
    const uint8_t *counts = is_gbc ? Data_001_697C : Data_001_6976;
    uint8_t c = (d213 < 6) ? counts[d213] : counts[0];

    gb_write(gb, wOAMNextAvailableSlot, 0);
    const uint8_t *rect = is_gbc ? Data_69D2 : Data_6982;
    RenderActiveEntitySpritesRect(gb, rect, c, NULL);
}

void PeachPictureState4Handler(GBState *gb) {
    if (!gb) return;
    func_6A7C(gb);
    func_1A39(gb, NULL, NULL);
    if (gb_read(gb, wTransitionSequenceCounter) == 4) {
        IncrementGameplaySubtype(gb);
        gb_write(gb, wD210, 0x80);
    }
}

void func_001_68D9(GBState *gb) {
    if (!gb) return;
    IncrementGameplaySubtype(gb);
    gb_write(gb, wTransitionSequenceCounter, 0);
    gb_write(gb, wC16C, 0);
}

void PeachPictureState5Handler(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, hMapId) == MAP_EAGLES_TOWER) {
        func_6A7C(gb);
        gb_write(gb, wGameplaySubtype, 7);
        return;
    }

    uint8_t joy = gb_read(gb, hJoypadState);
    if ((joy & (J_A | J_B | J_START)) != 0) {
        gb_write(gb, hJingle, JINGLE_VALIDATE);
        func_001_68D9(gb);
    }
}

void PeachPictureState7Handler(GBState *gb) {
    if (!gb) return;
    func_6A7C(gb);
    uint8_t d210 = gb_read(gb, wD210);
    d210--;
    gb_write(gb, wD210, d210);
    if (d210 == 0) {
        gb_write(gb, wScreenShakeVertical, 0);
        gb_write(gb, wD210, 0x20);
        IncrementGameplaySubtype(gb);
    } else {
        uint8_t e = (d210 & 0x04) ? 0xFE : 0x00;
        gb_write(gb, wScreenShakeVertical, e);
    }
}

void PeachPictureState8Handler(GBState *gb) {
    if (!gb) return;
    func_6A7C(gb);
    func_001_695B(gb);
    uint8_t d210 = gb_read(gb, wD210);
    d210--;
    gb_write(gb, wD210, d210);
    if (d210 != 0) return;

    PlayBombExplosionSfx(gb);
    gb_write(gb, wD210, 0x30);
    gb_write(gb, wD214, 0x30);
    gb_write(gb, wD215, 0x18);
    uint8_t d211 = (uint8_t)(gb_read(gb, wD211) + 8);
    gb_write(gb, wD211, d211);
    uint8_t d213 = (uint8_t)(gb_read(gb, wD213) + 1);
    gb_write(gb, wD213, d213);
    if (d213 == 4) {
        gb_write(gb, wD210, 0x80);
        IncrementGameplaySubtype(gb);
    }
}

void PeachPictureState9Handler(GBState *gb) {
    if (!gb) return;
    func_6A7C(gb);
    func_001_695B(gb);
    uint8_t d210 = gb_read(gb, wD210);
    d210--;
    gb_write(gb, wD210, d210);
    if (d210 != 0) return;

    IncrementGameplaySubtype(gb);
    gb_write(gb, wTransitionSequenceCounter, 0);
    gb_write(gb, wC16C, 0);
}

void FileSaveFadeOut(GBState *gb) {
    if (!gb) return;
    func_1A22(gb, NULL, NULL);
    if (gb_read(gb, wTransitionSequenceCounter) != 4) {
        return;
    }

    if (gb_read(gb, hIsGBC) != 0) {
        for (uint16_t i = 0; i < 0x80; i++) {
            gb_write(gb, rSVBK, 3);
            uint8_t b = gb_read(gb, (uint16_t)(wBGPal1 + i));
            gb_write(gb, rSVBK, 2);
            gb_write(gb, (uint16_t)(wBGPal1 + i), b);
        }
        gb_write(gb, rSVBK, 3);
        gb_write(gb, wIsFileSelectionArrowShifted, 0);
        gb_write(gb, rSVBK, 0);
    }

    gb_write(gb, wPaletteUnknownE, 1);
    gb_write(gb, wBlockItemUsage, 0);
    gb_write(gb, wC116, 0);
    gb_write(gb, hBaseScrollX, 0);
    gb_write(gb, hBaseScrollY, 0);
    gb_write(gb, wC167, 0);
    gb_write(gb, hVolumeRight, 7);
    gb_write(gb, hVolumeLeft, 0x70);
    gb_write(gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(gb, hContinueMusicAfterWarp, GAMEPLAY_WORLD);
    gb_write(gb, wGameplaySubtype, GAMEPLAY_WORLD_LOAD_2);

    uint8_t tileset = (gb_read(gb, wIsIndoor) != 0) ? TILESET_INDOOR : TILESET_BASE_OVERWORLD_DUP;
    gb_write(gb, wTilesetToLoad, tileset);

    func_001_5888(gb);
    InitializeInventoryBar(gb);
}

void PeachPictureStateAHandler(GBState *gb) {
    if (!gb) return;
    func_6A7C(gb);
    FileSaveFadeOut(gb);
}

void PeachPictureState1Handler(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wC167, 1);
    func_1A22(gb, NULL, NULL);
    if (gb_read(gb, wTransitionSequenceCounter) != 4) {
        return;
    }
    func_001_5888(gb);
    if (gb_read(gb, hMapId) != MAP_EAGLES_TOWER) {
        gb_write(gb, hVolumeRight, 3);
        gb_write(gb, hVolumeLeft, 0x30);
    }
    IncrementGameplaySubtype(gb);
    gb_write(gb, wScrollXOffset, 0);
    gb_write(gb, wTilesetToLoad, TILESET_0F);
}

void PeachPictureState0Handler(GBState *gb) {
    if (!gb) return;
    IncrementGameplaySubtype(gb);
    if (gb_read(gb, hIsGBC) != 0) {
        for (uint16_t i = 0; i < 0x80; i++) {
            gb_write(gb, rSVBK, 0);
            uint8_t b = gb_read(gb, (uint16_t)(wBGPal1 + i));
            gb_write(gb, rSVBK, 3);
            gb_write(gb, (uint16_t)(wBGPal1 + i), b);
        }
        gb_write(gb, rSVBK, 0);
    }
    PeachPictureState1Handler(gb);
}

void PeachPictureEntryPoint(GBState *gb) {
    if (!gb) return;
    uint8_t subtype = gb_read(gb, wGameplaySubtype);
    switch (subtype) {
        case 0x00: PeachPictureState0Handler(gb); break;
        case 0x01: PeachPictureState1Handler(gb); break;
        case 0x02: PeachPictureState2Handler(gb); break;
        case 0x03: PeachPictureState3Handler(gb); break;
        case 0x04: PeachPictureState4Handler(gb); break;
        case 0x05: PeachPictureState5Handler(gb); break;
        case 0x06: FileSaveFadeOut(gb);           break;
        case 0x07: PeachPictureState7Handler(gb); break;
        case 0x08: PeachPictureState8Handler(gb); break;
        case 0x09: PeachPictureState9Handler(gb); break;
        case 0x0A: PeachPictureStateAHandler(gb); break;
        default: break;
    }
}
