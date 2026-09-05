#ifndef LADX_HOME_BANK_H
#define LADX_HOME_BANK_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Adjust bank number for Game Boy Color mode.
 * If running on GBC (hIsGBC != 0), sets bit 5 (bank | 0x20).
 * On DMG (hIsGBC == 0), returns the bank number unchanged.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Original bank number
 * @return Adjusted bank number
 */
uint8_t AdjustBankNumberForGBC(GBState *gb, uint8_t bank);

/**
 * Switch ROM bank to specified bank, and save it in wCurrentBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Target bank number
 */
void SwitchBank(GBState *gb, uint8_t bank);

/**
 * Switch to bank adjusted for GBC mode, and save it in wCurrentBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Original target bank number
 */
void SwitchAdjustedBank(GBState *gb, uint8_t bank);

/**
 * Reload the saved bank from wCurrentBank into rSelectROMBank.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ReloadSavedBank(GBState *gb);

/**
 * Copy object attributes from ROM bank hMultiPurpose0 to WRAM bank 2,
 * then restore ROM bank $20.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Destination address
 * @param hl Source address
 * @param bc Number of bytes to copy
 */
void CopyObjectsAttributesToWRAM2(GBState *gb, uint16_t de, uint16_t hl, uint16_t bc);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_BANK_H */
