#include "bank1/intro.h"
#include "home/link.h"
#include "constants/sfx.h"
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
        case 5:
            IntroStage5Handler(gb);
            break;
        case 6:
            IntroStage6Handler(gb);
            break;
        case 7:
            IntroBeachHandler(gb);
            break;
        case 8:
            IntroStage8Handler(gb);
            break;
        case 9:
            TitleScreenSfxHandler(gb);
            break;
        case 10:
            IntroStageAHandler(gb);
            break;
        case 11:
            TitleScreenHandler(gb);
            break;
        case 12:
            IntroStageCHandler(gb);
            break;
        case 13:
            IntroStageDHandler(gb);
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

/* Part 2 constant tables */
const uint8_t Data_001_7128[16] = {
    0, 0, 0, 0, 0x40, 0x40, 0x40, 0x80, 0x85, 0x85, 0x85, 0xC5, 0xC9, 0xC9, 0xC9, 0xC9
};

const uint8_t Data_001_7138[16] = {
    0, 0, 0, 0, 4, 4, 4, 4, 0x18, 0x18, 0x18, 0x18, 0x1C, 0x1C, 0x1C, 0x1C
};

const uint8_t Data_001_7148[12] = {
    0, 0, 0, 0, 0x40, 0x40, 0x40, 0x40, 0x90, 0x90, 0x90, 0x90
};

const uint8_t TitleTileMap1[19] = {
    0x9A, 0x16, 0x0F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F
};

const uint8_t TitleTileMap2[19] = {
    0x9A, 0x36, 0x0F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F
};

const uint8_t TitleTileMap3[19] = {
    0x9A, 0x56, 0x0F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF
};

const uint8_t TitleTileMap4[19] = {
    0x9A, 0x76, 0x0F, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF
};

const uint8_t TitleTileMap5[19] = {
    0x9A, 0x96, 0x0F, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF
};

const uint8_t TitleTileMap6[19] = {
    0x9A, 0xB6, 0x0F, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF
};

const uint8_t TitleTileMap7[19] = {
    0x9A, 0xD6, 0x0F, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF
};

const uint8_t *const TitleTileMap[7] = {
    TitleTileMap4, TitleTileMap3, TitleTileMap5, TitleTileMap2, TitleTileMap6, TitleTileMap1, TitleTileMap7
};

