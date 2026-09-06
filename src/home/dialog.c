#include "home/bank.h"
#include "constants/memory.h"
#include "constants/gameplay.h"
#include "constants/joypad.h"
#include "constants/sfx.h"
#include "constants/gfx.h"
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

static void ScrollDialogLine(GBState *gb, const uint8_t low_table[2], const uint8_t high_table[2]) {
    uint8_t de_idx = ((gb_read(gb, wDialogState) & DIALOG_BOX_BOTTOM_FLAG) != 0) ? 1 : 0;
    uint8_t b = (uint8_t)(gb_read(gb, wBGOriginHigh) + high_table[de_idx]);
    uint8_t c = (uint8_t)(gb_read(gb, wBGOriginLow) + low_table[de_idx]);

    for (uint8_t e = 0x10; e > 0; e--) {
        uint16_t src1 = ((uint16_t)b << 8) | c;
        uint16_t dst1 = ((uint16_t)b << 8) | (uint8_t)(c - 0x20);
        gb_write(gb, dst1, gb_read(gb, src1));

        uint16_t src2 = ((uint16_t)b << 8) | (uint8_t)(c + 0x20);
        uint16_t dst2 = ((uint16_t)b << 8) | c;
        gb_write(gb, dst2, gb_read(gb, src2));

        uint16_t dst3 = ((uint16_t)b << 8) | (uint8_t)(c + 0x20);
        gb_write(gb, dst3, gb_read(gb, hDialogBackgroundTile));

        c++;
        if ((c & 0x1F) == 0) {
            c -= 0x20;
        }
    }
}

void DialogBeginScrolling(GBState *gb) {
    if (!gb) return;
    static const uint8_t low[2] = { 0x62, 0x82 };
    static const uint8_t high[2] = { 0x98, 0x99 };
    ScrollDialogLine(gb, low, high);
    gb_write(gb, wDialogScrollDelay, 0x08);
    IncrementDialogStateAndReturn(gb);
}

void DialogFinishScrolling(GBState *gb) {
    if (!gb) return;
    static const uint8_t low[2] = { 0x42, 0x62 };
    static const uint8_t high[2] = { 0x98, 0x99 };
    ScrollDialogLine(gb, low, high);
    gb_write(gb, wDialogScrollDelay, 0x08);
    IncrementDialogStateAndReturn(gb);

    gb_write(gb, wDialogState, (uint8_t)((gb_read(gb, wDialogState) & 0xF0) | DIALOG_LETTER_IN_1));
    gb_write(gb, wDialogScrollDelay, 0);
}

void DialogBreakHandler(GBState *gb, void (*draw_dialog_arrow)(GBState *)) {
    if (!gb) return;

    uint8_t char_idx = gb_read(gb, wDialogCharacterIndex);
    if ((char_idx & 0x1F) == 0) {
        uint8_t next_char = gb_read(gb, wDialogNextChar);
        if (next_char == 0xFF) {
            gb_write(gb, wDrawCommand, 0);
            gb_write(gb, wDialogState, (uint8_t)((gb_read(gb, wDialogState) & 0xF0) | DIALOG_END));
            return;
        }
        if (next_char == 0xFE) {
            gb_write(gb, wDrawCommand, 0);
            gb_write(gb, wDialogState, (uint8_t)((gb_read(gb, wDialogState) & 0xF0) | DIALOG_CHOICE));
            gb_write(gb, hJingle, JINGLE_DIALOG_BREAK);
            return;
        }

        if (gb_read(gb, wDialogIsWaitingForButtonPress) == 0) {
            gb_write(gb, wDialogIsWaitingForButtonPress, 1);
            gb_write(gb, hJingle, JINGLE_DIALOG_BREAK);
        }

        DrawDialogArrowTrampoline(gb, draw_dialog_arrow);

        uint8_t joy = gb_read(gb, hJoypadState);
        if ((joy & J_A) != 0) {
            // Proceed to build draw command
        } else if ((joy & J_B) != 0) {
            SwitchBank(gb, 0x1C);
            if (gb_read(gb, wGameplayType) == GAMEPLAY_WORLD_MAP) {
                SkipDialog(gb);
                return;
            }
            uint16_t dlg_id = (uint16_t)gb_read(gb, wDialogIndex) | ((uint16_t)gb_read(gb, wDialogIndexHi) << 8);
            uint8_t bank = gb_read(gb, (uint16_t)(0x4741 + dlg_id));
            if (bank == 0) {
                SkipDialog(gb);
                return;
            }
            return;
        } else {
            return;
        }
    }

    // .buildDrawCommand
    uint8_t de_idx = ((gb_read(gb, wDialogState) & DIALOG_BOX_BOTTOM_FLAG) != 0) ? 1 : 0;
    static const uint8_t box_high[2] = { 0x98, 0x99 };
    static const uint8_t box_low[2]  = { 0x22, 0x42 };

    uint8_t dst_hi = (uint8_t)(gb_read(gb, wBGOriginHigh) + box_high[de_idx]);
    uint8_t dst_lo = (uint8_t)(gb_read(gb, wBGOriginLow)  + box_low[de_idx]);

    gb_write(gb, wDrawCommand, dst_hi);
    gb_write(gb, (uint16_t)(wDrawCommand + 1), dst_lo);
    gb_write(gb, (uint16_t)(wDrawCommand + 2), (uint8_t)(DC_FILL_ROW | 0x0F));
    gb_write(gb, (uint16_t)(wDrawCommand + 3), gb_read(gb, hDialogBackgroundTile));
    gb_write(gb, (uint16_t)(wDrawCommand + 4), 0);

    IncrementDialogState(gb);
}

