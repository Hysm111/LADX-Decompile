#include "bank1/marin_beach.h"
#include "bank1/room_transition.h"
#include "home/audio.h"
#include "home/dialog.h"
#include "home/gameplay.h"
#include "home/link.h"
#include "constants/dialog.h"
#include "constants/gameplay.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/inventory.h"
#include "constants/memory.h"
#include "constants/sfx.h"
#include "constants/vfx.h"
#include <stddef.h>

const uint8_t Data_001_63AA[16] = {
    0, 0, 0, 0, 0x40, 0x40, 0x40, 0x40, 0x94, 0x94, 0x94, 0x94, 0xE4, 0xE4, 0xE4, 0xE4
};

const uint8_t Data_001_63BA[16] = {
    0, 0, 0, 0, 4, 4, 4, 4, 0x18, 0x18, 0x18, 0x18, 0x1C, 0x1C, 0x1C, 0x1C
};

const uint8_t Data_001_650A[4] = { 0x40, 1, 0x40, 0x21 };
const uint8_t Data_001_650E[4] = { 0x46, 1, 0x48, 1 };

const uint8_t Data_001_6512[4] = { 0x42, 0, 0x44, 0 };
const uint8_t Data_001_6516[4] = { 0x42, 2, 0x44, 2 };
const uint8_t Data_001_651A[4] = { 0x42, 3, 0x44, 3 };

static const uint8_t Data_001_65EF[4] = { 0x50, 0, 0x50, 0x20 };
static const uint8_t Data_001_65F3[4] = { 0x52, 0, 0x52, 0x20 };
static const uint8_t Data_001_65F7[4] = { 0x54, 0, 0x54, 0x20 };
static const uint8_t Data_001_65FB[4] = { 0x56, 0, 0x56, 0x20 };
static const uint8_t Data_001_65FF[4] = { 0x58, 0, 0x58, 0x20 };
static const uint8_t Data_001_6603[4] = { 0x5A, 0, 0x5A, 0x20 };

static const uint8_t * const Data_001_65E3[6] = {
    Data_001_65EF, Data_001_65F3, Data_001_65F7, Data_001_65FB, Data_001_65FF, Data_001_6603
};

const uint8_t Data_001_6607[108] = {
    3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1,
    2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5,
    0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0,
    1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4,
    5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5
};

static const uint8_t Data_001_66E0[4] = { 0x4C, 0, 0x4C, 0x20 };
static const uint8_t Data_001_66E4[4] = { 0x4E, 0, 0x4E, 0x20 };
static const uint8_t Data_001_66E8[4] = { 0x5C, 0, 0x5C, 0x20 };
static const uint8_t Data_001_66EC[4] = { 0x5E, 0, 0x5E, 0x20 };

static const uint8_t * const Data_001_66D8[4] = {
    Data_001_66E0, Data_001_66E4, Data_001_66E8, Data_001_66EC
};

const uint8_t Data_001_66F0[8] = { 1, 0xFF, 1, 0xFF, 0xFE, 2, 1, 0xFF };
const uint8_t Data_001_66F6[7] = { 1, 0xFF, 0x4C, 0x52, 0x58, 0x5C, 0x60 };

void MarinBeachEntryPoint(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wDialogState) == 0) {
        uint8_t c7 = gb_read(gb, wC3C7);
        if (c7 != 0) {
            gb_write(gb, wC3C7, c7 - 1);
        }
    }
    uint8_t c4 = gb_read(gb, wC3C4);
    if (c4 != 0) {
        gb_write(gb, wC3C4, c4 - 1);
    }
    uint8_t subtype = gb_read(gb, wGameplaySubtype);
    switch (subtype) {
        case 0:  MarinBeachPrepare0(gb); break;
        case 1:  MarinBeachPrepare1(gb); break;
        case 2:  MarinBeachPrepare2(gb); break;
        case 3:  MarinBeachPrepare3(gb); break;
        case 4:  MarinBeachScroll1(gb); break;
        case 5:  MarinBeachScroll2(gb); break;
        case 6:  MarinBeachScrollStop(gb); break;
        case 7:  MarinBeachDialog1(gb); break;
        case 8:  MarinBeachPause1(gb); break;
        case 9:  MarinBeachDialog2(gb); break;
        case 10: MarinBeachPause2(gb); break;
        case 11: MarinBeachAreYouListening(gb); break;
        case 12: MarinBeachDialog3(gb); break;
        case 13: MarinBeachDialog4(gb); break;
        case 14: FileSaveFadeOut(gb); break;
        default: break;
    }
}

