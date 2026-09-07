#include "constants/joypad.h"
#include "bank1/world_map.h"
#include "bank1/room_transition.h"
#include "home/animated_tiles.h"
#include "home/entities.h"
#include "home/link.h"
#include "home/dialog.h"
#include "constants/sfx.h"
#include "constants/gameplay.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/rooms.h"

const uint8_t MapLocationNamesTable[64] = {
    /* 00 */ 0x6C, 0x6C, 0x6C, 0x6B, 0x6C, 0x6C, 0x6C, 0x6C,
    /* 20 */ 0x76, 0x76, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79,
    /* 40 */ 0x6A, 0x6A, 0x72, 0x7A, 0x78, 0x78, 0x71, 0x71,
    /* 60 */ 0x6A, 0x6A, 0x72, 0x70, 0x78, 0x78, 0x71, 0x71,
    /* 80 */ 0x6A, 0x6E, 0x69, 0x69, 0x69, 0x69, 0x77, 0x71,
    /* A0 */ 0x6E, 0x6E, 0x69, 0x69, 0x69, 0x69, 0x77, 0x77,
    /* C0 */ 0x7B, 0x7B, 0x6D, 0x62, 0x74, 0x74, 0x6F, 0x68,
    /* E0 */ 0x73, 0x73, 0x73, 0x74, 0x74, 0x74, 0x75, 0x68,
};

const uint8_t MapSpecialLocationNamesLookupTable[80] = {
    /* 00 - Owl reminders */
    0x00, 0xD9, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD,
    /* 10 - Dungeon icons */
    0x00, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 20 - Shop icons */
    0x00, 0x7C, 0x67, 0x00, 0x00, 0x80, 0x65, 0x00, 0x64, 0x88, 0x7D, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 30 - "!" icons */
    0x00, 0x5E, 0x5F, 0x7F, 0x7E, 0x7D, 0x82, 0x84, 0x85, 0x86, 0x87, 0x81, 0x66, 0xA7, 0x5E, 0x63,
    /* 40 - "!" icons */
    0x00, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t MapSpecialLocationNamesTable[256] = {
    /* 00 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x0E, 0x00, 0x39, 0x00, 0x00, 0x00, 0x17, 0x00,
    /* 10 */ 0x18, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 20 */ 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00,
    /* 30 */ 0x33, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x05, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29,
    /* 40 */ 0x00, 0x03, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x00, 0x00, 0x00, 0x00,
    /* 50 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 60 */ 0x00, 0x00, 0x00, 0x00, 0x0D, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 70 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 80 */ 0x02, 0x21, 0x3B, 0x36, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x00, 0x37, 0x00, 0x16, 0x00, 0x00, 0x00,
    /* 90 */ 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x0B, 0x09, 0x00, 0x00,
    /* A0 */ 0x00, 0x35, 0x3C, 0x00, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00,
    /* B0 */ 0x3A, 0x34, 0x3D, 0x28, 0x00, 0x13, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* C0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* D0 */ 0x00, 0x00, 0x04, 0x11, 0x00, 0x00, 0x38, 0x00, 0x00, 0x15, 0x00, 0x3D, 0x00, 0x00, 0x00, 0x00,
    /* E0 */ 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
    /* F0 */ 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void PlayValidationJingle(GBState *gb) {
    if (!gb) return;
    gb_write(gb, hJingle, JINGLE_VALIDATE);
}

void PlayValidationJingleAndReturn(GBState *gb) {
    PlayValidationJingle(gb);
}

void func_001_5A59(GBState *gb) {
    if (!gb) return;
    uint8_t room = gb_read(gb, hMapRoom);
    uint8_t spec = MapSpecialLocationNamesTable[room];
    uint8_t dialog = MapSpecialLocationNamesLookupTable[spec];
    OpenDialogInTable0(gb, dialog);
}

void WorldMapState0Handler(GBState *gb) {
    if (!gb) return;
    IncrementGameplaySubtype(gb);

    if (gb_read(gb, hIsGBC) != 0) {
        gb_write(gb, rSVBK, 3);
        uint8_t arrow_shifted = gb_read(gb, wIsFileSelectionArrowShifted);
        if (arrow_shifted == 0) {
            for (uint16_t i = 0; i < 0x80; i++) {
                gb_write(gb, rSVBK, 0);
                uint8_t b = gb_read(gb, (uint16_t)(wBGPal1 + i));
                gb_write(gb, rSVBK, 3);
                gb_write(gb, (uint16_t)(wBGPal1 + i), b);
            }
            gb_write(gb, wIsFileSelectionArrowShifted, 1);
        }
        gb_write(gb, rSVBK, 0);
    }

    WorldMapState1Handler(gb);
}

void WorldMapState1Handler(GBState *gb) {
    if (!gb) return;

    DrawLinkSprite(gb);
    AnimateEntitiesAndRestoreBank01(gb, NULL);
    func_1A22(gb, NULL, NULL);

    if (gb_read(gb, wTransitionSequenceCounter) != 4) {
        return;
    }

    gb_write(gb, hVolumeRight, 3);
    gb_write(gb, hVolumeLeft, 0x30);
    IncrementGameplaySubtype(gb);
    gb_write(gb, wTransitionSequenceCounter, 0);
    gb_write(gb, wC16C, 0);
    gb_write(gb, hBaseScrollX, 0);
    gb_write(gb, wScrollXOffset, 0);
    gb_write(gb, hBaseScrollY, 0);
    gb_write(gb, wInventoryAppearing, 0);
    gb_write(gb, wC1B2, 0);
    gb_write(gb, wC1B3, 0);

    uint8_t room = gb_read(gb, wDB54);
    gb_write(gb, wDBB4, room);

    uint8_t val = MapSpecialLocationNamesTable[room];
    uint8_t a = 0;
    if (val != 0) {
        a = (uint8_t)(((val >> 4) & 0x07) + 1);
        if (a == 1) {
            a = 0;
            if (gb_read(gb, wC5A2) == 0) {
                uint8_t status = gb_read(gb, (uint16_t)(wOverworldRoomStatus + room));
                if ((status & OW_ROOM_STATUS_OWL_TALKED) != 0) {
                    a = 1;
                }
            }
        }
    }

    gb_write(gb, wC1B1, a);
    gb_write(gb, wC1B4, gb_read(gb, wDBB4));

    uint8_t lcdc = (uint8_t)(gb_read(gb, rLCDC) & ~LCDCF_WINON);
    gb_write(gb, wLCDControl, lcdc);
    gb_write(gb, rLCDC, lcdc);

    func_001_5888(gb);
    gb_write(gb, wBGMapToLoad, TILEMAP_WORLD_MAP);
}

void WorldMapState2Handler(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wTilesetToLoad, TILESET_WORLD_MAP);
    IncrementGameplaySubtype(gb);
}

void WorldMapState3Handler(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wTilesetToLoad, TILESET_WORLD_MAP_TILEMAP);
    gb_write(gb, wPaletteUnknownE, 1);
    IncrementGameplaySubtype(gb);
}

