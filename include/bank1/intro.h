#ifndef BANK1_INTRO_H
#define BANK1_INTRO_H

#include <stdint.h>
#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Constant data tables */
extern const uint8_t IntroSeaPaletteTable[4];
extern const uint8_t Data_001_6F93[9];
extern const uint8_t Data_001_6F9C[9];
extern const uint8_t Data_001_707B[6];
extern const uint8_t Data_001_7081[6];
extern const uint8_t Data_001_74B8[15];
extern const uint8_t IntroShipTiles[24];
extern const uint8_t Data_001_7550[16];
extern const uint8_t ShipHeaveTable[8];
extern const uint8_t IntroLightningTiles[96];
extern const uint8_t Data_001_789B;
extern const uint8_t Data_001_789F;
extern const uint8_t Data_001_7CE1[8];
extern const uint8_t Data_001_7CE9[8];
extern const uint8_t Data_001_7CF1[8];
extern const uint8_t IntroBGVerticalOffsetTable[8];

/* Bank 1 Intro Cutscene Part 2 tables */
extern const uint8_t Data_001_7128[16];
extern const uint8_t Data_001_7138[16];
extern const uint8_t Data_001_7148[12];
extern const uint8_t TitleTileMap1[19];
extern const uint8_t TitleTileMap2[19];
extern const uint8_t TitleTileMap3[19];
extern const uint8_t TitleTileMap4[19];
extern const uint8_t TitleTileMap5[19];
extern const uint8_t TitleTileMap6[19];
extern const uint8_t TitleTileMap7[19];
extern const uint8_t *const TitleTileMap[7];
extern const uint8_t TitleAttrMap1[19];
extern const uint8_t TitleAttrMap2[19];
extern const uint8_t TitleAttrMap3[19];
extern const uint8_t TitleAttrMap4[19];
extern const uint8_t TitleAttrMap5[19];
extern const uint8_t TitleAttrMap6[19];
extern const uint8_t TitleAttrMap7[19];
extern const uint8_t *const TitleAttrMap[7];
extern const uint8_t TitleScreenCopyrightDrawCommand[18];
extern const uint8_t CopyrightAttrmap[18];
extern const uint8_t Data_001_73C0[8];
extern const uint8_t Data_001_73C8[8];
extern const uint8_t DXTilesDMG[72];
extern const uint8_t DXTilesGBC[72];
extern const uint8_t Data_001_7898[4];
extern const uint8_t Data_001_789C[4];
extern const uint8_t DXFadeInPalette[128];

/* Bank 1 Intro Cutscene Part 3 tables */
extern const uint8_t IntroMarinSpriteVariants[16];
extern const uint8_t IntroSparkleSpriteVariants[32];
extern const uint8_t Data_001_79EC[24];
extern const uint8_t Data_001_79FD[24];
extern const uint8_t InertLinkSpriteVariants[8];
extern const uint8_t TitleScreenPostBeachTilemap[380];

/* Core Intro handlers */
void IntroHandlerEntryPoint(GBState *gb);
void RenderIntroFrame(GBState *gb);
void IntroSceneStage0Handler(GBState *gb);
void IntroSceneStage1Handler(GBState *gb);
void IntroSceneStage2Handler(GBState *gb);
void IntroShipOnSeaHandler(GBState *gb);
void RenderLightning(GBState *gb);
void IntroLinkFaceHandler(GBState *gb);
void LoadTileMapZero_trampoline(GBState *gb);
void ResetIntroTimers(GBState *gb);
void RenderRain(GBState *gb);
void IntroLinkScream(GBState *gb);
void RenderIntroEntities(GBState *gb);
void RenderIntroEntity(GBState *gb, uint16_t bc);
void RenderIntroShip(GBState *gb);
void func_001_762B(GBState *gb, uint16_t bc);
void func_001_7D01(GBState *gb);
void func_001_7D46(GBState *gb);
void func_001_7D4E(GBState *gb);

/* Part 2 handlers */
void IntroStage5Handler(GBState *gb);
void IntroStage6Handler(GBState *gb);
void IntroBeachHandler(GBState *gb);
void func_001_71C7(GBState *gb);
void IntroStage8Handler(GBState *gb);
void func_001_7338(GBState *gb);
void TitleScreenSfxHandler(GBState *gb);
void IntroStageAHandler(GBState *gb);
void func_001_73B1(GBState *gb);
void TitleScreenHandler(GBState *gb);
void IntroStageCHandler(GBState *gb);
void IntroStageDHandler(GBState *gb);
void func_001_79AE(GBState *gb, uint8_t a);
void func_001_79C2(GBState *gb, uint8_t a);
void func_001_7920(GBState *gb);
void func_001_7D9C(GBState *gb);
void func_001_7DCF(GBState *gb);

/* Part 3 handlers (Intro Entities: Marin, Inert Link, Sparkle) */
void RenderIntroMarin(GBState *gb);
void IntroMarinState0(GBState *gb);
void IntroMarinState1(GBState *gb);
void IntroMarinState2(GBState *gb);
void IntroMarinState3(GBState *gb);
void IntroMarinState4(GBState *gb);
void RenderIntroSparkle(GBState *gb);
void func_001_7A11(GBState *gb);
void func_001_7A16(GBState *gb);
void RenderIntroInertLink(GBState *gb);
void InertLinkState0Handler(GBState *gb);
void InertLinkState1Handler(GBState *gb);
void InertLinkState2Handler(GBState *gb);
void InertLinkState3Handler(GBState *gb);
void func_7C60(GBState *gb);
void func_001_7CCB(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* BANK1_INTRO_H */