const uint8_t TitleAttrMap1[19] = {
    0x9A, 0x16, 0x0F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t TitleAttrMap2[19] = {
    0x9A, 0x36, 0x0F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t TitleAttrMap3[19] = {
    0x9A, 0x56, 0x0F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t TitleAttrMap4[19] = {
    0x9A, 0x76, 0x0F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t TitleAttrMap5[19] = {
    0x9A, 0x96, 0x0F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t TitleAttrMap6[19] = {
    0x9A, 0xB6, 0x0F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t TitleAttrMap7[19] = {
    0x9A, 0xD6, 0x0F, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t *const TitleAttrMap[7] = {
    TitleAttrMap4, TitleAttrMap3, TitleAttrMap5, TitleAttrMap2, TitleAttrMap6, TitleAttrMap1, TitleAttrMap7
};

const uint8_t TitleScreenCopyrightDrawCommand[18] = {
    0x9B, 0xB7, 0x0D, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x00
};

const uint8_t CopyrightAttrmap[18] = {
    0x9B, 0xB7, 0x4D, 0x07, 0x00, 0x11, 0xAC, 0x73, 0x21, 0x91, 0xDC, 0x0E, 0x12, 0x1A, 0x13, 0x22, 0x0D, 0x20
};

const uint8_t Data_001_73C0[8] = {
    0x18, 0x18, 0x38, 0x40, 0x58, 0x60, 0x80, 0x88
};

const uint8_t Data_001_73C8[8] = {
    0x20, 0x48, 0x44, 0x28, 0x44, 0x28, 0x28, 0x40
};

const uint8_t DXTilesDMG[72] = {
    /* DXOnDarkTilesDMG (32 bytes, 8 sprites) */
    0x10, 0x18, 0x2E, 0x05,
    0x00, 0x18, 0x2C, 0x05,
    0x10, 0x10, 0x2A, 0x05,
    0x00, 0x10, 0x28, 0x05,
    0x10, 0x08, 0x26, 0x05,
    0x00, 0x08, 0x24, 0x05,
    0x10, 0x00, 0x22, 0x05,
    0x00, 0x00, 0x20, 0x05,
    /* DXOnWhiteTilesDMG (40 bytes, 10 sprites) */
    0x00, 0x28, 0x52, 0x16,
    0x00, 0x20, 0x50, 0x16,
    0x10, 0x18, 0x4E, 0x16,
    0x00, 0x18, 0x4C, 0x16,
    0x10, 0x10, 0x4A, 0x16,
    0x00, 0x10, 0x48, 0x16,
    0x10, 0x08, 0x46, 0x16,
    0x00, 0x08, 0x44, 0x16,
    0x10, 0x00, 0x42, 0x16,
    0x00, 0x00, 0x40, 0x16
};

const uint8_t DXTilesGBC[72] = {
    /* DXOnDarkTilesGBC (32 bytes, 8 sprites) */
    0x10, 0x18, 0x2E, 0x05,
    0x00, 0x18, 0x2C, 0x05,
    0x10, 0x10, 0x2A, 0x05,
    0x00, 0x10, 0x28, 0x05,
    0x10, 0x08, 0x26, 0x05,
    0x00, 0x08, 0x24, 0x05,
    0x10, 0x00, 0x22, 0x05,
    0x00, 0x00, 0x20, 0x05,
    /* DXOnWhiteAndJP1pxTilesGBC (40 bytes, 10 sprites) */
    0x00, 0x28, 0x52, 0x16,
    0x00, 0x20, 0x50, 0x16,
    0x10, 0x18, 0x4E, 0x16,
    0x00, 0x18, 0x4C, 0x16,
    0x10, 0x10, 0x4A, 0x16,
    0x00, 0x10, 0x48, 0x16,
    0x10, 0x08, 0x46, 0x16,
    0x00, 0x08, 0x44, 0x16,
    0x10, 0x00, 0x42, 0x16,
    0x00, 0x00, 0x40, 0x16
};

const uint8_t Data_001_7898[4] = {
    0x54, 0x58, 0x68, 0x1C
};

const uint8_t Data_001_789C[4] = {
    0x00, 0x04, 0x18, 0x6C
};

const uint8_t DXFadeInPalette[128] = {
    0xF5, 0x7A, 0x8D, 0x7D, 0x8D, 0x7D, 0x8D, 0x7D, 0xF5, 0x7A, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F,
    0xF5, 0x7A, 0x6C, 0x6D, 0x8D, 0x71, 0xCF, 0x75, 0xF5, 0x7A, 0x7B, 0x6F, 0xBD, 0x73, 0xFF, 0x77,
    0xF5, 0x7A, 0x2A, 0x5D, 0x8E, 0x65, 0x12, 0x6E, 0xF5, 0x7A, 0xF7, 0x5E, 0x5B, 0x67, 0xDF, 0x6F,
    0xF5, 0x7A, 0xE8, 0x48, 0x8E, 0x59, 0x54, 0x66, 0xF5, 0x7A, 0x52, 0x4A, 0x19, 0x5B, 0xBF, 0x67,
    0xF5, 0x7A, 0xC6, 0x38, 0x8F, 0x49, 0x97, 0x5A, 0xF5, 0x7A, 0xCE, 0x39, 0xB7, 0x4A, 0xBF, 0x5B,
    0xF5, 0x7A, 0x84, 0x24, 0x8F, 0x3D, 0xD9, 0x52, 0xF5, 0x7A, 0x29, 0x25, 0x75, 0x3E, 0x9F, 0x53,
    0xF5, 0x7A, 0x42, 0x14, 0x90, 0x31, 0x1C, 0x4B, 0xF5, 0x7A, 0xA5, 0x14, 0x13, 0x32, 0x7F, 0x4B,
    0xF5, 0x7A, 0x00, 0x00, 0xB1, 0x21, 0x5F, 0x3F, 0xF5, 0x7A, 0x00, 0x00, 0xB1, 0x21, 0x5F, 0x3F
};

void IntroStage5Handler(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wBGMapToLoad, TILEMAP_INTRO_BEACH);
    gb_write(gb, wPaletteUnknownE, 0x01);
    IncrementGameplaySubtype(gb);
}

void IntroStage6Handler(GBState *gb) {
    if (!gb) return;
    func_001_71C7(gb);
    uint8_t timer = gb_read(gb, wIntroTimer);
    if (timer == 0xA0) {
        gb_write(gb, rLYC, 0x02);
    }
    timer--;
    gb_write(gb, wIntroTimer, timer);
    if (timer == 0) {
        gb_write(gb, wGameplaySubtype, GAMEPLAY_INTRO_BEACH);
        gb_write(gb, wEntitiesStatusTable, 0x06);
        gb_write(gb, wEntitiesPosXTable, 0xB0);
        gb_write(gb, wEntitiesPosYTable, 0x68);
        gb_write(gb, wEntitiesInertiaTable, 0x01);
        return;
    }

    if (timer >= 0x34) {
        return;
    }

    if ((timer & 0x03) == 0) {
        uint8_t d010 = gb_read(gb, wD010);
        if (d010 != 0x0C) {
            gb_write(gb, wD010, (uint8_t)(d010 + 1));
        }
    }

    uint8_t fc = (uint8_t)(gb_read_hram(gb, hFrameCounter) & 0x03);
    uint8_t de = (uint8_t)(gb_read(gb, wD010) + fc);
    if (de < 16) {
        gb_write(gb, wBGPalette, Data_001_7128[de]);
        gb_write(gb, wOBJ0Palette, Data_001_7138[de]);
        if (de < 12) {
            gb_write(gb, wOBJ1Palette, Data_001_7148[de]);
        }
    }
    func_020_6AC1_trampoline(gb, NULL);
}

void IntroBeachHandler(GBState *gb) {
    if (!gb) return;
    RenderIntroEntities(gb);
}

void func_001_71C7(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, (uint16_t)(wEntitiesStateTable + 1)) >= 0x02) {
        return;
    }
    uint8_t counter = (uint8_t)(gb_read(gb, wNoiseSfxSeaWavesCounter) + 1);
    if (counter == 0xA0) {
        gb_write_hram(gb, hNoiseSfx, NOISE_SFX_SEA_WAVES);
        counter = 0;
    }
    gb_write(gb, wNoiseSfxSeaWavesCounter, counter);
}

void IntroStage8Handler(GBState *gb) {
    if (!gb) return;
    uint8_t sub_timer = gb_read(gb, wIntroSubTimer);
    if (sub_timer < 7) {
        const uint8_t *src = TitleTileMap[sub_timer];
        for (int i = 0; i < 0x13; i++) {
            gb_write(gb, (uint16_t)(wDrawCommand + i), src[i]);
        }
        gb_write(gb, (uint16_t)(wDrawCommand + 0x13), 0x00);
        if (gb_read(gb, hIsGBC) != 0) {
            func_001_7338(gb);
        }
    }
    sub_timer = (uint8_t)(gb_read(gb, wIntroSubTimer) + 1);
    gb_write(gb, wIntroSubTimer, sub_timer);
    if (sub_timer == 0x07) {
        IncrementGameplaySubtype(gb);
    }
}

void func_001_7338(GBState *gb) {
    if (!gb) return;
    uint8_t sub_timer = gb_read(gb, wIntroSubTimer);
    if (sub_timer < 7) {
        const uint8_t *src = TitleAttrMap[sub_timer];
        for (int i = 0; i < 0x13; i++) {
            gb_write(gb, (uint16_t)(wDrawCommandVRAM1 + i), src[i]);
        }
        gb_write(gb, (uint16_t)(wDrawCommandVRAM1 + 0x13), 0x00);
    }
}

void TitleScreenSfxHandler(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wC17E) < 0x10) {
        return;
    }
    gb_write_hram(gb, hNoiseSfx, NOISE_SFX_PING);
    IncrementGameplaySubtype(gb);
}

void IntroStageAHandler(GBState *gb) {
    if (!gb) return;
    for (int i = 0; i < 0x12; i++) {
        gb_write(gb, (uint16_t)(wDrawCommand + i), TitleScreenCopyrightDrawCommand[i]);
    }
    if (gb_read(gb, hIsGBC) != 0) {
        func_001_79C2(gb, 0);
        func_001_73B1(gb);
    } else {
        func_001_79AE(gb, 0);
    }
    gb_write(gb, wD015, 0x3C);
    IncrementGameplaySubtype(gb);
    ResetIntroTimers(gb);
}

void func_001_73B1(GBState *gb) {
    if (!gb) return;
    for (int i = 0; i < 0x12; i++) {
        gb_write(gb, (uint16_t)(wDrawCommandVRAM1 + i), CopyrightAttrmap[i]);
    }
}

void TitleScreenHandler(GBState *gb) {
    if (!gb) return;
    RenderIntroEntities(gb);
    func_001_7920(gb);

    if ((gb_read_hram(gb, hFrameCounter) & 0x3F) == 0) {
        for (int e = 1; e >= 0; e--) {
            if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + e)) == 0) {
                gb_write(gb, (uint16_t)(wEntitiesStatusTable + e), 0x08);
                gb_write(gb, (uint16_t)(wEntitiesTransitionCountdownTable + e), 0x3F);
                uint8_t d003 = (uint8_t)(gb_read(gb, wD003) + 1);
                gb_write(gb, wD003, d003);
                uint8_t bc = d003 & 0x07;
                gb_write(gb, (uint16_t)(wEntitiesPosXTable + e), Data_001_73C0[bc]);
                gb_write(gb, (uint16_t)(wEntitiesPosYTable + e), Data_001_73C8[bc]);
                break;
            }
        }
    }

    uint8_t sub_timer = (uint8_t)(gb_read(gb, wIntroSubTimer) + 1);
    gb_write(gb, wIntroSubTimer, sub_timer);
    if ((sub_timer & 0x0F) == 0) {
        uint8_t intro_timer = (uint8_t)(gb_read(gb, wIntroTimer) - 1);
        gb_write(gb, wIntroTimer, intro_timer);
        if (intro_timer == 0) {
            IncrementGameplaySubtype(gb);
            gb_write(gb, wTransitionSequenceCounter, 0);
            gb_write(gb, wC16C, 0);
            ResetMusicFadeTimer(gb);
        }
    }
}