void MarinBeachPrepare0(GBState *gb) {
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
    MarinBeachPrepare1(gb);
}

void MarinBeachPrepare1(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wC167, 1);
    func_1A22(gb, NULL, NULL);
    if (gb_read(gb, wTransitionSequenceCounter) == 4) {
        IncrementGameplaySubtype(gb);
        gb_write(gb, wScrollXOffset, 0);
        func_001_5888(gb);
        func_001_64FF(gb);
        gb_write(gb, wTilesetToLoad, TILESET_0F);
    }
}

void MarinBeachPrepare2(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wTilesetToLoad, TILESET_MARIN_BEACH);
    gb_write(gb, wC13F, 0);
    IncrementGameplaySubtype(gb);
}

void MarinBeachPrepare3(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wBGMapToLoad, TILEMAP_MARIN_BEACH);
    gb_write(gb, wWindowY, 0xFF);
    gb_write_hram(gb, hBaseScrollX, 0);
    gb_write(gb, wTransitionSequenceCounter, 0);
    gb_write(gb, wC16C, 0);
    gb_write_hram(gb, hBaseScrollY, 0x90);
    gb_write(gb, wNoiseSfxSeaWavesCounter, 0x40);
    gb_write(gb, wD466, 0xA0);
    gb_write(gb, wPaletteUnknownE, 1);

    // Seagull 0
    gb_write(gb, wTranscientVfxPosYTable, 0xE0);
    gb_write(gb, wTranscientVfxPosXTable, 0x00);
    gb_write(gb, wTranscientVfxTypeTable, TRANSCIENT_VFX_WATER_SPLASH);
    gb_write(gb, wC560, 0x0C);
    gb_write(gb, wC550, 0x08);
    gb_write(gb, wTranscientVfxCountdownTable, 0x00);
    gb_write(gb, wD200, 0x00);

    // Seagull 1
    gb_write(gb, wTranscientVfxPosYTable + 1, 0x20);
    gb_write(gb, wTranscientVfxPosXTable + 1, 0xA0);
    gb_write(gb, wTranscientVfxTypeTable + 1, 0x01);
    gb_write(gb, wC560 + 1, 0x08);
    gb_write(gb, wC550 + 1, 0xF8);
    gb_write(gb, wTranscientVfxCountdownTable + 1, 0x40);
    gb_write(gb, wD200 + 1, 0x24);

    // Seagulls 2..6
    static const struct { uint8_t y; uint8_t x; uint8_t d2; } init_seagulls[5] = {
        { 0x48, 0x30, 2 },
        { 0x3C, 0x40, 0 },
        { 0x40, 0x50, 0 },
        { 0x3C, 0x60, 0 },
        { 0x44, 0x68, 0 }
    };
    for (int i = 0; i < 5; i++) {
        int idx = i + 2;
        gb_write(gb, (uint16_t)(wTranscientVfxPosYTable + idx), init_seagulls[i].y);
        gb_write(gb, (uint16_t)(wTranscientVfxPosXTable + idx), init_seagulls[i].x);
        gb_write(gb, (uint16_t)(wTranscientVfxTypeTable + idx), 0x02);
        gb_write(gb, (uint16_t)(wC560 + idx), 0x00);
        gb_write(gb, (uint16_t)(wC550 + idx), 0x00);
        gb_write(gb, (uint16_t)(wTranscientVfxCountdownTable + idx), 0x00);
        gb_write(gb, (uint16_t)(wD200 + idx), init_seagulls[i].d2);
    }
    IncrementGameplaySubtype(gb);
}

