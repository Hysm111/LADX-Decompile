#include "bank1/file_menu.h"
#include "bank1/world_map.h"
#include "bank1/room_transition.h"
#include "bank1/save.h"
#include "home/bank.h"
#include "constants/memory.h"
#include "constants/joypad.h"
#include "constants/gameplay.h"
#include "constants/gfx.h"
#include <stddef.h>

const uint8_t Data_001_48E4[4] = {
    0x3B, /* Slot 0 */
    0x53, /* Slot 1 */
    0x6B, /* Slot 2 */
    0x83  /* Copy / Erase */
};

void func_001_4954(GBState *gb) {
    if (!gb) return;

    uint8_t slot = gb_read(gb, wSaveSlot);
    if (slot >= 4) {
        slot = 0;
    }
    uint8_t y = Data_001_48E4[slot];
    uint8_t fc = gb_read(gb, hFrameCounter);

    if (fc & 0x08) {
        /* Frame 1: normal */
        gb_write(gb, wOAMBuffer + 0, y);
        gb_write(gb, wOAMBuffer + 1, 0x18);
        gb_write(gb, wOAMBuffer + 2, 0x00);
        gb_write(gb, wOAMBuffer + 3, 0x00);

        gb_write(gb, wOAMBuffer + 4, y);
        gb_write(gb, wOAMBuffer + 5, 0x20);
        gb_write(gb, wOAMBuffer + 6, 0x02);
        gb_write(gb, wOAMBuffer + 7, 0x00);
    } else {
        /* Frame 2: flipped horizontally */
        gb_write(gb, wOAMBuffer + 0, y);
        gb_write(gb, wOAMBuffer + 1, 0x18);
        gb_write(gb, wOAMBuffer + 2, 0x02);
        gb_write(gb, wOAMBuffer + 3, 0x20);

        gb_write(gb, wOAMBuffer + 4, y);
        gb_write(gb, wOAMBuffer + 5, 0x20);
        gb_write(gb, wOAMBuffer + 6, 0x00);
        gb_write(gb, wOAMBuffer + 7, 0x20);
    }
}

void FileSelectionInteractiveHandler(GBState *gb) {
    if (!gb) return;

    MoveSelect(gb);

    uint8_t joypad = gb_read(gb, hJoypadState);
    if (joypad & (J_A | J_START)) {
        IncrementGameplaySubtype(gb);
        return;
    }

    if (joypad & (J_UP | J_DOWN)) {
        uint8_t max_choice = 2;
        if (gb_read(gb, wSaveFilesCount) != 0) {
            max_choice = 3;
        }

        uint8_t slot = gb_read(gb, wSaveSlot);
        if (joypad & J_UP) {
            if (slot == 0) {
                slot = max_choice;
            } else {
                slot--;
            }
        } else {
            /* DOWN */
            slot++;
            if (slot > max_choice) {
                slot = 0;
            }
        }
        gb_write(gb, wSaveSlot, slot);
    }

    uint8_t slot = gb_read(gb, wSaveSlot);
    if (slot == 3) {
        if (joypad & (J_RIGHT | J_LEFT)) {
            playMoveSelectionJingle(gb);
            uint8_t shifted = gb_read(gb, wIsFileSelectionArrowShifted) ^ 1;
            gb_write(gb, wIsFileSelectionArrowShifted, shifted);
        }

        uint8_t fc = gb_read(gb, hFrameCounter);
        if (!(fc & 0x10)) {
            uint8_t arrow_x = (gb_read(gb, wIsFileSelectionArrowShifted) == 0) ? 0x2C : 0x64;
            gb_write(gb, wOAMBuffer + 8, 0x88);
            gb_write(gb, wOAMBuffer + 9, arrow_x);
            gb_write(gb, wOAMBuffer + 10, 0xBE);
            gb_write(gb, wOAMBuffer + 11, 0x00);
        }
    }

    func_001_4954(gb);
}

void HandleFileSelectionCommand(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wGameplaySubtype, 0);

    if (gb_read(gb, wIsFileSelectionArrowShifted) == 0) {
        gb_write(gb, wGameplayType, GAMEPLAY_FILE_DELETE);
    } else {
        gb_write(gb, wGameplayType, GAMEPLAY_FILE_COPY);
    }

    PlayValidationJingle(gb);
}

void LoadSelectedFile(GBState *gb) {
    if (!gb) return;

    PlayValidationJingle(gb);

    gb_write(gb, wBGPalette, 0);
    gb_write(gb, wOBJ0Palette, 0);
    gb_write(gb, wOBJ1Palette, 0);

    ClearFileMenuBG_trampoline(gb, 1, NULL);

    gb_write(gb, wTilesetToLoad, TILESET_BASE_OVERWORLD);
    IncrementGameplaySubtype(gb);
}

void FileSelectionExecuteChoice(GBState *gb) {
    if (!gb) return;

    uint8_t slot = gb_read(gb, wSaveSlot);
    if (slot == 3) {
        HandleFileSelectionCommand(gb);
        return;
    }

    if (slot >= 3) {
        slot = 0;
    }

    /* Check if the selected slot has a name */
    uint16_t name_addr = (uint16_t)(wSaveSlotNames + slot * 5);
    uint8_t has_name = 0;
    for (uint8_t i = 0; i < 5; i++) {
        if (gb_read(gb, (uint16_t)(name_addr + i)) != 0) {
            has_name = 1;
            break;
        }
    }

    if (has_name) {
        LoadSelectedFile(gb);
    } else {
        gb_write(gb, wGameplaySubtype, 0);
        gb_write(gb, wGameplayType, GAMEPLAY_FILE_NEW);
        PlayValidationJingle(gb);
    }
}

void FileSelectionLoadSavedFile(GBState *gb) {
    LoadSavedFile(gb);
}
