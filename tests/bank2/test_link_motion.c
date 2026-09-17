#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/link_animation.h"
#include "bank2/link_motion.h"
#include "bank2/vfx.h"
#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/inventory.h"
#include "constants/joypad.h"
#include "constants/link.h"
#include "constants/memory.h"
#include "constants/physics.h"
#include "constants/sfx.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void test_bank2_link_motion(void) {
    /* Test 17: DirectionToLinkAnimationState, HorizontalIncrementForLinkPosition,
     * VerticalIncrementForLinkPosition, JoypadToLinkDirection Tables */
    {
        assert(DirectionToLinkAnimationState[DIRECTION_RIGHT] == LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_RIGHT);
        assert(DirectionToLinkAnimationState[DIRECTION_LEFT]  == LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_LEFT);
        assert(DirectionToLinkAnimationState[DIRECTION_UP]    == LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_UP);
        assert(DirectionToLinkAnimationState[DIRECTION_DOWN]  == LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_DOWN);

        assert(sizeof(HorizontalIncrementForLinkPosition) == 32);
        assert(sizeof(VerticalIncrementForLinkPosition) == 32);
        assert(sizeof(JoypadToLinkDirection) == 11);

        assert(JoypadToLinkDirection[0] == DIRECTION_KEEP);
        assert(JoypadToLinkDirection[1] == DIRECTION_RIGHT);
        assert(JoypadToLinkDirection[2] == DIRECTION_LEFT);
        assert(JoypadToLinkDirection[3] == DIRECTION_KEEP);
        assert(JoypadToLinkDirection[4] == DIRECTION_UP);
        assert(JoypadToLinkDirection[8] == DIRECTION_DOWN);
        assert(JoypadToLinkDirection[5] == DIRECTION_KEEP);
    }

    /* Test 18: func_002_4338 - Lifted Object State */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hLinkAnimationState, 0);
        gb_write_hram(&gb, hLinkInteractiveMotionBlocked, 0);

        /* a < 2: no effect */
        gb_write(&gb, wIsCarryingLiftedObject, 0);
        func_002_4338(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == 0);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 0);

        gb_write(&gb, wIsCarryingLiftedObject, 1);
        func_002_4338(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == 0);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 0);

        /* a >= 2: animation state updated and motion blocked */
        gb_write(&gb, wIsCarryingLiftedObject, 2);
        func_002_4338(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == 2);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 1);

        gb_write(&gb, wIsCarryingLiftedObject, 0x38);
        func_002_4338(&gb);
        assert(gb_read_hram(&gb, hLinkAnimationState) == 0x38);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 1);
    }

    /* Test 19: func_002_434A - Attack Step Countdown & Animation State */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hLinkAnimationState, 0);

        /* Countdown mask is zero */
        gb_write(&gb, wLinkAttackStepAnimationCountdown, 0x80);
        func_002_434A(&gb);
        assert(gb_read(&gb, wLinkAttackStepAnimationCountdown) == 0);
        assert(gb_read_hram(&gb, hLinkAnimationState) == 0);

        /* Countdown > 0: decrements and updates state based on direction */
        const uint8_t dirs[4] = { DIRECTION_RIGHT, DIRECTION_LEFT, DIRECTION_UP, DIRECTION_DOWN };
        const uint8_t exp_anim[4] = {
            LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_RIGHT,
            LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_LEFT,
            LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_UP,
            LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_DOWN
        };

        for (int d = 0; d < 4; d++) {
            gb_write(&gb, wLinkAttackStepAnimationCountdown, 0x15);
            gb_write_hram(&gb, hLinkDirection, dirs[d]);
            func_002_434A(&gb);
            assert(gb_read(&gb, wLinkAttackStepAnimationCountdown) == 0x14);
            assert(gb_read_hram(&gb, hLinkAnimationState) == exp_anim[d]);
        }
    }

    /* Test 20: MoveLinkToPressedButtonDirection - Normal & Piece of Power */
    {
        GBState gb;
        gb_init(&gb);

        /* Normal: Right (1) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x01);
        MoveLinkToPressedButtonDirection(&gb, 0);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedX) == 0x10);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Normal: Left (2) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x02);
        MoveLinkToPressedButtonDirection(&gb, 0);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedX) == (int8_t)0xF0);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Normal: Up + Right (5) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x05);
        MoveLinkToPressedButtonDirection(&gb, 0);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedX) == 0x0C);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedY) == (int8_t)0xF4);

        /* Piece of Power (offset 0x10): Right (1) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x01);
        MoveLinkToPressedButtonDirection(&gb, 0x10);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedX) == 0x14);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Piece of Power: Up + Left (6) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x06);
        MoveLinkToPressedButtonDirection(&gb, 0x10);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedX) == (int8_t)0xF1);
        assert((int8_t)gb_read_hram(&gb, hLinkSpeedY) == (int8_t)0xF1);
    }

    /* Test 21: func_002_438F - Smooth Acceleration / Speed Nudging */
    {
        GBState gb;
        gb_init(&gb);

        /* Target speed for Right (1) is X=0x10, Y=0x00 */
        gb_write_hram(&gb, hPressedButtonsMask, 0x01);

        /* Case 1: current speed X < target X -> increments speed X */
        gb_write_hram(&gb, hLinkSpeedX, 0x0E);
        gb_write_hram(&gb, hLinkSpeedY, 0x00);
        func_002_438F(&gb, 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x0F);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Case 2: current speed X > target X -> decrements speed X */
        gb_write_hram(&gb, hLinkSpeedX, 0x12);
        gb_write_hram(&gb, hLinkSpeedY, 0x00);
        func_002_438F(&gb, 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x11);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Case 3: current speed X == target X -> unmodified */
        gb_write_hram(&gb, hLinkSpeedX, 0x10);
        gb_write_hram(&gb, hLinkSpeedY, 0x00);
        func_002_438F(&gb, 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x10);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Target speed for Up (4) is X=0x00, Y=0xF0 (-16) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x04);

        /* Case 4: current speed Y is 0xF4 (-12, which is > -16) -> decrements towards 0xF0 */
        gb_write_hram(&gb, hLinkSpeedX, 0x00);
        gb_write_hram(&gb, hLinkSpeedY, 0xF4);
        func_002_438F(&gb, 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x00);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0xF3);

        /* Case 5: current speed Y is 0xEC (-20, which is < -16) -> increments towards 0xF0 */
        gb_write_hram(&gb, hLinkSpeedX, 0x00);
        gb_write_hram(&gb, hLinkSpeedY, 0xEC);
        func_002_438F(&gb, 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x00);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0xED);
    }


    /* Test 22: Data_002_44E7 and Data_002_68B1 Lookup Tables */
    {
        assert(sizeof(Data_002_44E7) == 6);
        assert(Data_002_44E7[0] == 0);
        assert(Data_002_44E7[1] == -16);
        assert(Data_002_44E7[2] == 16);
        assert(Data_002_44E7[3] == 0);
        assert(Data_002_44E7[4] == -1);
        assert(Data_002_44E7[5] == 1);

        assert(sizeof(Data_002_68B1) == 3);
        assert(Data_002_68B1[0] == 0);
        assert(Data_002_68B1[1] == 0x10);
        assert(Data_002_68B1[2] == (int8_t)0xF0);
    }

    /* Test 23: shallowWaterVfx */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hLinkPositionY, 0x48);
        gb_write_hram(&gb, hLinkPositionX, 0x5C);
        gb_write_hram(&gb, hJingle, 0);

        shallowWaterVfx(&gb);

        assert(gb_read_hram(&gb, hMultiPurpose1) == 0x48);
        assert(gb_read_hram(&gb, hMultiPurpose0) == 0x5C);
        assert(gb_read_hram(&gb, hJingle) == JINGLE_WATER_SPLASH);
    }

    /* Test 24: label_002_44B5 and func_002_44AD */
    {
        GBState gb;
        gb_init(&gb);

        /* label_002_44B5 */
        gb_write(&gb, wLinkGroundStatus, 0x05);
        gb_write(&gb, wC130, 0x00);
        g_mock_map_transition_calls = 0;

        label_002_44B5(&gb, mock_check_map_transition);
        assert(gb_read(&gb, wC130) == 0x05);
        assert(gb_read(&gb, wLinkGroundStatus) == 0x00);
        assert(g_mock_map_transition_calls == 1);

        /* func_002_44AD: wInventoryAppearing != 0 -> returns early */
        gb_write(&gb, wInventoryAppearing, 1);
        g_mock_map_transition_calls = 0;
        func_002_44AD(&gb, mock_check_map_transition);
        assert(g_mock_map_transition_calls == 0);

        /* func_002_44AD: wInventoryAppearing == 0 -> executes */
        gb_write(&gb, wInventoryAppearing, 0);
        gb_write(&gb, wLinkGroundStatus, 0x09);
        func_002_44AD(&gb, mock_check_map_transition);
        assert(gb_read(&gb, wC130) == 0x09);
        assert(gb_read(&gb, wLinkGroundStatus) == 0x00);
        assert(g_mock_map_transition_calls == 1);
    }

    /* Test 25: func_002_44C2 - Collision Countdown & Collision Handling */
    {
        GBState gb;
        gb_init(&gb);

        /* When countdown == 0, returns false immediately */
        gb_write(&gb, wIgnoreLinkCollisionsCountdown, 0);
        g_mock_map_transition_calls = 0;
        bool handled = func_002_44C2(&gb, mock_check_map_transition);
        assert(!handled);
        assert(g_mock_map_transition_calls == 0);

        /* When countdown > 0 and collision == 0 */
        gb_write(&gb, wIgnoreLinkCollisionsCountdown, 5);
        gb_write(&gb, wCollisionType, 0);
        gb_write_hram(&gb, hLinkSpeedX, 0x10);
        gb_write_hram(&gb, hLinkSpeedY, 0x20);

        handled = func_002_44C2(&gb, mock_check_map_transition);
        assert(handled);
        assert(gb_read(&gb, wIgnoreLinkCollisionsCountdown) == 4);
        assert(g_mock_map_transition_calls == 1);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x10);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0x20);

        /* Collision vertical (collision & 3 != 0) -> clears speed Y */
        gb_write(&gb, wIgnoreLinkCollisionsCountdown, 2);
        gb_write(&gb, wCollisionType, COLLISION_TYPE_UP);
        handled = func_002_44C2(&gb, mock_check_map_transition);
        assert(handled);
        assert(gb_read(&gb, wIgnoreLinkCollisionsCountdown) == 1);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x10);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0x00);

        /* Collision horizontal (collision & 3 == 0) -> clears speed X */
        gb_write(&gb, wIgnoreLinkCollisionsCountdown, 1);
        gb_write(&gb, wCollisionType, COLLISION_TYPE_LEFT);
        gb_write_hram(&gb, hLinkSpeedX, 0x10);
        gb_write_hram(&gb, hLinkSpeedY, 0x20);
        handled = func_002_44C2(&gb, mock_check_map_transition);
        assert(handled);
        assert(gb_read(&gb, wIgnoreLinkCollisionsCountdown) == 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x00);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0x20);
    }

    /* Test 26: ApplyLinkGroundMotion */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Not in air -> returns early */
        gb_write(&gb, wIsLinkInTheAir, 0);
        gb_write_hram(&gb, hLinkVelocityZ, 0x10);
        ApplyLinkGroundMotion(&gb, NULL);
        assert(gb_read_hram(&gb, hLinkVelocityZ) == 0x10);

        /* 2. Side scrolling -> returns early */
        gb_write(&gb, wIsLinkInTheAir, 1);
        gb_write_hram(&gb, hIsSideScrolling, 1);
        ApplyLinkGroundMotion(&gb, NULL);
        assert(gb_read_hram(&gb, hLinkVelocityZ) == 0x10);

        /* 3. In air, overhead, still airborne (pos Z > 0, bit 7 == 0) */
        gb_write_hram(&gb, hIsSideScrolling, 0);
        gb_write_hram(&gb, hLinkPositionZ, 0x08);
        gb_write_hram(&gb, hLinkVelocityZ, 0x04);
        gb_write(&gb, wConsecutiveStepsCount, 0x00);
        ApplyLinkGroundMotion(&gb, NULL);
        assert(gb_read_hram(&gb, hLinkVelocityZ) == 0x02);
        assert(gb_read(&gb, wConsecutiveStepsCount) == 0xFF);
        /* Still in air */
        assert(gb_read(&gb, wIsLinkInTheAir) == 1);

        /* 4. Joypad movement in air: Right pressed (speed nudged towards +16) */
        gb_write_hram(&gb, hPressedButtonsMask, 0x01);
        gb_write_hram(&gb, hLinkSpeedX, 0x0E);
        ApplyLinkGroundMotion(&gb, NULL);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x0F);

        /* 5. Landing on normal ground (pos Z == 0): triggers landing reset and footstep SFX */
        gb_write_hram(&gb, hLinkPositionZ, 0x00);
        gb_write_hram(&gb, hLinkPositionY, 0x50);
        gb_write_hram(&gb, hObjectUnderLink, 0x20);
        gb_write(&gb, wLinkObjectPhysics, OBJ_PHYSICS_NONE);
        gb_write_hram(&gb, hNoiseSfx, 0);
        g_mock_ground_physics_calls = 0;

        ApplyLinkGroundMotion(&gb, mock_apply_ground_physics);
        assert(gb_read_hram(&gb, hLinkPositionZ) == 0);
        assert(gb_read_hram(&gb, hLinkVelocityZ) == 0);
        assert(gb_read(&gb, wIsLinkInTheAir) == 0);
        assert(gb_read(&gb, wC149) == 0);
        assert(gb_read(&gb, wC152) == 0);
        assert(gb_read(&gb, wC153) == 0);
        assert(gb_read(&gb, wC10A) == 0);
        assert(g_mock_ground_physics_calls == 1);
        assert(gb_read_hram(&gb, hNoiseSfx) == NOISE_SFX_FOOTSTEP);

        /* 6. Landing in shallow water: triggers shallowWaterVfx */
        gb_write(&gb, wIsLinkInTheAir, 1);
        gb_write_hram(&gb, hLinkPositionZ, 0x80); /* bit 7 set -> ground reached */
        gb_write_hram(&gb, hLinkPositionY, 0x60);
        gb_write_hram(&gb, hLinkPositionX, 0x30);
        gb_write(&gb, wLinkObjectPhysics, OBJ_PHYSICS_SHALLOW_WATER);
        gb_write_hram(&gb, hJingle, 0);

        ApplyLinkGroundMotion(&gb, NULL);
        assert(gb_read(&gb, wIsLinkInTheAir) == 0);
        assert(gb_read_hram(&gb, hJingle) == JINGLE_WATER_SPLASH);
        assert(gb_read_hram(&gb, hMultiPurpose1) == 0x60);
        assert(gb_read_hram(&gb, hMultiPurpose0) == 0x30);

        /* 7. Landing on water/lava/pit: no footstep SFX */
        const uint8_t no_footstep_physics[] = {
            OBJ_PHYSICS_DEEP_WATER, OBJ_PHYSICS_LAVA, OBJ_PHYSICS_PIT, OBJ_PHYSICS_PIT_WARP
        };
        for (size_t p = 0; p < sizeof(no_footstep_physics); p++) {
            gb_write(&gb, wIsLinkInTheAir, 1);
            gb_write_hram(&gb, hLinkPositionZ, 0x00);
            gb_write(&gb, wLinkObjectPhysics, no_footstep_physics[p]);
            gb_write_hram(&gb, hNoiseSfx, 0);

            ApplyLinkGroundMotion(&gb, NULL);
            assert(gb_read_hram(&gb, hNoiseSfx) == 0);
        }
    }


    /* Test 27: OverheadWalkPhysics */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Sets wD46B from wIndoorRoom */
        gb_write(&gb, wIndoorRoom, 0x55);
        gb_write(&gb, wD46B, 0x00);
        OverheadWalkPhysics(&gb, NULL);
        assert(gb_read(&gb, wD46B) == 0x55);

        /* 2. Interactive motion blocked on ground (Z == 0, blocked != 0) -> calls label_002_44B5 */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkPositionZ, 0);
        gb_write_hram(&gb, hLinkInteractiveMotionBlocked, 1);
        gb_write(&gb, wLinkGroundStatus, 0x03);
        g_mock_map_transition_calls = 0;
        OverheadWalkPhysics(&gb, mock_check_map_transition);
        assert(gb_read(&gb, wC130) == 0x03);
        assert(gb_read(&gb, wLinkGroundStatus) == 0x00);
        assert(g_mock_map_transition_calls == 1);

        /* 3. Pegasus Boots Running: Straight */
        gb_init(&gb);
        gb_write(&gb, wIsRunningWithPegasusBoots, 1);
        gb_write_hram(&gb, hJoypadState, J_RIGHT);
        gb_write_hram(&gb, hLinkDirection, DIRECTION_RIGHT);
        gb_write(&gb, wConsecutiveStepsCount, 10);
        OverheadWalkPhysics(&gb, NULL);
        assert(gb_read(&gb, wConsecutiveStepsCount) == 12);

        /* 4. Pegasus Boots Running: Turning (joypad dir != link dir) */
        gb_init(&gb);
        gb_write(&gb, wIsRunningWithPegasusBoots, 1);
        gb_write_hram(&gb, hJoypadState, J_UP);
        gb_write_hram(&gb, hLinkDirection, DIRECTION_RIGHT);
        gb_write(&gb, wC199, 0x10);
        gb_write(&gb, (uint16_t)(wC199 + 1), 0x00);
        OverheadWalkPhysics(&gb, NULL);
        assert(gb_read(&gb, (uint16_t)(wC199 + 1)) == DIRECTION_RIGHT);
        assert(gb_read(&gb, wC199) == 0x1C);

        /* 5. Normal walking: no buttons pressed -> resets consecutive steps to 7 */
        gb_init(&gb);
        gb_write_hram(&gb, hPressedButtonsMask, 0x00);
        gb_write(&gb, wConsecutiveStepsCount, 0x20);
        OverheadWalkPhysics(&gb, NULL);
        assert(gb_read(&gb, wConsecutiveStepsCount) == 0x07);

        /* 6. Normal walking: button pressed -> increments consecutive steps and sets direction */
        gb_init(&gb);
        gb_write_hram(&gb, hPressedButtonsMask, 0x04); /* UP */
        gb_write_hram(&gb, hLinkDirection, DIRECTION_RIGHT);
        gb_write(&gb, wConsecutiveStepsCount, 0x05);
        gb_write(&gb, wC16E, 0);
        OverheadWalkPhysics(&gb, NULL);
        assert(gb_read(&gb, wConsecutiveStepsCount) == 0x06);
        assert(gb_read_hram(&gb, hLinkDirection) == DIRECTION_UP);

        /* 7. Free movement mode doubles speed */
        gb_init(&gb);
        gb_write(&gb, wFreeMovementMode, 1);
        gb_write_hram(&gb, hPressedButtonsMask, 0x01); /* RIGHT: base speed X is 0x10 */
        OverheadWalkPhysics(&gb, NULL);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0x20);

        /* 8. Gel clinging throttles movement to 1 in 4 frames */
        gb_init(&gb);
        gb_write(&gb, wIsGelClingingToLink, 1);
        gb_write(&gb, wLinkGroundStatus, 0x04);

        /* Frame 1: 1 & 3 != 0 -> throttled, executes label_002_44B5 */
        gb_write_hram(&gb, hFrameCounter, 1);
        g_mock_map_transition_calls = 0;
        OverheadWalkPhysics(&gb, mock_check_map_transition);
        assert(gb_read(&gb, wC130) == 0x04);
        assert(g_mock_map_transition_calls == 1);

        /* Frame 4: 4 & 3 == 0 -> executes func_002_44AD */
        gb_write(&gb, wLinkGroundStatus, 0x06);
        gb_write_hram(&gb, hFrameCounter, 4);
        g_mock_map_transition_calls = 0;
        OverheadWalkPhysics(&gb, mock_check_map_transition);
        assert(gb_read(&gb, wC130) == 0x06);
        assert(g_mock_map_transition_calls == 1);
    }

    /* Test 28: func_002_436C */
    {
        GBState gb;
        gb_init(&gb);

        /* Not side scrolling -> overhead walk physics */
        gb_write_hram(&gb, hIsSideScrolling, 0);
        gb_write(&gb, wIndoorRoom, 0x77);
        g_mock_side_scrolling_calls = 0;
        func_002_436C(&gb, mock_side_scrolling_physics, NULL);
        assert(g_mock_side_scrolling_calls == 0);
        assert(gb_read(&gb, wD46B) == 0x77);

        /* Side scrolling and normal movement -> calls side scrolling callback */
        gb_write_hram(&gb, hIsSideScrolling, 1);
        gb_write(&gb, wFreeMovementMode, 0);
        func_002_436C(&gb, mock_side_scrolling_physics, NULL);
        assert(g_mock_side_scrolling_calls == 1);

        /* Side scrolling with free movement -> uses overhead walk physics */
        gb_write(&gb, wFreeMovementMode, 1);
        gb_write(&gb, wIndoorRoom, 0x88);
        func_002_436C(&gb, mock_side_scrolling_physics, NULL);
        assert(g_mock_side_scrolling_calls == 1); /* no new call */
        assert(gb_read(&gb, wD46B) == 0x88);
    }

    /* Test 29: LinkMotionDefault */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Decrements countdown timers */
        gb_write(&gb, wIsShootingArrow, 3);
        gb_write(&gb, wC1C4, 5);
        gb_write(&gb, wBombArrowCooldown, 2);
        gb_write(&gb, wC16E, 4);

        LinkMotionDefault(&gb, NULL, NULL, NULL, NULL, mock_update_link_animation, mock_func_002_4b49, NULL);

        assert(gb_read(&gb, wIsShootingArrow) == 2);
        assert(gb_read(&gb, wC1C4) == 4);
        assert(gb_read(&gb, wBombArrowCooldown) == 1);
        assert(gb_read(&gb, wC16E) == 3);

        /* 2. Motion blocked == 2 branch */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkInteractiveMotionBlocked, 2);
        gb_write_hram(&gb, hLinkSpeedX, 0x10);
        gb_write_hram(&gb, hLinkSpeedY, 0x20);
        gb_write_hram(&gb, hLinkVelocityZ, 0x05);
        g_mock_ocarina_calls = 0;
        g_mock_753a_calls = 0;

        LinkMotionDefault(&gb, NULL, NULL, mock_ocarina_handler, mock_func_002_753a, NULL, NULL, NULL);

        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 0);
        assert(gb_read_hram(&gb, hLinkSpeedX) == 0);
        assert(gb_read_hram(&gb, hLinkSpeedY) == 0);
        assert(gb_read_hram(&gb, hLinkVelocityZ) == 0);
        assert(g_mock_ocarina_calls == 1);
        assert(g_mock_753a_calls == 1);

        /* 3. Sword Charging */
        gb_init(&gb);
        gb_write(&gb, wSwordAnimationState, SWORD_ANIMATION_STATE_HOLDING);
        gb_write(&gb, wSwordCharge, 0x26);
        gb_write_hram(&gb, hJingle, 0);

        LinkMotionDefault(&gb, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        assert(gb_read(&gb, wSwordCharge) == 0x27);
        assert(gb_read_hram(&gb, hJingle) == 0);

        /* Reaching max charge triggers charging jingle */
        gb_write(&gb, wSwordAnimationState, SWORD_ANIMATION_STATE_HOLDING);
        LinkMotionDefault(&gb, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        assert(gb_read(&gb, wSwordCharge) == MAX_SWORD_CHARGE);
        assert(gb_read_hram(&gb, hJingle) == JINGLE_CHARGING_SWORD);

        /* 4. Releasing sword at max charge triggers spin attack (wC16E == 0) */
        gb_write(&gb, wSwordAnimationState, 0);
        gb_write(&gb, wC16E, 0);
        gb_write(&gb, wIsUsingSpinAttack, 0);
        gb_write_hram(&gb, hNoiseSfx, 0);

        LinkMotionDefault(&gb, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        assert(gb_read(&gb, wIsUsingSpinAttack) == USING_SPIN_ATTACK_MAX);
        assert(gb_read_hram(&gb, hNoiseSfx) == NOISE_SFX_SPIN_ATTACK);
        assert(gb_read(&gb, wSwordCharge) == 0);

        /* 5. Releasing sword near NPC does not trigger spin attack */
        gb_write(&gb, wSwordCharge, MAX_SWORD_CHARGE);
        gb_write(&gb, wItemUsageContext, ITEM_USAGE_NEAR_NPC);
        gb_write(&gb, wIsUsingSpinAttack, 0);
        gb_write_hram(&gb, hNoiseSfx, 0);

        LinkMotionDefault(&gb, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        assert(gb_read(&gb, wIsUsingSpinAttack) == 0);
        assert(gb_read_hram(&gb, hNoiseSfx) == 0);
        assert(gb_read(&gb, wSwordCharge) == 0);
    }


}
