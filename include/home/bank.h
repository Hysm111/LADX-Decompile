#ifndef LADX_HOME_BANK_H
#define LADX_HOME_BANK_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Adjust bank number for GBC compatibility (sets bit 5 if running on GBC).
 * Corresponds to AdjustBankNumberForGBC (00:0B0B) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Base bank number
 * @return Adjusted bank number
 */
uint8_t AdjustBankNumberForGBC(GBState *gb, uint8_t bank);

/**
 * Switch ROM bank and persist to wCurrentBank.
 * Corresponds to SwitchBank (00:080C) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Target ROM bank number
 */
void SwitchBank(GBState *gb, uint8_t bank);

/**
 * Switch to adjusted ROM bank (adjusted for GBC) and persist to wCurrentBank.
 * Corresponds to SwitchAdjustedBank (00:0813) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Target base ROM bank number
 */
void SwitchAdjustedBank(GBState *gb, uint8_t bank);

/**
 * Reload wCurrentBank into rSelectROMBank without updating wCurrentBank.
 * Corresponds to ReloadSavedBank (00:081D) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ReloadSavedBank(GBState *gb);

/**
 * Restores wCurrentBank into rSelectROMBank.
 * Corresponds to RestoreBankAndReturn (00:08DF) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void RestoreBankAndReturn(GBState *gb);

/**
 * Loads bank 1 into rSelectROMBank and returns.
 * Corresponds to LoadBank1AndReturn (00:0917) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadBank1AndReturn(GBState *gb);

/**
 * Restores a stacked bank into rSelectROMBank without modifying wCurrentBank.
 * Corresponds to RestoreStackedBankAndReturn (00:0973) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param stacked_bank Bank number popped from the stack
 */
void RestoreStackedBankAndReturn(GBState *gb, uint8_t stacked_bank);

/**
 * Restores a stacked bank by calling SwitchBank (updating both rSelectROMBank and wCurrentBank).
 * Corresponds to RestoreStackedBank (00:0AB0) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param stacked_bank Bank number popped from the stack
 */
void RestoreStackedBank(GBState *gb, uint8_t stacked_bank);

/**
 * Dispatches a far call to wFarcallBank and returns to wFarcallReturnBank.
 * Corresponds to Farcall (00:0BD7) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param target_func Function pointer to execute in the target bank
 */
void Farcall(GBState *gb, void (*target_func)(GBState *));

/**
 * Trampoline that extracts 16-bit address from wFarcallAdressHigh/Low.
 * Corresponds to Farcall_trampoline (00:0BE7) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @return Target 16-bit address
 */
uint16_t Farcall_trampoline(GBState *gb);

/**
 * Copy data from a source bank to WRAM bank 2, then switch back to bank $20.
 * Corresponds to CopyObjectsAttributesToWRAM2 (00:0B1A) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Destination address in WRAM
 * @param hl Source address in ROM
 * @param bc Byte count
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

/**
 * Calls ChangeBGColumnPalette in bank $24, processes draw commands at wDrawCommand,
 * and restores stacked bank using RestoreStackedBank.
 * Corresponds to ChangeBGColumnPaletteAndExecuteDrawCommands (00:0AB5) in disassembly.
 */
void ChangeBGColumnPaletteAndExecuteDrawCommands(GBState *gb, uint8_t stacked_bank, void (*change_palette)(GBState *));

/**
 * Switches to BANK(FontTiles) ($0F), executes dialog handler, and restores stacked bank.
 * Corresponds to func_A9B (00:0A9B) in disassembly.
 */
void func_A9B(GBState *gb, uint8_t stacked_bank, void (*execute_dialog)(GBState *));

/**
 * Switches to bank $36 via SwitchBank, calls Spawn2x2RubbleEntities, and restores stacked bank via RestoreStackedBank.
 * Corresponds to Spawn2x2RubbleEntities_trampoline (00:0AA7) in disassembly.
 */
void Spawn2x2RubbleEntities_trampoline(GBState *gb, uint8_t stacked_bank, void (*spawn_func)(GBState *));

/**
 * Switches to bank $20, calls RenderActiveEntitySpritesRect, and restores stacked bank.
 * Corresponds to func_A5F (00:0A5F) in disassembly.
 */
void func_A5F(GBState *gb, uint8_t stacked_bank, void (*render_func)(GBState *));

/**
 * Switches to bank $36, calls func_036_703E, and restores stacked bank.
 * Corresponds to func_036_703E_trampoline (00:0AC6) in disassembly.
 */
void func_036_703E_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $36, calls cycleInstrumentItemColor, and restores stacked bank.
 * Corresponds to cycleInstrumentItemColor_trampoline (00:0AD2) in disassembly.
 */
void cycleInstrumentItemColor_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $36 via SwitchBank, calls func_036_4A77, and restores stacked bank.
 * Corresponds to func_036_4A77_trampoline (00:0ADE) in disassembly.
 */
