#ifndef LADX_HOME_ENTITIES_H
#define LADX_HOME_ENTITIES_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * IsZero (00:0C08)
 * Tests if the byte at (hl + bc) is equal to zero.
 * Returns the byte read from memory at (hl + bc).
 */
uint8_t IsZero(GBState *gb, uint16_t hl, uint16_t bc);

/**
 * GetEntitySlowTransitionCountdown (00:0BFB)
 * Retrieves the slow transition countdown for the entity at index bc.
 * Lookups value in wEntitiesSlowTransitionCountdownTable (0xC450).
 */
uint8_t GetEntitySlowTransitionCountdown(GBState *gb, uint16_t entity_index);

/**
 * GetEntityPrivateCountdown1 (00:0C00)
 * Retrieves private countdown 1 for the entity at index bc.
 * Lookups value in wEntitiesPrivateCountdown1Table (0xC2F0).
 */
uint8_t GetEntityPrivateCountdown1(GBState *gb, uint16_t entity_index);

/**
 * GetEntityTransitionCountdown (00:0C05)
 * Retrieves the transition countdown for the entity at index bc.
 * Lookups value in wEntitiesTransitionCountdownTable (0xC2E0).
 */
uint8_t GetEntityTransitionCountdown(GBState *gb, uint16_t entity_index);

/**
 * DecrementEntityIgnoreHitsCountdown (00:0C56)
 * Decrements the ignore-hits countdown for entity at index bc if it is > 0.
 * Operates on wEntitiesIgnoreHitsCountdownTable (0xC410).
 */
void DecrementEntityIgnoreHitsCountdown(GBState *gb, uint16_t entity_index);

/**
 * CreateTradingItemEntity (00:0C0C)
 * Spawns a temporary entity with ENTITY_TRADING_ITEM (0xAF) at Link's current position.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param spawn_func Callback to spawn entity (e.g. SpawnNewEntity_trampoline)
 */
void CreateTradingItemEntity(GBState *gb, uint16_t (*spawn_func)(GBState *, uint8_t entity_type));

/**
 * SpawnNewEntity_trampoline (00:3B86)
 * Switches to BANK(SpawnNewEntity) ($03), calls SpawnNewEntity, and restores saved bank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param entity_type Type ID of entity to spawn
 * @param spawn_new_entity Target callback
 * @return Entity slot offset (de)
 */
uint16_t SpawnNewEntity_trampoline(GBState *gb, uint8_t entity_type, uint16_t (*spawn_new_entity)(GBState *, uint8_t));

/**
 * SpawnNewEntityInRange_trampoline (00:3B98)
 * Switches to BANK(SpawnNewEntityInRange) ($03), calls SpawnNewEntityInRange, and restores saved bank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param entity_type Type ID of entity to spawn
 * @param spawn_in_range Target callback
 * @return Entity slot offset (de)
 */
uint16_t SpawnNewEntityInRange_trampoline(GBState *gb, uint8_t entity_type, uint16_t (*spawn_in_range)(GBState *, uint8_t));

/**
 * AnimateEntitiesAndRestoreBank (00:0EF4)
 * Calls AnimateEntities, then switches to bank given in argument via SwitchBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Bank number to restore/switch to
 * @param animate_entities Callback to AnimateEntities handler
 */
void AnimateEntitiesAndRestoreBank(GBState *gb, uint8_t bank, void (*animate_entities)(GBState *));

/**
 * AnimateEntitiesAndRestoreBank17 (00:0EED)
 * Selects bank $03 in rSelectROMBank, then calls AnimateEntitiesAndRestoreBank with bank $17.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param animate_entities Callback to AnimateEntities handler
 */
void AnimateEntitiesAndRestoreBank17(GBState *gb, void (*animate_entities)(GBState *));

/**
 * AnimateEntitiesAndRestoreBank01 (00:0EFC)
 * Selects bank $03 in rSelectROMBank, then calls AnimateEntitiesAndRestoreBank with bank $01.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param animate_entities Callback to AnimateEntities handler
 */
void AnimateEntitiesAndRestoreBank01(GBState *gb, void (*animate_entities)(GBState *));

/**
 * AnimateEntitiesAndRestoreBank02 (00:0F05)
 * Selects bank $03 in rSelectROMBank, then calls AnimateEntitiesAndRestoreBank with bank $02.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param animate_entities Callback to AnimateEntities handler
 */
void AnimateEntitiesAndRestoreBank02(GBState *gb, void (*animate_entities)(GBState *));

/**
 * CanBowWowEatEntity (00:3925)
 * Checks whether Bow-Wow can eat an entity of the given type.
 * Switches to Bank $14, reads from BowWowEatableEntitiesTable, switches to Bank $05, and returns the byte.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param entity_type Type ID of entity
 * @return Value read from table (nonzero if eatable)
 */
uint8_t CanBowWowEatEntity(GBState *gb, uint8_t entity_type);

/**
 * label_3935 (00:3935)
 * Switches to Bank $19 via SwitchBank, calls func_019_7c50, and then switches to Bank $03 via SwitchBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param func_019_7c50 Target callback in Bank $19
 */
void label_3935(GBState *gb, void (*func_019_7c50)(GBState *));

/**
 * LiftableRockStartSmashingAnimation_trampoline (00:3942)
 * Switches to Bank $03, calls LiftableRockStartSmashingAnimation, and restores saved bank via ReloadSavedBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param start_smashing Target callback in Bank $03
 */
void LiftableRockStartSmashingAnimation_trampoline(GBState *gb, void (*start_smashing)(GBState *));

/**
 * label_394D (00:394D)
 * Switches to Bank $14, calls func_014_54ac, and restores saved bank via ReloadSavedBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param func_014_54ac Target callback in Bank $14
 */
void label_394D(GBState *gb, void (*func_014_54ac)(GBState *));

/**
 * CreateFollowingNpcEntity_trampoline (00:3958)
 * Switches to Bank $01 via SwitchBank, calls create_following_npc, and switches to Bank $02 via SwitchBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param create_following_npc Target callback in Bank $01
 */
void CreateFollowingNpcEntity_trampoline(GBState *gb, void (*create_following_npc)(GBState *));

/**
 * ConfigureNewEntity_trampoline (00:3965)
 * Switches to Bank $03, calls configure_new_entity, and restores saved bank via ReloadSavedBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param configure_new_entity Target callback in Bank $03
 */
void ConfigureNewEntity_trampoline(GBState *gb, void (*configure_new_entity)(GBState *));

/**
 * GetEntityDirectionToLink_trampoline (00:3970)
 * Switches to Bank $03, calls get_direction, and restores saved bank via ReloadSavedBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param get_direction Target callback in Bank $03
 */
void GetEntityDirectionToLink_trampoline(GBState *gb, void (*get_direction)(GBState *));

/**
 * label_397B (00:397B)
 * Switches to Bank $14, calls func_014_5347, and sets rSelectROMBank to $03.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param func_014_5347 Target callback in Bank $14
 */
void label_397B(GBState *gb, void (*func_014_5347)(GBState *));

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_ENTITIES_H */
