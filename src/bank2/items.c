#include "bank2/items.h"
#include "constants/dialog.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/link.h"
#include "constants/memory.h"
#include "constants/sfx.h"
#include "home/check_items_to_use.h"
#include "home/link.h"

const int8_t HookshotChainSpeedX[4] = {
    0x30,  /* DIRECTION_RIGHT:  HOOKSHOT_CHAIN_SPEED ($30) */
    -0x30, /* DIRECTION_LEFT:  -HOOKSHOT_CHAIN_SPEED (-$30) */
    0x00,  /* DIRECTION_UP:     0 */
    0x00   /* DIRECTION_DOWN:   0 */
};

const int8_t HookshotChainSpeedY[4] = {
    0x00,  /* DIRECTION_RIGHT:  0 */
    0x00,  /* DIRECTION_LEFT:   0 */
    -0x30, /* DIRECTION_UP:    -HOOKSHOT_CHAIN_SPEED (-$30) */
    0x30   /* DIRECTION_DOWN:   HOOKSHOT_CHAIN_SPEED ($30) */
};

bool FireHookshot(GBState *gb) {
    if (!gb) return false;

    /* ld a, [wIsLinkInTheAir]; and a; ret nz */
    if (gb_read(gb, wIsLinkInTheAir) != 0) {
        return false;
    }

    /* Spawn hookshot chain projectile */
    uint8_t slot = 0;
    if (!SpawnPlayerProjectile(gb, ENTITY_HOOKSHOT_CHAIN, &slot)) {
        return false;
    }

    /* Transition countdown = $2A; variant = 0 */
    gb_write(gb, (uint16_t)(wEntitiesTransitionCountdownTable + slot), 0x2A);
    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + slot), 0);

    /* Assign directional velocity vectors */
    uint8_t dir = gb_read_hram(gb, hLinkDirection) & 0x03;
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + slot), (uint8_t)HookshotChainSpeedX[dir]);
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + slot), (uint8_t)HookshotChainSpeedY[dir]);

    return true;
}

uint8_t func_002_523A(GBState *gb, uint16_t bc, uint8_t e) {
    if (gb) gb_write(gb, bc, e);
    return 0x14;
}

uint8_t func_002_523F(GBState *gb, uint16_t bc, uint8_t e) {
    if (gb) gb_write(gb, bc, e);
    return 0x14;
}

uint8_t func_002_524A(GBState *gb, uint16_t bc, uint8_t e) {
    if (gb) gb_write(gb, bc, e);
    return 0x10;
}

void HandleGotItemB(GBState *gb,
                    void (*apply_motion_state)(GBState *),
                    void (*func_020_4ab3)(GBState *)) {
    if (!gb) return;

    ResetSpinAttack(gb);
    gb_write(gb, wC16A, 0x00);
    gb_write(gb, wSwordAnimationState, 0x00);
    gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x00);

    if (apply_motion_state) {
        apply_motion_state(gb);
    } else {
        ApplyLinkMotionState(gb, NULL, NULL, NULL);
    }

    func_21E1(gb);

    int8_t vz = (int8_t)(gb_read_hram(gb, hLinkVelocityZ) - 2);
    gb_write_hram(gb, hLinkVelocityZ, (uint8_t)vz);

    uint8_t pz = gb_read_hram(gb, hLinkPositionZ);
    if ((pz & 0x80) != 0) {
        gb_write_hram(gb, hLinkPositionZ, 0x00);
        gb_write(gb, wC149, 0x00);
        gb_write_hram(gb, hLinkVelocityZ, 0x00);
    }

    gb_write_hram(gb, hLinkAnimationState, LINK_ANIMATION_STATE_UNKNOWN_6B);

    uint16_t bc = (uint16_t)(wLinkOAMBuffer + 0x10);
    uint8_t py = gb_read_hram(gb, hLinkPositionY);
    pz = gb_read_hram(gb, hLinkPositionZ);
    uint8_t c13b = gb_read(gb, wC13B);
    uint8_t multi0 = (uint8_t)(py - pz + c13b - 0x10);
    gb_write_hram(gb, hMultiPurpose0, multi0);

    uint8_t got_item = gb_read(gb, wDialogGotItem);
    if (got_item == DIALOG_GOT_PIECE_OF_POWER) {
        gb_write_hram(gb, hMultiPurpose1, (uint8_t)(gb_read_hram(gb, hLinkPositionX) - 0x08));
        uint8_t fc = gb_read_hram(gb, hFrameCounter);
        gb_write_hram(gb, hMultiPurpose3, (uint8_t)((fc << 2) & 0x10));
        gb_write_hram(gb, hMultiPurpose2, 0x06);
        func_1819(gb, func_020_4ab3);
        return;
    }

    gb_write(gb, bc++, (uint8_t)(multi0 + 0x02));
    gb_write(gb, bc++, gb_read_hram(gb, hLinkPositionX));

    uint8_t tile;
    uint8_t attr;
    if (got_item == DIALOG_GOT_GUARDIAN_ACORN) {
        tile = 0xAE;
        attr = func_002_523A(gb, bc, tile);
    } else if (got_item == DIALOG_GOT_MAGIC_POWDER) {
        tile = 0x8E;
        gb_write(gb, bc, tile);
        attr = 0x16;
    } else if (got_item == DIALOG_GOT_ROD) {
        tile = 0x8C;
        attr = func_002_524A(gb, bc, tile);
    } else {
        tile = 0x8E;
        attr = func_002_523F(gb, bc, tile);
    }

    bc++;
    gb_write(gb, bc, attr);
}

