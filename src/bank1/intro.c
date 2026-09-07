#include "bank1/intro.h"
#include "bank1/room_transition.h"
#include "bank1/file_menu.h"
#include "home/clear_memory.h"
#include "home/audio.h"
#include "home/bank.h"
#include "home/gameplay.h"
#include "home/entities.h"
#include "constants/memory.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/gfx.h"
#include "constants/audio.h"
#include "constants/joypad.h"

/* Constant data tables */
const uint8_t IntroSeaPaletteTable[4] = {
    0xC6, 0xC2, 0xC0, 0xC2
};

const uint8_t Data_001_6F93[9] = {
    0x81, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t Data_001_6F9C[9] = {
    0x08, 0x08, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t Data_001_707B[6] = {
    0x28, 0x78, 0x60, 0x38, 0x68, 0x58
};

const uint8_t Data_001_7081[6] = {
    4, 2, 1, 4, 3, 1
};

const uint8_t Data_001_74B8[15] = {
    0x99, 0x2B, 0x83, 0x1E, 0x20, 0x22, 0x24,
    0x99, 0x2C, 0x83, 0x1F, 0x21, 0x23, 0x25,
    0x00
};

const uint8_t IntroShipTiles[24] = {
    0x00, 0x00, 0x1C, 0x02,
    0x00, 0x08, 0x1E, 0x02,
    0x10, 0xF8, 0x20, 0x02,
    0x10, 0x00, 0x22, 0x02,
    0x10, 0x08, 0x24, 0x02,
    0x10, 0x10, 0x26, 0x02
};

const uint8_t Data_001_7550[16] = {
    0xF8, 0x04, 0x32, 0x01,
    0xE8, 0x04, 0x32, 0x01,
    0xD8, 0x04, 0x32, 0x01,
    0xC8, 0x04, 0x32, 0x01
};

const uint8_t ShipHeaveTable[8] = {
    2, 1, 0, 0, 0, 1, 2, 2
};

const uint8_t IntroLightningTiles[96] = {
    /* Variant 0 */
    0x00, 0x00, 0x34, 0x01,
    0x00, 0x08, 0x36, 0x01,
    0x10, 0x00, 0x2C, 0x01,
    0x20, 0xF8, 0x2C, 0x01,
    0x28, 0x00, 0x2E, 0x21,
    0x30, 0xF0, 0x2E, 0x01,

    /* Variant 1 */
    0x08, 0x00, 0x36, 0x21,
    0x08, 0x08, 0x34, 0x21,
    0x18, 0x00, 0x30, 0x01,
    0x18, 0x08, 0x2C, 0x21,
    0x28, 0x10, 0x2E, 0x21,
    0x28, 0x10, 0x2E, 0x21,

    /* Variant 2 */
    0x00, 0x08, 0x34, 0x21,
    0x00, 0x00, 0x36, 0x21,
    0x10, 0x08, 0x2C, 0x21,
    0x20, 0x10, 0x2C, 0x21,
    0x28, 0x08, 0x2E, 0x01,
    0x30, 0x18, 0x2E, 0x21,

    /* Variant 3 */
    0x08, 0x08, 0x36, 0x01,
    0x08, 0x00, 0x34, 0x01,
    0x18, 0x08, 0x30, 0x21,
    0x18, 0x00, 0x2C, 0x01,
    0x28, 0xF8, 0x2E, 0x01,
    0x28, 0xF8, 0x2E, 0x01
};

const uint8_t Data_001_789B = 0x1C;
const uint8_t Data_001_789F = 0x6C;

const uint8_t Data_001_7CE1[8] = {
    0x00, 0x50, 0x80, 0x50, 0x00, 0x51, 0x80, 0x51
};

const uint8_t Data_001_7CE9[8] = {
    0x00, 0x52, 0x80, 0x52, 0x00, 0x53, 0x80, 0x53
};

const uint8_t Data_001_7CF1[8] = {
    0x00, 0x02, 0x04, 0x06, 0x06, 0x04, 0x02, 0x00
};

const uint8_t IntroBGVerticalOffsetTable[8] = {
    3, 2, 1, 0, 0, 1, 2, 3
};

/* File-internal lightning helper variables for RenderLightning */
static uint8_t s_lightning_entity_index = 0;
static uint8_t s_lightning_param_index = 0;

void IntroHandlerEntryPoint(GBState *gb) {
    if (!gb) return;

    uint8_t inactive = gb_read(gb, hButtonsInactiveDelay);
    if (inactive != 0) {
        gb_write(gb, hButtonsInactiveDelay, (uint8_t)(inactive - 1));
        RenderIntroFrame(gb);
        return;
    }

    uint8_t joypad = gb_read(gb, hJoypadState);
    if ((joypad & J_START) == 0) {
        RenderIntroFrame(gb);
        return;
    }

    /* Start button pressed */
    label_27F2(gb);

    uint8_t subtype = gb_read(gb, wGameplaySubtype);
    if (subtype == GAMEPLAY_INTRO_TITLE) {
        TransitionToFileMenu(gb, 0);
        return;
    }

    /* Transition to Title screen */
    gb_write(gb, hButtonsInactiveDelay, 40);
    gb_write(gb, wBGMapToLoad, TILEMAP_TITLE);

    if (gb_read(gb, hIsGBC) != 0) {
        ClearFileMenuBG_trampoline(gb, 0x01, NULL);
        gb_write(gb, wPaletteUnknownE, 0);
        gb_write(gb, wD013, 0x08);
    } else {
        gb_write(gb, wOBJ0Palette, Data_001_789B);
        gb_write(gb, wOBJ1Palette, Data_001_789F);
        gb_write(gb, wD013, 0x04);
    }

    gb_write(gb, wGameplaySubtype, 0x0D);

    /* Reset entities 0..4 */
    for (int i = 0; i < 5; i++) {
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + i), 0);
    }

    gb_write(gb, rBGP, 0);
    gb_write(gb, wBGPalette, 0);
    gb_write(gb, wC17E, 0x10);

    ResetIntroTimers(gb);
    gb_write(gb, wMusicTrackToPlay, MUSIC_TITLE_SCREEN_NO_INTRO);
    gb_write(gb, wD00F, MUSIC_TITLE_SCREEN_NO_INTRO);
    func_001_7D4E(gb);

    gb_write(gb, rIE, IEF_VBLANK);
    gb_write(gb, rLYC, 0x4F);
}