void MarinBeachScroll1(GBState *gb) {
    if (!gb) return;
    if (gb_read_hram(gb, hIsGBC) != 0) {
        if ((gb_read_hram(gb, hFrameCounter) & 7) == 0) {
            func_1A39(gb, NULL, NULL);
            if (gb_read(gb, wTransitionSequenceCounter) == 4) {
                IncrementGameplaySubtype(gb);
            }
        }
    } else {
        if ((gb_read_hram(gb, hFrameCounter) & 7) == 0) {
            uint8_t c5 = (uint8_t)(gb_read(gb, wC3C5) + 1);
            gb_write(gb, wC3C5, c5);
            if (c5 == 0x0C) {
                IncrementGameplaySubtype(gb);
            }
        }
        uint8_t offset = (uint8_t)((gb_read_hram(gb, hFrameCounter) & 3) + gb_read(gb, wC3C5));
        gb_write(gb, wBGPalette, Data_001_63AA[offset & 0x0F]);
        gb_write(gb, wOBJ1Palette, Data_001_63AA[offset & 0x0F]);
        gb_write(gb, wOBJ0Palette, Data_001_63BA[offset & 0x0F]);
    }
    MarinBeachScroll2(gb);
}

void MarinBeachScroll2(GBState *gb) {
    if (!gb) return;
    if ((gb_read_hram(gb, hFrameCounter) & 3) == 0) {
        uint8_t scrolly = (uint8_t)(gb_read_hram(gb, hBaseScrollY) + 1);
        gb_write_hram(gb, hBaseScrollY, scrolly);
        if (scrolly == 0) {
            gb_write(gb, wC3C7, 0x80);
            IncrementGameplaySubtype(gb);
        }
    }
    func_001_651E(gb);
}

void MarinBeachScrollStop(GBState *gb) {
    if (!gb) return;
    func_001_651E(gb);
    if (gb_read(gb, wDialogState) != 0) {
        gb_write(gb, wC3C4, 2);
        return;
    }
    if (gb_read(gb, wC3C7) != 0) {
        return;
    }
    OpenMarinBeachDialog(gb, 0xD8);
    IncrementGameplaySubtype(gb);
}

void MarinBeachDialog1(GBState *gb) {
    if (!gb) return;
    func_001_651E(gb);
    if (gb_read(gb, wDialogState) != 0) {
        return;
    }
    gb_write(gb, wC3C4, 0x80);
    gb_write(gb, wC3C7, 0xC0);
    IncrementGameplaySubtype(gb);
}

void MarinBeachPause1(GBState *gb) {
    if (!gb) return;
    func_001_651E(gb);
    if (gb_read(gb, wC3C7) != 0) {
        return;
    }
    OpenMarinBeachDialog(gb, 0xD9);
    IncrementGameplaySubtype(gb);
}

void MarinBeachDialog2(GBState *gb) {
    if (!gb) return;
    func_001_651E(gb);
    if (gb_read(gb, wDialogState) != 0) {
        return;
    }
    gb_write(gb, wC3C4, 0x80);
    gb_write(gb, wC3C7, 0xC0);
    IncrementGameplaySubtype(gb);
}

void MarinBeachPause2(GBState *gb) {
    if (!gb) return;
    func_001_651E(gb);
    if (gb_read(gb, wC3C7) != 0) {
        return;
    }
    OpenMarinBeachDialog(gb, 0xDA);
    IncrementGameplaySubtype(gb);
}

void MarinBeachAreYouListening(GBState *gb) {
    if (!gb) return;
    func_001_651E(gb);
    if (gb_read(gb, wDialogState) != 0) {
        gb_write(gb, wC3C4, 2);
        return;
    }
    if (gb_read(gb, wDialogAskSelectionIndex) == 0) {
        OpenMarinBeachDialog(gb, 0xDB);
        IncrementGameplaySubtype(gb);
    } else {
        OpenMarinBeachDialog(gb, 0xDE);
        gb_write(gb, wGameplaySubtype, 6);
        gb_write(gb, wC3C7, 5);
    }
}

