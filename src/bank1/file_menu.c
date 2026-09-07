#include "home/dialog.h"
#include "constants/dialog.h"
#include "constants/audio.h"
#include "home/gameplay.h"
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

const uint8_t SaveSlotNameAddresses[3] = {
    0, 5, 10
};

const uint16_t Data_001_49F2[3] = {
    0xA405, /* SaveGame1.main (0xA105) + 0x300 */
    0xA7B2, /* SaveGame2.main (0xA4B2) + 0x300 */
    0xAB5F  /* SaveGame3.main (0xA85F) + 0x300 */
};

void FileCreationInit1Handler(GBState *gb) {
    if (!gb) return;

    IncrementGameplaySubtype(gb);
    gb_write(gb, wTilesetToLoad, TILESET_FILL_TILEMAP);
    gb_write(gb, wDBA8, 0);
    gb_write(gb, wNameEntryCurrentChar, 0);
    gb_write(gb, wSaveSlotNameCharIndex, 0);
}

void FileCreationInit2Handler(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wBGMapToLoad, TILEMAP_MENU_FILE_CREATION);

    gb_write(gb, wDrawCommand + 0, (uint8_t)(FILE_NEW_SAVE_SLOT_INDEX_BG >> 8));
    gb_write(gb, wDrawCommand + 1, (uint8_t)(FILE_NEW_SAVE_SLOT_INDEX_BG & 0xFF));
    gb_write(gb, wDrawCommand + 2, 0);

    uint8_t slot = gb_read(gb, wSaveSlot);
    gb_write(gb, wDrawCommand + 3, (uint8_t)(slot + FILE_NEW_SAVE_SLOT_1_TILE));
    gb_write(gb, wDrawCommand + 4, 0);

    IncrementGameplaySubtype(gb);
}

void WriteByteToSRAM(GBState *gb, uint16_t hl, uint16_t bc, uint8_t val) {
    if (!gb) return;

    EnableSRAM(gb);
    gb_write(gb, (uint16_t)(hl + bc), val);
}

void label_001_4555(GBState *gb) {
    if (!gb) return;

    /* SaveGame1: name at 0xA454, health at 0xA45F, max hearts at 0xA460, death count at 0xA45C */
    for (uint8_t i = 0; i < 5; i++) {
        EnableSRAM(gb);
        gb_write(gb, (uint16_t)(wSaveSlot1Name + i), gb_read(gb, (uint16_t)(0xA454 + i)));
    }
    EnableSRAM(gb);
    gb_write(gb, wFile1Health, gb_read(gb, 0xA45F));
    EnableSRAM(gb);
    gb_write(gb, wFile1MaxHearts, gb_read(gb, 0xA460));
    EnableSRAM(gb);
    gb_write(gb, wFile1DeathCountHigh, gb_read(gb, 0xA45C));
    EnableSRAM(gb);
    gb_write(gb, wFile1DeathCountLow, gb_read(gb, 0xA45D));

    /* SaveGame2: name at 0xA801, health at 0xA80C, max hearts at 0xA80D, death count at 0xA809 */
    for (uint8_t i = 0; i < 5; i++) {
        EnableSRAM(gb);
        gb_write(gb, (uint16_t)(wSaveSlot2Name + i), gb_read(gb, (uint16_t)(0xA801 + i)));
    }
    EnableSRAM(gb);
    gb_write(gb, wFile2Health, gb_read(gb, 0xA80C));
    EnableSRAM(gb);
    gb_write(gb, wFile2MaxHearts, gb_read(gb, 0xA80D));
    EnableSRAM(gb);
    gb_write(gb, wFile2DeathCountHigh, gb_read(gb, 0xA809));
    EnableSRAM(gb);
    gb_write(gb, wFile2DeathCountLow, gb_read(gb, 0xA80A));

    /* SaveGame3: name at 0xABAE, health at 0xABB9, max hearts at 0xABBA, death count at 0xABB6 */
    for (uint8_t i = 0; i < 5; i++) {
        EnableSRAM(gb);
        gb_write(gb, (uint16_t)(wSaveSlot3Name + i), gb_read(gb, (uint16_t)(0xABAE + i)));
    }
    EnableSRAM(gb);
    gb_write(gb, wFile3Health, gb_read(gb, 0xABB9));
    EnableSRAM(gb);
    gb_write(gb, wFile3MaxHearts, gb_read(gb, 0xABBA));
    EnableSRAM(gb);
    gb_write(gb, wFile3DeathCountHigh, gb_read(gb, 0xABB6));
    EnableSRAM(gb);
    gb_write(gb, wFile3DeathCountLow, gb_read(gb, 0xABB7));

    /* Reset to file select screen */
    gb_write(gb, wGameplayType, GAMEPLAY_FILE_SELECT);
    gb_write(gb, wGameplaySubtype, 0);
    gb_write(gb, hBaseScrollY, 0);
    gb_write(gb, hBaseScrollX, 0);
    gb_write(gb, wBGPalette, 0);
    gb_write(gb, wOBJ0Palette, 0);
    gb_write(gb, wOBJ1Palette, 0);
    ClearFileMenuBG_trampoline(gb, 1, NULL);
}