void RenderIntroFrame(GBState *gb) {
    if (!gb) return;

    uint8_t subtype = gb_read(gb, wGameplaySubtype);
    if (subtype >= GAMEPLAY_INTRO_SEA && subtype < GAMEPLAY_INTRO_LIGHTNING) {
        uint8_t cd = gb_read(gb, wIntroLightningVisibleCountdown);
        if (cd > 0) {
            cd--;
            gb_write(gb, wIntroLightningVisibleCountdown, cd);
        }
        uint8_t e = (uint8_t)((cd >> 1) & 0x03);
        gb_write(gb, wBGPalette, IntroSeaPaletteTable[e]);
        UpdateIntroSeaBGPalettes_trampoline(gb, NULL);
    }

    subtype = gb_read(gb, wGameplaySubtype);
    switch (subtype) {
        case 0:
            IntroSceneStage0Handler(gb);
            break;
        case 1:
            IntroSceneStage1Handler(gb);
            break;
        case 2:
            IntroSceneStage2Handler(gb);
            break;
        case 3:
            IntroShipOnSeaHandler(gb);
            break;
        case 4:
            IntroLinkFaceHandler(gb);
            break;
        default:
            break;
    }
}

void IntroSceneStage0Handler(GBState *gb) {
    if (!gb) return;

    ClearLowerAndMiddleWRAM(gb);
    label_27F2(gb);
    ClearFileMenuBG_trampoline(gb, 0x01, NULL);

    SetWorldMusicTrack(gb, 0x1A);
    gb_write(gb, wTilesetToLoad, TILESET_CLEAR_TILEMAP);

    gb_write(gb, hFrameCounter, 0);
    gb_write(gb, wRandomSeed, 0xA2);

    uint8_t lcdc = gb_read(gb, rLCDC) & (uint8_t)(~LCDCF_WINON);
    gb_write(gb, wLCDControl, lcdc);
    gb_write(gb, rLCDC, lcdc);

    gb_write(gb, wD016, 0xB4);
    gb_write(gb, wD017, 0x00);

    IncrementGameplaySubtype(gb);
}

void IntroSceneStage1Handler(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wTilesetToLoad, TILESET_INTRO);
    gb_write(gb, wPaletteUnknownE, 0);
    IncrementGameplaySubtype(gb);
}

