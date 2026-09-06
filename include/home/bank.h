#ifndef LADX_HOME_BANK_H
#define LADX_HOME_BANK_H

#include "common.h"
#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Adjust bank number for Game Boy Color mode.
 * If running on GBC (hIsGBC is non-zero), sets bit 5 (adds $20) to bank number.
 * On DMG, leaves the bank number unchanged.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Original bank number
 * @return Adjusted bank number
 */
uint8_t AdjustBankNumberForGBC(GBState *gb, uint8_t bank);

/**
 * Switch to the specified ROM bank and save it as active bank in wCurrentBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Bank number to switch to
 */
void SwitchBank(GBState *gb, uint8_t bank);

/**
 * Adjust bank number for GBC, switch to it, and save it in wCurrentBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Original bank number
 */
void SwitchAdjustedBank(GBState *gb, uint8_t bank);

/**
 * Reload active bank from wCurrentBank into rSelectROMBank without modifying wCurrentBank.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ReloadSavedBank(GBState *gb);

/**
 * Restore active bank from wCurrentBank into rSelectROMBank.
 * Corresponds to RestoreBankAndReturn (00:08DF) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void RestoreBankAndReturn(GBState *gb);

/**
 * Load bank 1 into rSelectROMBank and return.
 * Corresponds to LoadBank1AndReturn (00:0917) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadBank1AndReturn(GBState *gb);

/**
 * Restore bank saved on stack to rSelectROMBank and return.
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
 * Jump target helper for Farcall: reads address from wFarcallAdressHigh/Low.
 * Corresponds to Farcall_trampoline (00:0BE7) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @return Target 16-bit address (wFarcallAdressHigh << 8) | wFarcallAdressLow
 */
uint16_t Farcall_trampoline(GBState *gb);

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
 * Copy object attributes from ROM bank hMultiPurpose0 to WRAM bank 2,
 * then restore ROM bank $20.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Destination address
 * @param hl Source address
 * @param bc Number of bytes to copy
 */
void CopyObjectsAttributesToWRAM2(GBState *gb, uint16_t de, uint16_t hl, uint16_t bc);

/**
 * On GBC, copy overworld object at [hl] to RAM bank 2, with optional ignore list filtering.
 * Corresponds to BackupObjectInRAM2 (00:0B2F) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Address in RAM bank 0 and 2
 * @param flags_and_return_bank Bit 7: if clear, check ignore list; bits 0-6: return bank
 */
void BackupObjectInRAM2(GBState *gb, uint16_t hl, uint8_t flags_and_return_bank);

/**
 * Calls func_020_6A30 in bank $20 and restores wCurrentBank with RestoreBankAndReturn.
 * Corresponds to func_020_6A30_trampoline (00:08D7) in disassembly.
 */
void func_020_6A30_trampoline(GBState *gb, void (*target_func)(GBState *));

/**
 * Calls func_020_6AC1 in bank $20 and restores wCurrentBank with RestoreBankAndReturn.
 * Corresponds to func_020_6AC1_trampoline (00:08E6) in disassembly.
 */
void func_020_6AC1_trampoline(GBState *gb, void (*target_func)(GBState *));

/**
 * Calls UpdateIntroSeaBGPalettes in bank $20 and restores wCurrentBank with RestoreBankAndReturn.
 * Corresponds to UpdateIntroSeaBGPalettes_trampoline (00:08F0) in disassembly.
 */
void UpdateIntroSeaBGPalettes_trampoline(GBState *gb, void (*target_func)(GBState *));

/**
 * Calls ClearFileMenuBG (func_020_6BDC) in bank $20 and restores stacked bank with RestoreStackedBankAndReturn.
 * Corresponds to ClearFileMenuBG_trampoline (00:08FA) in disassembly.
 */
void ClearFileMenuBG_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Calls LoadFileMenuBG in bank $20 and loads bank 1 with LoadBank1AndReturn.
 * Corresponds to LoadFileMenuBG_trampoline (00:0905) in disassembly.
 */
void LoadFileMenuBG_trampoline(GBState *gb, void (*target_func)(GBState *));

/**
 * Calls CopyLinkTunicPalette in bank $20 and loads bank 1 with LoadBank1AndReturn.
 * Corresponds to CopyLinkTunicPalette_trampoline (00:090F) in disassembly.
 */
void CopyLinkTunicPalette_trampoline(GBState *gb, void (*target_func)(GBState *));

/**
 * Calls LoadBaseTiles and restores stacked bank with RestoreStackedBankAndReturn.
 * Corresponds to LoadBaseTiles_trampoline (00:0BBE) in disassembly.
 */
void LoadBaseTiles_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_BANK_H */