void IntroStageCHandler(GBState *gb) {
    if (!gb) return;
    func_1A22(gb, NULL, NULL);
    if (gb_read(gb, wTransitionSequenceCounter) == 0x04) {
        func_001_6162(gb, NULL);
    }
}

void IntroStageDHandler(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wTilesetToLoad, 0x11);
    gb_write(gb, wGameplaySubtype, GAMEPLAY_INTRO_TITLE);
    gb_write(gb, wBGPalette, 0xC9);
    gb_write(gb, wOBJ0Palette, 0x1C);
    gb_write_hram(gb, hBaseScrollX, 0);
    gb_write_hram(gb, hBaseScrollY, 0);
    gb_write(gb, wD018, 0xFF);
}

void func_001_79AE(GBState *gb, uint8_t a) {
    if (!gb) return;
    if (a >= 4) a = 0;
    gb_write(gb, wOBJ0Palette, Data_001_7898[a]);
    gb_write(gb, wOBJ1Palette, Data_001_789C[a]);
}

void func_001_79C2(GBState *gb, uint8_t a) {
    if (!gb) return;
    uint8_t bc = (uint8_t)(a << 4);
    for (int i = 0; i < 0x10; i++) {
        gb_write(gb, (uint16_t)(wObjPal6 + i), DXFadeInPalette[bc + i]);
    }
    gb_write(gb, wPalettePartialCopyColorIndexStart, 0x14);
    gb_write(gb, wPalettePartialCopyColorCount, 0x08);
    gb_write(gb, wPaletteDataFlags, 0x82);
}

