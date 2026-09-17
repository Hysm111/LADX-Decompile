#include "bank2/shovel.h"
#include "constants/entities.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "home/bank.h"
#include "home/dialog.h"
#include "home/entities.h"
#include "home/gameplay.h"
#include "home/link.h"
#include "home/room.h"

/* Bank 2 Shovel Digging & Animation Tables */
const uint8_t LinkDirectionToLinkAnimationState2[8] = {
    0x71, 0x72, 0x6F, 0x70, 0x73, 0x74, 0x6D, 0x6E
};

const int8_t LinkDirectionToAdjacentTileIndexX[4] = {
    0x14, (int8_t)0xFC, 0x08, 0x08
};

const int8_t LinkDirectionToAdjacentTileIndexY[4] = {
    0x0A, 0x0A, (int8_t)0xFC, 0x14
};

void func_002_4BD4(GBState *gb, uint16_t hl) {
    if (!gb) return;

    uint8_t bg_high = gb_read_hram(gb, hIntersectedObjectBGAddressHigh);
    uint8_t bg_low = gb_read_hram(gb, hIntersectedObjectBGAddressLow);

    gb_write(gb, hl++, bg_high);
    gb_write(gb, hl++, bg_low);
    gb_write(gb, hl++, 0x81);

    if (gb_read(gb, wIsIndoor) == 0) {
        gb_write(gb, hl++, 0x6A);
        gb_write(gb, hl++, 0x7A);
        gb_write(gb, hl++, bg_high);
        gb_write(gb, hl++, (uint8_t)(bg_low + 1));
        gb_write(gb, hl++, 0x81);
        gb_write(gb, hl++, 0x6B);
        gb_write(gb, hl++, 0x7B);
    } else {
        gb_write(gb, hl++, 0x04);
        gb_write(gb, hl++, 0x06);
        gb_write(gb, hl++, bg_high);
        gb_write(gb, hl++, (uint8_t)(bg_low + 1));
        gb_write(gb, hl++, 0x81);
        gb_write(gb, hl++, 0x05);
        gb_write(gb, hl++, 0x07);
    }

    gb_write(gb, hl++, 0x00);
}

void func_002_4C14(GBState *gb, uint16_t hl) {
    if (!gb) return;

    uint8_t vram1_size = gb_read(gb, wDrawCommandsVRAM1Size);
    uint16_t de = (uint16_t)(wDrawCommandVRAM1 + vram1_size);
    gb_write(gb, wDrawCommandsVRAM1Size, (uint8_t)(vram1_size + 0x0A));

    uint8_t bg_high = gb_read_hram(gb, hIntersectedObjectBGAddressHigh);
    uint8_t bg_low = gb_read_hram(gb, hIntersectedObjectBGAddressLow);

    gb_write(gb, hl++, bg_high);
    gb_write(gb, de++, bg_high);

    gb_write(gb, hl++, bg_low);
    gb_write(gb, de++, bg_low);

    gb_write(gb, hl++, 0x81);
    gb_write(gb, de++, 0x81);

    if (gb_read(gb, wIsIndoor) == 0) {
        gb_write(gb, hl++, 0x6A);
        gb_write(gb, hl++, 0x7A);
        gb_write(gb, de++, 0x03);
        gb_write(gb, de++, 0x03);

        gb_write(gb, hl++, bg_high);
        gb_write(gb, de++, bg_high);

        gb_write(gb, hl++, (uint8_t)(bg_low + 1));
        gb_write(gb, de++, (uint8_t)(bg_low + 1));

        gb_write(gb, hl++, 0x81);
        gb_write(gb, de++, 0x81);

        gb_write(gb, hl++, 0x6B);
        gb_write(gb, de++, 0x03);
        gb_write(gb, de++, 0x03);
        gb_write(gb, hl++, 0x7B);
    } else {
        gb_write(gb, hl++, 0x04);
        gb_write(gb, hl++, 0x06);
        gb_write(gb, de++, 0x05);
        gb_write(gb, de++, 0x05);

        gb_write(gb, hl++, bg_high);
        gb_write(gb, de++, bg_high);

        gb_write(gb, hl++, (uint8_t)(bg_low + 1));
        gb_write(gb, de++, (uint8_t)(bg_low + 1));

        gb_write(gb, hl++, 0x81);
        gb_write(gb, de++, 0x81);

        gb_write(gb, hl++, 0x05);
        gb_write(gb, de++, 0x05);
        gb_write(gb, de++, 0x05);
        gb_write(gb, hl++, 0x07);
    }

    gb_write(gb, hl++, 0x00);
    gb_write(gb, de++, 0x00);
}

