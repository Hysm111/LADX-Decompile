#ifndef LADX_HOME_CHECK_ITEMS_TO_USE_H
#define LADX_HOME_CHECK_ITEMS_TO_USE_H

#include "gb.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ItemUsageCallbacks {
    void (*use_sword)(GBState *);
    void (*use_power_bracelet)(GBState *);
    void (*shoot_arrow)(GBState *);
    void (*use_boomerang)(GBState *);
    void (*fire_hookshot)(GBState *);
    void (*use_rocs_feather)(GBState *);
    void (*use_ocarina)(GBState *);
    void (*use_magic_powder)(GBState *);
    bool (*check_shovel_poking)(GBState *);
    bool (*spawn_bomb_projectile)(GBState *);
    void (*convert_to_bomb_arrow)(GBState *);
} ItemUsageCallbacks;

/**
 * Check if one of the inventory items should be used (00:1177).
 */
void CheckItemsToUse(GBState *gb, void (*use_pegasus_boots)(GBState *),
                     void (*use_item)(GBState *, uint8_t),
                     void (*color_dungeon_check)(GBState *));

/**
 * Use an inventory item (00:129C).
 */
void UseItem(GBState *gb, uint8_t item, const ItemUsageCallbacks *callbacks);

/**
 * Magic rod item action (00:12D8).
 */
void UseMagicRod(GBState *gb);

/**
 * Shield item action (00:12EE).
 */
void UseShield(GBState *gb);

/**
 * Shovel item action (00:12F8).
 */
void UseShovel(GBState *gb, bool (*check_poking)(GBState *));

/**
 * Hookshot item action (00:1319).
 */
void UseHookshot(GBState *gb, void (*fire_hookshot)(GBState *));

/**
 * Hold sword if button is pressed (00:1321).
 */
void HoldSwordIfNeeded(GBState *gb, uint8_t item);

/**
 * Set shield variables and sync (00:1340).
 */
void SetShieldVals(GBState *gb, void (*sync_shield_trampoline)(GBState *));

/**
 * Place a bomb (00:135A).
 */
void PlaceBomb(GBState *gb, bool (*spawn_bomb_projectile)(GBState *), void (*convert_to_bomb_arrow)(GBState *));

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_CHECK_ITEMS_TO_USE_H */
