#include "bank2/falling.h"
#include "bank2/revolving_door.h"
#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/link.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/physics.h"
#include "constants/rooms.h"
#include "constants/sfx.h"
#include "home/link.h"

void LinkMotionUnknownHandler(GBState *gb) {
    if (!gb) return;

    gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x01);
    UpdateFinalLinkPosition(gb);
    func_21E1(gb);

    uint8_t px = (uint8_t)(gb_read_hram(gb, hLinkPositionX) & 0xF0);
    uint8_t pz = gb_read_hram(gb, hLinkPositionZ);

    if (px != 0xE0 && pz < 0x78) {
        return;
    }

    ApplyMapFadeOutTransition(gb);
    ClearLinkPositionIncrement(gb);
    gb_write_hram(gb, hLinkPositionZ, 0);
    gb_write_hram(gb, hLinkVelocityZ, 0);
    gb_write(gb, wMapEntrancePositionZ, 0x70);
}

const uint8_t LinkFallingDownAnimation[10] = {
    LINK_ANIMATION_STATE_FALLING_PIT_1,
    LINK_ANIMATION_STATE_FALLING_PIT_2,
    LINK_ANIMATION_STATE_FALLING_PIT_3,
    LINK_ANIMATION_STATE_FALLING_PIT_3,
    LINK_ANIMATION_STATE_HIDDEN,
    LINK_ANIMATION_STATE_HIDDEN,
    LINK_ANIMATION_STATE_HIDDEN,
    LINK_ANIMATION_STATE_HIDDEN,
    LINK_ANIMATION_STATE_HIDDEN,
    LINK_ANIMATION_STATE_HIDDEN
};

void func_002_52D6(GBState *gb) {
    if (!gb) return;

    if (gb_read_hram(gb, hStaircase) != 0) {
        gb_write_hram(gb, hStaircase, STAIRCASE_INACTIVE);
    }
}

void label_002_52B9(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wInvincibilityCounter, 0x40);

    uint8_t entry_x = gb_read(gb, wLinkMapEntryPositionX);
    gb_write_hram(gb, hLinkPositionX, entry_x);
    gb_write_hram(gb, hLinkFinalPositionX, entry_x);

    uint8_t entry_y = gb_read(gb, wLinkMapEntryPositionY);
    gb_write_hram(gb, hLinkPositionY, entry_y);
    gb_write_hram(gb, hLinkFinalPositionY, entry_y);

    uint8_t pz = gb_read_hram(gb, hLinkPositionZ);
    gb_write(gb, wC145, (uint8_t)(entry_y - pz));

    func_002_4EDD(gb);
}

