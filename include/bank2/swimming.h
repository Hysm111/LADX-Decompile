#ifndef LADX_BANK2_SWIMMING_H
#define LADX_BANK2_SWIMMING_H

#include "gb.h"

/* Bank 2 Swimming Velocity Tables */
extern const int8_t Data_002_4EF0[16];
extern const int8_t Data_002_4F00[16];
extern const int8_t Data_002_4F10[16];
extern const int8_t Data_002_4F20[16];

/**
 * Handles Link's swimming and diving physics, movement, B diving toggle, and sunken items (02:4F30).
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_map_transition Optional callback for map transition check.
 * @param func_753a Optional callback for surface offset and shadow physics (02:753A).
 * @param spawn_new_entity Optional callback to spawn entity (00:3B86).
 * @param splash_vfx Optional callback to create splash VFX (02:5928).
 */
void LinkMotionSwimmingHandler(GBState *gb,
                               void (*check_map_transition)(GBState *),
                               void (*func_753a)(GBState *),
                               uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                               void (*splash_vfx)(GBState *, uint8_t));

#endif /* LADX_BANK2_SWIMMING_H */
