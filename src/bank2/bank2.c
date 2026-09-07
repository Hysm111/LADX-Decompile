#include "bank2/bank2.h"
#include "constants/directions.h"
#include "constants/entities.h"
#include "constants/memory.h"
#include "constants/sfx.h"
#include "home/check_items_to_use.h"
#include "home/entities.h"
#include "home/link.h"

const int8_t HookshotChainSpeedX[4] = {
    0x30,  /* DIRECTION_RIGHT:  HOOKSHOT_CHAIN_SPEED ($30) */
    -0x30, /* DIRECTION_LEFT:  -HOOKSHOT_CHAIN_SPEED ($D0) */
    0x00,  /* DIRECTION_UP */
    0x00   /* DIRECTION_DOWN */
};

const int8_t HookshotChainSpeedY[4] = {
    0x00,  /* DIRECTION_RIGHT */
    0x00,  /* DIRECTION_LEFT */
    -0x30, /* DIRECTION_UP:    -HOOKSHOT_CHAIN_SPEED ($D0) */
    0x30   /* DIRECTION_DOWN:   HOOKSHOT_CHAIN_SPEED ($30) */
};

bool SpawnChestWithItem(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb) return false;

    uint16_t de = 0;
    if (spawn_new_entity) {
        de = SpawnNewEntity_trampoline(gb, ENTITY_CHEST_WITH_ITEM, spawn_new_entity);
        if (de >= MAX_ENTITIES) {
            return false;
        }
    } else {
        int found_slot = -1;
        for (int i = MAX_ENTITIES - 1; i >= 0; i--) {
            if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + i)) == ENTITY_STATUS_DISABLED) {
                found_slot = i;
                break;
            }
        }
        if (found_slot < 0) {
            return false;
        }
        de = (uint16_t)found_slot;
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + de), ENTITY_STATUS_ACTIVE);
        gb_write(gb, (uint16_t)(wEntitiesTypeTable + de), ENTITY_CHEST_WITH_ITEM);
    }

    /* ld hl, wEntitiesStatusTable; add hl, de; dec [hl] */
    uint8_t status = gb_read(gb, (uint16_t)(wEntitiesStatusTable + de));
    gb_write(gb, (uint16_t)(wEntitiesStatusTable + de), (uint8_t)(status - 1));

    /* ldh a, [hIntersectedObjectLeft]; and $F0; add $08; ld [wEntitiesPosXTable + de], a */
    uint8_t left = gb_read_hram(gb, hIntersectedObjectLeft);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + de), (uint8_t)((left & 0xF0) + 0x08));

    /* ldh a, [hIntersectedObjectTop]; and $F0; add $10; ld [wEntitiesPosYTable + de], a */
    uint8_t top = gb_read_hram(gb, hIntersectedObjectTop);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + de), (uint8_t)((top & 0xF0) + 0x10));

    /* ldh a, [hMultiPurpose8]; ld [wEntitiesSpriteVariantTable + de], a */
    uint8_t variant = gb_read_hram(gb, hMultiPurpose8);
    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + de), variant);

    return true;
}

void UseOcarina(GBState *gb) {
    if (!gb) return;

    /* Check if Link is in air, already playing ocarina, or using hookshot */
    uint8_t cond = gb_read(gb, wLinkPlayingOcarinaCountdown) |
                   gb_read(gb, wIsLinkInTheAir) |
                   gb_read(gb, wIsUsingHookshot);
    if (cond != 0) {
        return;
    }

    gb_write(gb, wC5A4, 0);
    gb_write(gb, wC5A5, 0);

    CopyLinkFinalPositionToPosition(gb);

    uint8_t song_flags = gb_read(gb, wOcarinaSongFlags) & 0x07;
    if (song_flags == 0) {
        /* No songs available: tone-deaf notes */
        gb_write(gb, wLinkPlayingOcarinaCountdown, 0xD0);
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_NOSONG);
        return;
    }

    uint8_t song_index = gb_read(gb, wSelectedSongIndex);
    if (song_index == 1) {
        /* Manbo's Mambo */
        gb_write(gb, wLinkPlayingOcarinaCountdown, 0xD0);
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_MAMBO);
    } else if (song_index == 2) {
        /* Frog's Song of the Soul */
        gb_write(gb, wLinkPlayingOcarinaCountdown, 0xBB);
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_FROG);
    } else {
        /* Ballad of the Wind Fish */
        gb_write(gb, wLinkPlayingOcarinaCountdown, 0xDC);
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_BALLAD);
    }
}

bool FireHookshot(GBState *gb) {
    if (!gb) return false;

    if (gb_read(gb, wIsLinkInTheAir) != 0) {
        return false;
    }

    uint8_t de = 0;
    if (!SpawnPlayerProjectile(gb, ENTITY_HOOKSHOT_CHAIN, &de)) {
        return false;
    }

    gb_write(gb, (uint16_t)(wEntitiesTransitionCountdownTable + de), 0x2A);
    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + de), 0x00);

    uint8_t dir = gb_read_hram(gb, hLinkDirection) & 0x03;
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + de), (uint8_t)HookshotChainSpeedX[dir]);
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + de), (uint8_t)HookshotChainSpeedY[dir]);

    return true;
}