void MarinBeachDialog3(GBState *gb) {
    if (!gb) return;
    func_001_651E(gb);
    if (gb_read(gb, wDialogState) != 0) {
        return;
    }
    OpenMarinBeachDialog(gb, 0xDC);
    gb_write(gb, wC3C7, 0x30);
    IncrementGameplaySubtype(gb);
}

void MarinBeachDialog4(GBState *gb) {
    if (!gb) return;
    func_001_651E(gb);
    gb_write(gb, wC3C4, 2);
    if (gb_read(gb, wC3C7) != 0) {
        return;
    }
    label_27F2(gb);
    func_001_68D9(gb);
    gb_write(gb, wIsMarinFollowingLink, 1);
}

void OpenMarinBeachDialog(GBState *gb, uint8_t dialog_low) {
    if (!gb) return;
    uint8_t old_y = gb_read_hram(gb, hLinkPositionY);
    gb_write_hram(gb, hLinkPositionY, 0x60);
    OpenDialogInTable1(gb, dialog_low);
    gb_write_hram(gb, hLinkPositionY, old_y);
}

void func_001_64FF(GBState *gb) {
    if (!gb) return;
    for (uint8_t i = 0; i < 0x10; i++) {
        gb_write(gb, (uint16_t)(wTranscientVfxTypeTable + i), 0);
    }
}

void func_001_658B(GBState *gb, uint16_t hl, const uint8_t *de) {
    if (!gb || !de) return;
    uint8_t scrolly = gb_read_hram(gb, hBaseScrollY);
    uint8_t pos_y = gb_read_hram(gb, hActiveEntityVisualPosY);
    uint8_t y = (uint8_t)(pos_y - scrolly);
    gb_write_hram(gb, hMultiPurposeG, y);
    uint8_t x = gb_read_hram(gb, hActiveEntityPosX);

    gb_write(gb, hl, y);
    gb_write(gb, (uint16_t)(hl + 1), x);
    gb_write(gb, (uint16_t)(hl + 2), de[0]);
    gb_write(gb, (uint16_t)(hl + 3), de[1]);

    gb_write(gb, (uint16_t)(hl + 4), y);
    gb_write(gb, (uint16_t)(hl + 5), (uint8_t)(x + 8));
    gb_write(gb, (uint16_t)(hl + 6), de[2]);
    gb_write(gb, (uint16_t)(hl + 7), de[3]);
}

void func_001_67B5(GBState *gb, uint8_t bc_idx) {
    if (!gb) return;
    uint8_t vel = gb_read(gb, (uint16_t)(wC550 + bc_idx));
    uint8_t frac_vel = (uint8_t)((vel << 4) & 0xF0);
    uint8_t cur_frac = gb_read(gb, (uint16_t)(wC570 + bc_idx));
    uint16_t sum_frac = (uint16_t)(frac_vel + cur_frac);
    gb_write(gb, (uint16_t)(wC570 + bc_idx), (uint8_t)sum_frac);
    uint8_t carry = (uint8_t)((sum_frac >> 8) & 1);

    uint8_t int_vel = (uint8_t)(vel >> 4);
    if (vel & 0x80) {
        int_vel |= 0xF0;
    }
    uint8_t cur_pos = gb_read(gb, (uint16_t)(wTranscientVfxPosXTable + bc_idx));
    gb_write(gb, (uint16_t)(wTranscientVfxPosXTable + bc_idx), (uint8_t)(cur_pos + int_vel + carry));
}

void func_001_67A8(GBState *gb, uint8_t bc_idx) {
    if (!gb) return;
    func_001_67B5(gb, bc_idx);
    func_001_67B5(gb, (uint8_t)(bc_idx + 0x10));
}