void func_001_7920(GBState *gb) {
    if (!gb) return;
    uint8_t d015 = gb_read(gb, wD015);
    if (d015 != 0) {
        gb_write(gb, wD015, (uint8_t)(d015 - 1));
    }
    if (gb_read(gb, hIsGBC) != 0) {
        uint8_t d013 = gb_read(gb, wD013);
        if (d013 != 0x08) {
            uint8_t d014 = (uint8_t)(gb_read(gb, wD014) + 1);
            gb_write(gb, wD014, d014);
            if (d014 == 0x08) {
                gb_write(gb, wD014, 0);
                d013 = (uint8_t)(gb_read(gb, wD013) + 1);
                gb_write(gb, wD013, d013);
                if (d013 != 0x08) {
                    func_001_79C2(gb, d013);
                }
            }
        }
        d013 = gb_read(gb, wD013);
        if (d013 == 0x08) {
            RenderActiveEntitySpritesRect(gb, &DXTilesGBC[32], 10, NULL);
        } else {
            RenderActiveEntitySpritesRect(gb, &DXTilesGBC[0], 18, NULL);
        }
    } else {
        uint8_t d013 = gb_read(gb, wD013);
        if (d013 != 0x04) {
            uint8_t d014 = (uint8_t)(gb_read(gb, wD014) + 1);
            gb_write(gb, wD014, d014);
            if (d014 == 0x0C) {
                gb_write(gb, wD014, 0);
                d013 = (uint8_t)(gb_read(gb, wD013) + 1);
                gb_write(gb, wD013, d013);
                if (d013 != 0x04) {
                    func_001_79AE(gb, d013);
                }
            }
        }
        d013 = gb_read(gb, wD013);
        if (d013 >= 0x03) {
            RenderActiveEntitySpritesRect(gb, &DXTilesDMG[32], 10, NULL);
        } else {
            RenderActiveEntitySpritesRect(gb, &DXTilesDMG[0], 18, NULL);
        }
    }
}

