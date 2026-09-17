#include "bank2/swimming.h"
#include "bank2/link_motion.h"
#include "bank2/vfx.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/joypad.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/sfx.h"
#include "home/link.h"

/* Bank 2 Swimming Velocity Tables */
const int8_t Data_002_4EF0[16] = {
    0x00, 0x08, -0x08, 0x00, 0x00, 0x06, -0x06, 0x00,
    0x00, 0x06, -0x06, 0x00, 0x00, 0x00,  0x00, 0x00
};

const int8_t Data_002_4F00[16] = {
    0x00, 0x10, -0x10, 0x00, 0x00, 0x0C, -0x0C, 0x00,
    0x00, 0x0C, -0x0C, 0x00, 0x00, 0x00,  0x00, 0x00
};

const int8_t Data_002_4F10[16] = {
    0x00, 0x00, 0x00, 0x00, -0x08, -0x06, -0x06, 0x00,
    0x08, 0x06, 0x06, 0x00,  0x00,  0x00,  0x00, 0x00
};

const int8_t Data_002_4F20[16] = {
    0x00, 0x00, 0x00, 0x00, -0x10, -0x0C, -0x0C, 0x00,
    0x10, 0x0C, 0x0C, 0x00,  0x00,  0x00,  0x00, 0x00
};