void WorldMapState4Handler(GBState *gb) {
    if (!gb) return;
    func_1A39(gb, NULL, NULL);
    if (gb_read(gb, wTransitionSequenceCounter) != 4) {
        return;
    }
    IncrementGameplaySubtype(gb);
    PlayValidationJingle(gb);
}

void playMoveSelectionJingle(GBState *gb) {
    if (!gb) return;
    gb_write(gb, hJingle, JINGLE_MOVE_SELECTION);
}

void MoveSelect(GBState *gb) {
    if (!gb) return;
    if ((gb_read(gb, hJoypadState) & (J_UP | J_DOWN)) != 0) {
        playMoveSelectionJingle(gb);
    }
}

void label_001_5B3F(GBState *gb) {
    if (!gb) return;

    uint8_t room = gb_read(gb, wDBB4);
    uint8_t e = (uint8_t)(((room >> 1) & 0x78) + 0x14);
    uint8_t swapped = (uint8_t)((room << 4) | (room >> 4));
    uint8_t d = (uint8_t)(((swapped >> 1) & 0x78) + 0x14);

    uint16_t oam_addr = (uint16_t)(wDynamicOAMBuffer + 0x50);
    gb_write(gb, oam_addr + 0, e);
    gb_write(gb, oam_addr + 1, d);
    gb_write(gb, oam_addr + 2, 0xF0);
    gb_write(gb, oam_addr + 3, 0x00);

    gb_write(gb, oam_addr + 4, e);
    gb_write(gb, oam_addr + 5, (uint8_t)(d + 0x08));
    gb_write(gb, oam_addr + 6, 0xF0);
    gb_write(gb, oam_addr + 7, 0x20);

    if ((gb_read(gb, hFrameCounter) & 0x10) != 0) {
        return;
    }

    uint16_t arrow_addr = (uint16_t)(wDynamicOAMBuffer + 0x58);
    gb_write(gb, arrow_addr + 0, (uint8_t)(e + 0x04));
    gb_write(gb, arrow_addr + 1, (uint8_t)(d + 0xF6));
    gb_write(gb, arrow_addr + 2, 0xF6);
    gb_write(gb, arrow_addr + 3, 0x00);

    gb_write(gb, arrow_addr + 4, (uint8_t)(e + 0x04));
    gb_write(gb, arrow_addr + 5, (uint8_t)(d + 0x13));
    gb_write(gb, arrow_addr + 6, 0xF6);
    gb_write(gb, arrow_addr + 7, 0x20);

    gb_write(gb, arrow_addr + 8, (uint8_t)(e + 0xF6));
    gb_write(gb, arrow_addr + 9, (uint8_t)(d + 0x04));
    gb_write(gb, arrow_addr + 10, 0xF8);
    gb_write(gb, arrow_addr + 11, 0x00);

    gb_write(gb, arrow_addr + 12, (uint8_t)(e + 0x0B));
    gb_write(gb, arrow_addr + 13, (uint8_t)(d + 0x04));
    gb_write(gb, arrow_addr + 14, 0xF8);
    gb_write(gb, arrow_addr + 15, 0x40);
}

