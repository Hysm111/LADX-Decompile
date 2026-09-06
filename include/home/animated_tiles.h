#ifndef LADX_HOME_ANIMATED_TILES_H
#define LADX_HOME_ANIMATED_TILES_H

#include "gb.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Animated background tiles group IDs.
 */
#define ANIMATED_TILES_NONE            0x00
#define ANIMATED_TILES_COUNTER         0x01
#define ANIMATED_TILES_TIDE            0x02
#define ANIMATED_TILES_VILLAGE         0x03
#define ANIMATED_TILES_DUNGEON_1       0x04
#define ANIMATED_TILES_UNDERGROUND     0x05
#define ANIMATED_TILES_LAVA            0x06
#define ANIMATED_TILES_DUNGEON_2       0x07
#define ANIMATED_TILES_WARP_TILE       0x08
#define ANIMATED_TILES_CURRENTS        0x09
#define ANIMATED_TILES_WATERFALL       0x0A
#define ANIMATED_TILES_WATERFALL_SLOW  0x0B
#define ANIMATED_TILES_WATER_DUNGEON   0x0C
#define ANIMATED_TILES_LIGHT_BEAM      0x0D
#define ANIMATED_TILES_CRYSTAL_BLOCK   0x0E
#define ANIMATED_TILES_BUBBLES         0x0F
#define ANIMATED_TILES_WEATHER_VANE    0x10
#define ANIMATED_TILES_PHOTO           0x11

/**
 * Special animated waves tiles for Marin dialog on beach (00:1ACC).
 */
void AnimateMarinBeachTiles(GBState *gb);

/**
 * Counter animated tiles group (00:1BF9).
 */
void AnimateCounterTilesGroup(GBState *gb,
                             void (*load_counter_tiles)(GBState *),
                             void (*func_020_54f5)(GBState *));

/**
 * Loads animated tiles frame at given offset into VRAM (00:1C13).
 */
void LoadAnimatedTilesFrameAtOffset(GBState *gb, uint8_t group_high, uint8_t offset,
                                    void (*configure_copy)(GBState *, uint8_t, uint8_t *));

/**
 * Slow animated tile group handlers (00:1C16, 00:1C1A, 00:1C1E, 00:1C22, 00:1C8A).
 */
void AnimateTideTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                           void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateVillageTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                              void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateWaterDungeonTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                   void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateSlowWaterfallTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                    void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateUndergroundTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                  void (*configure_copy)(GBState *, uint8_t, uint8_t *));

/**
 * General slow-speed animated tile updater (every 16 frames) (00:1C24).
 */
void AnimateTilesSlowSpeed(GBState *gb, uint8_t group_high,
                           void (*func_020_54f5)(GBState *),
                           void (*configure_copy)(GBState *, uint8_t, uint8_t *));

/**
 * Dungeon 1 and Lava animated tiles groups (00:1C39, 00:1C8E).
 */
void AnimateDungeon1TilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                               void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateLavaTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                           void (*configure_copy)(GBState *, uint8_t, uint8_t *));

/**
 * Loads animated tiles frame and handles Color Dungeon special VRAM configuration (00:1C51).
 */
void LoadAnimatedTilesFrame(GBState *gb, uint16_t src_hl, uint16_t dest_de,
                            void (*configure_copy)(GBState *, uint8_t, uint8_t *));

/**
 * Dungeon 2 scrolling tiles group and auxiliary frame switcher (00:1CA9, 00:1CB8).
 */
void AnimateDungeon2TilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                               void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void label_1CB8(GBState *gb, void (*func_020_54f5)(GBState *),
                void (*configure_copy)(GBState *, uint8_t, uint8_t *));

/**
 * Medium-speed animated tile groups (every 8 frames) (00:1CC6, 00:1CC8, 00:1CF3, 00:1CF7, 00:1CFB).
 */
void AnimateWarpTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                           void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateTilesMediumSpeed(GBState *gb, uint8_t group_high,
                             void (*func_020_54f5)(GBState *),
                             void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateBubblesTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                              void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateWeatherVaneTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                  void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateCrystalBlockTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                   void (*configure_copy)(GBState *, uint8_t, uint8_t *));

