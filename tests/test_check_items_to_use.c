#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gb.h"
#include "home/check_items_to_use.h"
#include "constants/inventory.h"
#include "constants/hardware.h"
#include "constants/joypad.h"
#include "constants/memory.h"
#include "constants/sfx.h"

static int failures = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("  [FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
            failures++; \
        } \
    } while (0)

static int mock_sync_shield_calls = 0;
static void mock_sync_shield(GBState *gb) {
    (void)gb;
    mock_sync_shield_calls++;
}

static void test_set_shield_vals(void) {
    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wShieldLevel, 0x02);
    mock_sync_shield_calls = 0;

    SetShieldVals(&gb, mock_sync_shield);
    TEST_ASSERT(gb_read(&gb, wIsUsingShield) == 0x01, "wIsUsingShield not set to 1");
    TEST_ASSERT(gb_read(&gb, wHasMirrorShield) == 0x02, "wHasMirrorShield not set to wShieldLevel");
    TEST_ASSERT(mock_sync_shield_calls == 1, "sync_shield trampoline not called");
}

static void test_hold_sword_if_needed(void) {
    GBState gb;

    /* 1. Item not sword: nothing happens */
    gb_init(&gb);
    HoldSwordIfNeeded(&gb, INVENTORY_SHIELD);
    TEST_ASSERT(gb_read(&gb, wSwordAnimationState) == 0, "Sword state modified for shield");

    /* 2. Item is sword, but near NPC */
    gb_init(&gb);
    gb_write(&gb, wItemUsageContext, ITEM_USAGE_NEAR_NPC);
    HoldSwordIfNeeded(&gb, INVENTORY_SWORD);
    TEST_ASSERT(gb_read(&gb, wSwordAnimationState) == 0, "Sword state modified near NPC");

    /* 3. Item is sword, reading text */
    gb_init(&gb);
    gb_write(&gb, wItemUsageContext, ITEM_USAGE_READING_TEXT);
    HoldSwordIfNeeded(&gb, INVENTORY_SWORD);
    TEST_ASSERT(gb_read(&gb, wSwordAnimationState) == 0, "Sword state modified reading text");

    /* 4. Item is sword, wC160 != 0 */
    gb_init(&gb);
    gb_write(&gb, wC160, 0x01);
    HoldSwordIfNeeded(&gb, INVENTORY_SWORD);
    TEST_ASSERT(gb_read(&gb, wSwordAnimationState) == 0, "Sword state modified when wC160 != 0");

    /* 5. Item is sword, normal conditions -> hold sword */
    gb_init(&gb);
    gb_write(&gb, wC1AC, 0x12);
    HoldSwordIfNeeded(&gb, INVENTORY_SWORD);
    TEST_ASSERT(gb_read(&gb, wC1AC) == 0x00, "wC1AC not cleared");
    TEST_ASSERT(gb_read(&gb, wSwordAnimationState) == SWORD_ANIMATION_STATE_HOLDING, "wSwordAnimationState not HOLDING");
    TEST_ASSERT(gb_read(&gb, wSwordCollisionEnabled) == SWORD_ANIMATION_STATE_HOLDING, "wSwordCollisionEnabled not HOLDING");
}

static void test_use_shield(void) {
    GBState gb;

    /* Pushing: do not draw shield */
    gb_init(&gb);
    gb_write(&gb, wIsLinkPushing, 0x01);
    UseShield(&gb);
    TEST_ASSERT(gb_read(&gb, hNoiseSfx) == 0, "Shield SFX triggered while pushing");

    /* Not pushing: draw shield */
    gb_init(&gb);
    UseShield(&gb);
    TEST_ASSERT(gb_read(&gb, hNoiseSfx) == NOISE_SFX_DRAW_SHIELD, "Shield SFX not triggered");
}

static bool mock_poking_flag = false;
static bool mock_check_poking(GBState *gb) {
    (void)gb;
    return mock_poking_flag;
}

