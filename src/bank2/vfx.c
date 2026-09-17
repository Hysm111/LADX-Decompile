#include "bank2/vfx.h"
#include "bank2/room_events.h"
#include "constants/memory.h"
#include "constants/sfx.h"
#include "constants/vfx.h"
#include "home/bank.h"
#include "home/gameplay.h"
#include "home/vfx.h"

void shallowWaterVfx(GBState *gb) {
    if (!gb) return;

    /* ldh a, [hLinkPositionY]; ldh [hMultiPurpose1], a */
    gb_write_hram(gb, hMultiPurpose1, gb_read_hram(gb, hLinkPositionY));

    /* ldh a, [hLinkPositionX]; ldh [hMultiPurpose0], a */
    gb_write_hram(gb, hMultiPurpose0, gb_read_hram(gb, hLinkPositionX));

    /* ld a, JINGLE_WATER_SPLASH; ldh [hJingle], a */
    gb_write_hram(gb, hJingle, JINGLE_WATER_SPLASH);

    /* ld a, TRANSCIENT_VFX_PEGASUS_SPLASH; jp AddTranscientVfx */
    AddTranscientVfx(gb, TRANSCIENT_VFX_PEGASUS_SPLASH);
}

void func_002_5928(GBState *gb, uint8_t y) {
    if (!gb) return;

    gb_write_hram(gb, hMultiPurpose1, y);
    gb_write_hram(gb, hMultiPurpose0, gb_read_hram(gb, hLinkPositionX));
    gb_write_hram(gb, hJingle, JINGLE_WATER_SPLASH);
    AddTranscientVfx(gb, TRANSCIENT_VFX_WATER_SPLASH);
}

/* ========================================================================= */
/* Bank 2: Transient Visual Effects Subsystem (02:5567 - 02:5926)            */
/* ========================================================================= */

const uint8_t Data_002_559C[32] = {
    0x00, 0x00, 0x08, 0x20, 0x00, 0x08, 0x06, 0x20,
    0x00, 0x00, 0x06, 0x00, 0x00, 0x08, 0x08, 0x00,
    0x00, 0x04, 0x04, 0x40, 0x00, 0x04, 0x04, 0x40,
    0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00
};

const uint8_t Data_002_55BC[32] = {
    0x00, 0x00, 0x08, 0x30, 0x00, 0x08, 0x06, 0x30,
    0x00, 0x00, 0x06, 0x10, 0x00, 0x08, 0x08, 0x10,
    0x00, 0x04, 0x04, 0x50, 0x00, 0x04, 0x04, 0x50,
    0x00, 0x04, 0x04, 0x10, 0x00, 0x04, 0x04, 0x10
};

const uint8_t Data_002_5642[4] = {
    0x7E, 0x1F, 0x0C, 0x1F
};

const uint8_t Data_002_5708[16] = {
    0x00, 0x04, 0x24, 0x01, 0x00, 0x04, 0x24, 0x01,
    0x00, 0x00, 0x1E, 0x01, 0x00, 0x08, 0x1E, 0x61
};

const uint8_t Data_002_5736[16] = {
    0x00, 0x00, 0x1E, 0x01, 0x00, 0x08, 0x1E, 0x61,
    0x00, 0x00, 0x30, 0x01, 0x00, 0x08, 0x30, 0x61
};

const uint8_t Data_002_5756[4] = {
    0x01, 0xFF, 0x01, 0xFF
};

const uint8_t Data_002_575A[4] = {
    0x01, 0x01, 0xFF, 0xFF
};

const uint8_t Data_002_57DD[16] = {
    0x00, 0xFF, 0x3C, 0x00, 0x00, 0x07, 0x3C, 0x20,
    0x00, 0xFF, 0x3A, 0x00, 0x00, 0x07, 0x3A, 0x20
};

const uint8_t Data_002_57FD[16] = {
    0xF6, 0xFE, 0x18, 0x00, 0xF8, 0x0A, 0x18, 0x20,
    0xFC, 0x00, 0x18, 0x00, 0xFE, 0x08, 0x18, 0x20
};

const uint8_t Data_002_580D[16] = {
    0x00, 0xFA, 0x18, 0x00, 0x00, 0x0E, 0x18, 0x20,
    0x02, 0xFC, 0x18, 0x00, 0x02, 0x0C, 0x18, 0x20
};

