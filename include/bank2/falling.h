#ifndef LADX_BANK2_FALLING_H
#define LADX_BANK2_FALLING_H

#include "gb.h"

/**
 * Handles Link's unknown / hole falling motion state 0x0F (02:50A3).
 *
 * @param gb Pointer to Game Boy system state.
 */
void LinkMotionUnknownHandler(GBState *gb);

/* Bank 2 Falling Down Animation Table */
extern const uint8_t LinkFallingDownAnimation[10];

/**
 * Resets staircase flag if active (02:52D6).
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_52D6(GBState *gb);

/**
 * Resets Link map entry position, invincibility counter, and motion state (02:52B9).
 *
 * @param gb Pointer to Game Boy system state.
 */
void label_002_52B9(GBState *gb);

/**
 * Handles Link falling down into a hole or pit (02:50D4).
 *
 * @param gb Pointer to Game Boy system state.
 */
void LinkMotionFallingDownHandler(GBState *gb);

/**
 * Handles Link recovering from pit fall or damage, countdown animation, and entry repositioning (02:5267).
 *
 * @param gb Pointer to Game Boy system state.
 */
void LinkMotionRecoverHandler(GBState *gb);

#endif /* LADX_BANK2_FALLING_H */