static const uint8_t s_dialog_char_x[32] = {
    0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
    0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0,
    0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
    0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0
};

static const uint8_t s_dialog_char_y[32] = {
    0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
    0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
    0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
    0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E
};

static const uint8_t s_data_01C_45A1[32] = {
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
    0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
    0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF
};

static const uint8_t s_data_01C_45C1[64] = {
    0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
    0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
    0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
    0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
    0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99
};

static const uint8_t s_data_01C_4601[64] = {
    0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91,
    0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
    0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9,
    0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1
};

void DialogDrawNextCharacterHandler(GBState *gb) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x1C);
    uint8_t char_idx = (uint8_t)(gb_read(gb, wDialogCharacterIndex) & 0x1F);
    gb_write(gb, (uint16_t)(wDrawCommand + 4), s_dialog_char_y[char_idx]);
    gb_write(gb, (uint16_t)(wDrawCommand + 5), s_dialog_char_x[char_idx]);
    gb_write(gb, (uint16_t)(wDrawCommand + 6), 0x0F);

    uint16_t dlg_id = (uint16_t)gb_read(gb, wDialogIndex) | ((uint16_t)gb_read(gb, wDialogIndexHi) << 8);
    uint16_t ptr_addr = (uint16_t)(0x4001 + (dlg_id << 1));
    uint16_t text_addr = (uint16_t)gb_read(gb, ptr_addr) | ((uint16_t)gb_read(gb, (uint16_t)(ptr_addr + 1)) << 8);

    uint8_t bank = (uint8_t)(gb_read(gb, (uint16_t)(0x4741 + dlg_id)) & 0x3F);
    gb_write(gb, rSelectROMBank, bank);

    uint16_t full_char_idx = (uint16_t)gb_read(gb, wDialogCharacterIndex) | ((uint16_t)gb_read(gb, wDialogCharacterIndexHi) << 8);
    uint8_t ch = gb_read(gb, (uint16_t)(text_addr + full_char_idx));
    uint8_t next_ch = gb_read(gb, (uint16_t)(text_addr + full_char_idx + 1));
    gb_write(gb, wDialogNextChar, next_ch);

    ReloadSavedBank(gb);

    gb_write(gb, hMultiPurpose0, ch);

    if (ch == 0xFE) { /* <ask> */
        gb_write(gb, wDrawCommand, 0);
        gb_write(gb, wDialogState, (uint8_t)((gb_read(gb, wDialogState) & 0xF0) | DIALOG_CHOICE));
        gb_write(gb, hJingle, JINGLE_DIALOG_BREAK);
        return;
    }

    if (ch == 0xFF) { /* @ */
        gb_write(gb, wDrawCommand, 0);
        gb_write(gb, wDialogState, (uint8_t)((gb_read(gb, wDialogState) & 0xF0) | DIALOG_END));
        return;
    }

    if (ch != ' ') {
        uint8_t sfx = gb_read(gb, wDialogSFX);
        uint8_t mask;
        if (sfx == WAVE_SFX_TEXT_PRINT) {
            mask = 0x01;
        } else if (sfx == WAVE_SFX_OWL_HOOT) {
            mask = 0x07;
        } else {
            mask = 0x03;
        }
        if (((gb_read(gb, wDialogCharacterIndex) + 4) & mask) == 0) {
            gb_write(gb, hWaveSfx, sfx);
        }
    }

    if (ch == '#') {
        uint8_t name_idx = gb_read(gb, wNameIndex);
        uint8_t next_idx = (uint8_t)(name_idx + 1);
        if (next_idx == NAME_LENGTH) {
            next_idx = 0;
        }
        gb_write(gb, wNameIndex, next_idx);

        static const uint8_t s_thief[5] = { 'T'+1, 'H'+1, 'I'+1, 'E'+1, 'F'+1 };
        uint8_t raw;
        if (gb_read(gb, wIsThief) != 0) {
            raw = s_thief[name_idx];
        } else {
            raw = gb_read(gb, (uint16_t)(wName + name_idx));
        }
        ch = (uint8_t)(raw - 1);
        if (ch == 0xFF) {
            ch = ' ';
        }
    }

    gb_write(gb, hMultiPurpose1, ch);

    gb_write(gb, rSelectROMBank, 0x1C);
    uint8_t tile_idx = gb_read(gb, (uint16_t)(ADDR_CodepointToTileMap + ch));
    ReloadSavedBank(gb);

    uint16_t font_addr = (uint16_t)(0x5000 + ((uint16_t)tile_idx << 4));
    for (uint8_t i = 0; i < 0x10; i++) {
        gb_write(gb, (uint16_t)(wDrawCommand + 7 + i), gb_read(gb, (uint16_t)(font_addr + i)));
    }
    gb_write(gb, (uint16_t)(wDrawCommand + 7 + 0x10), 0);

    /* .noDiacritic */
    uint16_t new_idx = full_char_idx + 1;
    gb_write(gb, wDialogCharacterIndex, (uint8_t)(new_idx & 0xFF));
    gb_write(gb, wDialogCharacterIndexHi, (uint8_t)(new_idx >> 8));

    gb_write(gb, wDialogIsWaitingForButtonPress, 0);

    if (gb_read(gb, wDialogNextCharPosition) == 0x1F) {
        IncrementDialogStateAndReturn(gb);
    } else {
        gb_write(gb, wDialogState, (uint8_t)((gb_read(gb, wDialogState) & 0xF0) | DIALOG_LETTER_IN_1));
        gb_write(gb, wDialogScrollDelay, 0);
    }
}