static void test_use_shovel(void) {
    GBState gb;

    /* Already using shovel: ignored */
    gb_init(&gb);
    gb_write(&gb, wLinkUsingShovel, 0x01);
    UseShovel(&gb, mock_check_poking);
    TEST_ASSERT(gb_read(&gb, hNoiseSfx) == 0, "Shovel SFX played while already using");

    /* Link in air: ignored */
    gb_init(&gb);
    gb_write(&gb, wIsLinkInTheAir, 0x01);
    UseShovel(&gb, mock_check_poking);
    TEST_ASSERT(gb_read(&gb, hNoiseSfx) == 0, "Shovel SFX played while in air");

    /* Normal shovel digging (not poking) */
    gb_init(&gb);
    mock_poking_flag = false;
    UseShovel(&gb, mock_check_poking);
    TEST_ASSERT(gb_read(&gb, hNoiseSfx) == NOISE_SFX_SHOVEL_DIG, "Digging SFX not set");
    TEST_ASSERT(gb_read(&gb, wLinkUsingShovel) == 0x01, "wLinkUsingShovel not set");
    TEST_ASSERT(gb_read(&gb, wLinkUsingShovelTimer) == 0x00, "wLinkUsingShovelTimer not reset");

    /* Shovel poking solid wall */
    gb_init(&gb);
    mock_poking_flag = true;
    UseShovel(&gb, mock_check_poking);
    TEST_ASSERT(gb_read(&gb, hJingle) == JINGLE_SWORD_POKING, "Poking jingle not set");
}

static int mock_hookshot_calls = 0;
static void mock_fire_hookshot(GBState *gb) {
    (void)gb;
    mock_hookshot_calls++;
}

static void test_use_hookshot(void) {
    GBState gb;

    /* Already using hookshot */
    gb_init(&gb);
    gb_write(&gb, wIsUsingHookshot, 0x01);
    mock_hookshot_calls = 0;
    UseHookshot(&gb, mock_fire_hookshot);
    TEST_ASSERT(mock_hookshot_calls == 0, "Hookshot fired while already using");

    /* Normal hookshot fire */
    gb_init(&gb);
    mock_hookshot_calls = 0;
    UseHookshot(&gb, mock_fire_hookshot);
    TEST_ASSERT(mock_hookshot_calls == 1, "Hookshot not fired");
}

static void test_use_magic_rod(void) {
    GBState gb;

    /* Sword animating: blocked */
    gb_init(&gb);
    gb_write(&gb, wSwordAnimationState, 0x01);
    UseMagicRod(&gb);
    TEST_ASSERT(gb_read(&gb, wLinkAttackStepAnimationCountdown) == 0, "Magic rod fired during sword animation");

    /* Too many projectiles (>= 2): blocked */
    gb_init(&gb);
    gb_write(&gb, wActiveProjectileCount, 0x02);
    UseMagicRod(&gb);
    TEST_ASSERT(gb_read(&gb, wLinkAttackStepAnimationCountdown) == 0, "Magic rod fired with >= 2 projectiles");

    /* Normal magic rod fire */
    gb_init(&gb);
    UseMagicRod(&gb);
    TEST_ASSERT(gb_read(&gb, wLinkAttackStepAnimationCountdown) == (0x0E | ATTACK_STEP_ITEM_MAGIC_ROD),
                "Magic rod animation countdown not set correctly");
}

static int mock_bomb_arrow_calls = 0;
static void mock_convert_to_bomb_arrow(GBState *gb) {
    (void)gb;
    mock_bomb_arrow_calls++;
}

static bool mock_spawn_projectile_res = true;
static bool mock_spawn_bomb_projectile(GBState *gb) {
    (void)gb;
    return mock_spawn_projectile_res;
}

static void test_place_bomb(void) {
    GBState gb;

    /* Bomb already placed */
    gb_init(&gb);
    gb_write(&gb, wHasPlacedBomb, 0x01);
    gb_write(&gb, wBombCount, 0x10);
    PlaceBomb(&gb, mock_spawn_bomb_projectile, mock_convert_to_bomb_arrow);
    TEST_ASSERT(gb_read(&gb, wBombCount) == 0x10, "Bomb placed when already placed");

    /* Zero bombs: plays wrong answer jingle */
    gb_init(&gb);
    gb_write(&gb, wBombCount, 0x00);
    PlaceBomb(&gb, mock_spawn_bomb_projectile, mock_convert_to_bomb_arrow);
    TEST_ASSERT(gb_read(&gb, hJingle) == JINGLE_WRONG_ANSWER, "Wrong answer jingle not played on 0 bombs");

    /* Normal placement with BCD decrement (0x20 -> 0x19) */
    gb_init(&gb);
    gb_write(&gb, wBombCount, 0x20);
    mock_bomb_arrow_calls = 0;
    mock_spawn_projectile_res = true;
    PlaceBomb(&gb, mock_spawn_bomb_projectile, mock_convert_to_bomb_arrow);
    TEST_ASSERT(gb_read(&gb, wBombCount) == 0x19, "BCD decrement failed on bomb count");
    TEST_ASSERT(mock_bomb_arrow_calls == 1, "Bomb arrow conversion not called");
}