const uint8_t Data_002_5867[16] = {
    0x00, 0x00, 0x7A, 0x00, 0x00, 0x08, 0x7A, 0x20,
    0x00, 0x00, 0x78, 0x00, 0x00, 0x08, 0x78, 0x20
};

const uint8_t Data_002_5884[32] = {
    0x00, 0x00, 0x32, 0x01, 0x00, 0x08, 0x32, 0x21,
    0x00, 0x00, 0x32, 0x01, 0x00, 0x08, 0x32, 0x21,
    0x00, 0x00, 0x30, 0x01, 0x00, 0x08, 0x30, 0x21,
    0x00, 0x00, 0x30, 0x01, 0x00, 0x08, 0x30, 0x21
};

const uint8_t Data_002_58ED[8] = {
    0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38
};

void ClearTranscientVfx(GBState *gb, uint8_t slot) {
    if (!gb) return;
    gb_write(gb, (uint16_t)(wTranscientVfxTypeTable + slot), 0x00);
}

void func_002_58D0(GBState *gb, uint8_t slot) {
    if (!gb) return;

    uint8_t py = gb_read(gb, (uint16_t)(wTranscientVfxPosYTable + slot));
    gb_write_hram(gb, hMultiPurpose1, py);
    if (py >= 0x88) {
        ClearTranscientVfx(gb, slot);
        return;
    }

    uint8_t px = gb_read(gb, (uint16_t)(wTranscientVfxPosXTable + slot));
    gb_write_hram(gb, hMultiPurpose2, px);
    if (px >= 0xA8) {
        ClearTranscientVfx(gb, slot);
        return;
    }
}

void label_002_5854(GBState *gb, const uint8_t **src, uint16_t *dst_oam_addr) {
    if (!gb || !src || !*src || !dst_oam_addr) return;

    uint8_t py = gb_read_hram(gb, hMultiPurpose1);
    uint8_t px = gb_read_hram(gb, hMultiPurpose2);
    const uint8_t *s = *src;
    uint16_t de = *dst_oam_addr;

    gb_write(gb, de, (uint8_t)(py + s[0]));
    gb_write(gb, (uint16_t)(de + 1), (uint8_t)(px + s[1]));
    gb_write(gb, (uint16_t)(de + 2), s[2]);
    gb_write(gb, (uint16_t)(de + 3), s[3]);

    *src = s + 4;
    *dst_oam_addr = (uint16_t)(de + 4);
}

void label_002_58F5(GBState *gb, uint8_t count) {
    if (!gb) return;

    uint8_t e = (uint8_t)(count << 2);
    uint8_t next = (uint8_t)(gb_read(gb, wOAMNextAvailableSlot) + e);
    if (next >= 0x60) {
        next -= 0x60;
    }
    gb_write(gb, wOAMNextAvailableSlot, next);

    uint8_t c1 = (uint8_t)(gb_read(gb, wC3C1) + e);
    gb_write(gb, wC3C1, c1);
    if (c1 < 0x60) {
        return;
    }

    uint8_t fc = gb_read_hram(gb, hFrameCounter);
    uint8_t entity_idx = gb_read(gb, wActiveEntityIndex);
    uint8_t idx = (uint8_t)((fc + entity_idx) & 0x07);
    gb_write(gb, wOAMNextAvailableSlot, Data_002_58ED[idx]);
}

void label_002_583A(GBState *gb, const uint8_t *table, uint8_t offset) {
    if (!gb || !table) return;

    const uint8_t *src = table + offset;
    uint8_t oam_slot = gb_read(gb, wOAMNextAvailableSlot);
    uint16_t de = (uint16_t)(wDynamicOAMBuffer + oam_slot);

    label_002_5854(gb, &src, &de);
    label_002_5854(gb, &src, &de);
    label_002_58F5(gb, 2);
}

void label_002_5877(GBState *gb) {
    if (!gb) return;
    uint8_t offset = (uint8_t)(gb_read_hram(gb, hMultiPurpose0) & 0x08);
    label_002_583A(gb, Data_002_5867, offset);
}

void RenderTranscientWaterSplash(GBState *gb, uint8_t slot) {
    if (!gb) return;

    func_002_58D0(gb, slot);
    if (gb_read(gb, wC1A7) == 0x02) {
        label_002_5877(gb);
        return;
    }

    uint8_t offset = (uint8_t)(gb_read_hram(gb, hMultiPurpose0) & 0x08);
    label_002_583A(gb, Data_002_57FD, offset);
}