void TransitionToFileMenu(GBState *gb, uint8_t force_music) {
    if (!gb) return;

    gb_write(gb, wForceFileSelectionScreenMusic, force_music);
    label_001_4555(gb);
}

const uint8_t Data_001_4B30[64] = {
    0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
    0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
    0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48,
    0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48,
    0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58,
    0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58,
    0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68,
    0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68
};

const uint8_t Data_001_4B70[64] = {
    0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C, 0x44, 0x4C,
    0x54, 0x5C, 0x64, 0x6C, 0x74, 0x7C, 0x84, 0x8C,
    0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C, 0x44, 0x4C,
    0x54, 0x5C, 0x64, 0x6C, 0x74, 0x7C, 0x84, 0x8C,
    0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C, 0x44, 0x4C,
    0x54, 0x5C, 0x64, 0x6C, 0x74, 0x7C, 0x84, 0x8C,
    0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C, 0x44, 0x4C,
    0x54, 0x5C, 0x64, 0x6C, 0x74, 0x7C, 0x84, 0x8C
};

const uint8_t Data_001_4BB0[5] = {
    0x4C, 0x54, 0x5C, 0x64, 0x6C
};

const uint8_t NameEntryCharacterTable[64] = {
    0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x00, 0x00, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x00, 0x00, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x00, 0x00, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
    0x57, 0x58, 0x59, 0x5A, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x00, 0x00
};

void DrawSaveSlotName(GBState *gb, uint16_t dest_bg, uint16_t name_addr) {
    if (!gb) return;

    uint8_t cmd_size = gb_read(gb, wDrawCommandsSize);
    uint16_t hl = (uint16_t)(wDrawCommand + cmd_size);
    gb_write(gb, wDrawCommandsSize, (uint8_t)(cmd_size + 0x10));

    uint8_t b = (uint8_t)(dest_bg >> 8);
    uint8_t c = (uint8_t)(dest_bg & 0xFF);

    /* Character row */
    gb_write(gb, hl++, b);
    gb_write(gb, hl++, c);
    gb_write(gb, hl++, 0x04); /* length 4 -> 5 tiles */

    for (uint8_t i = 0; i < 5; i++) {
        uint8_t ch = gb_read(gb, (uint16_t)(name_addr + i));
        uint8_t tile;
        if (ch == 0) {
            tile = DIALOG_BG_TILE_DARK; /* 0x7E */
        } else {
            tile = ReadTileValueFromAsciiTable(gb, (uint16_t)(ch - 1));
        }
        gb_write(gb, hl++, tile);
    }

    /* Spacing row above character row */
    gb_write(gb, hl++, b);
    gb_write(gb, hl++, (uint8_t)(c - 0x20));
    gb_write(gb, hl++, 0x04);

    for (uint8_t i = 0; i < 5; i++) {
        gb_write(gb, hl++, DIALOG_BG_TILE_DARK);
    }

    /* Terminator */
    gb_write(gb, hl, 0x00);
}

void func_001_4CDA(GBState *gb) {
    if (!gb) return;

    uint8_t curr = gb_read(gb, wNameEntryCurrentChar);
    if (curr >= 64) curr = 0;
    uint8_t ch = NameEntryCharacterTable[curr];

    uint8_t slot = gb_read(gb, wSaveSlot);
    if (slot >= 3) slot = 0;

    uint8_t idx = gb_read(gb, wSaveSlotNameCharIndex);
    if (idx >= 5) idx = 4;

    uint16_t addr = (uint16_t)(wSaveSlot1Name + slot * 5 + idx);
    gb_write(gb, addr, ch);
}