/**
 * Fast-speed animated tile groups (every 4 frames) (00:1CD3, 00:1CD6, 00:1CE1, 00:1CEF).
 */
void AnimateWaterCurrentsTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                    void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateTilesFastSpeed(GBState *gb, uint8_t group_high,
                           void (*func_020_54f5)(GBState *),
                           void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateWaterfallTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                void (*configure_copy)(GBState *, uint8_t, uint8_t *));
void AnimateLightBeamTilesGroup(GBState *gb, void (*func_020_54f5)(GBState *),
                                void (*configure_copy)(GBState *, uint8_t, uint8_t *));

/**
 * Advances the animated tiles data offset by $40 bytes (00:1CEE).
 */
uint8_t IncrementAnimatedTilesDataOffset(GBState *gb);

/**
 * Photo album animated tiles group dispatcher (00:1D07).
 */
void AnimatePhotoTilesGroup(GBState *gb, void (*func_038_7830)(GBState *));

/**
 * Copies D bytes of Link sprite graphics from BC to HL in bank $0C, then switches to bank $20 (00:1D0A).
 */
void CopyLinkTilesPair(GBState *gb, uint16_t src_bc, uint16_t dest_hl, uint8_t length_d);

/**
 * Skips tile group update and calls bank $20 helper func_020_54F5 (00:1D1E).
 */
void SkipTilesGroupAnimation(GBState *gb, void (*func_020_54f5)(GBState *));


/**
 * Draws Link's 2-part sprite into wLinkOAMBuffer during V-Blank (00:1D2E).
 */
void DrawLinkSprite(GBState *gb);
void DrawLinkSpriteAndReturn(GBState *gb);

/**
 * Replaces Marin sprite tiles between standing and sitting (00:1DF4).
 */
void ReplaceMarinTiles(GBState *gb, bool is_sitting);

/**
 * Updates tile graphics for the current trading sequence item (00:1E0C).
 */
void ReplaceTradingItemTiles(GBState *gb);

/**
 * Tile replacement routines triggered by hReplaceTiles (00:1E2B - 00:1EBC).
 */
void ReplaceMagicPowderTilesByToadstool(GBState *gb);
void ReplaceDialogTilesByInstruments(GBState *gb);
void ReplaceEndCreditsTiles(GBState *gb, uint16_t src_hl, uint16_t dest_de);
void ReplaceTiles_08(GBState *gb);
void ReplaceToadstoolTilesByMagicPowder(GBState *gb);
void ReplaceSlimeKeyTilesByGoldenLeaf(GBState *gb);
void ReplaceTilesPairAndDrawLinkSprite(GBState *gb, uint16_t src_hl, uint16_t dest_de);
void ReplaceTilesButtonPressed(GBState *gb);
void ReplaceTiles_04(GBState *gb);

/**
 * Updates switch block tiles during V-Blank based on animation stage (00:1ED7).
 */
void UpdateSwitchBlockTiles(GBState *gb, uint8_t stage);

/**
 * Copies tile data to VRAM and draws Link sprite (00:1F38, 00:1F3B).
 */
void Copy4TilesAndDrawLinkSprite(GBState *gb, uint16_t src_hl, uint16_t dest_de);
void CopyDataAndDrawLinkSprite(GBState *gb, uint16_t src_hl, uint16_t dest_de, uint16_t size_bc);

/**
 * Main V-Blank animated background tiles dispatcher (00:1B0D).
 */

/**
 * AnimateTilesGroup / AnimateTiles.jumpTable (00:1BD2)
 * Jump table executing the animation routine for the active hAnimatedTilesGroup.
 */
void AnimateTilesGroup(GBState *gb,
                       void (*load_counter_tiles)(GBState *),
                       void (*func_020_54f5)(GBState *),
                       void (*configure_copy)(GBState *, uint8_t, uint8_t *),
                       void (*func_038_7830)(GBState *));

void AnimateTiles(GBState *gb,
                  void (*load_counter_tiles)(GBState *),
                  void (*func_020_54f5)(GBState *),
                  void (*configure_copy)(GBState *, uint8_t, uint8_t *),
                  void (*func_038_7830)(GBState *));

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_ANIMATED_TILES_H */
