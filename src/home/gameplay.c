#include "home/gameplay.h"
#include "home/bank.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/joypad.h"
#include "constants/memory.h"

bool CheckPresentSaveScreen(GBState *gb) {
    if (!gb) return false;

    /* If indoor/outdoor transition is running (wTransitionSequenceCounter != 4), don't open */
    if (gb_read(gb, wTransitionSequenceCounter) != 0x04) {
        return false;
    }

    /* If dialog is visible, or wC167 is non-zero, or screen is scrolling room transition */
    uint8_t dialog_state = gb_read(gb, wDialogState);
    uint8_t c167 = gb_read(gb, wC167);
    uint8_t transition_state = gb_read(gb, wRoomTransitionState);
    if ((dialog_state | c167 | transition_state) != 0) {
        return false;
    }

    /* If GameplayType >= GAMEPLAY_INVENTORY (0x0C) */
    uint8_t gameplay_type = gb_read(gb, wGameplayType);
    if (gameplay_type >= GAMEPLAY_INVENTORY) {
        return false;
    }

    /* Check if all 4 buttons (A + B + Start + Select) are pressed */
    uint8_t pressed = gb_read(gb, hPressedButtonsMask);
    if ((pressed & (J_A | J_B | J_START | J_SELECT)) != (J_A | J_B | J_START | J_SELECT)) {
        return false;
    }

    /* Check wD474 and wD464 are zero */
    if (gb_read(gb, wD474) != 0 || gb_read(gb, wD464) != 0) {
        return false;
    }

    /* Present save screen */
    gb_write(gb, wTransitionSequenceCounter, 0);
    gb_write(gb, wC16C, 0);
    gb_write(gb, wDialogState, 0);
    gb_write(gb, wGameplaySubtype, 0);
    gb_write(gb, wGameplayType, GAMEPLAY_FILE_SAVE);

    return true;
}

void returnFromGameplayHandler(GBState *gb, void (*execute_dialog)(GBState *), void (*load_bg_palettes)(GBState *)) {
    if (!gb) return;

    /* Present dialog if needed */
    SwitchBank(gb, 0x0F); /* BANK(FontTiles) */
    if (execute_dialog) {
        execute_dialog(gb);
    }

    /* If on DMG, return now to main game loop */
    if (gb_read(gb, hIsGBC) == 0) {
        return;
    }

    /* Load Background palettes if needed */
    SwitchBank(gb, 0x24); /* BANK(LoadBGPalettes) */
    if (load_bg_palettes) {
        load_bg_palettes(gb);
    }
}
