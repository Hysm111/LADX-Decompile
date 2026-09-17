#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/link_animation.h"
#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/inventory.h"
#include "constants/memory.h"

#include <assert.h>
#include <stddef.h>

void test_bank2_sword(void) {
    /* Test 30: Sword tables size & validation */
    {
        assert(sizeof(LinkDirectionToStaticSwordCollitionCheckNeeded) == 24);
        assert(sizeof(LinkDirectionTo_wC141) == 24);
        assert(sizeof(LinkDirectionToOffset) == 24);
        assert(sizeof(LinkDirectionTo_wC143) == 24);
        assert(sizeof(LinkDirectionToSwordDirection) == 24);
        assert(sizeof(LinkDirectionToLinkAnimationState1) == 24);
        assert(sizeof(LinkDirectionTo_wC13A) == 24);
        assert(sizeof(LinkDirectionTo_wC139) == 24);
        assert(sizeof(LinkDirectionTo_wC13C) == 24);
        assert(sizeof(LinkDirectionTo_wC13B) == 24);
        assert(sizeof(SwordAnimationStateToUnknow) == 8);
        assert(sizeof(UnkownToLinkStateTable) == 16);
        assert(sizeof(FrameCounterToLinkDirection) == 4);
        assert(sizeof(LinkDirectionToSwordAnimationState) == 32);
        assert(sizeof(LinkDirectionToAbsolute) == 32);

        assert(FrameCounterToLinkDirection[0] == DIRECTION_RIGHT);
        assert(FrameCounterToLinkDirection[1] == DIRECTION_DOWN);
        assert(FrameCounterToLinkDirection[2] == DIRECTION_LEFT);
        assert(FrameCounterToLinkDirection[3] == DIRECTION_UP);
    }

    /* Test 31: label_002_48B0 (sword reset) */
    {
        GBState gb;
        gb_init(&gb);

        gb_write(&gb, wC1AC, 5);
        gb_write(&gb, wSwordAnimationState, SWORD_ANIMATION_STATE_HOLDING);
        gb_write(&gb, wC16A, 2);
        gb_write(&gb, wIsUsingSpinAttack, 10);
        gb_write(&gb, wIsRunningWithPegasusBoots, 0);

        label_002_48B0(&gb);
        assert(gb_read(&gb, wC1AC) == 0);
        assert(gb_read(&gb, wSwordAnimationState) == SWORD_ANIMATION_STATE_NONE);
        assert(gb_read(&gb, wC16A) == 0);
        assert(gb_read(&gb, wIsUsingSpinAttack) == 0);

        /* Pegasus boots preserves sword animation */
        gb_write(&gb, wC1AC, 9);
        gb_write(&gb, wSwordAnimationState, SWORD_ANIMATION_STATE_HOLDING);
        gb_write(&gb, wIsRunningWithPegasusBoots, 1);

        label_002_48B0(&gb);
        assert(gb_read(&gb, wC1AC) == 0);
        assert(gb_read(&gb, wSwordAnimationState) == SWORD_ANIMATION_STATE_HOLDING);
    }

    /* Test 32: label_002_4827 (sword collision box & direction) */
    {
        GBState gb;
        gb_init(&gb);

        /* Direction RIGHT (0), animation SWING_START (1): bc = 1 */
        gb_write_hram(&gb, hLinkDirection, DIRECTION_RIGHT);
        gb_write(&gb, wSwordAnimationState, SWORD_ANIMATION_STATE_SWING_START);
        gb_write_hram(&gb, hLinkPositionX, 0x40);
        gb_write(&gb, wC145, 0x50);
        g_mock_sword_collision_calls = 0;

        label_002_4827(&gb, mock_check_collision);
        assert(g_mock_sword_collision_calls == 1);
        assert(gb_read(&gb, wSwordDirection) == SWORD_DIRECTION_TOP);
        /* bc = 1 -> LinkDirectionToStaticSwordCollitionCheckNeeded[1] == 0 -> collision not enabled */
        assert(gb_read(&gb, wSwordCollisionEnabled) == 0);

        /* Direction RIGHT (0), animation SWING_MIDDLE (3): bc = 3 */
        /* CheckNeeded = 6, LinkDirectionTo_wC141 = 0x0A, Offset = 8, wC143 = 8 */
        gb_write(&gb, wSwordAnimationState, SWORD_ANIMATION_STATE_SWING_MIDDLE);
        label_002_4827(&gb, mock_check_collision);
        assert(g_mock_sword_collision_calls == 2);
        assert(gb_read(&gb, wSwordDirection) == SWORD_DIRECTION_RIGHT);
        assert(gb_read(&gb, wSwordCollisionEnabled) == 1);
        /* wC140 = wC13A (3) + CheckNeeded (bash6) + posX (0) = 9 */
        assert(gb_read(&gb, wC140) == 0x59);
        assert(gb_read(&gb, wC141) == 0x0A);
        /* wC142 = wC139 (bash0) + Offset (bash8) + wC145 (0) = 0x58 */
        assert(gb_read(&gb, wC142) == 0x58);
        assert(gb_read(&gb, wC143) == 0x08);

        /* With shield and odd frame counter: skips sword collision box */
        gb_write(&gb, wIsUsingShield, 1);
        gb_write_hram(&gb, hFrameCounter, 1);
        gb_write(&gb, wSwordCollisionEnabled, 0);
        label_002_4827(&gb, mock_check_collision);
        assert(gb_read(&gb, wSwordCollisionEnabled) == 0);
    }

    /* Test 33: label_002_476B & UpdateSpinAttackAnimation */
    {
        GBState gb;
        gb_init(&gb);

        /* label_002_476B decrements wC16D */
        gb_write(&gb, wC16D, 2);
        gb_write(&gb, wIsRunningWithPegasusBoots, 0);
        g_mock_sword_collision_calls = 0;
        label_002_476B(&gb, mock_check_collision);
        assert(gb_read(&gb, wC16D) == 1);
        assert(gb_read(&gb, wC16E) == 4);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 1);
        assert(gb_read(&gb, wSwordAnimationState) == SWORD_ANIMATION_STATE_SWING_MIDDLE);
        assert(g_mock_sword_collision_calls == 1);

        /* Decrement to 0 terminates via label_002_48B0 */
        label_002_476B(&gb, mock_check_collision);
        assert(gb_read(&gb, wC16D) == 0);
        assert(gb_read(&gb, wSwordAnimationState) == SWORD_ANIMATION_STATE_NONE);

        /* UpdateSpinAttackAnimation */
        gb_init(&gb);
        gb_write(&gb, wIsUsingSpinAttack, 0x20);
        gb_write_hram(&gb, hLinkDirection, DIRECTION_RIGHT);
        gb_write_hram(&gb, hLinkPositionX, 0x30);
        gb_write(&gb, wC145, 0x40);
        gb_write_hram(&gb, hFrameCounter, 1); /* odd frame -> no dec [hl] */
        g_mock_sword_collision_calls = 0;

        UpdateSpinAttackAnimation(&gb, mock_check_collision);
        assert(g_mock_sword_collision_calls == 1);
        assert(gb_read(&gb, wIsUsingSpinAttack) == 0x1F);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 1);
        assert(gb_read(&gb, wC140) == 0x38);
        assert(gb_read(&gb, wC141) == 0x18);
        assert(gb_read(&gb, wC143) == 0x18);
        assert(gb_read(&gb, wC142) == 0x48);
        assert(gb_read(&gb, wSwordCollisionEnabled) == 0x48);
    }

    /* Test 34: UpdateLinkAnimation */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Whirlpool rotation when wD475 != 0 */
        gb_write(&gb, wD475, 1);
        gb_write_hram(&gb, hFrameCounter, 0 << 2);
        UpdateLinkAnimation(&gb, NULL);
        assert(gb_read_hram(&gb, hLinkDirection) == DIRECTION_RIGHT);

        gb_write_hram(&gb, hFrameCounter, 1 << 2);
        UpdateLinkAnimation(&gb, NULL);
        assert(gb_read_hram(&gb, hLinkDirection) == DIRECTION_DOWN);

        gb_write_hram(&gb, hFrameCounter, 2 << 2);
        UpdateLinkAnimation(&gb, NULL);
        assert(gb_read_hram(&gb, hLinkDirection) == DIRECTION_LEFT);

        gb_write_hram(&gb, hFrameCounter, 3 << 2);
        UpdateLinkAnimation(&gb, NULL);
        assert(gb_read_hram(&gb, hLinkDirection) == DIRECTION_UP);

        /* 2. Airborne jumping animation */
        gb_init(&gb);
        gb_write(&gb, wIsLinkInTheAir, 1);
        gb_write_hram(&gb, hLinkDirection, DIRECTION_DOWN); /* bc = (2 << 2) & 0x0C = 0x08 */
        gb_write(&gb, wC152, 0);
        gb_write(&gb, wC153, 0);

        UpdateLinkAnimation(&gb, NULL);
        assert(gb_read_hram(&gb, hLinkAnimationState) == UnkownToLinkStateTable[12]);
        assert(gb_read(&gb, wC153) == 1);

        /* 3. Sword swinging progression */
        gb_init(&gb);
        gb_write(&gb, wSwordAnimationState, SWORD_ANIMATION_STATE_SWING_START);
        gb_write(&gb, wC138, 0);
        g_mock_sword_collision_calls = 0;

        UpdateLinkAnimation(&gb, mock_check_collision);
        assert(gb_read(&gb, wSwordAnimationState) == 2);
        assert(gb_read(&gb, wC138) == 2);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 1);
        assert(g_mock_sword_collision_calls == 1);
    }

}
