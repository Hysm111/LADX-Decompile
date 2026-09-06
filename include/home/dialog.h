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

/**
 * OpenDialogInTable0 (00:2385)
 * Initializes dialog variables and sets state based on Link position.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param dialog_index Dialog index in table 0 ($00-$FF)
 */
void OpenDialogInTable0(GBState *gb, uint8_t dialog_index);

/**
 * OpenDialogInTable1 (00:2373)
 * Opens dialog in table 1 ($100-$1FF).
 *
 * @param gb Pointer to Game Boy hardware state
 * @param dialog_index Dialog index in table 1
 */
void OpenDialogInTable1(GBState *gb, uint8_t dialog_index);

/**
 * OpenDialogInTable2 (00:237C)
 * Opens dialog in table 2 ($200-$2FF).
 *
 * @param gb Pointer to Game Boy hardware state
 * @param dialog_index Dialog index in table 2
 */
void OpenDialogInTable2(GBState *gb, uint8_t dialog_index);

/**
 * DialogOpenAnimationStartHandler (00:236B)
 * Switches to Bank $14 and invokes DialogOpenAnimationStart ($5449).
 */
void DialogOpenAnimationStartHandler(GBState *gb, void (*func_014_5449)(GBState *));

/**
 * DialogOpenAnimationHandler (00:23B0)
 * NOP animation frame handler.
 */
void DialogOpenAnimationHandler(GBState *gb);

/**
 * DialogClosingEndHandler (00:23B1)
 * Clears dialog state, sets cooldown, and optionally calls func_021_53CF on CGB.
 */
void DialogClosingEndHandler(GBState *gb, void (*func_021_53CF)(GBState *));

/**
 * DialogOpenAnimationEndHandler (00:247D)
 * Switches to Bank $1C and invokes DialogOpenAnimationEnd ($4A2C).
 */
void DialogOpenAnimationEndHandler(GBState *gb, void (*func_01C_4A2C)(GBState *));

/**
 * IncrementDialogState / IncrementDialogStateAndReturn (00:2485)
 * Increments wDialogState.
 */
void IncrementDialogState(GBState *gb);
void IncrementDialogStateAndReturn(GBState *gb);

/**
 * UpdateDialogState (00:2496)
 * Resets animation frame and sets closing dialog state or closes dialog in photo album mode.
 */
void UpdateDialogState(GBState *gb);

/**
 * DialogFinishedHandler (00:248A)
 * Advances dialog state to closing when user presses A or B.
 */
void DialogFinishedHandler(GBState *gb);

/**
 * DialogClosingBeginHandler (00:24AF)
 * Switches to Bank $1C and invokes AnimateDialogClosing ($4AA8).
 */
void DialogClosingBeginHandler(GBState *gb, void (*func_01C_4AA8)(GBState *));

/**
 * DialogLetterAnimationStartHandler (00:24B7)
 * Decrements scroll delay or clears letter pixels in Bank $1C and advances dialog state.
 */
void DialogLetterAnimationStartHandler(GBState *gb, void (*clear_letter_pixels)(GBState *));

/**
 * ExecuteDialog (00:2321)
 * Main dialog dispatcher; configures background tile, wraps next char position,
 * and dispatches to handler for current wDialogState.
 */
void ExecuteDialog(GBState *gb, void (*state_handlers[15])(GBState *));

/**
 * func_23E4 (00:23E4)
 * Backs up 18x2 BG map tiles under the dialog box to wD500 buffer during dialog opening.
 */
void func_23E4(GBState *gb);

/**
 * DialogScrollingStartHandler (00:2714)
 * Empty handler returning immediately.
 */
void DialogScrollingStartHandler(GBState *gb);

/**
 * DialogScrollingEndHandler (00:2768)
 * Empty handler returning immediately.
 */
void DialogScrollingEndHandler(GBState *gb);

/**
 * SkipDialog (00:278B)
 * Sets wDialogAskSelectionIndex to 2 and transitions dialog to closing state.
 */
void SkipDialog(GBState *gb);

/**
 * DialogChoiceHandler (00:2793)
 * Handles input on ask/choice dialogs, toggling selection and advancing on A press.
 */
void DialogChoiceHandler(GBState *gb, void (*draw_dialog_choice_marker)(GBState *));

/**
 * DrawDialogArrowTrampoline (00:27BB)
 * Switches to Bank $17 and invokes DrawDialogArrow ($7D7C).
 */
void DrawDialogArrowTrampoline(GBState *gb, void (*draw_dialog_arrow)(GBState *));

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_DIALOG_H */