void func_001_66FD(GBState *gb, uint8_t bc_idx) {
    if (!gb) return;
    uint8_t c560_val = gb_read(gb, (uint16_t)(wC560 + bc_idx));
    uint8_t frame_idx;
    if ((c560_val & 0x80) == 0) {
        frame_idx = 3;
    } else {
        if ((gb_read_hram(gb, hFrameCounter) & 7) == 0) {
            uint8_t d200_val = (uint8_t)((gb_read(gb, (uint16_t)(wD200 + bc_idx)) + 1) & 3);
            gb_write(gb, (uint16_t)(wD200 + bc_idx), d200_val);
        }
        frame_idx = gb_read(gb, (uint16_t)(wD200 + bc_idx)) & 3;
    }
    uint16_t oam_dest = (uint16_t)(wDynamicOAMBuffer + 0x10 + ((bc_idx << 3) & 0x78));
    func_001_658B(gb, oam_dest, Data_001_66D8[frame_idx]);

    func_001_67A8(gb, bc_idx);

    uint8_t counter_shift = (uint8_t)((bc_idx << 4) + gb_read_hram(gb, hFrameCounter));
    gb_write_hram(gb, hMultiPurposeH, counter_shift);
    if ((counter_shift & 0x3F) == 0) {
        uint8_t rnd = (uint8_t)(GetRandomByte(gb) & 7);
        gb_write(gb, (uint16_t)(wC550 + bc_idx), Data_001_66F0[rnd]);
    }

    if (((uint8_t)(counter_shift + 0x40) & 0x3F) == 0) {
        uint8_t rnd = (uint8_t)(GetRandomByte(gb) & 7);
        gb_write(gb, (uint16_t)(wC560 + bc_idx), Data_001_66F0[rnd]);
    }

    uint8_t c590_val = (uint8_t)(gb_read(gb, (uint16_t)(wC590 + bc_idx)) + 1);
    gb_write(gb, (uint16_t)(wC590 + bc_idx), c590_val);
    if (c590_val >= 0x13) {
        gb_write(gb, (uint16_t)(wC590 + bc_idx), 0);
        uint8_t target_x = (bc_idx < 7) ? Data_001_66F6[bc_idx] : 0;
        uint8_t cur_x = gb_read(gb, (uint16_t)(wTranscientVfxPosXTable + bc_idx));
        int8_t step_x = ((uint8_t)(cur_x - target_x) & 0x80) ? 1 : -1;
        gb_write(gb, (uint16_t)(wTranscientVfxPosXTable + bc_idx), (uint8_t)(cur_x + step_x));

        uint8_t cur_y = gb_read(gb, (uint16_t)(wTranscientVfxPosYTable + bc_idx));
        int8_t step_y = ((uint8_t)(cur_y - 0x48) & 0x80) ? 1 : -1;
        gb_write(gb, (uint16_t)(wTranscientVfxPosYTable + bc_idx), (uint8_t)(cur_y + step_y));
    }
}

void func_001_6673(GBState *gb, uint8_t bc_idx) {
    if (!gb) return;
    if (gb_read(gb, (uint16_t)(wTranscientVfxCountdownTable + bc_idx)) != 0) {
        return;
    }
    uint8_t d210_val = (uint8_t)(gb_read(gb, (uint16_t)(wD210 + bc_idx)) + 1);
    gb_write(gb, (uint16_t)(wD210 + bc_idx), d210_val);
    if (d210_val >= 6) {
        gb_write(gb, (uint16_t)(wD210 + bc_idx), 0);
        uint8_t d200_val = (uint8_t)(gb_read(gb, (uint16_t)(wD200 + bc_idx)) + 1);
        gb_write(gb, (uint16_t)(wD200 + bc_idx), d200_val);
    }
    uint8_t anim_idx = gb_read(gb, (uint16_t)(wD200 + bc_idx));
    uint8_t sprite_frame = (anim_idx < sizeof(Data_001_6607)) ? Data_001_6607[anim_idx] : 0;
    if (sprite_frame > 5) sprite_frame = 5;
    uint16_t oam_dest = (uint16_t)(wDynamicOAMBuffer + 0x10 + ((bc_idx << 3) & 0x78));
    func_001_658B(gb, oam_dest, Data_001_65E3[sprite_frame]);

    func_001_67A8(gb, bc_idx);

    if ((gb_read_hram(gb, hFrameCounter) & 7) == 0) {
        uint8_t c560_val = gb_read(gb, (uint16_t)(wC560 + bc_idx));
        if (c560_val != 0xFB) {
            gb_write(gb, (uint16_t)(wC560 + bc_idx), (uint8_t)(c560_val - 1));
        }
    }

    uint8_t mpg = gb_read_hram(gb, hMultiPurposeG);
    if (mpg >= 0xF0) {
        uint8_t c560_val = gb_read(gb, (uint16_t)(wC560 + bc_idx));
        if (c560_val & 0x80) {
            gb_write(gb, (uint16_t)(wTranscientVfxTypeTable + bc_idx), 0);
        }
    }
}

