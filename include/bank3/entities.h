#ifndef LADX_BANK3_ENTITIES_H
#define LADX_BANK3_ENTITIES_H

#include "gb.h"

/* Entity Configuration (03:485B-03:4891) */
void ConfigureNewEntity(GBState *gb);

/* Entity Health Configuration (03:4895-03:48AC) */
void ConfigureEntityHealth(GBState *gb, uint16_t bc, uint8_t entity_type, uint8_t d);

/* Entity Initialization Handler (03:48B5-03:4923) */
void EntityInitHandler(GBState *gb);

/* Master Stalfos Defeated Handler (03:48AD-03:48BE) */
void MasterStalfosDefeated(GBState *gb);

/* Entity Init Functions (03:4926+) */
void EntityInitHorsePiece(GBState *gb);
void EntityInitMarinAtTalTalHeights(GBState *gb);
void EntityInitSnake(GBState *gb);
void EntityInitSideViewPlatformVertical(GBState *gb);
void EntityInitZol(GBState *gb);
void EntityInitMarinAtTheShore(GBState *gb);
void EntityInitBomber(GBState *gb);
void EntityInitBushCrawler(GBState *gb);
void EntityInitTarinBeekeeper(GBState *gb);
void EntityInitTelephone(GBState *gb);
void EntityInitRichard(GBState *gb);
void SetMusicTrackIfHasSword(GBState *gb);
void SetMusicTrack(GBState *gb);
void EntityInitFinalNightmare(GBState *gb);
void EntityInitDreamShrineBed(GBState *gb);
void EntityInitFishermanUnderBridge(GBState *gb);
void EntityInitKikiTheMonkey(GBState *gb);
void EntityInitFireballShooter(GBState *gb);
void EntityInitAntiKirby(GBState *gb);
void EntityInitMovingBlockMover(GBState *gb);
void EntityInitDesertLanmola(GBState *gb);
void EntityInitFloatingItem2(GBState *gb);
void EntityInitFloatingItem(GBState *gb);
void SetZPosForFloatingItem(GBState *gb);
void EntityInitKid71(GBState *gb);
void EntityInitKid72(GBState *gb);
void EntityInitMrWrite(GBState *gb);
void EntityInitBigFairy(GBState *gb);
void EntityInitBowWow(GBState *gb);
void EntityInitOwlEvent(GBState *gb);
void EntityInitSword(GBState *gb);
void UnloadEntityIfRoomStatusSet(GBState *gb);
void EntityInitMarin(GBState *gb);
void EntityInitTarin(GBState *gb);
void EntityInitMadamMeowMeow(GBState *gb);
void EntityInitRaftRaftOwner(GBState *gb);
void EntityInitNpcFacingDown(GBState *gb);
void EntityInitStoreOwner(GBState *gb);
void EntityInitWitch(GBState *gb);
void EntityInitShopOwner(GBState *gb);
void EntityInitWithRandomDirection(GBState *gb);
void SetEntityDirection(GBState *gb);

#endif /* LADX_BANK3_ENTITIES_H */