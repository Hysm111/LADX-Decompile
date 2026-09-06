#include "home/bank.h"
#include "constants/memory.h"
#include "constants/gameplay.h"
#include "constants/joypad.h"
#include "constants/sfx.h"
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

void ExecuteDialog(GBState *gb, void (*state_handlers[15])(GBState *)) {
    if (!gb) return;

    uint8_t state = gb_read(gb, wDialogState);
    if (state == 0) return;

    uint8_t bg_tile = (gb_read(gb, wGameplayType) == GAMEPLAY_CREDITS)
                    ? DIALOG_BG_TILE_LIGHT
                    : DIALOG_BG_TILE_DARK;
    gb_write(gb, hDialogBackgroundTile, bg_tile);

    uint8_t char_hi = gb_read(gb, wDialogCharacterIndexHi);
    uint8_t char_lo = gb_read(gb, wDialogCharacterIndex);
    uint8_t next_pos;
    if (char_hi != 0 || char_lo >= 0x20) {
        next_pos = (char_lo & 0x0F) | 0x10;
    } else {
        next_pos = char_lo;
    }
    gb_write(gb, wDialogNextCharPosition, next_pos);

    uint8_t state_index = (state & (uint8_t)~DIALOG_BOX_BOTTOM_FLAG) - 1;
    if (state_index < 15 && state_handlers && state_handlers[state_index]) {
        state_handlers[state_index](gb);
    }
}

void func_23E4(GBState *gb) {
    if (!gb) return;

    static const uint8_t s_dialog_save_table[20] = {
        0xCF, 0x53,
        0x00, 0x24,
        0x48, 0x00,
        0x24, 0x48, 0x98, 0x98, 0x98, 0x99,
        0x99, 0x99, 0x21, 0x61, 0xA1, 0x41, 0x81, 0xC1
    };

    uint8_t state = gb_read(gb, wDialogState);
    uint8_t de_idx = state & (uint8_t)~DIALOG_BOX_BOTTOM_FLAG;
    if ((state & DIALOG_BOX_BOTTOM_FLAG) != 0) {
        de_idx += 3;
    }

    uint8_t bc_offset = s_dialog_save_table[de_idx];
    uint16_t dest_bc = (uint16_t)(wD500 + bc_offset);

    uint8_t bg_high_offset = s_dialog_save_table[6 + de_idx];
    uint8_t bg_low_offset = s_dialog_save_table[12 + de_idx];

    uint8_t src_l = (uint8_t)(gb_read(gb, wBGOriginLow) + bg_low_offset);
    gb_write(gb, hMultiPurpose0, src_l);
    uint8_t src_h = (uint8_t)(gb_read(gb, wBGOriginHigh) + bg_high_offset);

    uint16_t src_hl = ((uint16_t)src_h << 8) | src_l;

    if (gb_read(gb, hIsGBC) == 0) {
        for (uint8_t row = 0; row < 2; row++) {
            for (uint8_t col = 0; col < 0x12; col++) {
                uint8_t tile = gb_read(gb, src_hl);
                gb_write(gb, dest_bc, tile);
                dest_bc++;
                src_hl++;
                if ((src_hl & 0x1F) == 0) {
                    src_hl = (src_hl & 0xFF00) | ((src_hl - 1) & 0xE0);
                }
            }
            uint8_t mp0 = gb_read(gb, hMultiPurpose0);
            uint8_t next_mp0 = (uint8_t)(mp0 + 0x20);
            gb_write(gb, hMultiPurpose0, next_mp0);
            if (next_mp0 < mp0) {
                src_h++;
            }
            src_hl = ((uint16_t)src_h << 8) | next_mp0;
        }
    } else {
        for (uint8_t row = 0; row < 2; row++) {
            for (uint8_t col = 0; col < 0x12; col++) {
                uint8_t tile = gb_read(gb, src_hl);
                gb_write(gb, dest_bc, tile);

                gb_write(gb, rVBK, 1);
                gb_write(gb, rSVBK, 2);
                uint8_t attr = gb_read(gb, src_hl);
                gb_write(gb, dest_bc, attr);
                gb_write(gb, rVBK, 0);
                gb_write(gb, rSVBK, 0);

                dest_bc++;
                src_hl++;
                if ((src_hl & 0x1F) == 0) {
                    src_hl = (src_hl & 0xFF00) | ((src_hl - 1) & 0xE0);
                }
            }
            uint8_t mp0 = gb_read(gb, hMultiPurpose0);
            uint8_t next_mp0 = (uint8_t)(mp0 + 0x20);
            gb_write(gb, hMultiPurpose0, next_mp0);
            if (next_mp0 < mp0) {
                src_h++;
            }
            src_hl = ((uint16_t)src_h << 8) | next_mp0;
        }
    }
}

void DialogScrollingStartHandler(GBState *gb) {
    (void)gb;
}

void DialogScrollingEndHandler(GBState *gb) {
    (void)gb;
}

void SkipDialog(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wDialogAskSelectionIndex, 0x02);
    UpdateDialogState(gb);
}

void DialogChoiceHandler(GBState *gb, void (*draw_dialog_choice_marker)(GBState *)) {
    if (!gb) return;

    uint8_t joy = gb_read(gb, hJoypadState);
    if ((joy & J_A) != 0) {
        UpdateDialogState(gb);
        return;
    }

    if ((joy & (J_RIGHT | J_LEFT)) != 0) {
        uint8_t sel = gb_read(gb, wDialogAskSelectionIndex);
        sel = (uint8_t)((sel + 1) & 0x01);
        gb_write(gb, wDialogAskSelectionIndex, sel);
        gb_write(gb, hJingle, JINGLE_MOVE_SELECTION);
    }

    if ((gb_read(gb, hFrameCounter) & 0x10) != 0) {
        SwitchBank(gb, 0x17);
        if (draw_dialog_choice_marker) {
            draw_dialog_choice_marker(gb);
        }
    }
}

void DrawDialogArrowTrampoline(GBState *gb, void (*draw_dialog_arrow)(GBState *)) {
    if (!gb) return;
    SwitchBank(gb, 0x17);
    if (draw_dialog_arrow) {
        draw_dialog_arrow(gb);
    }
}
