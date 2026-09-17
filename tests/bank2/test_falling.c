#include "test_bank2.h"

#include "gb.h"
#include "bank2/falling.h"
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

#include <assert.h>
#include <stdint.h>

void test_bank2_falling(void) {
    /* Test 50: LinkFallingDownAnimation Table */
    {
        assert(LinkFallingDownAnimation[0] == LINK_ANIMATION_STATE_FALLING_PIT_1);
        assert(LinkFallingDownAnimation[1] == LINK_ANIMATION_STATE_FALLING_PIT_2);
        assert(LinkFallingDownAnimation[2] == LINK_ANIMATION_STATE_FALLING_PIT_3);
        assert(LinkFallingDownAnimation[3] == LINK_ANIMATION_STATE_FALLING_PIT_3);
        assert(LinkFallingDownAnimation[4] == LINK_ANIMATION_STATE_HIDDEN);
        assert(LinkFallingDownAnimation[5] == LINK_ANIMATION_STATE_HIDDEN);
        assert(LinkFallingDownAnimation[6] == LINK_ANIMATION_STATE_HIDDEN);
        assert(LinkFallingDownAnimation[7] == LINK_ANIMATION_STATE_HIDDEN);
        assert(LinkFallingDownAnimation[8] == LINK_ANIMATION_STATE_HIDDEN);
        assert(LinkFallingDownAnimation[9] == LINK_ANIMATION_STATE_HIDDEN);
    }

    /* Test 51: func_002_52D6 Staircase Inactive Handler */
    {
        GBState gb;
        gb_init(&gb);

        /* Inactive when zero remains zero */
        gb_write_hram(&gb, hStaircase, 0);
        func_002_52D6(&gb);
        assert(gb_read_hram(&gb, hStaircase) == 0);

        /* Active staircase reset to STAIRCASE_INACTIVE (1) */
        gb_write_hram(&gb, hStaircase, 5);
        func_002_52D6(&gb);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_INACTIVE);
    }

    /* Test 52: label_002_52B9 Link Map Entry Position & Motion State Reset */
    {
        GBState gb;
        gb_init(&gb);

        gb_write(&gb, wLinkMapEntryPositionX, 0x48);
        gb_write(&gb, wLinkMapEntryPositionY, 0x60);
        gb_write_hram(&gb, hLinkPositionZ, 0x10);
        gb_write(&gb, wLinkMotionState, LINK_MOTION_FALLING_DOWN);
        gb_write(&gb, wLinkAnimationFrame, 0x50);
        gb_write(&gb, wC167, 0x01);

        label_002_52B9(&gb);

        assert(gb_read(&gb, wInvincibilityCounter) == 0x40);
        assert(gb_read_hram(&gb, hLinkPositionX) == 0x48);
        assert(gb_read_hram(&gb, hLinkFinalPositionX) == 0x48);
        assert(gb_read_hram(&gb, hLinkPositionY) == 0x60);
        assert(gb_read_hram(&gb, hLinkFinalPositionY) == 0x60);
        assert(gb_read(&gb, wC145) == (uint8_t)(0x60 - 0x10));
        assert(gb_read(&gb, wLinkAnimationFrame) == 0);
        assert(gb_read(&gb, wC167) == 0);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_DEFAULT);
    }

    /* Test 53: LinkMotionFallingDownHandler (02:50D4) */
    {
        GBState gb;

        /* 1. Animation frame advancement when index != 6 */
        gb_init(&gb);
        gb_write(&gb, wLinkAnimationFrame, 0x00);
        LinkMotionFallingDownHandler(&gb);
        assert(gb_read(&gb, wC167) == 1);
        assert(gb_read(&gb, wLinkAnimationFrame) == 0x01);
        assert(gb_read_hram(&gb, hLinkAnimationState) == LINK_ANIMATION_STATE_FALLING_PIT_1);

        gb_write(&gb, wLinkAnimationFrame, 0x1F);
        LinkMotionFallingDownHandler(&gb);
        assert(gb_read(&gb, wLinkAnimationFrame) == 0x20);
        assert(gb_read_hram(&gb, hLinkAnimationState) == LINK_ANIMATION_STATE_FALLING_PIT_3);

        /* 2. Overworld warp hole rooms trigger TELEPORT motion */
        gb_init(&gb);
        gb_write(&gb, wLinkAnimationFrame, 0x5F); /* increment to 0x60 -> index 6 */
        gb_write(&gb, wIsIndoor, 0);
        gb_write_hram(&gb, hMapRoom, ROOM_OW_TURTLE_ROCK_WARP_HOLE);
        gb_write(&gb, wIgnoreLinkCollisionsCountdown, 5);
        gb_write(&gb, wIsUsingSpinAttack, 1);
        gb_write(&gb, wSwordCharge, 10);
        gb_write_hram(&gb, hStaircase, 2);

        LinkMotionFallingDownHandler(&gb);
        assert(gb_read(&gb, wIgnoreLinkCollisionsCountdown) == 0);
        assert(gb_read(&gb, wIsUsingSpinAttack) == 0);
        assert(gb_read(&gb, wSwordCharge) == 0);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_INACTIVE);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_TELEPORT);
        assert(gb_read_hram(&gb, hLinkCountdown) == 0x40);
        assert(gb_read_hram(&gb, hLinkPhysicsModifier) == 0x00);
        assert(gb_read_hram(&gb, hLinkAnimationState) == 0xFF);

        /* 3. Non-pit falling with wWarp0MapCategory == 2 */
        gb_init(&gb);
        gb_write(&gb, wLinkAnimationFrame, 0x5F);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wLinkFallingDownObjectPhysics, 0x00);
        gb_write(&gb, wWarp0MapCategory, 0x02);
        gb_write_hram(&gb, hLinkPositionY, 0x55);

        LinkMotionFallingDownHandler(&gb);
        assert(gb_read(&gb, wWarp0DestinationX) == 0x55);
        assert(gb_read(&gb, wMapEntrancePositionZ) == 0x00);
        assert(gb_read_hram(&gb, hLinkVelocityZ) == 0);
        assert(gb_read(&gb, wIsLinkInTheAir) == 0);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT);

        /* 4. Non-pit falling with wWarp0MapCategory != 2 */
        gb_init(&gb);
        gb_write(&gb, wLinkAnimationFrame, 0x5F);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wLinkFallingDownObjectPhysics, 0x00);
        gb_write(&gb, wWarp0MapCategory, 0x01);
        gb_write_hram(&gb, hLinkPositionX, 0x42);
        gb_write_hram(&gb, hLinkPositionY, 0x36);

        LinkMotionFallingDownHandler(&gb);
        assert(gb_read(&gb, wWarp0DestinationX) == 0x48);
        assert(gb_read(&gb, wWarp0DestinationY) == 0x30);
        assert(gb_read(&gb, wD475) == 0x01);
        assert(gb_read(&gb, wMapEntrancePositionZ) == 0x70);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT);

        /* 5. Tractor device physics triggers fade out */
        gb_init(&gb);
        gb_write(&gb, wLinkAnimationFrame, 0x5F);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wLinkFallingDownObjectPhysics, OBJ_PHYSICS_TRACTOR_DEVICE);

        LinkMotionFallingDownHandler(&gb);
        assert(gb_read(&gb, wD475) == 0x01);
        assert(gb_read(&gb, wMapEntrancePositionZ) == 0x70);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT);

        /* 6. Overworld pit in UNKNOWN_ROOM_1E */
        gb_init(&gb);
        gb_write(&gb, wLinkAnimationFrame, 0x5F);
        gb_write(&gb, wIsIndoor, 0);
        gb_write(&gb, wLinkFallingDownObjectPhysics, OBJ_PHYSICS_PIT);
        gb_write_hram(&gb, hMapRoom, UNKNOWN_ROOM_1E);

        LinkMotionFallingDownHandler(&gb);
        assert(gb_read(&gb, wD475) == 0x01);
        assert(gb_read(&gb, wMapEntrancePositionZ) == 0x70);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT);

        /* 7. Mountain cave pit -> Waterfall warp */
        gb_init(&gb);
        gb_write(&gb, wLinkAnimationFrame, 0x5F);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wLinkFallingDownObjectPhysics, OBJ_PHYSICS_PIT);
        gb_write_hram(&gb, hMapId, MAP_CAVE_B);
        gb_write_hram(&gb, hMapRoom, ROOM_INDOOR_B_MOUNTAIN_CAVE_ROOM_2);

        LinkMotionFallingDownHandler(&gb);
        assert(gb_read(&gb, wWarp0MapCategory) == 0x00);
        assert(gb_read(&gb, wWarp0Map) == 0x00);
        assert(gb_read(&gb, wWarp0Room) == 0x1A);
        assert(gb_read(&gb, wWarp0DestinationX) == 0x68);
        assert(gb_read(&gb, wWarp0DestinationY) == 0x56);
        assert(gb_read(&gb, wMapEntrancePositionZ) == 0x24);
        assert(gb_read_hram(&gb, hLinkDirection) == DIRECTION_DOWN);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT);

        /* 8. Standard pit damage and entry respawn (jr_002_51AC) */
        gb_init(&gb);
        gb_write(&gb, wLinkAnimationFrame, 0x5F);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wLinkFallingDownObjectPhysics, OBJ_PHYSICS_PIT);
        gb_write_hram(&gb, hMapId, 0x01); /* not MAP_CAVE_B */
        gb_write(&gb, wSubtractHealthBuffer, 0x08);
        gb_write(&gb, wLinkMapEntryPositionX, 0x30);
        gb_write(&gb, wLinkMapEntryPositionY, 0x40);

        LinkMotionFallingDownHandler(&gb);
        assert(gb_read(&gb, wInvincibilityCounter) == 0x40);
        assert(gb_read_hram(&gb, hLinkPositionX) == 0x30);
        assert(gb_read_hram(&gb, hLinkPositionY) == 0x40);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_DEFAULT);
        assert(gb_read(&gb, wSubtractHealthBuffer) == 0x0C);
        assert(gb_read(&gb, wC167) == 0x00);
    }

}