void RenderTranscientPegasusSplash(GBState *gb, uint8_t slot) {
    if (!gb) return;

    func_002_58D0(gb, slot);
    uint8_t offset = (uint8_t)(gb_read_hram(gb, hMultiPurpose0) & 0x08);
    label_002_583A(gb, Data_002_580D, offset);
}

void RenderTranscientPoof(GBState *gb, uint8_t slot, void (*spawn_chest)(GBState *), void (*spawn_stairs)(GBState *)) {
    if (!gb) return;

    func_002_58D0(gb, slot);
    uint8_t mp0 = gb_read_hram(gb, hMultiPurpose0);
    if (mp0 == 0x04) {
        uint8_t type = gb_read(gb, (uint16_t)(wTranscientVfxTypeTable + slot));
        if (type == TRANSCIENT_VFX_CHEST_APPEARS) {
            if (spawn_chest) {
                spawn_chest(gb);
            }
        } else if (type == TRANSCIENT_VFX_STAIRS_APPEARS) {
            if (spawn_stairs) {
                spawn_stairs(gb);
            }
        }
    }

    uint8_t offset = (uint8_t)((mp0 << 1) & 0x18);
    label_002_583A(gb, Data_002_5884, offset);
}

void RenderTranscientSmoke(GBState *gb, uint8_t slot) {
    if (!gb) return;

    func_002_58D0(gb, slot);
    uint8_t offset = (uint8_t)(gb_read_hram(gb, hMultiPurpose0) & 0x08);
    label_002_583A(gb, Data_002_5736, offset);
}

void RenderTranscientSwordPoke(GBState *gb, uint8_t slot) {
    if (!gb) return;

    func_002_58D0(gb, slot);
    uint8_t offset = (uint8_t)(gb_read_hram(gb, hMultiPurpose0) & 0x08);
    label_002_583A(gb, Data_002_57DD, offset);
}

void RenderTranscientLaserBeam(GBState *gb, uint8_t slot) {
    if (!gb) return;

    func_002_58D0(gb, slot);
    uint8_t oam_slot = gb_read(gb, wOAMNextAvailableSlot);
    uint16_t de = (uint16_t)(wDynamicOAMBuffer + oam_slot);

    gb_write(gb, de, gb_read_hram(gb, hMultiPurpose1));
    gb_write(gb, (uint16_t)(de + 1), gb_read_hram(gb, hMultiPurpose2));
    gb_write(gb, (uint16_t)(de + 2), 0x24);

    uint8_t attr = (uint8_t)(((gb_read_hram(gb, hFrameCounter) ^ slot) & 0x01) ? 0x10 : 0x00);
    gb_write(gb, (uint16_t)(de + 3), attr);

    label_002_58F5(gb, 1);
}

void RenderTranscientMovingSparkle(GBState *gb, uint8_t slot) {
    if (!gb) return;

    uint8_t mp0 = gb_read_hram(gb, hMultiPurpose0);
    if (mp0 >= 0x0A) {
        uint8_t dir = (uint8_t)(gb_read(gb, (uint16_t)(wC590 + slot)) & 0x03);
        uint8_t px = gb_read(gb, (uint16_t)(wTranscientVfxPosXTable + slot));
        px = (uint8_t)(px + Data_002_5756[dir]);
        gb_write(gb, (uint16_t)(wTranscientVfxPosXTable + slot), px);

        uint8_t py = gb_read(gb, (uint16_t)(wTranscientVfxPosYTable + slot));
        py = (uint8_t)(py + Data_002_575A[dir]);
        gb_write(gb, (uint16_t)(wTranscientVfxPosYTable + slot), py);
    }

    func_002_58D0(gb, slot);

    uint8_t tile = (gb_read_hram(gb, hMultiPurpose0) >= 0x07) ? 0x3A : 0x3C;
    uint8_t oam_slot = gb_read(gb, wOAMNextAvailableSlot);
    uint16_t de = (uint16_t)(wDynamicOAMBuffer + oam_slot);
    uint8_t py = gb_read_hram(gb, hMultiPurpose1);
    uint8_t px = gb_read_hram(gb, hMultiPurpose2);

    /* Sprite 0 */
    gb_write(gb, de, py);
    gb_write(gb, (uint16_t)(de + 1), px);
    gb_write(gb, (uint16_t)(de + 2), tile);
    gb_write(gb, (uint16_t)(de + 3), 0x00);

    /* Sprite 1 */
    gb_write(gb, (uint16_t)(de + 4), py);
    gb_write(gb, (uint16_t)(de + 5), (uint8_t)(px + 0x08));
    gb_write(gb, (uint16_t)(de + 6), tile);
    gb_write(gb, (uint16_t)(de + 7), 0x20);

    label_002_58F5(gb, 2);
}

