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

void EnableSRAM(GBState *gb) {
    if (!gb) return;

    gb_write(gb, rRAMB, 0x00);
    gb_write(gb, rRAMG, CART_SRAM_ENABLE);
}

void SynchronizeDungeonsItemFlags_trampoline(GBState *gb, void (*sync_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x01);
    if (sync_func) {
        sync_func(gb);
    }
    ReloadSavedBank(gb);
}

uint8_t GetRandomByte(GBState *gb) {
    if (!gb) return 0;

    uint8_t a = (uint8_t)(gb_read(gb, hFrameCounter) + gb_read(gb, wRandomSeed) + gb_read(gb, rLY));
    a = (uint8_t)((a >> 1) | (a << 7));
    gb_write(gb, wRandomSeed, a);
    return a;
}

void ReadJoypadState(GBState *gb) {
    if (!gb) return;

    /* Ignore joypad during map transitions */
    if (gb_read(gb, wRoomTransitionState) != 0) {
        return;
    }

    bool interactive = true;
    if (gb_read(gb, wGameplayType) == GAMEPLAY_WORLD) {
        if (gb_read(gb, wGameplaySubtype) != GAMEPLAY_WORLD_INTERACTIVE) {
            interactive = false;
        } else {
            bool pass_out_read = false;
            if (gb_read(gb, wLinkMotionState) == LINK_MOTION_PASS_OUT) {
                if (gb_read(gb, hLinkPhysicsModifier) == 0x04) {
                    pass_out_read = true;
                }
            }

            if (!pass_out_read) {
                if (gb_read(gb, wTransitionSequenceCounter) != 0x04 ||
                    gb_read(gb, wPaletteUnknownE) != 0) {
                    interactive = false;
                }
            }
        }
    }

    if (!interactive) {
        gb_write(gb, hPressedButtonsMask, 0);
        gb_write(gb, hJoypadState, 0);
        return;
    }

    /* .readState */
    uint8_t c = gb->joypad_input;
    uint8_t prev = gb_read(gb, hPressedButtonsMask);
    uint8_t newly_pressed = (uint8_t)((prev ^ c) & c);
    gb_write(gb, hJoypadState, newly_pressed);
    gb_write(gb, hPressedButtonsMask, c);
    gb_write(gb, rP1, J_BUTTONS | J_DPAD);
}

uint16_t GetIntersectedObjectBGAddress(GBState *gb) {
    if (!gb) return 0;

    uint8_t top = (uint8_t)(gb_read(gb, hIntersectedObjectTop) + gb_read(gb, hBaseScrollY));
    uint16_t row = (top & 0xF8) >> 3;

    uint16_t hl = (uint16_t)(vBGMap0 + row * 0x20);

    uint8_t left = (uint8_t)(gb_read(gb, hIntersectedObjectLeft) + gb_read(gb, hBaseScrollX));
    uint16_t col = (left & 0xF8) >> 3;
    hl = (uint16_t)(hl + col);

    gb_write(gb, hIntersectedObjectBGAddressHigh, (uint8_t)((hl >> 8) & 0xFF));
    gb_write(gb, hIntersectedObjectBGAddressLow, (uint8_t)(hl & 0xFF));

    return hl;
}

void TableJump(GBState *gb, uint8_t index, const JumpTableFunc *table) {
    if (!gb || !table) return;

    if (table[index]) {
        table[index](gb);
    }
}
