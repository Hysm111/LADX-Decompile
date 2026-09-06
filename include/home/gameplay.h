#ifndef LADX_HOME_GAMEPLAY_H
#define LADX_HOME_GAMEPLAY_H

#include "gb.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_GAMEPLAY_H */