void LinkMotionFallingDownHandler(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wC167, 0x01);
    uint8_t anim_frame = (uint8_t)(gb_read(gb, wLinkAnimationFrame) + 1);
    gb_write(gb, wLinkAnimationFrame, anim_frame);

    uint8_t index = (anim_frame >> 4) & 0x0F;
    if (index != 0x06) {
        if (index < sizeof(LinkFallingDownAnimation)) {
            gb_write_hram(gb, hLinkAnimationState, LinkFallingDownAnimation[index]);
        }
        return;
    }

    /* reset sword parameter */
    gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x00);
    gb_write(gb, wIsUsingSpinAttack, 0x00);
    gb_write(gb, wSwordCharge, 0x00);
    func_002_52D6(gb);

    if (gb_read(gb, wIsIndoor) == 0) {
        uint8_t room = gb_read_hram(gb, hMapRoom);
        if (room == ROOM_OW_TURTLE_ROCK_WARP_HOLE ||
            room == ROOM_OW_UKUKU_PRAIRIE_WARP_HOLE ||
            room == ROOM_OW_WATERFALL_WARP_HOLE ||
            room == ROOM_OW_ANIMAL_VILLAGE_WARP_HOLE) {
            gb_write(gb, wLinkMotionState, LINK_MOTION_TELEPORT);
            gb_write_hram(gb, hLinkCountdown, 0x40);
            gb_write_hram(gb, hLinkPhysicsModifier, 0x00);
            gb_write_hram(gb, hLinkAnimationState, 0xFF);
            return;
        }
    }

    uint8_t phys = gb_read(gb, wLinkFallingDownObjectPhysics);
    if (phys != OBJ_PHYSICS_PIT) {
        if (phys != OBJ_PHYSICS_TRACTOR_DEVICE) {
            if (gb_read(gb, wWarp0MapCategory) == 0x02) {
                gb_write(gb, wWarp0DestinationX, gb_read_hram(gb, hLinkPositionY));
                gb_write(gb, wMapEntrancePositionZ, 0x00);
                ClearLinkPositionIncrement(gb);
                gb_write_hram(gb, hLinkVelocityZ, 0x00);
                gb_write(gb, wIsLinkInTheAir, 0x00);
                ApplyMapFadeOutTransition(gb);
                return;
            }

            uint8_t dest_x = (uint8_t)((gb_read_hram(gb, hLinkPositionX) & 0xF0) + 0x08);
            uint8_t dest_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) & 0xF0);
            gb_write(gb, wWarp0DestinationX, dest_x);
            gb_write(gb, wWarp0DestinationY, dest_y);
        }

        gb_write(gb, wD475, 0x01);
        gb_write(gb, wMapEntrancePositionZ, 0x70);
        ClearLinkPositionIncrement(gb);
        gb_write_hram(gb, hLinkVelocityZ, 0x00);
        gb_write(gb, wIsLinkInTheAir, 0x00);
        ApplyMapFadeOutTransition(gb);
        return;
    }

    if (gb_read(gb, wIsIndoor) == 0) {
        if (gb_read_hram(gb, hMapRoom) == UNKNOWN_ROOM_1E) {
            gb_write(gb, wD475, 0x01);
            gb_write(gb, wMapEntrancePositionZ, 0x70);
            ClearLinkPositionIncrement(gb);
            gb_write_hram(gb, hLinkVelocityZ, 0x00);
            gb_write(gb, wIsLinkInTheAir, 0x00);
            ApplyMapFadeOutTransition(gb);
            return;
        }
    }

    if (gb_read_hram(gb, hMapId) == MAP_CAVE_B) {
        uint8_t room = gb_read_hram(gb, hMapRoom);
        if (room == ROOM_INDOOR_B_MOUNTAIN_CAVE_ROOM_1 ||
            room == ROOM_INDOOR_B_MOUNTAIN_CAVE_ROOM_2 ||
            room == ROOM_INDOOR_B_MOUNTAIN_CAVE_ROOM_3 ||
            room == ROOM_INDOOR_B_MOUNTAIN_CAVE_ROOM_4) {
            gb_write(gb, wWarp0MapCategory, 0x00);
            gb_write(gb, wWarp0Map, 0x00);
            gb_write(gb, wWarp0Room, 0x1A);
            gb_write(gb, wWarp0DestinationX, 0x68);
            gb_write(gb, wWarp0DestinationY, 0x56);
            gb_write(gb, wMapEntrancePositionZ, 0x24);
            gb_write_hram(gb, hLinkDirection, DIRECTION_DOWN);
            ApplyMapFadeOutTransition(gb);
            return;
        }
    }

    label_002_52B9(gb);
    gb_write(gb, wSubtractHealthBuffer, (uint8_t)(gb_read(gb, wSubtractHealthBuffer) + 0x04));
    gb_write(gb, wC167, 0x00);
}

void LinkMotionRecoverHandler(GBState *gb) {
    if (!gb) return;

    ResetSpinAttack(gb);
    ClearLinkPositionIncrement(gb);

    uint8_t countdown = gb_read_hram(gb, hLinkCountdown);
    if (countdown != 0) {
        uint8_t anim;
        if (countdown < 0x30) {
            anim = LINK_ANIMATION_STATE_HIDDEN;
        } else if (countdown < 0x40) {
            anim = LINK_ANIMATION_STATE_HOLD_SWIMMING_2;
        } else {
            if (countdown == 0x40) {
                gb_write_hram(gb, hWaveSfx, NOISE_SFX_SPIN_ATTACK);
            }
            anim = LINK_ANIMATION_STATE_HOLD_SWIMMING_1_DOWN;
        }
        gb_write_hram(gb, hLinkAnimationState, anim);
        return;
    }

    gb_write(gb, wC167, 0x00);

    if (gb_read_hram(gb, hLinkPhysicsModifier) == 0x06) {
        gb_write(gb, wSubtractHealthBuffer, (uint8_t)(gb_read(gb, wSubtractHealthBuffer) + 0x04));
    }
    gb_write_hram(gb, hLinkPhysicsModifier, 0x00);

    if (gb_read(gb, wIsIndoor) == 0) {
        if (gb_read_hram(gb, hMapRoom) == ROOM_OW_ANGLERS_TUNNEL_ENTRANCE) {
            gb_write(gb, wLinkMapEntryPositionX, 0x48);
            gb_write(gb, wLinkMapEntryPositionY, 0x30);
        }
    }

    label_002_52B9(gb);
}
