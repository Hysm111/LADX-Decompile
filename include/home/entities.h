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

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_ENTITIES_H */