void test_bank2_recovery(void) {
    /* Test 57: LinkMotionRecoverHandler (02:5267) */
    {
        GBState gb;

        /* 1. Countdown > 0x40 -> HOLD_SWIMMING_1_DOWN without sound */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkCountdown, 0x50);
        gb_write_hram(&gb, hWaveSfx, 0);

        LinkMotionRecoverHandler(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == LINK_ANIMATION_STATE_HOLD_SWIMMING_1_DOWN);
        assert(gb_read_hram(&gb, hWaveSfx) == 0);

        /* 2. Countdown == 0x40 -> HOLD_SWIMMING_1_DOWN with spin attack sfx */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkCountdown, 0x40);
        gb_write_hram(&gb, hWaveSfx, 0);

        LinkMotionRecoverHandler(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == LINK_ANIMATION_STATE_HOLD_SWIMMING_1_DOWN);
        assert(gb_read_hram(&gb, hWaveSfx) == NOISE_SFX_SPIN_ATTACK);

        /* 3. 0x30 <= Countdown < 0x40 -> HOLD_SWIMMING_2 */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkCountdown, 0x35);

        LinkMotionRecoverHandler(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == LINK_ANIMATION_STATE_HOLD_SWIMMING_2);

        /* 4. Countdown < 0x30 -> HIDDEN */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkCountdown, 0x25);

        LinkMotionRecoverHandler(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == LINK_ANIMATION_STATE_HIDDEN);

        /* 5. Countdown == 0, physics modifier 0x06 -> damage buffer increment */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkCountdown, 0x00);
        gb_write_hram(&gb, hLinkPhysicsModifier, 0x06);
        gb_write(&gb, wSubtractHealthBuffer, 0x02);
        gb_write(&gb, wLinkMapEntryPositionX, 0x50);
        gb_write(&gb, wLinkMapEntryPositionY, 0x60);
        gb_write(&gb, wIsIndoor, 1);

        LinkMotionRecoverHandler(&gb);
        assert(gb_read(&gb, wC167) == 0);
        assert(gb_read(&gb, wSubtractHealthBuffer) == 0x06);
        assert(gb_read_hram(&gb, hLinkPhysicsModifier) == 0);
        assert(gb_read(&gb, wInvincibilityCounter) == 0x40);
        assert(gb_read_hram(&gb, hLinkPositionX) == 0x50);
        assert(gb_read_hram(&gb, hLinkPositionY) == 0x60);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_DEFAULT);

        /* 6. Countdown == 0 on Angler's Tunnel Entrance room overrides entry position */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkCountdown, 0x00);
        gb_write_hram(&gb, hLinkPhysicsModifier, 0x00);
        gb_write(&gb, wIsIndoor, 0);
        gb_write_hram(&gb, hMapRoom, ROOM_OW_ANGLERS_TUNNEL_ENTRANCE);
        gb_write(&gb, wLinkMapEntryPositionX, 0x10);
        gb_write(&gb, wLinkMapEntryPositionY, 0x20);

        LinkMotionRecoverHandler(&gb);
        assert(gb_read(&gb, wLinkMapEntryPositionX) == 0x48);
        assert(gb_read(&gb, wLinkMapEntryPositionY) == 0x30);
        assert(gb_read_hram(&gb, hLinkPositionX) == 0x48);
        assert(gb_read_hram(&gb, hLinkPositionY) == 0x30);
    }

}