void DialogLetterAnimationEndHandler(GBState *gb) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x1C);
    uint8_t state = gb_read(gb, wDialogState);
    uint8_t pos = gb_read(gb, wDialogNextCharPosition);
    if ((state & DIALOG_BOX_BOTTOM_FLAG) != 0) {
        pos += 0x20;
    }

    uint8_t dest_hi = (uint8_t)(gb_read(gb, wBGOriginHigh) + s_data_01C_45C1[pos]);
    gb_write(gb, wDrawCommand, dest_hi);
    gb_write(gb, wC175, dest_hi);

    uint8_t val = s_data_01C_4601[pos];
    uint8_t e = (uint8_t)((val & 0xE0) + 0x20);
    uint8_t d = (uint8_t)(gb_read(gb, wBGOriginLow) + val);
    if (d >= e) {
        d -= 0x20;
    }
    gb_write(gb, wC176, d);
    gb_write(gb, (uint16_t)(wDrawCommand + 1), d);
    gb_write(gb, (uint16_t)(wDrawCommand + 2), 0);

    uint8_t char_idx = (uint8_t)(gb_read(gb, wDialogCharacterIndex) & 0x1F);
    gb_write(gb, (uint16_t)(wDrawCommand + 3), s_data_01C_45A1[char_idx]);

    IncrementDialogState(gb);
    DialogDrawNextCharacterHandler(gb);
}