void func_001_4C8A(GBState *gb) {
    if (!gb) return;

    uint8_t joypad = gb_read(gb, hJoypadState);
    if (joypad & (J_A | J_B)) {
        if (joypad & J_B) {
            PlayValidationJingle(gb);
            uint8_t idx = gb_read(gb, wSaveSlotNameCharIndex);
            if (idx > 0) {
                idx--;
            }
            gb_write(gb, wSaveSlotNameCharIndex, idx);
        } else {
            /* A button */
            PlayValidationJingle(gb);
            func_001_4CDA(gb);
            uint8_t idx = gb_read(gb, wSaveSlotNameCharIndex);
            if (idx < 4) {
                idx++;
            }
            gb_write(gb, wSaveSlotNameCharIndex, idx);
        }
    }

    uint8_t idx = gb_read(gb, wSaveSlotNameCharIndex);
    if (idx >= 5) idx = 4;
    uint8_t e = Data_001_4BB0[idx];

    /* Blink cursor underline every 16 frames: visible when (fc & 0x10) != 0 */
    uint8_t fc = gb_read(gb, hFrameCounter);
    if (fc & 0x10) {
        gb_write(gb, wOAMBuffer + 4, 0x18 + 0x0B); /* 0x23 */
        gb_write(gb, wOAMBuffer + 5, (uint8_t)(e + 0x0C));
        gb_write(gb, wOAMBuffer + 6, 0xE0);
        gb_write(gb, wOAMBuffer + 7, 0x00);
    }
}

void func_001_4BF5(GBState *gb) {
    if (!gb) return;

    uint8_t joypad = gb_read(gb, hJoypadState);
    uint8_t dpad = joypad;

    /* Button repeat logic */
    if (!(dpad & (J_UP | J_DOWN | J_LEFT | J_RIGHT))) {
        uint8_t pressed = gb_read(gb, hPressedButtonsMask) & 0x0F;
        if (!pressed) {
            gb_write(gb, wC182, 0);
        } else {
            uint8_t timer = gb_read(gb, wC182) + 1;
            gb_write(gb, wC182, timer);
            if (timer == 0x18) {
                gb_write(gb, wC182, 0x15);
                dpad = pressed;
            }
        }
    }

    if (dpad & (J_UP | J_DOWN)) {
        playMoveSelectionJingle(gb);
        uint8_t curr = gb_read(gb, wNameEntryCurrentChar);
        if (dpad & J_UP) {
            if (curr < 0x10) {
                curr += 0x40 - 0x10;
            } else {
                curr -= 0x10;
            }
        } else {
            /* DOWN */
            curr += 0x10;
            if (curr >= 0x40) {
                curr -= 0x40;
            }
        }
        gb_write(gb, wNameEntryCurrentChar, curr);
    } else if (dpad & (J_RIGHT | J_LEFT)) {
        playMoveSelectionJingle(gb);
        uint8_t curr = gb_read(gb, wNameEntryCurrentChar);
        if (dpad & J_LEFT) {
            if (curr == 0) {
                curr = 0x3F;
            } else {
                curr--;
            }
        } else {
            /* RIGHT */
            curr++;
            if (curr >= 0x40) {
                curr = 0;
            }
        }
        gb_write(gb, wNameEntryCurrentChar, curr);
    }

    /* Update cursor sprite at wOAMBuffer[0..3] */
    uint8_t curr = gb_read(gb, wNameEntryCurrentChar);
    if (curr >= 64) curr = 0;
    uint8_t y = (uint8_t)(Data_001_4B30[curr] + 0x0B);
    uint8_t x = (uint8_t)(Data_001_4B70[curr] + 0x04);
    gb_write(gb, wOAMBuffer + 0, y);
    gb_write(gb, wOAMBuffer + 1, x);
    gb_write(gb, wOAMBuffer + 2, 0xE0);
    gb_write(gb, wOAMBuffer + 3, 0x00);
}