void LinkMotionSwimmingHandler(GBState *gb,
                               void (*check_map_transition)(GBState *),
                               void (*func_753a)(GBState *),
                               uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                               void (*splash_vfx)(GBState *, uint8_t)) {
    if (!gb) return;

    if (gb_read(gb, wFreeMovementMode) != 0) {
        gb_write(gb, wLinkMotionState, LINK_MOTION_DEFAULT);
        return;
    }

    ResetSpinAttack(gb);
    gb_write_hram(gb, hLinkPositionZ, 0);
    gb_write(gb, wIsLinkInTheAir, 0);
    gb_write(gb, wLinkAttackStepAnimationCountdown, 0);
    gb_write(gb, wSwordAnimationState, 0);
    gb_write(gb, wC16A, 0);
    gb_write(gb, wC16D, 0);

    gb_write(gb, wD46B, gb_read(gb, wIndoorRoom));

    if ((gb_read_hram(gb, hLinkInteractiveMotionBlocked) | gb_read(gb, wDialogState)) != 0) {
        ClearLinkPositionIncrement(gb);
        if (gb_read_hram(gb, hLinkInteractiveMotionBlocked) != 0) {
            gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0);
        }
        UpdateLinkWalkingAnimation(gb);
        return;
    }

    /* label_002_4F6D: Check B button for diving */
    uint8_t joypad = gb_read_hram(gb, hJoypadState);
    if ((joypad & J_B) != 0) {
        uint8_t phys = (uint8_t)(gb_read_hram(gb, hLinkPhysicsModifier) ^ 0x01);
        gb_write_hram(gb, hLinkPhysicsModifier, phys);
        if (phys != 0) {
            gb_write_hram(gb, hLinkCountdown, 0xA0);
            uint8_t py = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - 3);
            if (splash_vfx) {
                splash_vfx(gb, py);
            } else {
                func_002_5928(gb, py);
            }
        }
    }

    /* Check A button for swim stroke speed boost */
    uint8_t c183 = gb_read(gb, wC183);
    if (c183 != 0) {
        gb_write(gb, wC183, (uint8_t)(c183 - 1));
    } else if ((joypad & J_A) != 0) {
        gb_write_hram(gb, hJingle, JINGLE_SWIM);
        gb_write(gb, wC183, 0x20);
    }

    /* jr_002_4FA1: Velocity adjustments every even frame */
    if ((gb_read_hram(gb, hFrameCounter) & 0x01) == 0) {
        uint8_t btn = (uint8_t)(gb_read_hram(gb, hPressedButtonsMask) & 0x0F);
        uint8_t speed_idx = gb_read(gb, wC183);

        const int8_t *x_table = (speed_idx < 0x10) ? Data_002_4EF0 : Data_002_4F00;
        int8_t target_x = x_table[btn];
        int8_t cur_speed_x = (int8_t)gb_read_hram(gb, hLinkSpeedX);
        uint8_t diff_x = (uint8_t)((uint8_t)target_x - (uint8_t)cur_speed_x);

        if (diff_x != 0) {
            cur_speed_x++;
            if ((diff_x & 0x80) != 0) {
                cur_speed_x -= 2;
            }
            gb_write_hram(gb, hLinkSpeedX, (uint8_t)cur_speed_x);
        }

        const int8_t *y_table = (speed_idx < 0x10) ? Data_002_4F10 : Data_002_4F20;
        int8_t target_y = y_table[btn];
        int8_t cur_speed_y = (int8_t)gb_read_hram(gb, hLinkSpeedY);
        uint8_t diff_y = (uint8_t)((uint8_t)target_y - (uint8_t)cur_speed_y);

        if (diff_y != 0) {
            cur_speed_y++;
            if ((diff_y & 0x80) != 0) {
                cur_speed_y -= 2;
            }
            gb_write_hram(gb, hLinkSpeedY, (uint8_t)cur_speed_y);
        }

        uint8_t moving = gb_read_hram(gb, hLinkSpeedX) | gb_read_hram(gb, hLinkSpeedY) | gb_read_hram(gb, hLinkPhysicsModifier);
        if (moving != 0) {
            gb_write(gb, wConsecutiveStepsCount, (uint8_t)(gb_read(gb, wConsecutiveStepsCount) + 1));
        } else {
            gb_write(gb, wConsecutiveStepsCount, 0x03);
        }

        uint8_t dir = JoypadToLinkDirection[btn];
        if (dir != 0x0F) {
            gb_write_hram(gb, hLinkDirection, dir);
        }
    }

    UpdateLinkWalkingAnimation(gb);

    if (gb_read_hram(gb, hLinkInteractiveMotionBlocked) != 0) {
        gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0);
    } else {
        func_002_44AD(gb, check_map_transition);
    }

    if (func_753a) {
        func_753a(gb);
    }

    /* Sunk heart piece and underwater cave warp check */
    if (gb_read_hram(gb, hLinkPhysicsModifier) == 0) {
        return;
    }

    if (gb_read_hram(gb, hLinkCountdown) == 0) {
        gb_write_hram(gb, hLinkPhysicsModifier, 0);
    }

    uint8_t map_id = gb_read_hram(gb, hMapId);
    uint8_t map_room = gb_read_hram(gb, hMapRoom);

    if (map_id == 0) {
        if (map_room != ROOM_OW_KANALET_MOAT_HEARTPIECE) {
            return;
        }
    } else if (map_id == MAP_CAVE_WATER) {
        if (map_room == ROOM_INDOOR_A_WATER_FLOODED_GROTTO) {
            /* Flooded grotto sunk piece of heart */
        } else if (map_room == UNKNOWN_ROOM_8D) {
            /* Underwater grotto tunnel warp */
            uint8_t dy = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - 0x50 + 0x08);
            if (dy >= 0x10) return;
            uint8_t dx = (uint8_t)(gb_read_hram(gb, hLinkPositionX) - 0x58 + 0x08);
            if (dx >= 0x10) return;

            gb_write(gb, wWarp0DestinationX, gb_read_hram(gb, hLinkPositionX));
            gb_write(gb, wD463, gb_read(gb, wLinkMotionState));
            ApplyMapFadeOutTransitionWithNoise(gb);
            return;
        } else {
            return;
        }
    } else {
        return;
    }

    /* Check coordinates for sunken heart piece (02:503B) */
    uint8_t dy = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - 0x50 + 0x08);
    if (dy >= 0x10) return;
    uint8_t dx = (uint8_t)(gb_read_hram(gb, hLinkPositionX) - 0x58 + 0x08);
    if (dx >= 0x10) return;

    uint8_t status = gb_read_hram(gb, hRoomStatus);
    if ((status & (1 << 5)) != 0) {
        return;
    }
    gb_write_hram(gb, hRoomStatus, (uint8_t)(status | (1 << 5)));

    if (spawn_new_entity) {
        uint16_t de = spawn_new_entity(gb, ENTITY_HEART_PIECE);
        if (de != 0xFFFF) {
            gb_write(gb, (uint16_t)(wEntitiesPosXTable + de), gb_read_hram(gb, hLinkPositionX));
            gb_write(gb, (uint16_t)(wEntitiesPosYTable + de), gb_read_hram(gb, hLinkPositionY));
            gb_write(gb, (uint16_t)(wEntitiesPosZTable + de), 0x03);
            uint16_t phys_addr = (uint16_t)(wEntitiesPhysicsFlagsTable + de);
            gb_write(gb, phys_addr, (uint8_t)(gb_read(gb, phys_addr) & ~(1 << 3)));
        }
    }
}