void RenderTranscientLavaSplash(GBState *gb, uint8_t slot) {
    if (!gb) return;

    func_002_58D0(gb, slot);
    uint8_t oam_slot = gb_read(gb, wOAMNextAvailableSlot);
    uint16_t de = (uint16_t)(wDynamicOAMBuffer + oam_slot);
    uint8_t py = gb_read_hram(gb, hMultiPurpose1);
    uint8_t px = gb_read_hram(gb, hMultiPurpose2);

    static const struct {
        uint8_t offset_x;
        uint8_t tile;
        uint8_t attr;
    } splash_sprites[4] = {
        { 0x10, 0x6C, 0x20 },
        { 0x08, 0x6E, 0x20 },
        { 0x00, 0x6E, 0x00 },
        { 0xF8, 0x6C, 0x00 }
    };

    for (int i = 0; i < 4; i++) {
        gb_write(gb, de++, py);
        gb_write(gb, de++, (uint8_t)(px + splash_sprites[i].offset_x));
        gb_write(gb, de++, splash_sprites[i].tile);
        gb_write(gb, de++, splash_sprites[i].attr);
    }

    label_002_58F5(gb, 4);
}

void RenderTranscientPegasusDust(GBState *gb, uint8_t slot) {
    if (!gb) return;

    func_002_58D0(gb, slot);
    uint8_t offset = (uint8_t)(gb_read_hram(gb, hMultiPurpose0) & 0x08);
    if (gb_read(gb, wIsRunningWithPegasusBoots) != 0) {
        label_002_583A(gb, Data_002_5708, offset);
    } else {
        const uint8_t *src = Data_002_5708 + offset;
        uint16_t de = wOAMBuffer;
        label_002_5854(gb, &src, &de);
        label_002_5854(gb, &src, &de);
    }
}

void RenderTranscientRumble(GBState *gb, uint8_t slot) {
    if (!gb) return;

    gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x02);
    gb_write(gb, wC167, 0x02);
    gb_write(gb, wScreenShakeHorizontal, 0x00);

    uint8_t mp0 = gb_read_hram(gb, hMultiPurpose0);
    if (mp0 < 0x02) {
        gb_write(gb, wC167, 0x00);
    }

    if (mp0 == 0xDE) {
        EnqueueDoorUnlockedSfx(gb);
    }

    if (mp0 == 0xA0) {
        gb_write_hram(gb, hNoiseSfx, NOISE_SFX_OPEN_KEY_CAVERN);
    }

    if (mp0 == 0x0A) {
        gb_write(gb, wNextWorldMusicTrackCountdown, 0x50);
    }

    if (mp0 >= 0x20) {
        if (mp0 >= 0x9C) {
            return;
        }
        uint8_t shake = ((mp0 & 0x04) != 0) ? 0xFE : 0x01;
        gb_write(gb, wScreenShakeHorizontal, shake);
        return;
    }

    /* mp0 < 0x20 */
    if ((mp0 & 0x0F) != 0x08) {
        return;
    }

    uint8_t de_offset = (uint8_t)((mp0 >> 3) & 0x02);
    uint8_t tile0 = Data_002_5642[de_offset];
    uint8_t tile1 = Data_002_5642[de_offset + 1];
    gb_write_hram(gb, hMultiPurpose0, tile0);
    gb_write_hram(gb, hMultiPurpose1, tile1);

    gb_write_hram(gb, hIntersectedObjectLeft, 0x60);
    gb_write_hram(gb, hIntersectedObjectTop, 0x10);

    GetIntersectedObjectBGAddress(gb);

    uint8_t bg_hi = gb_read_hram(gb, hIntersectedObjectBGAddressHigh);
    uint8_t bg_lo = gb_read_hram(gb, hIntersectedObjectBGAddressLow);

    uint8_t cmd_size = gb_read(gb, wDrawCommandsSize);
    uint16_t cmd_ptr = (uint16_t)(wDrawCommand + cmd_size);

    gb_write(gb, cmd_ptr++, bg_hi);
    gb_write(gb, cmd_ptr++, bg_lo);
    gb_write(gb, cmd_ptr++, 0x41);
    gb_write(gb, cmd_ptr++, tile0);
    gb_write(gb, cmd_ptr++, bg_hi);
    gb_write(gb, cmd_ptr++, (uint8_t)(bg_lo + 0x20));
    gb_write(gb, cmd_ptr++, 0x41);
    gb_write(gb, cmd_ptr++, tile1);
    gb_write(gb, cmd_ptr, 0x00);

    gb_write(gb, wDrawCommandsSize, (uint8_t)(cmd_size + 8));

    if (gb_read(gb, (uint16_t)(wTranscientVfxCountdownTable + slot)) == 0x08) {
        uint16_t room_obj_addr = (uint16_t)(wRoomObjectsArea + 0x27);
        gb_write(gb, room_obj_addr, 0xE3);
        BackupObjectInRAM2(gb, room_obj_addr, 0x82);
        gb_write_hram(gb, hJingle, JINGLE_DUNGEON_OPENED);
    }
}