void func_001_7D9C(GBState *gb) {
    if (!gb) return;
    if ((gb_read_hram(gb, hFrameCounter) & 0x07) == 0) {
        gb_write(gb, wScrollXOffsetForSection, (uint8_t)(gb_read(gb, wScrollXOffsetForSection) + 1));
    }
    uint16_t d004_sum = (uint16_t)gb_read(gb, wD004) + 0x50;
    gb_write(gb, wD004, (uint8_t)d004_sum);
    if (d004_sum > 0xFF) {
        gb_write(gb, (uint16_t)(wScrollXOffsetForSection + 1), (uint8_t)(gb_read(gb, (uint16_t)(wScrollXOffsetForSection + 1)) + 1));
    }
    uint16_t d005_sum = (uint16_t)gb_read(gb, wD005) + 0x58;
    gb_write(gb, wD005, (uint8_t)d005_sum);
    if (d005_sum > 0xFF) {
        gb_write(gb, (uint16_t)(wScrollXOffsetForSection + 2), (uint8_t)(gb_read(gb, (uint16_t)(wScrollXOffsetForSection + 2)) + 1));
    }
    uint16_t d00d_sum = (uint16_t)gb_read(gb, wD00D) + 0xB0;
    gb_write(gb, wD00D, (uint8_t)d00d_sum);
    if (d00d_sum > 0xFF) {
        gb_write(gb, (uint16_t)(wScrollXOffsetForSection + 3), (uint8_t)(gb_read(gb, (uint16_t)(wScrollXOffsetForSection + 3)) + 1));
    }
    func_001_7D46(gb);
}

void func_001_7DCF(GBState *gb) {
    if (!gb) return;
    if ((gb_read_hram(gb, hFrameCounter) & 0x0F) == 0) {
        gb_write(gb, wScrollXOffsetForSection, (uint8_t)(gb_read(gb, wScrollXOffsetForSection) + 1));
    }
    uint16_t d004_sum = (uint16_t)gb_read(gb, wD004) + 0x28;
    gb_write(gb, wD004, (uint8_t)d004_sum);
    if (d004_sum > 0xFF) {
        gb_write(gb, (uint16_t)(wScrollXOffsetForSection + 1), (uint8_t)(gb_read(gb, (uint16_t)(wScrollXOffsetForSection + 1)) + 1));
    }
    uint16_t d005_sum = (uint16_t)gb_read(gb, wD005) + 0x2C;
    gb_write(gb, wD005, (uint8_t)d005_sum);
    if (d005_sum > 0xFF) {
        gb_write(gb, (uint16_t)(wScrollXOffsetForSection + 2), (uint8_t)(gb_read(gb, (uint16_t)(wScrollXOffsetForSection + 2)) + 1));
    }
    uint16_t d00d_sum = (uint16_t)gb_read(gb, wD00D) + 0x58;
    gb_write(gb, wD00D, (uint8_t)d00d_sum);
    if (d00d_sum > 0xFF) {
        gb_write(gb, (uint16_t)(wScrollXOffsetForSection + 3), (uint8_t)(gb_read(gb, (uint16_t)(wScrollXOffsetForSection + 3)) + 1));
    }
    func_001_7D46(gb);
}
