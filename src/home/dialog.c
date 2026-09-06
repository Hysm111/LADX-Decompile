#include "home/bank.h"
#include "constants/memory.h"
#include "constants/gameplay.h"
#include "constants/joypad.h"
#include "home/dialog.h"
#include "constants/hardware.h"
#include "constants/dialog.h"

uint8_t ReadValueInDialogsBank(GBState *gb, uint16_t hl, uint16_t bc) {
    if (!gb) return 0;

    gb_write(gb, rSelectROMBank, BANK_CodepointToTileMap);
    uint8_t val = gb_read(gb, hl + bc);
    gb_write(gb, rSelectROMBank, 0x01);
    return val;
}

uint8_t ReadTileValueFromAsciiTable(GBState *gb, uint16_t bc) {
    return ReadValueInDialogsBank(gb, ADDR_CodepointToTileMap, bc);
}

uint8_t ReadTileValueFromDiacriticsTable(GBState *gb, uint16_t bc) {
    return ReadValueInDialogsBank(gb, ADDR_CodepointToDiacritic, bc);
}

void OpenDialogInTable0(GBState *gb, uint8_t dialog_index) {
    if (!gb) return;
    gb_write(gb, wDialogAskSelectionIndex, 0);
    gb_write(gb, wDialogIndex, dialog_index);
    gb_write(gb, wDialogOpenCloseAnimationFrame, 0);
    gb_write(gb, wDialogCharacterIndex, 0);
    gb_write(gb, wDialogCharacterIndexHi, 0);
    gb_write(gb, wNameIndex, 0);
    gb_write(gb, wDialogIndexHi, 0);
    gb_write(gb, wDialogSFX, 0x0F);

    uint8_t link_y = gb_read(gb, hLinkPositionY);
    uint8_t state = (link_y < 0x48) ? (DIALOG_BOX_BOTTOM_FLAG | DIALOG_OPENING_1) : DIALOG_OPENING_1;
    gb_write(gb, wDialogState, state);
}

void OpenDialogInTable1(GBState *gb, uint8_t dialog_index) {
    if (!gb) return;
    OpenDialogInTable0(gb, dialog_index);
    gb_write(gb, wDialogIndexHi, 0x01);
}

void OpenDialogInTable2(GBState *gb, uint8_t dialog_index) {
    if (!gb) return;
    OpenDialogInTable0(gb, dialog_index);
    gb_write(gb, wDialogIndexHi, 0x02);
}

void DialogOpenAnimationStartHandler(GBState *gb, void (*func_014_5449)(GBState *)) {
    if (!gb) return;
    SwitchBank(gb, 0x14);
    if (func_014_5449) {
        func_014_5449(gb);
    }
}

void DialogOpenAnimationHandler(GBState *gb) {
    (void)gb;
}

void DialogClosingEndHandler(GBState *gb, void (*func_021_53CF)(GBState *)) {
    if (!gb) return;
    gb_write(gb, wDialogState, 0);
    gb_write(gb, wDialogCooldown, DIALOG_COOLDOWN);

    if (gb_read(gb, hIsGBC) == 0) return;
    if (gb_read(gb, wGameplayType) != GAMEPLAY_WORLD) return;
    if (gb_read(gb, wBGPaletteEffectAddress) < 0x08) return;

    SwitchBank(gb, 0x21);
    if (func_021_53CF) {
        func_021_53CF(gb);
    }
}

void DialogOpenAnimationEndHandler(GBState *gb, void (*func_01C_4A2C)(GBState *)) {
    if (!gb) return;
    SwitchBank(gb, 0x1C);
    if (func_01C_4A2C) {
        func_01C_4A2C(gb);
    }
}

void IncrementDialogState(GBState *gb) {
    if (!gb) return;
    uint8_t state = gb_read(gb, wDialogState);
    gb_write(gb, wDialogState, (uint8_t)(state + 1));
}

void IncrementDialogStateAndReturn(GBState *gb) {
    IncrementDialogState(gb);
}

void UpdateDialogState(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wDialogOpenCloseAnimationFrame, 0);

    uint8_t state;
    if (gb_read(gb, wGameplayType) == GAMEPLAY_PHOTO_ALBUM) {
        state = 0;
    } else {
        state = (uint8_t)((gb_read(gb, wDialogState) & 0xF0) | DIALOG_CLOSING_1);
    }
    gb_write(gb, wDialogState, state);
}

void DialogFinishedHandler(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wDialogInteractionLocked) != 0) {
        return;
    }
    uint8_t joy = gb_read(gb, hJoypadState);
    if ((joy & (J_A | J_B)) == 0) {
        return;
    }
    UpdateDialogState(gb);
}

void DialogClosingBeginHandler(GBState *gb, void (*func_01C_4AA8)(GBState *)) {
    if (!gb) return;
    SwitchBank(gb, 0x1C);
    if (func_01C_4AA8) {
        func_01C_4AA8(gb);
    }
}

void DialogLetterAnimationStartHandler(GBState *gb, void (*clear_letter_pixels)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x1C);

    uint8_t delay = gb_read(gb, wDialogScrollDelay);
    if (delay != 0) {
        gb_write(gb, wDialogScrollDelay, (uint8_t)(delay - 1));
        return;
    }

    if (clear_letter_pixels) {
        clear_letter_pixels(gb);
    }
    IncrementDialogStateAndReturn(gb);
}