void FileCreationInteractiveHandler(GBState *gb) {
    if (!gb) return;

    uint8_t slot = gb_read(gb, wSaveSlot);
    if (slot >= 3) slot = 0;

    uint16_t name_addr = (uint16_t)(wSaveSlotNames + SaveSlotNameAddresses[slot]);
    DrawSaveSlotName(gb, 0x984A, name_addr);

    uint8_t joypad = gb_read(gb, hJoypadState);
    if (!(joypad & J_START)) {
        func_001_4BF5(gb);
        func_001_4C8A(gb);
        return;
    }

    PlayValidationJingle(gb);

    /* Easter egg check for "ZELDA" (0x5B, 0x46, 0x4D, 0x45, 0x42) */
    static const uint8_t zelda_name[5] = { 0x5B, 0x46, 0x4D, 0x45, 0x42 };
    uint8_t is_zelda = 1;
    for (uint8_t i = 0; i < 5; i++) {
        if (gb_read(gb, (uint16_t)(name_addr + i)) != zelda_name[i]) {
            is_zelda = 0;
            break;
        }
    }
    if (is_zelda) {
        gb_write(gb, wMusicTrackToPlay, MUSIC_FILE_SELECT_ZELDA);
    }

    /* Write name to SRAM */
    static const uint16_t save_name_offsets[3] = { 0xA454, 0xA801, 0xABAE };
    static const uint16_t save_health_offsets[3] = { 0xA45F, 0xA80C, 0xABB9 };
    static const uint16_t save_max_hearts_offsets[3] = { 0xA460, 0xA80D, 0xABBA };
    static const uint16_t save_death_offsets[3] = { 0xA45C, 0xA809, 0xABB6 };

    uint16_t sram_name = save_name_offsets[slot];
    for (uint8_t i = 0; i < 5; i++) {
        WriteByteToSRAM(gb, sram_name, i, gb_read(gb, (uint16_t)(name_addr + i)));
    }

    /* Write starting health: 0x18 */
    WriteByteToSRAM(gb, save_health_offsets[slot], 0, 0x18);

    /* Write starting max hearts: 0x03 */
    WriteByteToSRAM(gb, save_max_hearts_offsets[slot], 0, 0x03);

    /* Write death count: 0, 0 */
    WriteByteToSRAM(gb, save_death_offsets[slot], 0, 0x00);
    WriteByteToSRAM(gb, save_death_offsets[slot], 1, 0x00);

    label_001_4555(gb);
}

void FileCreationEntryPoint(GBState *gb) {
    if (!gb) return;

    uint8_t subtype = gb_read(gb, wGameplaySubtype);
    switch (subtype) {
        case 0:
            FileCreationInit1Handler(gb);
            break;
        case 1:
            FileCreationInit2Handler(gb);
            break;
        case 2:
            FileCreationInteractiveHandler(gb);
            break;
        default:
            break;
    }
}

const uint8_t Data_001_4F3B[10] = {
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9
};

void CopyDigitsToFileScreenBG(GBState *gb, uint16_t dest_bg, uint8_t high_b, uint8_t low_c) {
    if (!gb) return;

    uint8_t cmd_size = gb_read(gb, wDrawCommandsSize);
    uint16_t hl = (uint16_t)(wDrawCommand + cmd_size);
    gb_write(gb, wDrawCommandsSize, (uint8_t)(cmd_size + 6));

    gb_write(gb, hl++, (uint8_t)(dest_bg >> 8));
    gb_write(gb, hl++, (uint8_t)(dest_bg & 0xFF));
    gb_write(gb, hl++, 0x02); /* length 2 -> 3 bytes */

    /* digit 0 (hundreds): low_c & 0x0F */
    uint8_t d0 = low_c & 0x0F;
    if (d0 > 9) d0 = 9;
    gb_write(gb, hl++, Data_001_4F3B[d0]);

    /* digit 1 (tens): (high_b >> 4) & 0x0F */
    uint8_t d1 = (high_b >> 4) & 0x0F;
    if (d1 > 9) d1 = 9;
    gb_write(gb, hl++, Data_001_4F3B[d1]);

    /* digit 2 (units): high_b & 0x0F */
    uint8_t d2 = high_b & 0x0F;
    if (d2 > 9) d2 = 9;
    gb_write(gb, hl++, Data_001_4F3B[d2]);

    gb_write(gb, hl, 0x00);
}

void CopyDeathCountsToBG(GBState *gb) {
    if (!gb) return;

    uint8_t count = gb_read(gb, wSaveFilesCount);
    if (count & 0x01) {
        CopyDigitsToFileScreenBG(gb, 0x98E7, gb_read(gb, wFile1DeathCountHigh), gb_read(gb, wFile1DeathCountLow));
    }
    if (count & 0x02) {
        CopyDigitsToFileScreenBG(gb, 0x9947, gb_read(gb, wFile2DeathCountHigh), gb_read(gb, wFile2DeathCountLow));
    }
    if (count & 0x04) {
        CopyDigitsToFileScreenBG(gb, 0x99A7, gb_read(gb, wFile3DeathCountHigh), gb_read(gb, wFile3DeathCountLow));
    }

    IncrementGameplaySubtype(gb);
}