void IntroSceneStage2Handler(GBState *gb) {
    if (!gb) return;

    func_001_7D01(gb);

    if (gb_read(gb, hIsGBC) != 0) {
        gb_write(gb, wBGMapToLoad, TILEMAP_INTRO_SEA_CGB);
    } else {
        gb_write(gb, wBGMapToLoad, TILEMAP_INTRO_SEA_DMG);
    }

    gb_write(gb, wOBJ0Palette, 0x1C);
    gb_write(gb, wOBJ1Palette, 0xE0);
    gb_write(gb, rIE, IEF_STAT | IEF_VBLANK);
    gb_write(gb, rLYC, 0x00);

    /* Zero out 0x11 bytes starting at wIntroLightningVisibleCountdown */
    for (int i = 0; i < 0x11; i++) {
        gb_write(gb, (uint16_t)(wIntroLightningVisibleCountdown + i), 0);
    }

    gb_write(gb, wEntitiesStatusTable + 0, 0);
    gb_write(gb, wEntitiesStatusTable + 1, 0);
    gb_write(gb, wEntitiesSpriteVariantTable + 0, 0);
    gb_write(gb, wEntitiesSpriteVariantTable + 1, 0);
    gb_write(gb, wEntitiesSpriteVariantTable + 2, 0);
    gb_write(gb, hActiveEntityFlipAttribute, 0);

    /* Configure Link's ship entity */
    gb_write(gb, wEntitiesStatusTable + 2, 0x05);
    gb_write(gb, wEntitiesPosXTable + 2, 0xC0);
    gb_write(gb, wEntitiesPosYTable + 2, 0x4E);

    gb_write(gb, wEntitiesPhysicsFlagsTable + 0, 0);
    gb_write(gb, wEntitiesPhysicsFlagsTable + 1, 0);
    gb_write(gb, wEntitiesPhysicsFlagsTable + 2, 0);
    gb_write(gb, wEntitiesPhysicsFlagsTable + 3, 0);

    IncrementGameplaySubtype(gb);
}

void IntroShipOnSeaHandler(GBState *gb) {
    if (!gb) return;

    RenderRain(gb);
    RenderIntroEntities(gb);

    uint8_t subtimer = gb_read(gb, wIntroSubTimer);
    if (subtimer != 0) {
        subtimer++;
        gb_write(gb, wIntroSubTimer, subtimer);
        if (subtimer < 0x18) {
            return;
        }

        uint8_t idx = (uint8_t)(((subtimer - 0x18) >> 3) & 0x0F);
        gb_write(gb, wBGPalette, Data_001_6F93[idx]);
        gb_write(gb, wOBJ0Palette, Data_001_6F9C[idx]);
        func_020_6A30_trampoline(gb, NULL);

        if (idx != 0x08) {
            return;
        }

        gb_write(gb, wEntitiesStatusTable + 0, 0);
        gb_write(gb, wEntitiesStatusTable + 1, 0);
        gb_write(gb, wEntitiesStatusTable + 2, 0);
        gb_write(gb, wEntitiesStateTable, 0);
        gb_write(gb, wGameplaySubtype, GAMEPLAY_INTRO_LIGHTNING);
        gb_write(gb, wD00F, GAMEPLAY_INTRO_LIGHTNING);
        func_001_7D4E(gb);

        gb_write(gb, wTilesetToLoad, TILESET_TITLE);
        gb_write(gb, wIntroTimer, 0xFF);

        gb_write(gb, hBaseScrollX, 0);
        gb_write(gb, wScrollXOffsetForSection + 0, 0);
        gb_write(gb, wScrollXOffsetForSection + 2, 0);
        gb_write(gb, wScrollXOffsetForSection + 3, 0);
        gb_write(gb, wScrollXOffsetForSection + 1, 0x92);
        gb_write(gb, rIE, IEF_STAT | IEF_VBLANK);
        return;
    }

    /* subtimer == 0 */
    uint8_t ship_x = gb_read(gb, wEntitiesPosXTable + 2);
    if (ship_x == 0x50) {
        gb_write(gb, rBGP, 0xFF);
        gb_write(gb, wGameplaySubtype, GAMEPLAY_INTRO_LINK_FACE);
        gb_write(gb, wBGMapToLoad, TILEMAP_INTRO_LINK_FACE);
        gb_write(gb, rIE, IEF_VBLANK);
        gb_write(gb, hBaseScrollX, 0);
        return;
    }

    func_001_7D01(gb);

    uint8_t fc = gb_read(gb, hFrameCounter);
    if ((fc & 0x07) != 0) {
        return;
    }

    uint8_t scroll_x = (uint8_t)(gb_read(gb, hBaseScrollX) + 1);
    gb_write(gb, hBaseScrollX, scroll_x);

    gb_write(gb, wEntitiesPosXTable + 0, (uint8_t)(gb_read(gb, wEntitiesPosXTable + 0) - 1));
    gb_write(gb, wEntitiesPosXTable + 1, (uint8_t)(gb_read(gb, wEntitiesPosXTable + 1) - 1));
    gb_write(gb, wEntitiesPosXTable + 2, (uint8_t)(gb_read(gb, wEntitiesPosXTable + 2) - 1));

    uint8_t c = 0;
    if (scroll_x == 0x10) {
        /* c = 0 */
    } else {
        c++;
        if (scroll_x == 0x30) {
            /* c = 1 */
        } else {
            c++;
            if (scroll_x == 0x38) {
                /* c = 2 */
            } else {
                c++;
                if (scroll_x == 0x58) {
                    /* c = 3 */
                } else {
                    c++;
                    if (scroll_x == 0x5A) {
                        /* c = 4 */
                    } else {
                        c++;
                        if (scroll_x == 0x69) {
                            /* c = 5 */
                        } else {
                            return;
                        }
                    }
                }
            }
        }
    }

    /* Check entity slots 1 down to 0 */
    for (int e = 1; e >= 0; e--) {
        if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + e)) == 0) {
            s_lightning_entity_index = (uint8_t)e;
            s_lightning_param_index = c;
            RenderLightning(gb);
            return;
        }
    }
}

