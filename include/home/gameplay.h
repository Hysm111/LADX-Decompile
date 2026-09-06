#ifndef LADX_HOME_GAMEPLAY_H
#define LADX_HOME_GAMEPLAY_H

#include "gb.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*JumpTableFunc)(GBState *gb);

/**
 * Checks if the A + B + Start + Select combo is pressed to open the save screen.
 * Verifies that no transitions, scrolling, or dialogs are active, and that
 * gameplay type is not INVENTORY or higher, wD474 == 0, and wD464 == 0.
 * If conditions are met:
 *   wTransitionSequenceCounter = 0
 *   wC16C = 0
 *   wDialogState = 0
 *   wGameplaySubtype = 0
 *   wGameplayType = GAMEPLAY_FILE_SAVE (0x06)
 *
 * @param gb Pointer to Game Boy hardware state
 * @return true if save screen was triggered, false otherwise
 */
bool CheckPresentSaveScreen(GBState *gb);

/**
 * Common return path from gameplay handlers (00:101A).
 * Switches to BANK(FontTiles) ($0F) and executes dialog handler.
 * If running on CGB (hIsGBC != 0), switches to BANK(LoadBGPalettes) ($24)
 * and executes LoadBGPalettes.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param execute_dialog Callback to ExecuteDialog in bank $0F (optional)
 * @param load_bg_palettes Callback to LoadBGPalettes in bank $24 (optional)
 */
void returnFromGameplayHandler(GBState *gb, void (*execute_dialog)(GBState *), void (*load_bg_palettes)(GBState *));

/**
 * EnableSRAM (00:27D0)
 * Selects cartridge RAM bank 0 and enables external cartridge SRAM (writes CART_SRAM_ENABLE to rRAMG).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void EnableSRAM(GBState *gb);

/**
 * SynchronizeDungeonsItemFlags_trampoline (00:2802)
 * Switches to BANK(SynchronizeDungeonsItemFlags) ($01), calls callback,
 * and reloads saved bank via ReloadSavedBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param sync_func Callback to SynchronizeDungeonsItemFlags
 */
void SynchronizeDungeonsItemFlags_trampoline(GBState *gb, void (*sync_func)(GBState *));

/**
 * GetRandomByte (00:280D)
 * Advances random number generator seed and returns pseudorandom byte.
 * Computes: wRandomSeed = rrca(hFrameCounter + wRandomSeed + rLY).
 *
 * @param gb Pointer to Game Boy hardware state
 * @return Pseudorandom 8-bit value
 */
uint8_t GetRandomByte(GBState *gb);

/**
 * ReadJoypadState (00:281E)
 * Reads the joypad matrix (d-pad and buttons) unless inhibited by map transition,
 * inactive world state, pass out animation, transition counters, or palette effect.
 * Updates hJoypadState (newly pressed) and hPressedButtonsMask (held).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ReadJoypadState(GBState *gb);

/**
 * GetIntersectedObjectBGAddress (label_2887 at 00:2887)
 * Calculates the VRAM BGMap0 address for the intersected object based on
 * hIntersectedObjectTop, hBaseScrollY, hIntersectedObjectLeft, and hBaseScrollX.
 * Stores result into hIntersectedObjectBGAddressHigh/Low and returns address.
 *
 * @param gb Pointer to Game Boy hardware state
 * @return 16-bit BG tilemap address in VRAM
 */
uint16_t GetIntersectedObjectBGAddress(GBState *gb);

/**
 * TableJump (00:28C0)
 * Jump table dispatcher for `rst 0`. Indexes into jump table of function pointers
 * and executes table[index](gb).
 *
 * @param gb Pointer to Game Boy hardware state
 * @param index Routine index in the jump table
 * @param table Array of function pointers
 */
void TableJump(GBState *gb, uint8_t index, const JumpTableFunc *table);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_GAMEPLAY_H */