void HandleGotItemA(GBState *gb,
                    void (*apply_motion_state)(GBState *),
                    void (*func_020_4ab3)(GBState *)) {
    if (!gb) return;

    if (gb_read(gb, wDialogGotItemCountdown) == 0x2E) {
        gb_write_hram(gb, hJingle, JINGLE_GOT_POWER_UP);
    }

    HandleGotItemB(gb, apply_motion_state, func_020_4ab3);
}

/* Bank 2 Magic Rod Visuals & Attack Sprites (02:52E0-02:53AF) */

const int8_t LinkDirectionToMagicRodXOffset[8] = {
    0x0D, (int8_t)0xF3, 0x00, (int8_t)0xFF,
    0x08, (int8_t)0xF8, 0x0C, (int8_t)0xF5
};

const int8_t LinkDirectionToMagicRodYOffset[8] = {
    0x00, 0x00, (int8_t)0xF3, 0x0E,
    (int8_t)0xF3, (int8_t)0xF3, (int8_t)0xFC, 0x00
};

const uint8_t LinkDirectionToMagicRodTiles[16] = {
    0x06, 0x08, 0x08, 0x06, 0x04, 0xFF, 0xFF, 0x04,
    0x04, 0xFF, 0xFF, 0x04, 0x06, 0x08, 0x08, 0x06
};

const uint8_t LinkDirectionToMagicRodOAMAttributes[16] = {
    0x02, 0x02, 0x22, 0x22, 0x22, 0x02, 0x02, 0x42,
    0x22, 0x02, 0x02, 0x22, 0x02, 0x02, 0x22, 0x22
};

const int8_t LinkDirectionToEntitiesPositionX[4] = {
    4, -4, -4, 4
};

const int8_t LinkDirectionToEntitiesPositionY[4] = {
    4, 4, -4, 4
};

static const uint8_t data_13AD[8] = {
    0x30, 0xD0, 0x00, 0x00,
    0x40, 0xC0, 0x00, 0x00
};

static const uint8_t data_13B5[8] = {
    0x00, 0x00, 0xD0, 0x30,
    0x00, 0x00, 0xC0, 0x40
};

void label_002_5310(GBState *gb) {
    if (!gb) return;

    uint8_t countdown = gb_read(gb, wLinkAttackStepAnimationCountdown) & ATTACK_STEP_DURATION_MASK;
    uint8_t dir = gb_read_hram(gb, hLinkDirection);
    if (countdown >= 0x08) {
        dir += 0x04;
    }
    uint8_t offset = dir & 0x07;

    uint8_t y_offset = (uint8_t)LinkDirectionToMagicRodYOffset[offset];
    gb_write_hram(gb, hMultiPurpose0, y_offset);

    uint8_t x_offset = (uint8_t)LinkDirectionToMagicRodXOffset[offset];
    gb_write_hram(gb, hMultiPurpose1, x_offset);

    uint8_t tile0 = LinkDirectionToMagicRodTiles[offset * 2];
    gb_write_hram(gb, hMultiPurpose2, tile0);

    uint8_t tile1 = LinkDirectionToMagicRodTiles[offset * 2 + 1];
    gb_write_hram(gb, hMultiPurpose3, tile1);

    uint8_t attr0 = LinkDirectionToMagicRodOAMAttributes[offset * 2];
    gb_write_hram(gb, hMultiPurpose4, attr0);

    uint8_t attr1 = LinkDirectionToMagicRodOAMAttributes[offset * 2 + 1];
    gb_write_hram(gb, hMultiPurpose5, attr1);

    uint16_t de = wLinkOAMBuffer + 0x10;
    uint16_t bc = wLinkOAMBuffer + 0x14;

    uint8_t final_y = (uint8_t)(gb_read(gb, wC145) + gb_read(gb, wC13B) + y_offset);
    gb_write_hram(gb, hMultiPurpose0, final_y);

    if (tile0 != 0xFF) {
        gb_write(gb, de, final_y);
    }
    if (tile1 != 0xFF) {
        gb_write(gb, bc, final_y);
    }

    de++;
    bc++;

    uint8_t pos_x = (uint8_t)(x_offset + gb_read_hram(gb, hLinkPositionX));
    gb_write(gb, de, pos_x);
    gb_write(gb, bc, (uint8_t)(pos_x + 0x08));

    de++;
    bc++;

    gb_write(gb, de, tile0);
    gb_write(gb, bc, tile1);

    de++;
    bc++;

    gb_write(gb, de, attr0);
    gb_write(gb, bc, attr1);
}

void label_002_538B_entity(GBState *gb, uint8_t de) {
    if (!gb) return;

    uint8_t dir = gb_read_hram(gb, hLinkDirection) & 0x03;
    uint8_t pos_x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) + (uint8_t)LinkDirectionToEntitiesPositionX[dir]);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + de), pos_x);

    uint8_t pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) + (uint8_t)LinkDirectionToEntitiesPositionY[dir]);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + de), pos_y);

    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + de), 0x00);

    /* label_140F */
    uint8_t offset = dir;
    if (gb_read(gb, wActivePowerUp) == POWER_UP_PIECE_OF_POWER) {
        offset += 4;
    }
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + de), data_13AD[offset]);
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + de), data_13B5[offset]);
}

void label_002_538B(GBState *gb) {
    label_002_538B_entity(gb, 0);
}