void RenderLightning(GBState *gb) {
    if (!gb) return;

    uint8_t de = s_lightning_entity_index;
    uint8_t bc = s_lightning_param_index;

    gb_write(gb, (uint16_t)(wEntitiesStatusTable + de), Data_001_7081[bc]);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + de), Data_001_707B[bc]);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + de), 0x30);
    gb_write(gb, (uint16_t)(wEntitiesTransitionCountdownTable + de), 0x20);

    gb_write(gb, wIntroLightningVisibleCountdown, 0x1C);
    PlayBombExplosionSfx(gb);
}

void IntroLinkFaceHandler(GBState *gb) {
    if (!gb) return;

    RenderRain(gb);
    uint8_t timer = (uint8_t)(gb_read(gb, wIntroTimer) + 1);
    gb_write(gb, wIntroTimer, timer);

    if (timer == 128) {
        IntroLinkScream(gb);
    }

    if (timer == 200) {
        gb_write(gb, wGameplaySubtype, GAMEPLAY_INTRO_SEA);
        gb_write(gb, wIntroSubTimer, 0x01);

        if (gb_read(gb, hIsGBC) != 0) {
            gb_write(gb, wBGMapToLoad, TILEMAP_INTRO_SEA_CGB);
        } else {
            gb_write(gb, wBGMapToLoad, TILEMAP_INTRO_SEA_DMG);
        }

        gb_write(gb, wIntroTimer, 0x00);
        gb_write(gb, rBGP, 0xFF);
        gb_write(gb, rIE, IEF_STAT | IEF_VBLANK);
    }
}

void LoadTileMapZero_trampoline(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wFarcallBank, 0x00);
    gb_write(gb, wFarcallAdressHigh, 0x04);
    gb_write(gb, wFarcallAdressLow, 0x3A);
}

void ResetIntroTimers(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wIntroTimer, 0xA0);
    gb_write(gb, wIntroSubTimer, 0x00);
    gb_write(gb, wD003, 0xFF);
}