void label_002_4C92(GBState *gb,
                    uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                    void (*apply_vector)(GBState *)) {
    if (!gb) return;

    uint8_t mp1 = gb_read_hram(gb, hMultiPurpose1);
    uint16_t room_obj_addr = (uint16_t)(wRoomObjects + mp1);
    gb_write(gb, room_obj_addr, OBJECT_SHOVEL_HOLE);

    BackupObjectInRAM2(gb, room_obj_addr, 0x82);
    GetIntersectedObjectBGAddress(gb);

    uint8_t draw_size = gb_read(gb, wDrawCommandsSize);
    uint16_t hl = (uint16_t)(wDrawCommand + draw_size);
    gb_write(gb, wDrawCommandsSize, (uint8_t)(draw_size + 0x0A));

    if (gb_read_hram(gb, hIsGBC) != 0) {
        func_002_4C14(gb, hl);
    } else {
        func_002_4BD4(gb, hl);
    }

    /* Random drops (1 in 8 chance) */
    if ((GetRandomByte(gb) & 0x07) != 0) {
        return;
    }

    if (gb_read(gb, wIsIndoor) == 0) {
        if (gb_read_hram(gb, hMapRoom) == ROOM_OW_EAGLES_TOWER) {
            return;
        }
    }

    uint8_t rand_drop = GetRandomByte(gb);
    uint8_t drop_type = (rand_drop & 1) ? ENTITY_DROPPABLE_HEART : ENTITY_DROPPABLE_RUPEE;

    uint16_t slot = SpawnNewEntity_trampoline(gb, drop_type, spawn_new_entity);
    if (slot == 0xFFFF) {
        return;
    }

    uint8_t left = (uint8_t)(gb_read_hram(gb, hIntersectedObjectLeft) + 0x08);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), left);

    uint8_t top = (uint8_t)(gb_read_hram(gb, hIntersectedObjectTop) + 0x10);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), top);

    gb_write(gb, (uint16_t)(wEntitiesSlowTransitionCountdownTable + slot), 0x80);
    gb_write(gb, (uint16_t)(wEntitiesPrivateCountdown1Table + slot), 0x18);
    gb_write(gb, (uint16_t)(wEntitiesSpeedZTable + slot), 0x20);

    /* ApplyVectorTowardsLink_trampoline with speed $0C */
    ApplyVectorTowardsLink_trampoline(gb, apply_vector);

    uint8_t spd_y = (uint8_t)(-(int8_t)gb_read_hram(gb, hMultiPurpose0));
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + slot), spd_y);

    uint8_t spd_x = (uint8_t)(-(int8_t)gb_read_hram(gb, hMultiPurpose1));
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + slot), spd_x);
}

