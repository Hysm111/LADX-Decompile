#include "bank1/face_shrine_mural.h"
#include "bank1/room_transition.h"
#include "home/audio.h"
#include "home/dialog.h"
#include "home/link.h"
#include "constants/gameplay.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/joypad.h"
#include "constants/sfx.h"
#include "constants/memory.h"
#include <string.h>

void FaceShrineMuralStage0Handler(GBState *gb) {
    if (!gb) return;
    IncrementGameplaySubtype(gb);
    if (gb_read_hram(gb, hIsGBC) != 0) {
        for (uint16_t i = 0; i < 0x80; i++) {
            gb_write(gb, rSVBK, 0);
            uint8_t b = gb_read(gb, (uint16_t)(wBGPal1 + i));
            gb_write(gb, rSVBK, 3);
            gb_write(gb, (uint16_t)(wBGPal1 + i), b);
        }
        gb_write(gb, rSVBK, 0);
    }
    FaceShrineMuralStage1Handler(gb);
}

void FaceShrineMuralStage1Handler(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wC167, 1);
    func_1A22(gb, NULL, NULL);
    if (gb_read(gb, wTransitionSequenceCounter) == 4) {
        func_001_5888(gb);
        gb_write_hram(gb, hVolumeRight, 0x03);
        gb_write_hram(gb, hVolumeLeft, 0x30);
        IncrementGameplaySubtype(gb);
        gb_write(gb, wScrollXOffset, 0);
        gb_write(gb, wTilesetToLoad, TILESET_FACE_SHRINE_MURAL);
    }
}

void FaceShrineMuralStage2Handler(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wBGMapToLoad, TILEMAP_FACE_SHRINE_MURAL);
    gb_write(gb, wWindowY, 0xFF);
    gb_write_hram(gb, hBaseScrollX, 0);
    gb_write_hram(gb, hBaseScrollY, 0);
    gb_write(gb, wTransitionSequenceCounter, 0);
    gb_write(gb, wC16C, 0);
    gb_write(gb, wPaletteUnknownE, 1);
    IncrementGameplaySubtype(gb);
}

void FaceShrineMuralStage3Handler(GBState *gb) {
    if (!gb) return;
    func_1A39(gb, NULL, NULL);
    if (gb_read(gb, wTransitionSequenceCounter) == 4) {
        IncrementGameplaySubtype(gb);
        gb_write(gb, wC3C4, 0);
    }
}

void FaceShrineMuralStage4Handler(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wDialogState) != 0) {
        return;
    }
    uint8_t count = (uint8_t)(gb_read(gb, wC3C4) + 1);
    gb_write(gb, wC3C4, count);
    if (count == 0) {
        IncrementGameplaySubtype(gb);
    } else if (count == 0x80) {
        OpenDialogInTable0(gb, 0xE7);
    }
}

void FaceShrineMuralStage5Handler(GBState *gb) {
    if (!gb) return;
    uint8_t joy = gb_read_hram(gb, hJoypadState);
    if (joy & (J_A | J_B | J_START)) {
        gb_write_hram(gb, hJingle, JINGLE_VALIDATE);
        func_001_68D9(gb);
    }
}

void FaceShrineMuralEntryPoint(GBState *gb) {
    if (!gb) return;
    uint8_t subtype = gb_read(gb, wGameplaySubtype);
    switch (subtype) {
        case 0: FaceShrineMuralStage0Handler(gb); break;
        case 1: FaceShrineMuralStage1Handler(gb); break;
        case 2: FaceShrineMuralStage2Handler(gb); break;
        case 3: FaceShrineMuralStage3Handler(gb); break;
        case 4: FaceShrineMuralStage4Handler(gb); break;
        case 5: FaceShrineMuralStage5Handler(gb); break;
        case 6: FileSaveFadeOut(gb);              break;
        default: break;
    }
}