void RenderRain(GBState *gb) {
    if (!gb) return;

    uint8_t mp1 = (uint8_t)((GetRandomByte(gb) & 0x18) + 0x10);
    uint8_t mp0 = (uint8_t)((GetRandomByte(gb) & 0x18) + 0x10);
    gb_write(gb, hMultiPurpose1, mp1);
    gb_write(gb, hMultiPurpose0, mp0);

    uint16_t hl = wDynamicOAMBuffer + 0x1C;
    uint8_t count = (gb_read(gb, wGameplaySubtype) == GAMEPLAY_INTRO_LINK_FACE) ? 0x15 : 0x10;

    for (uint8_t c = count; c > 0; c--) {
        gb_write(gb, hl++, gb_read(gb, hMultiPurpose1));
        gb_write(gb, hl++, gb_read(gb, hMultiPurpose0));

        uint8_t tile = 0x28;
        if ((GetRandomByte(gb) & 0x01) != 0) {
            tile = (uint8_t)((GetRandomByte(gb) & 0x06) + 0x70);
        }
        gb_write(gb, hl++, tile);
        gb_write(gb, hl++, 0x00);

        mp0 = (uint8_t)(gb_read(gb, hMultiPurpose0) + 0x1C);
        gb_write(gb, hMultiPurpose0, mp0);
        if (mp0 >= 0xA0) {
            mp0 = (uint8_t)(mp0 - 0x98);
            gb_write(gb, hMultiPurpose0, mp0);
            mp1 = (uint8_t)(gb_read(gb, hMultiPurpose1) + 0x25);
            gb_write(gb, hMultiPurpose1, mp1);
        }
    }
}

void IntroLinkScream(GBState *gb) {
    if (!gb) return;

    for (int i = 0; i < 15; i++) {
        gb_write(gb, (uint16_t)(wDrawCommand + i), Data_001_74B8[i]);
    }
}

void RenderIntroEntities(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wOAMNextAvailableSlot, 0);

    for (int c = 2; c >= 0; c--) {
        gb_write(gb, wActiveEntityIndex, (uint8_t)c);

        uint8_t status = gb_read(gb, (uint16_t)(wEntitiesStatusTable + c));
        if (status == 0) {
            continue;
        }

        gb_write(gb, hActiveEntityPosX, gb_read(gb, (uint16_t)(wEntitiesPosXTable + c)));
        gb_write(gb, hActiveEntityVisualPosY, gb_read(gb, (uint16_t)(wEntitiesPosYTable + c)));
        gb_write(gb, hActiveEntitySpriteVariant, gb_read(gb, (uint16_t)(wEntitiesSpriteVariantTable + c)));
        gb_write(gb, hActiveEntityState, gb_read(gb, (uint16_t)(wEntitiesStateTable + c)));

        RenderIntroEntity(gb, (uint16_t)c);
    }
}

void RenderIntroEntity(GBState *gb, uint16_t bc) {
    if (!gb) return;

    uint8_t status = gb_read(gb, (uint16_t)(wEntitiesStatusTable + bc));
    if (status == ENTITY_INTRO_SHIP) {
        RenderIntroShip(gb);
        return;
    }
    if (status == ENTITY_INTRO_MARIN) {
        /* Forward reference, handled in subsequent batch */
        return;
    }
    if (status == ENTITY_INTRO_INERT_LINK) {
        /* Forward reference, handled in subsequent batch */
        return;
    }
    if (status == ENTITY_INTRO_SPARKLE) {
        /* Forward reference, handled in subsequent batch */
        return;
    }

    uint8_t cd = GetEntityTransitionCountdown(gb, bc);
    if (cd == 0) {
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + bc), 0);
        return;
    }

    /* Decrement transition countdown */
    gb_write(gb, (uint16_t)(wEntitiesTransitionCountdownTable + bc), (uint8_t)(cd - 1));
    func_001_762B(gb, bc);
}

void RenderIntroShip(GBState *gb) {
    if (!gb) return;

    uint8_t subtimer = gb_read(gb, wIntroSubTimer);
    uint8_t heave_idx = 0;
    if (subtimer == 0) {
        heave_idx = (uint8_t)(((gb_read(gb, hFrameCounter) + 0xD0) >> 4) & 0x07);
    }

    uint8_t heave = ShipHeaveTable[heave_idx];
    uint8_t visual_y = (uint8_t)(gb_read(gb, hActiveEntityVisualPosY) + heave);
    gb_write(gb, hActiveEntityVisualPosY, visual_y);

    uint8_t visual_x = gb_read(gb, hActiveEntityPosX);
    uint16_t oam_dest = wOAMBuffer;

    /* Render 6 sprites of the ship */
    for (int i = 0; i < 6; i++) {
        const uint8_t *sprite = &IntroShipTiles[i * 4];
        gb_write(gb, oam_dest++, (uint8_t)(visual_y + sprite[0]));
        gb_write(gb, oam_dest++, (uint8_t)(visual_x + sprite[1]));
        gb_write(gb, oam_dest++, sprite[2]);
        gb_write(gb, oam_dest++, sprite[3]);
    }

    if (subtimer >= 0x10) {
        oam_dest = wOAMBuffer + 0x18;
        for (int i = 0; i < 4; i++) {
            const uint8_t *sprite = &Data_001_7550[i * 4];
            gb_write(gb, oam_dest++, (uint8_t)(visual_y + sprite[0]));
            gb_write(gb, oam_dest++, (uint8_t)(visual_x + sprite[1]));
            gb_write(gb, oam_dest++, sprite[2]);
            gb_write(gb, oam_dest++, sprite[3]);
        }
    }
}