bool func_002_4D20(GBState *gb) {
    if (!gb) return false;

    /* jump to .done if one or more is true:
     * - Link carry something
     * - Link is in the air
     * - Link is in motion
     * - screen scrolling is happening
     */
    uint8_t carrying = gb_read(gb, wIsCarryingLiftedObject);
    uint8_t pos_z = gb_read_hram(gb, hLinkPositionZ);
    uint8_t motion = gb_read(gb, wLinkMotionState);
    uint8_t scrolling = gb_read_hram(gb, hIsSideScrolling);
    if ((carrying | pos_z | motion | scrolling) != 0) {
        return false; /* carry set */
    }

    uint8_t dir = gb_read_hram(gb, hLinkDirection) & 0x03;
    uint8_t posX = gb_read_hram(gb, hLinkPositionX);
    uint8_t left = (uint8_t)((posX + (uint8_t)LinkDirectionToAdjacentTileIndexX[dir] - 0x08) & 0xF0);
    gb_write_hram(gb, hIntersectedObjectLeft, left);
    uint8_t c = (uint8_t)(left >> 4);

    uint8_t posY = gb_read_hram(gb, hLinkPositionY);
    uint8_t top = (uint8_t)((posY + (uint8_t)LinkDirectionToAdjacentTileIndexY[dir] - 0x10) & 0xF0);
    gb_write_hram(gb, hIntersectedObjectTop, top);

    uint8_t tile_idx = (uint8_t)(top | c);
    gb_write_hram(gb, hMultiPurpose1, tile_idx);

    uint16_t room_obj_addr = (uint16_t)(wRoomObjects + tile_idx);
    if ((room_obj_addr >> 8) != 0xD7) {
        return false; /* carry set */
    }

    uint8_t obj = gb_read(gb, room_obj_addr);
    gb_write_hram(gb, hMultiPurpose0, obj);

    uint8_t is_indoor = gb_read(gb, wIsIndoor);
    uint16_t de = (uint16_t)((is_indoor << 8) | obj);
    uint8_t flags = GetObjectPhysicsFlags_trampoline(gb, de);
    if (flags != 0) {
        return false; /* carry set */
    }

    if (is_indoor != 0) {
        if (obj != 0x05) {
            return false; /* carry set */
        }
    } else {
        if (obj == 0x0C || obj == 0x0D || obj == 0xB9) {
            return false; /* carry set */
        }
    }

    return true; /* carry clear */
}

bool func_002_4BC8(GBState *gb,
                   uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                   void (*apply_vector)(GBState *)) {
    if (!gb) return false;

    if (!func_002_4D20(gb)) {
        return false;
    }

    gb_write(gb, wLinkUsingShovel, 0x02);
    label_002_4C92(gb, spawn_new_entity, apply_vector);
    return true;
}

void func_002_4B49(GBState *gb,
                   void (*func_020_4b4a)(GBState *),
                   bool (*shovel_dig_action)(GBState *)) {
    if (!gb) return;

    if (gb_read(gb, wLinkUsingShovel) == 0) {
        return;
    }

    if (gb_read_hram(gb, hMapId) == MAP_WINDFISHS_EGG) {
        if (gb_read(gb, wFinalNightmareForm) == 0x02) {
            func_020_4B4A_trampoline(gb, func_020_4b4a);
            gb_write(gb, wSwordCollisionEnabled, 0x01);
        }
    }

    gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x01);
    ClearLinkPositionIncrement(gb);

    gb_write(gb, wSwordAnimationState, 0x00);
    gb_write(gb, wIsUsingSpinAttack, 0x00);
    gb_write(gb, wSwordCharge, 0x00);

    uint8_t timer = (uint8_t)(gb_read(gb, wLinkUsingShovelTimer) + 1);
    gb_write(gb, wLinkUsingShovelTimer, timer);

    if (timer == 0x10) {
        if (shovel_dig_action) {
            shovel_dig_action(gb);
        } else {
            func_002_4BC8(gb, NULL, NULL);
        }
    }

    if (timer == 0x18) {
        if (gb_read(gb, wLinkUsingShovel) == 0x02) {
            if (gb_read(gb, wIsMarinFollowingLink) != 0) {
                if (gb_read(gb, wDialogState) == 0) {
                    OpenDialogInTable2(gb, 0x79);
                }
            }
        }
        gb_write(gb, wLinkUsingShovel, 0x00);
        gb_write(gb, wC1AC, 0x00);
        return;
    }

    uint8_t e = (uint8_t)((timer >> 4) & 1);
    uint8_t dir = gb_read_hram(gb, hLinkDirection) & 3;
    uint8_t idx = (uint8_t)((dir << 1) + e);
    gb_write_hram(gb, hLinkAnimationState, LinkDirectionToLinkAnimationState2[idx]);
}