void RenderTranscientSwordBeam(GBState *gb, uint8_t slot) {
    if (!gb) return;

    if (((gb_read_hram(gb, hFrameCounter) ^ slot) & 0x01) == 0) {
        return;
    }

    func_002_58D0(gb, slot);

    uint8_t val = gb_read(gb, (uint16_t)(wC590 + slot));
    uint8_t offset = (uint8_t)((val << 3) & 0xF8);
    const uint8_t *table = ((gb_read_hram(gb, hFrameCounter) & 0x02) == 0) ? Data_002_559C : Data_002_55BC;
    label_002_583A(gb, table, offset);
}

void RenderTranscientVfx(GBState *gb, uint8_t slot, void (*spawn_chest)(GBState *), void (*spawn_stairs)(GBState *)) {
    if (!gb) return;

    uint8_t vfx_type = gb_read(gb, (uint16_t)(wTranscientVfxTypeTable + slot));
    if (vfx_type == TRANSCIENT_VFX_NONE) {
        return;
    }

    if (gb_read(gb, wRoomTransitionState) != 0) {
        ClearTranscientVfx(gb, slot);
    } else {
        uint8_t countdown = gb_read(gb, (uint16_t)(wTranscientVfxCountdownTable + slot));
        if (countdown != 0) {
            countdown--;
            gb_write(gb, (uint16_t)(wTranscientVfxCountdownTable + slot), countdown);
            gb_write_hram(gb, hMultiPurpose0, countdown);
            if (countdown == 0) {
                ClearTranscientVfx(gb, slot);
            }
        }
    }

    switch (vfx_type) {
        case TRANSCIENT_VFX_WATER_SPLASH:
            RenderTranscientWaterSplash(gb, slot);
            break;
        case TRANSCIENT_VFX_POOF:
        case TRANSCIENT_VFX_CHEST_APPEARS:
        case TRANSCIENT_VFX_STAIRS_APPEARS:
            RenderTranscientPoof(gb, slot, spawn_chest, spawn_stairs);
            break;
        case TRANSCIENT_VFX_SWORD_POKE:
            RenderTranscientSwordPoke(gb, slot);
            break;
        case TRANSCIENT_VFX_LASER_BEAM:
            RenderTranscientLaserBeam(gb, slot);
            break;
        case TRANSCIENT_VFX_MOVING_SPARKLE:
            RenderTranscientMovingSparkle(gb, slot);
            break;
        case TRANSCIENT_VFX_SMOKE:
            RenderTranscientSmoke(gb, slot);
            break;
        case TRANSCIENT_VFX_RUMBLE:
            RenderTranscientRumble(gb, slot);
            break;
        case TRANSCIENT_VFX_LAVA_SPLASH:
            RenderTranscientLavaSplash(gb, slot);
            break;
        case TRANSCIENT_VFX_PEGASUS_DUST:
            RenderTranscientPegasusDust(gb, slot);
            break;
        case TRANSCIENT_VFX_PEGASUS_SPLASH:
            RenderTranscientPegasusSplash(gb, slot);
            break;
        case TRANSCIENT_VFX_SWORD_BEAM:
            RenderTranscientSwordBeam(gb, slot);
            break;
        default:
            break;
    }
}

void RenderTranscientVfxDefault(GBState *gb, uint8_t slot) {
    RenderTranscientVfx(gb, slot, NULL, NULL);
}

void func_002_5926(GBState *gb) {
    if (!gb) return;
    func_002_5928(gb, gb_read_hram(gb, hLinkPositionY));
}
