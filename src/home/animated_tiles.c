#include "home/animated_tiles.h"
#include "home/bank.h"
#include "home/copy_data.h"
#include "constants/memory.h"
#include "constants/hardware.h"
#include "constants/gameplay.h"

static const uint8_t AnimatedTilesDataOffsets[8] = { 0, 0x40, 0x80, 0xC0, 0xC0, 0xC0, 0x80, 0x40 };

void AnimateMarinBeachTiles(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wDrawCommand) != 0) {
        return;
    }

    uint8_t bank = AdjustBankNumberForGBC(gb, 0x10);
    gb_write(gb, rSelectROMBank, bank);

    uint16_t hl = 0x6500; /* MarinBeachWavesTiles */
    uint16_t de = 0x9500; /* vTiles2 + $500 */

    uint8_t frame = gb_read(gb, hFrameCounter);
    if ((frame & 0x0F) != 0) {
        if ((frame & 0x0F) != 0x08) {
            return;
        }
        hl = (hl & 0xFF00) | 0x40;
        de = (de & 0xFF00) | 0x40;
    }

    uint16_t bc = (uint16_t)((frame & 0x30) << 3);
    hl = (uint16_t)(hl + bc);

    CopyData(gb, de, hl, 0x40);
}

uint8_t IncrementAnimatedTilesDataOffset(GBState *gb) {
    if (!gb) return 0;
    uint8_t offset = (uint8_t)(gb_read(gb, hAnimatedTilesDataOffset) + 0x40);
    gb_write(gb, hAnimatedTilesDataOffset, offset);
    return offset;
}

void SkipTilesGroupAnimation(GBState *gb, void (*func_020_54f5)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x20);
    if (func_020_54f5) {
        func_020_54f5(gb);
    }
    uint8_t bank = AdjustBankNumberForGBC(gb, 0x0C);
    gb_write(gb, rSelectROMBank, bank);
}

void LoadAnimatedTilesFrame(GBState *gb, uint16_t src_hl, uint16_t dest_de,
                            void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    if (!gb) return;

    CopyData(gb, dest_de, src_hl, 0x40);

    if (gb_read(gb, hMapId) == MAP_COLOR_DUNGEON) {
        gb_write(gb, rSelectROMBank, 0x20);
        uint8_t bank1 = 0;
        if (configure_copy) {
            configure_copy(gb, 1, &bank1);
            if (bank1 != 0) {
                gb_write(gb, rSelectROMBank, bank1);
                CopyData(gb, dest_de, src_hl, 0x40);
            }
            gb_write(gb, rSelectROMBank, 0x20);
            uint8_t bank0 = 0;
            configure_copy(gb, 0, &bank0);
            if (bank0 != 0) {
                gb_write(gb, rSelectROMBank, bank0);
                CopyData(gb, 0x96C0, src_hl, 0x40);
            }
        }
    }
}

void LoadAnimatedTilesFrameAtOffset(GBState *gb, uint8_t group_high, uint8_t offset,
                                    void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    if (!gb) return;
    uint16_t src = (uint16_t)(((uint16_t)group_high << 8) | offset);
    LoadAnimatedTilesFrame(gb, src, 0x96C0, configure_copy);
}

void AnimateTilesSlowSpeed(GBState *gb, uint8_t group_high,
                           void (*func_020_54f5)(GBState *),
                           void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    if (!gb) return;
    if ((gb_read(gb, hAnimatedTilesFrameCount) & 0x0F) != 0) {
        SkipTilesGroupAnimation(gb, func_020_54f5);
        return;
    }
    uint8_t offset = IncrementAnimatedTilesDataOffset(gb);
    LoadAnimatedTilesFrameAtOffset(gb, group_high, offset, configure_copy);
}

void AnimateTilesMediumSpeed(GBState *gb, uint8_t group_high,
                             void (*func_020_54f5)(GBState *),
                             void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    if (!gb) return;
    if ((gb_read(gb, hAnimatedTilesFrameCount) & 0x07) != 0) {
        SkipTilesGroupAnimation(gb, func_020_54f5);
        return;
    }
    uint8_t offset = IncrementAnimatedTilesDataOffset(gb);
    LoadAnimatedTilesFrameAtOffset(gb, group_high, offset, configure_copy);
}

void AnimateTilesFastSpeed(GBState *gb, uint8_t group_high,
                           void (*func_020_54f5)(GBState *),
                           void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    if (!gb) return;
    if ((gb_read(gb, hAnimatedTilesFrameCount) & 0x03) != 0) {
        SkipTilesGroupAnimation(gb, func_020_54f5);
        return;
    }
    uint8_t offset = IncrementAnimatedTilesDataOffset(gb);
    LoadAnimatedTilesFrameAtOffset(gb, group_high, offset, configure_copy);
}