void func_001_65DD(GBState *gb, uint8_t type, uint8_t bc_idx) {
    if (!gb) return;
    if (type == 1) {
        func_001_6673(gb, bc_idx);
    } else if (type == 2) {
        func_001_66FD(gb, bc_idx);
    }
}

void func_001_65AE(GBState *gb) {
    if (!gb) return;
    for (int c = 8; c >= 0; c--) {
        uint8_t type = gb_read(gb, (uint16_t)(wTranscientVfxTypeTable + c));
        if (type != 0) {
            gb_write_hram(gb, hActiveEntityPosX, gb_read(gb, (uint16_t)(wTranscientVfxPosXTable + c)));
            gb_write_hram(gb, hActiveEntityVisualPosY, gb_read(gb, (uint16_t)(wTranscientVfxPosYTable + c)));
            uint8_t countdown = gb_read(gb, (uint16_t)(wTranscientVfxCountdownTable + c));
            if (countdown != 0) {
                gb_write(gb, (uint16_t)(wTranscientVfxCountdownTable + c), (uint8_t)(countdown - 1));
            }
            func_001_65DD(gb, type, (uint8_t)c);
        }
    }
}

void func_001_651E(GBState *gb) {
    if (!gb) return;
    func_001_65AE(gb);

    uint8_t sea_timer = (uint8_t)(gb_read(gb, wNoiseSfxSeaWavesCounter) + 1);
    if (sea_timer == 0xA0) {
        gb_write_hram(gb, hNoiseSfx, NOISE_SFX_SEA_WAVES);
        sea_timer = 0;
    }
    gb_write(gb, wNoiseSfxSeaWavesCounter, sea_timer);

    uint8_t d466_val = gb_read(gb, wD466);
    if (d466_val == 0) {
        gb_write_hram(gb, hJingle, JINGLE_SEAGULL);
        d466_val = (uint8_t)((GetRandomByte(gb) & 0x7F) + 0x60);
    }
    gb_write(gb, wD466, (uint8_t)(d466_val - 1));

    uint8_t scrolly = (uint8_t)(gb_read_hram(gb, hBaseScrollY) - 1);
    if (scrolly < 0xC0) {
        return;
    }

    const uint8_t *marin_desc = Data_001_650A;
    uint8_t c3c4 = gb_read(gb, wC3C4);
    if (c3c4 != 0 && c3c4 < 0x60) {
        marin_desc = Data_001_650E;
    }
    gb_write_hram(gb, hActiveEntityVisualPosY, 0x7C);
    gb_write_hram(gb, hActiveEntityPosX, 0x58);
    func_001_658B(gb, wDynamicOAMBuffer, marin_desc);

    gb_write_hram(gb, hActiveEntityPosX, 0x48);
    const uint8_t *link_desc = Data_001_6512;
    uint8_t tunic = gb_read(gb, wTunicType);
    if (tunic == TUNIC_RED) {
        link_desc = Data_001_6516;
    } else if (tunic == TUNIC_BLUE) {
        link_desc = Data_001_651A;
    }
    func_001_658B(gb, (uint16_t)(wDynamicOAMBuffer + 8), link_desc);
}
