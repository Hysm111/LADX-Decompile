#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/link_animation.h"
#include "bank2/link_motion.h"
#include "bank2/ocarina.h"
#include "constants/directions.h"
#include "constants/hardware.h"
#include "constants/link.h"
#include "constants/memory.h"

#include <assert.h>
#include <stdint.h>

void test_bank2_link_animation(void) {
    /* Test 35: Walking, animation and data tables */
    {
        assert(sizeof(HorizontalIncrementForLinkPosition) == 32);
        assert(sizeof(VerticalIncrementForLinkPosition) == 32);
        assert(sizeof(JoypadToLinkDirection) == 11);
        assert(sizeof(LinkAnimationsList_WalkingNoShield) == 8);
        assert(sizeof(LinkAnimationsList_WalkCarryingDefaultShield) == 8);
        assert(sizeof(LinkAnimationsList_WalkUsingDefaultShield) == 8);
        assert(sizeof(LinkAnimationsList_WalkCarryingMirrorShield) == 8);
        assert(sizeof(LinkAnimationsList_WalkUsingMirrorShield) == 8);
        assert(sizeof(LinkAnimationsList_PushingObject) == 8);
        assert(sizeof(LinkAnimationsList_LiftingObject) == 8);
        assert(sizeof(Data_002_4948) == 8);
        assert(sizeof(Data_002_4950) == 8);
        assert(sizeof(LinkAnimationsList_WalkSideScrolling) == 8);
        assert(sizeof(Data_002_49CA) == 72);
        assert(sizeof(Data_002_4A12) == 2);
        assert(sizeof(Data_002_4A14) == 2);

        assert(HorizontalIncrementForLinkPosition[1] == 0x10);
        assert(HorizontalIncrementForLinkPosition[2] == (int8_t)0xF0);
        assert(VerticalIncrementForLinkPosition[4] == (int8_t)0xF0);
        assert(VerticalIncrementForLinkPosition[8] == 0x10);

        assert(JoypadToLinkDirection[0] == DIRECTION_KEEP);
        assert(JoypadToLinkDirection[1] == DIRECTION_RIGHT);
        assert(JoypadToLinkDirection[2] == DIRECTION_LEFT);
        assert(JoypadToLinkDirection[4] == DIRECTION_UP);
        assert(JoypadToLinkDirection[8] == DIRECTION_DOWN);

        assert(LinkAnimationsList_WalkingNoShield[0] == LINK_ANIMATION_STATE_STANDING_RIGHT);
        assert(LinkAnimationsList_WalkingNoShield[1] == LINK_ANIMATION_STATE_WALKING_RIGHT);
        assert(Data_002_4A12[0] == 0x08);
        assert(Data_002_4A12[1] == (int8_t)0xF8);
        assert(Data_002_4A14[0] == 0x06);
        assert(Data_002_4A14[1] == 0x01);
    }

    /* Test 36: LinkMotionUnstuckingHandler */
    {
        GBState gb;
        gb_init(&gb);

        /* Case 1: Physics modifier == 0 -> executes unstick loop and sets modifier to 1 */
        gb_write_hram(&gb, hLinkPhysicsModifier, 0x00);
        gb_write_hram(&gb, hLinkPositionY, 0x40);
        gb_write_hram(&gb, hLinkPositionZ, 0x00);
        /* Collision clears when hObjectUnderEntity == 0x61 */
        gb_write_hram(&gb, hObjectUnderEntity, 0x61);
        gb_write(&gb, wCollisionType, 0x00);
        g_mock_bg_collision_calls = 0;

        LinkMotionUnstuckingHandler(&gb, mock_bg_collision);

        assert(gb_read(&gb, wC1C4) == 0x02);
        assert(gb_read_hram(&gb, hLinkPhysicsModifier) == 0x01);
        assert(g_mock_bg_collision_calls == 1);
        /* Initial +0x10, then loop +0x08, then .jr_49A0 -0x03 -> 0x40 + 0x10 + 0x08 - 3 = 0x55 */
        assert(gb_read_hram(&gb, hLinkPositionY) == 0x55);
        assert(gb_read(&gb, wIsLinkInTheAir) == 1);

        /* Case 2: Physics modifier != 0 -> skips initial loop and continues to motion */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkPhysicsModifier, 0x01);
        gb_write_hram(&gb, hLinkPositionZ, 0x00);
        g_mock_bg_collision_calls = 0;

        LinkMotionUnstuckingHandler(&gb, mock_bg_collision);
        assert(gb_read(&gb, wC1C4) == 0x02);
        assert(g_mock_bg_collision_calls == 0);
        assert(gb_read(&gb, wLinkMotionState) == 0x00);
        assert(gb_read(&gb, wIsLinkInTheAir) == 1);
    }

}