void AnimateCounterTilesGroup(GBState *gb,
                             void (*load_counter_tiles)(GBState *),
                             void (*func_020_54f5)(GBState *)) {
    if (!gb) return;
    if ((gb_read(gb, hAnimatedTilesFrameCount) & 0x07) != 0) {
        SkipTilesGroupAnimation(gb, func_020_54f5);
        return;
    }
    gb_write(gb, rSelectROMBank, 0x01);
    if (load_counter_tiles) {
        load_counter_tiles(gb);
    }
    uint8_t bank = AdjustBankNumberForGBC(gb, 0x0C);
    gb_write(gb, rSelectROMBank, bank);
}

void AnimateTideTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                           void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesSlowSpeed(gb, 0x6B, func_020_54f5, configure_copy);
}

void AnimateVillageTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                              void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesSlowSpeed(gb, 0x6C, func_020_54f5, configure_copy);
}

void AnimateWaterDungeonTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                   void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesSlowSpeed(gb, 0x73, func_020_54f5, configure_copy);
}

void AnimateSlowWaterfallTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                    void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesSlowSpeed(gb, 0x6A, func_020_54f5, configure_copy);
}

void AnimateUndergroundTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                  void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesSlowSpeed(gb, 0x6E, func_020_54f5, configure_copy);
}

void AnimateDungeon1TilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                               void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    if (!gb) return;
    uint8_t count = gb_read(gb, hAnimatedTilesFrameCount);
    if ((count & 0x07) != 0) {
        SkipTilesGroupAnimation(gb, func_020_54f5);
        return;
    }
    uint8_t idx = (uint8_t)((count >> 3) & 0x07);
    uint8_t offset = AnimatedTilesDataOffsets[idx];
    uint16_t hl = (uint16_t)((0x6D << 8) | offset);
    LoadAnimatedTilesFrame(gb, hl, 0x96C0, configure_copy);
}

void AnimateLavaTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                           void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    if (!gb) return;
    uint8_t count = gb_read(gb, hAnimatedTilesFrameCount);
    if ((count & 0x07) != 0) {
        SkipTilesGroupAnimation(gb, func_020_54f5);
        return;
    }
    uint8_t idx = (uint8_t)((count >> 3) & 0x07);
    uint8_t offset = AnimatedTilesDataOffsets[idx];
    uint16_t hl = (uint16_t)((0x6F << 8) | offset);
    LoadAnimatedTilesFrame(gb, hl, 0x96C0, configure_copy);
}

void label_1CB8(GBState *gb, void (*func_020_54f5)(GBState *),
                void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    if (!gb) return;
    uint8_t next_count = (uint8_t)(gb_read(gb, hAnimatedTilesFrameCount) + 1);
    if ((next_count & 0x03) != 0) {
        AnimateDungeon1TilesGroup(gb, func_020_54f5, configure_copy);
        return;
    }
    LoadAnimatedTilesFrame(gb, wAnimatedScrollingTilesStorage, 0x90C0, configure_copy);
}

void AnimateDungeon2TilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                               void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    if (!gb) return;
    if (gb_read(gb, hMapId) != MAP_COLOR_DUNGEON) {
        label_1CB8(gb, func_020_54f5, configure_copy);
        return;
    }
    LoadAnimatedTilesFrame(gb, wAnimatedScrollingTilesStorage, 0x8400, configure_copy);
}

void AnimateWarpTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                           void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesMediumSpeed(gb, 0x70, func_020_54f5, configure_copy);
}

void AnimateWaterCurrentsTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                    void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesFastSpeed(gb, 0x71, func_020_54f5, configure_copy);
}

void AnimateWaterfallTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesFastSpeed(gb, 0x72, func_020_54f5, configure_copy);
}

void AnimateLightBeamTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesFastSpeed(gb, 0x75, func_020_54f5, configure_copy);
}

void AnimateBubblesTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                              void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesMediumSpeed(gb, 0x74, func_020_54f5, configure_copy);
}

void AnimateWeatherVaneTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                  void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesMediumSpeed(gb, 0x77, func_020_54f5, configure_copy);
}

void AnimateCrystalBlockTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                   void (*configure_copy)(GBState *, uint8_t, uint8_t *)) {
    AnimateTilesMediumSpeed(gb, 0x76, func_020_54f5, configure_copy);
}

void AnimatePhotoTilesGroup(GBState *gb, void (*func_038_7830)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x38);
    if (func_038_7830) {
        func_038_7830(gb);
    }
}

void CopyLinkTilesPair(GBState *gb, uint16_t src_bc, uint16_t dest_hl, uint8_t length_d) {
    if (!gb) return;
    uint8_t bank = AdjustBankNumberForGBC(gb, 0x0C);
    gb_write(gb, rSelectROMBank, bank);

    while (length_d > 0) {
        uint8_t val = gb_read(gb, src_bc++);
        gb_write(gb, dest_hl++, val);
        length_d--;
    }

    gb_write(gb, rSelectROMBank, 0x20);
}