void DrawSaveSlot1Name(GBState *gb) {
    DrawSaveSlotName(gb, 0x98C5, wSaveSlot1Name);
}

void DrawSaveSlot2Name(GBState *gb) {
    DrawSaveSlotName(gb, 0x9925, wSaveSlot2Name);
}

void DrawSaveSlot3Name(GBState *gb) {
    DrawSaveSlotName(gb, 0x9985, wSaveSlot3Name);
}

void DrawSaveSlot1MaxHearts(GBState *gb) {
    if (!gb) return;
    if (!(gb_read(gb, wSaveFilesCount) & 0x01)) return;

    gb_write(gb, hMultiPurpose4, 0);
    gb_write(gb, hMultiPurpose2, gb_read(gb, wFile1Health));
    gb_write(gb, hMultiPurpose3, gb_read(gb, wFile1MaxHearts));
    BuildSaveSlotHeartsDrawCommand(gb);
}

void DrawSaveSlot2MaxHearts(GBState *gb) {
    if (!gb) return;
    if (!(gb_read(gb, wSaveFilesCount) & 0x02)) return;

    gb_write(gb, hMultiPurpose4, 1);
    gb_write(gb, hMultiPurpose2, gb_read(gb, wFile2Health));
    gb_write(gb, hMultiPurpose3, gb_read(gb, wFile2MaxHearts));
    BuildSaveSlotHeartsDrawCommand(gb);
}

void DrawSaveSlot3MaxHearts(GBState *gb) {
    if (!gb) return;
    if (!(gb_read(gb, wSaveFilesCount) & 0x04)) return;

    gb_write(gb, hMultiPurpose4, 2);
    gb_write(gb, hMultiPurpose2, gb_read(gb, wFile3Health));
    gb_write(gb, hMultiPurpose3, gb_read(gb, wFile3MaxHearts));
    BuildSaveSlotHeartsDrawCommand(gb);
}

void FileSelectionPrepare2(GBState *gb) {
    FileDeletionState5Handler(gb);
}

void FileSelectionPrepare3(GBState *gb) {
    FileDeletionState6Handler(gb);
}

void FileDeletionState0Handler(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, hIsGBC)) {
        ClearFileMenuBG_trampoline(gb, 1, NULL);
        gb_write(gb, wPaletteDataFlags, 1);
    }
    IncrementGameplaySubtype(gb);
}

void FileDeletionState1Handler(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, hIsGBC)) {
        gb_write(gb, wPaletteDataFlags, 2);
    }
    IncrementGameplaySubtype(gb);
}

void FileDeletionState8Handler(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, hIsGBC)) {
        LoadFileMenuBG_trampoline(gb, NULL);
        gb_write(gb, wPaletteDataFlags, 1);
    }
    IncrementGameplaySubtype(gb);
}

void FileDeletionState9Handler(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, hIsGBC)) {
        gb_write(gb, wPaletteDataFlags, 2);
    }
    IncrementGameplaySubtype(gb);
}

void FileDeletionState2Handler(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wTilesetToLoad, TILESET_FILL_TILEMAP);
    gb_write(gb, wSaveSlot, 0);
    gb_write(gb, wCreditsScratch0, 0);
    IncrementGameplaySubtype(gb);
}

void FileDeletionState3Handler(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wBGMapToLoad, TILEMAP_MENU_FILE_ERASE);
    IncrementGameplaySubtype(gb);
}

void FileDeletionState4Handler(GBState *gb) {
    if (!gb) return;
    DrawSaveSlot1Name(gb);
    DrawSaveSlot2Name(gb);
    DrawSaveSlot3Name(gb);
    IncrementGameplaySubtype(gb);
}

void FileDeletionState5Handler(GBState *gb) {
    if (!gb) return;
    DrawSaveSlot1MaxHearts(gb);
    DrawSaveSlot2MaxHearts(gb);
    IncrementGameplaySubtype(gb);
}

void FileDeletionState6Handler(GBState *gb) {
    if (!gb) return;
    DrawSaveSlot3MaxHearts(gb);
    IncrementGameplaySubtype(gb);
}

void FileDeletionState7Handler(GBState *gb) {
    CopyDeathCountsToBG(gb);
}
