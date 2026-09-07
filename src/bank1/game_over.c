#include "bank1/game_over.h"
#include "bank1/room_transition.h"
#include "bank1/world_map.h"
#include "bank1/save.h"
#include "constants/gfx.h"
#include "constants/audio.h"
#include "constants/joypad.h"
#include "constants/link.h"
#include "constants/memory.h"
#include "home/bank.h"
#include "home/gameplay.h"

const uint8_t Data_001_41CF[24] = {
    0x6A, 0x6A, 0x6A, 0x6A, 0x6A, 0x6A, 0x6A, 0x6A,
    0x6A, 0x6A, 0x00, 0x00, 0x00, 0x0A, 0x04, 0x06,
    0x00, 0x0A, 0x04, 0x06, 0x00, 0x0A, 0x04, 0x06
};

const uint8_t Data_001_41E7[28] = {
    0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
    0x1C, 0x1C, 0x1B, 0x1A, 0x19, 0x18, 0x17, 0x16,
    0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10
};

const uint8_t Data_001_42BA[16] = {
    0x00, 0xFE, 0xFD, 0xFE, 0x00, 0x02, 0x03, 0x02,
    0x00, 0x04, 0x08, 0x0C, 0x10, 0x0C, 0x08, 0x04
};

const uint8_t Data_001_4336[3] = {
    0x50, /* GAMEOVER_OPTION_SAVE_CONTINUE_Y + $10 */
    0x60, /* GAMEOVER_OPTION_SAVE_QUIT_Y + $10 */
    0x70  /* GAMEOVER_OPTION_CONTINUE_Y + $10 */
};

void LinkPassOut(GBState *gb) {
    if (!gb) return;

    uint8_t stage = gb_read(gb, hGameOverStage);
    switch (stage) {
        case 0:
            LinkPassOutHandler(gb);
            break;
        case 1:
            LoadGameOverStage1Handler(gb);
            break;
        case 2:
            LoadGameOverStage2Handler(gb);
            break;
        case 3:
            LoadGameOverStage3Handler(gb);
            break;
        case 4:
            GameOverInteractiveHandler(gb);
            break;
        default:
            break;
    }
}

void LinkPassOutHandler(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wScreenShakeHorizontal, 0);
    gb_write(gb, wScreenShakeVertical, 0);

    uint8_t countdown = gb_read(gb, hLinkCountdown);
    if (countdown != 0) {
        /* Passing out animation */
        uint8_t anim_idx = (countdown >> 3) & 0x3F;
        if (anim_idx < sizeof(Data_001_41CF)) {
            gb_write(gb, hLinkAnimationState, Data_001_41CF[anim_idx]);
        }
        uint8_t pal_idx = (countdown >> 3) & 0x1F;
        if (pal_idx < sizeof(Data_001_41E7)) {
            gb_write(gb, wC3CD, Data_001_41E7[pal_idx]);
        }
        gb_write(gb, wObjectAffectingBGPalette, 1);
        gb_write(gb, wOBJ0Palette, 0x1C);
        gb_write(gb, wOBJ1Palette, gb_read(gb, wBGPalette));

        /* Call bank 0x20 trampolines */
        func_020_6A30_trampoline(gb, NULL);
        CopyLinkTunicPalette_trampoline(gb, NULL);
        return;
    }

    /* Passing out animation finished: start loading Game Over screen */
    gb_write(gb, hLinkCountdown, 16);
    gb_write(gb, hGameOverStage, 1);
    gb_write(gb, wTilesetToLoad, TILESET_0F);
    gb_write(gb, hLinkAnimationState, LINK_ANIMATION_STATE_HIDDEN);

    /* Increment death count in BCD */
    uint8_t low = gb_read(gb, wDeathCount);
    uint8_t high = gb_read(gb, wDeathCount + 1);

    uint8_t d0 = (low & 0x0F) + 1;
    uint8_t d1 = (low >> 4) & 0x0F;
    uint8_t d2 = high & 0x0F;
    uint8_t d3 = (high >> 4) & 0x0F;

    if (d0 > 9) {
        d0 = 0;
        d1++;
    }
    if (d1 > 9) {
        d1 = 0;
        d2++;
    }
    if (d2 > 9) {
        d2 = 0;
        d3++;
    }

    low = (uint8_t)((d1 << 4) | d0);
    high = (uint8_t)((d3 << 4) | d2);

    if (high >= 0x10) {
        low = 0x99;
        high = 0x09;
    }

    gb_write(gb, wDeathCount, low);
    gb_write(gb, wDeathCount + 1, high);

    gb_write(gb, wScrollXOffset, 0);
    gb_write(gb, wPieceOfPowerKillCount, 0);
    gb_write(gb, wActivePowerUp, 0);
    gb_write(gb, wPowerUpHits, 0);
    gb_write(gb, wObjectAffectingBGPalette, 0);
    gb_write(gb, wBGPaletteEffectAddress, 0);
    gb_write(gb, wC3CD, 0);
    gb_write(gb, rOBP0, 0);
}

