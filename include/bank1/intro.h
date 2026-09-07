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

#ifdef __cplusplus
}
#endif

#endif /* BANK1_INTRO_H */
