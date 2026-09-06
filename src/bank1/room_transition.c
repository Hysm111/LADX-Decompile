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