void func_001_762B(GBState *gb, uint16_t bc) {
    if (!gb) return;

    uint8_t status = gb_read(gb, (uint16_t)(wEntitiesStatusTable + bc));
    if (status == 0) return;

    uint8_t var_idx = (uint8_t)(status - 1);
    uint16_t offset = (uint16_t)(var_idx * 24);

    RenderActiveEntitySpritesRect(gb, &IntroLightningTiles[offset], 6, NULL);

    uint8_t slot = (uint8_t)(gb_read(gb, wOAMNextAvailableSlot) + 0x18);
    gb_write(gb, wOAMNextAvailableSlot, slot);
}

void func_001_7D01(GBState *gb) {
    if (!gb) return;

    uint8_t fc = gb_read(gb, hFrameCounter);

    /* Layer 0 */
    if ((fc & 0x07) == 0) {
        gb_write(gb, wScrollXOffsetForSection + 0, (uint8_t)(gb_read(gb, wScrollXOffsetForSection + 0) + 1));
    }

    /* Layer 1 */
    if ((fc & 0x0F) == 0) {
        gb_write(gb, wScrollXOffsetForSection + 1, (uint8_t)(gb_read(gb, wScrollXOffsetForSection + 1) + 1));
    }

    /* Layer 2 */
    if ((fc & 0x1F) == 0) {
        gb_write(gb, wScrollXOffsetForSection + 2, (uint8_t)(gb_read(gb, wScrollXOffsetForSection + 2) + 1));
    }

    /* Layer 3 */
    if ((fc & 0x0F) == 0) {
        gb_write(gb, wScrollXOffsetForSection + 3, (uint8_t)(gb_read(gb, wScrollXOffsetForSection + 3) + 1));
    }

    /* Layer 4 */
    uint16_t d004_calc = (uint16_t)(gb_read(gb, wD004) + 0x28);
    gb_write(gb, wD004, (uint8_t)d004_calc);
    if (d004_calc > 0xFF) {
        gb_write(gb, wScrollXOffsetForSection + 4, (uint8_t)(gb_read(gb, wScrollXOffsetForSection + 4) + 1));
    }

    uint8_t table_idx = (uint8_t)(((fc + 0xFC) >> 4) & 0x07);
    uint8_t offset_y = (uint8_t)(0 - IntroBGVerticalOffsetTable[table_idx]);
    gb_write(gb, wIntroBGYOffset, offset_y);
}

void func_001_7D46(GBState *gb) {
    if (!gb) return;

    uint8_t fc_mod = (uint8_t)(gb_read(gb, hFrameCounter) & 0x0F);
    if (fc_mod >= 4) {
        func_001_7D4E(gb);
    }
}

void func_001_7D4E(GBState *gb) {
    if (!gb) return;

    uint8_t fc = gb_read(gb, hFrameCounter);
    uint8_t e = (uint8_t)((fc >> 4) & 0x07);
    uint8_t de_idx = Data_001_7CF1[e];

    const uint8_t *table = Data_001_7CE1;
    if (gb_read(gb, wD00F) != 0) {
        table = Data_001_7CE9;
    }

    uint16_t hl_addr = (uint16_t)(table[de_idx] | (table[de_idx + 1] << 8));

    uint16_t de_base = 0x8900;
    if (gb_read(gb, wD00F) != 0) {
        de_base = 0x9300;
    }

    uint8_t shift = (uint8_t)((fc & 0x03) << 5);
    hl_addr = (uint16_t)(hl_addr + shift);

    gb_write(gb, wD006, (uint8_t)(hl_addr & 0xFF));
    gb_write(gb, wD007, (uint8_t)((hl_addr >> 8) & 0xFF));
    gb_write(gb, wD008, shift);
    gb_write(gb, wD009, (uint8_t)((de_base >> 8) & 0xFF));
}