void func_001_5A71(GBState *gb) {
    if (!gb) return;

    uint8_t old_room = gb_read(gb, wDBB4);
    gb_write(gb, hMultiPurpose0, old_room);

    uint8_t c1b3 = gb_read(gb, wC1B3);
    uint8_t c1b2 = gb_read(gb, wC1B2);
    uint8_t dialog_state = gb_read(gb, wDialogState);

    if ((c1b3 | c1b2 | dialog_state) != 0) {
        label_001_5B3F(gb);
        return;
    }

    uint8_t pressed = gb_read(gb, hPressedButtonsMask);
    uint8_t c = pressed;
    uint8_t hold = gb_read(gb, wC182);

    if ((pressed & 0x0F) != 0) {
        hold++;
        gb_write(gb, wC182, hold);
        if (hold == 0x18) {
            gb_write(gb, wC182, 0x15);
        } else {
            c = gb_read(gb, hJoypadState);
        }
    } else {
        gb_write(gb, wC182, 0);
        c = gb_read(gb, hJoypadState);
    }

    static const uint8_t Data_001_5A6B[4] = {0, 1, 0xFF, 0};
    uint8_t x_dir = c & 0x03;
    uint8_t room = gb_read(gb, wDBB4);
    uint8_t high_y = room & 0xF0;
    uint8_t new_x = (uint8_t)((room + Data_001_5A6B[x_dir]) & 0x0F);
    room = high_y | new_x;
    gb_write(gb, wDBB4, room);

    static const uint8_t Data_001_5A6E[4] = {0, 0xF0, 0x10, 0};
    uint8_t y_dir = (c >> 2) & 0x03;
    room = (uint8_t)(room + Data_001_5A6E[y_dir]);
    gb_write(gb, wDBB4, room);

    if (room == old_room) {
        label_001_5B3F(gb);
        return;
    }

    uint8_t c5a2 = gb_read(gb, wC5A2);
    uint8_t room_status = gb_read(gb, (uint16_t)(wOverworldRoomStatus + room));
    uint8_t free_move = gb_read(gb, wFreeMovementMode);

    if (c5a2 == 0 && room_status == 0 && free_move == 0) {
        gb_write(gb, hJingle, JINGLE_BUMP);
        gb_write(gb, wDBB4, old_room);
        label_001_5B3F(gb);
        return;
    }

    playMoveSelectionJingle(gb);

    uint8_t val = MapSpecialLocationNamesTable[room];
    if (val != 0) {
        uint8_t cat = (uint8_t)(((val >> 4) & 0x07) + 1);
        if (cat == 1) {
            if (gb_read(gb, wC5A2) != 0) {
                goto check_clear_c1b1;
            }
            uint8_t st = gb_read(gb, (uint16_t)(wOverworldRoomStatus + room));
            if ((st & 0x20) == 0) {
                goto check_clear_c1b1;
            }
        }
        if (gb_read(gb, wC1B1) != cat) {
            gb_write(gb, wC1B2, 0x10);
            gb_write(gb, wC1B3, 0);
            gb_write(gb, wC1B1, cat);
        }
        gb_write(gb, wC1B4, gb_read(gb, wDBB4));
        label_001_5B3F(gb);
        return;
    }

check_clear_c1b1:
    if (gb_read(gb, wC1B1) != 0) {
        gb_write(gb, wC1B1, 0);
        gb_write(gb, wC1B3, 0x10);
    }
    label_001_5B3F(gb);
}