void LoadGameOverStage1Handler(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wTilesetToLoad, TILESET_SAVE_MENU);
    gb_write(gb, hGameOverStage, gb_read(gb, hGameOverStage) + 1);
}

void LoadGameOverStage2Handler(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wBGPalette, 0xE4);
    gb_write(gb, wBGMapToLoad, TILEMAP_GAME_OVER);
    gb_write(gb, wWindowY, 0xFF);
    gb_write(gb, hBaseScrollX, 0);
    gb_write(gb, hBaseScrollY, 0);
    gb_write(gb, hGameOverStage, gb_read(gb, hGameOverStage) + 1);

    LoadFileMenuBG_trampoline(gb, NULL);
    SynchronizeDungeonsItemFlags_trampoline(gb, NULL);
}

void LoadGameOverStage3Handler(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, hLinkCountdown) != 0) {
        return;
    }

    gb_write(gb, hGameOverStage, gb_read(gb, hGameOverStage) + 1);
    gb_write(gb, wMusicTrackToPlay, MUSIC_GAME_OVER);
}

void GameOverInteractiveHandler(GBState *gb) {
    if (!gb) return;

    func_001_4339(gb);

    uint8_t joypad = gb_read(gb, hJoypadState);
    if (!(joypad & (J_A | J_B | J_START))) {
        return;
    }

    uint8_t opt = gb_read(gb, wC13F);
    if (opt == 1) {
        /* Save and Quit */
        SaveGameToFile(gb);
        gb_write(gb, hActiveEntityTilesOffset, 0);
        func_001_6162(gb, NULL);
        return;
    }

    if (opt == 0) {
        /* Save and Continue */
        SaveGameToFile(gb);
    } else {
        /* Continue without saving */
        gb_write(gb, wDBD1, opt);
    }

    /* Common continue handler */
    for (uint8_t i = 0; i < 16; i++) {
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + i), 0);
    }

    gb_write(gb, wOBJ0Palette, 0);
    gb_write(gb, wOBJ1Palette, 0);
    gb_write(gb, rOBP0, 0);
    gb_write(gb, rOBP1, 0);
    gb_write(gb, wBGPalette, 0);
    gb_write(gb, rBGP, 0);
    gb_write(gb, wSwitchBlocksState, 0);
    gb_write(gb, wD475, 0);

    gb_write(gb, wMapEntrancePositionX, gb_read(gb, hLinkPositionX));
    gb_write(gb, wMapEntrancePositionY, gb_read(gb, hLinkPositionY));

    LoadSavedFile(gb);
    ClearFileMenuBG_trampoline(gb, 1, NULL);
    gb_write(gb, wInvincibilityCounter, 0x80);
}

void func_001_4339(GBState *gb) {
    if (!gb) return;

    MoveSelect(gb);

    uint8_t joypad = gb_read(gb, hJoypadState);
    uint8_t opt = gb_read(gb, wC13F);

    if (joypad & J_DOWN) {
        opt++;
        if (opt >= 3) {
            opt = 0;
        }
        gb_write(gb, wC13F, opt);
    }

    if (joypad & J_UP) {
        if (opt == 0) {
            opt = 2;
        } else {
            opt--;
        }
        gb_write(gb, wC13F, opt);
    }

    opt = gb_read(gb, wC13F);
    if (opt > 2) {
        opt = 0;
    }

    gb_write(gb, (uint16_t)(wOAMBuffer + 0x18), Data_001_4336[opt]);
    gb_write(gb, (uint16_t)(wOAMBuffer + 0x19), 0x24); /* GAMEOVER_OPTION_X + 8 */
    gb_write(gb, (uint16_t)(wOAMBuffer + 0x1A), 0xBE);
    gb_write(gb, (uint16_t)(wOAMBuffer + 0x1B), 0x00);
}