static int mock_used_item = -1;
static void mock_use_item_cb(GBState *gb, uint8_t item) {
    (void)gb;
    mock_used_item = item;
}

static int mock_boots_calls = 0;
static void mock_use_pegasus_boots(GBState *gb) {
    (void)gb;
    mock_boots_calls++;
}

static void test_check_items_to_use(void) {
    GBState gb;

    /* 1. Item usage blocked */
    gb_init(&gb);
    gb_write(&gb, wBlockItemUsage, 0x01);
    mock_used_item = -1;
    CheckItemsToUse(&gb, mock_use_pegasus_boots, mock_use_item_cb, NULL);
    TEST_ASSERT(mock_used_item == -1, "Items checked when wBlockItemUsage != 0");

    /* 2. Normal item usage on Button A */
    gb_init(&gb);
    gb_write(&gb, wInventoryItems_AButtonSlot, INVENTORY_BOMBS);
    gb_write(&gb, hJoypadState, J_A);
    mock_used_item = -1;
    CheckItemsToUse(&gb, mock_use_pegasus_boots, mock_use_item_cb, NULL);
    TEST_ASSERT(mock_used_item == INVENTORY_BOMBS, "Item A not used on J_A");

    /* 3. Normal item usage on Button B */
    gb_init(&gb);
    gb_write(&gb, wInventoryItems_BButtonSlot, INVENTORY_BOW);
    gb_write(&gb, hJoypadState, J_B);
    mock_used_item = -1;
    CheckItemsToUse(&gb, mock_use_pegasus_boots, mock_use_item_cb, NULL);
    TEST_ASSERT(mock_used_item == INVENTORY_BOW, "Item B not used on J_B");

    /* 4. Reading text suppresses item A & B */
    gb_init(&gb);
    gb_write(&gb, wInventoryItems_AButtonSlot, INVENTORY_BOMBS);
    gb_write(&gb, wInventoryItems_BButtonSlot, INVENTORY_BOW);
    gb_write(&gb, hJoypadState, J_A | J_B);
    gb_write(&gb, wItemUsageContext, ITEM_USAGE_READING_TEXT);
    mock_used_item = -1;
    CheckItemsToUse(&gb, mock_use_pegasus_boots, mock_use_item_cb, NULL);
    TEST_ASSERT(mock_used_item == -1, "Items used while reading text");

    /* 5. Pegasus Boots held on button A */
    gb_init(&gb);
    gb_write(&gb, wInventoryItems_AButtonSlot, INVENTORY_PEGASUS_BOOTS);
    gb_write(&gb, hPressedButtonsMask, J_A);
    mock_boots_calls = 0;
    CheckItemsToUse(&gb, mock_use_pegasus_boots, mock_use_item_cb, NULL);
    TEST_ASSERT(mock_boots_calls == 1, "Pegasus boots not used on held button A");

    /* 6. Pegasus Boots released -> charge meter reset */
    gb_init(&gb);
    gb_write(&gb, wInventoryItems_AButtonSlot, INVENTORY_PEGASUS_BOOTS);
    gb_write(&gb, hPressedButtonsMask, 0x00);
    gb_write(&gb, wPegasusBootsChargeMeter, 0x15);
    CheckItemsToUse(&gb, mock_use_pegasus_boots, mock_use_item_cb, NULL);
    TEST_ASSERT(gb_read(&gb, wPegasusBootsChargeMeter) == 0, "Boots charge meter not reset on button release");
}

void run_check_items_to_use_tests(void) {
    printf("[*] Running SetShieldVals tests...\n");
    test_set_shield_vals();

    printf("[*] Running HoldSwordIfNeeded tests...\n");
    test_hold_sword_if_needed();

    printf("[*] Running UseShield tests...\n");
    test_use_shield();

    printf("[*] Running UseShovel tests...\n");
    test_use_shovel();

    printf("[*] Running UseHookshot tests...\n");
    test_use_hookshot();

    printf("[*] Running UseMagicRod tests...\n");
    test_use_magic_rod();

    printf("[*] Running PlaceBomb tests...\n");
    test_place_bomb();

    printf("[*] Running CheckItemsToUse tests...\n");
    test_check_items_to_use();

    if (failures == 0) {
        printf("  [PASS] All check_items_to_use.asm routines verified successfully!\n\n");
    }
}
