#ifndef LADX_HOME_DIALOG_H
#define LADX_HOME_DIALOG_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read a byte from ROM bank $1C at address HL + BC, then restore ROM bank 1.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Base address in ROM bank $1C
 * @param bc Offset added to base address
 * @return Byte value read
 */
uint8_t ReadValueInDialogsBank(GBState *gb, uint16_t hl, uint16_t bc);

/**
 * Look up tile value for the given codepoint offset from CodepointToTileMap ($4641) in bank $1C.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bc Codepoint offset
 * @return Tile value
 */
uint8_t ReadTileValueFromAsciiTable(GBState *gb, uint16_t bc);

/**
 * Look up diacritic value for the given codepoint offset from CodepointToDiacritic ($4741) in bank $1C.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bc Codepoint offset
 * @return Diacritic value
 */
uint8_t ReadTileValueFromDiacriticsTable(GBState *gb, uint16_t bc);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_DIALOG_H */
