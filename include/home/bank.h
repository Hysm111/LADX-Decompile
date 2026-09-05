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
 * Corresponds to AdjustBankNumberForGBC (00:0B0B) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Original bank number
 * @return Adjusted bank number
 */
uint8_t AdjustBankNumberForGBC(GBState *gb, uint8_t bank);

/**
 * Switch ROM bank to specified bank, and save it in wCurrentBank.
 * Corresponds to SwitchBank (00:080C) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Target bank number
 */
void SwitchBank(GBState *gb, uint8_t bank);

/**
 * Switch to bank adjusted for GBC mode, and save it in wCurrentBank.
 * Corresponds to SwitchAdjustedBank (00:0813) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Original target bank number
 */
void SwitchAdjustedBank(GBState *gb, uint8_t bank);

/**
 * Reload the saved bank from wCurrentBank into rSelectROMBank.
 * Corresponds to ReloadSavedBank (00:081D) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ReloadSavedBank(GBState *gb);

/**
 * Restore wCurrentBank into rSelectROMBank without modifying flags.
 * Corresponds to RestoreBankAndReturn (00:08DF) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void RestoreBankAndReturn(GBState *gb);

/**
 * Switch to ROM bank 1 and return.
 * Corresponds to LoadBank1AndReturn (00:0917) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadBank1AndReturn(GBState *gb);

/**
 * Restore stacked bank into rSelectROMBank and return.
 * Corresponds to RestoreStackedBankAndReturn (00:0973) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param stacked_bank Popped bank number
 */
void RestoreStackedBankAndReturn(GBState *gb, uint8_t stacked_bank);

/**
 * Pop stacked bank and call SwitchBank (saving to wCurrentBank).
 * Corresponds to RestoreStackedBank (00:0AB0) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param stacked_bank Popped bank number
 */
void RestoreStackedBank(GBState *gb, uint8_t stacked_bank);

/**
 * Perform a farcall: switch to wFarcallBank, execute target, then restore wFarcallReturnBank.
 * Corresponds to Farcall (00:0BD7) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param target_func Optional callback to execute in target bank (can be NULL)
 */
void Farcall(GBState *gb, void (*target_func)(GBState *));

/**
 * Checks if an overworld object is in the ignore list.
 * Corresponds to CheckOverworldObjectIgnoreList (20:6E50) in disassembly.
 *
 * @param object_id Object tile/entity identifier byte
 * @return true if object is in the ignore list, false otherwise
 */
bool CheckOverworldObjectIgnoreList(uint8_t object_id);

/**
 * Backup an overworld object at HL into WRAM bank 2 on GBC outdoor screens.
 * Corresponds to BackupObjectInRAM2 (00:0B2F) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Object address in RAM bank
 * @param flags_and_return_bank Bit 7: if 1, bypass ignore check; Bits 0-6: ROM bank to restore
 */
void BackupObjectInRAM2(GBState *gb, uint16_t hl, uint8_t flags_and_return_bank);

/**
 * Copy object attributes from ROM bank hMultiPurpose0 to WRAM bank 2,
 * then restore ROM bank $20.
 * Corresponds to CopyObjectsAttributesToWRAM2 (00:0B1A) in disassembly.
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
