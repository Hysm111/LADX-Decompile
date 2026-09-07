#ifndef LADX_BANK1_SIREN_INSTRUMENTS_H
#define LADX_BANK1_SIREN_INSTRUMENTS_H

#include <stdint.h>
#include "gb.h"

extern const uint8_t Data_001_6BCF[8];
extern const uint8_t Data_001_6BD7[8];
extern const uint8_t Data_001_6BDF[8];
extern const uint8_t Data_001_6BE7[3];

/**
 * Copy tiles and BG map for inventory Siren Instruments (01:6BB5).
 */
void LoadSirenInstruments(GBState *gb);

/**
 * Load 4 siren instruments into BG map at 0x9D00 (01:6BEF).
 * side != 0: rightSide (instruments 0..3)
 * side == 0: leftSide (instruments 4..7)
 */
void LoadInstrumentsBG(GBState *gb, uint8_t side);

/**
 * Return the address of next BG location for an instrument (01:6C69).
 */
uint16_t GetInstrumentNextBGAddress(uint16_t hl, uint8_t bc_idx);

/**
 * Copy tiles for a siren instrument to VRAM and increment stage (01:6C77).
 */
void LoadSirenInstrumentTiles(GBState *gb, uint8_t instrument_idx);

#endif /* LADX_BANK1_SIREN_INSTRUMENTS_H */
