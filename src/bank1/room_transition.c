#include "bank1/room_transition.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/dialog.h"

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