void func_036_4A77_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $36, calls GetOwlStatueDialogId, and restores stacked bank.
 * Corresponds to GetOwlStatueDialogId_trampoline (00:0AEA) in disassembly.
 */
void GetOwlStatueDialogId_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $36, calls SpawnPhotographer, and restores stacked bank.
 * Corresponds to SpawnPhotographer_trampoline (00:0AF6) in disassembly.
 */
void SpawnPhotographer_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $3D and calls LoadPhotoBgMap.
 * Corresponds to LoadPhotoBgMap_trampoline (00:0B02) in disassembly.
 */
void LoadPhotoBgMap_trampoline(GBState *gb, void (*target_func)(GBState *));

/**
 * Switches to bank $20, calls func_020_6D0E, and restores stacked bank.
 * Corresponds to func_020_6D0E_trampoline (00:0978) in disassembly.
 */
void func_020_6D0E_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $20, calls CheckPushedTombStone, and restores stacked bank.
 * Corresponds to CheckPushedTombStone_trampoline (00:09C8) in disassembly.
 */
void CheckPushedTombStone_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $20, calls GetEntityInitHandler, and restores stacked bank.
 * Corresponds to GetEntityInitHandler_trampoline (00:09D3) in disassembly.
 */
void GetEntityInitHandler_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $20, calls func_020_4874, and restores stacked bank.
 * Corresponds to func_020_4874_trampoline (00:09DE) in disassembly.
 */
void func_020_4874_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $20, calls func_020_4954, and restores stacked bank.
 * Corresponds to func_020_4954_trampoline (00:09E9) in disassembly.
 */
void func_020_4954_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $20, calls ReplaceObjects56and57, and restores stacked bank.
 * Corresponds to ReplaceObjects56and57_trampoline (00:09F5) in disassembly.
 */
void ReplaceObjects56and57_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $36, calls func_036_505F, and restores stacked bank.
 * Corresponds to func_036_505F_trampoline (00:0A47) in disassembly.
 */
void func_036_505F_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $36, calls func_036_4F9B, and restores stacked bank.
 * Corresponds to func_036_4F9B_trampoline (00:0A53) in disassembly.
 */
void func_036_4F9B_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $03, calls func_003_5A2E, and restores stacked bank.
 * Corresponds to func_003_5A2E_trampoline (00:0A6B) in disassembly.
 */
void func_003_5A2E_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $36, calls func_036_4F68, and restores stacked bank.
 * Corresponds to func_036_4F68_trampoline (00:0A77) in disassembly.
 */
void func_036_4F68_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $20, calls func_020_6D52, and restores stacked bank.
 * Corresponds to func_020_6D52_trampoline (00:0A83) in disassembly.
 */
void func_020_6D52_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Switches to bank $36, calls func_036_4BE8, and restores stacked bank.
 * Corresponds to func_036_4BE8_trampoline (00:0A8F) in disassembly.
 */
void func_036_4BE8_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *));

/**
 * Room BG attributes update for intersected object.
 * Computes bc = wDDD8 * 4, calls GetBGAttributesAddressForObject in bank $1A,
 * appends a 10-byte 2-column draw command to wDrawCommandVRAM1, and restores stacked bank.
 * Corresponds to func_91D (00:091D) in disassembly.
 */
void func_91D(GBState *gb, uint8_t stacked_bank, void (*get_bg_attr_addr)(GBState *));

/**
 * Same as func_91D but caller provides explicit bc (object attribute index * 4).
 * Corresponds to func_91D.jp_92E (00:092E) in disassembly.
 */
void func_91D_jp_92E(GBState *gb, uint16_t bc, uint8_t stacked_bank, void (*get_bg_attr_addr)(GBState *));

/**
 * Loads a palette data byte via func_01A_6710 into hMultiPurpose9:hMultiPurposeA in bank $1A,
 * switches to hMultiPurpose8, reads the byte, and increments de.
 * Corresponds to func_983 (00:0983) in disassembly.
 */
uint8_t func_983(GBState *gb, uint16_t *de, void (*func_01A_6710)(GBState *));

/**
 * Calls func_983 twice to fetch 2 palette attribute bytes, appends a 5-byte row draw command
 * to wDrawCommandVRAM1, and restores stacked bank.
 * Corresponds to func_999 (00:0999) in disassembly.
 */
void func_999(GBState *gb, uint16_t *de, uint8_t stacked_bank, void (*func_01A_6710)(GBState *));

#ifdef __cplusplus
}
#endif

/**
 * Calls func_020_4B4A in bank $20 and restores wCurrentBank.
 * Corresponds to func_020_4B4A_trampoline (00:134B) in disassembly.
 */
void func_020_4B4A_trampoline(GBState *gb, void (*func_020_4B4A)(GBState *));

#endif /* LADX_HOME_BANK_H */
