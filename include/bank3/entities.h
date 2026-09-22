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
void SetMusicTrackIfHasSword(GBState *gb, uint8_t music_track);
void SetMusicTrack(GBState *gb, uint8_t music_track);
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
void SetZPosForFloatingItem(GBState *gb, uint16_t bc);
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
void EntityInitNpcFacingDown(GBState *gb, uint16_t bc);
void EntityInitStoreOwner(GBState *gb, uint16_t bc);
void EntityInitWitch(GBState *gb);
void EntityInitShopOwner(GBState *gb);
void EntityInitShopOwner_setDirectionLeft(GBState *gb, uint16_t bc);
void EntityInitWithRandomDirection(GBState *gb);
void SetEntityDirection(GBState *gb, uint16_t bc, uint8_t direction);
void EntityInitNoop(GBState *gb);

/* Helper Functions */
void EntityShiftPosition(GBState *gb, uint16_t bc);
void EntityShiftPosition_shiftBy8(GBState *gb, uint16_t bc, uint16_t sign_table, uint16_t pos_table);

/* Entity Init Functions (03:4B57+) */
void EntityInitSouthFaceShrineDoor(GBState *gb);
void EntityInitLeever(GBState *gb);
void EntityInitZora(GBState *gb);
void EntityInitWithRightDirection(GBState *gb);
uint8_t GetColorDungeonRoomStatus(GBState *gb);
void EntityInitRotoswitchRed(GBState *gb);
void EntityInitRotoswitchYellow(GBState *gb);
void EntityInitRotoswitchBlue(GBState *gb);
void EntityInitHopper(GBState *gb);
void EntityInitFlyingHopperBombs(GBState *gb);
void EntityInitHardHitBeetle(GBState *gb);
void EntityInitAvalaunch(GBState *gb);
void EntityInitColorGuardianBlue(GBState *gb);
void EntityInitColorGuardianRed(GBState *gb);
void EntityInitColorDungeonBook(GBState *gb);
void EntityInitGiantBuzzBlob(GBState *gb);

/* Entity Handlers */
void EntityBurningHandler(GBState *gb, uint16_t bc);
void EntityFallHandler(GBState *gb, uint16_t bc);
void EntityThrownHandler(GBState *gb, uint16_t bc);
void EntityStunnedHandler(GBState *gb, uint16_t bc);
void EntityGetLiftedUp(GBState *gb, uint16_t bc);
void EntityLiftedHandler(GBState *gb, uint16_t bc);
void EntityBecomeStunned(GBState *gb, uint16_t bc);

/* Pushed Block Entity Handler (03:5249) */
void PushedBlockEntityHandler(GBState *gb, uint16_t bc);

/* Liftable Rock Entity Handlers (03:5326-03:5406) */
void Entity4BHandler(GBState *gb, uint16_t bc);
void LiftableRockEntityHandler(GBState *gb, uint16_t bc);
void LiftableRockIntactHandler(GBState *gb, uint16_t bc);
void LiftableRockStartSmashingAnimation(GBState *gb, uint16_t bc);

/* Entity Init Functions (03:4EA8+) */
void EntityInitWithRandomSpeed(GBState *gb);
void EntityInitSparkClockwise(GBState *gb);
void EntityInitSparkCounterClockwise(GBState *gb);
void EntityInitWizrobe(GBState *gb);
void EntityInitMoblinSword(GBState *gb);
void EntityInitSecretSeashell(GBState *gb);

/* Helper functions (03:4F12+) */
void func_003_4F12(GBState *gb, uint16_t bc);
void SetHiddenDroppableOptions1(GBState *gb, uint16_t bc);
void EntityInitDiggableBushOrPotDroppable(GBState *gb);
void EntityInitKeyDropPoint(GBState *gb);
void EntityInitTradingItem(GBState *gb);
void EntityInitWarp(GBState *gb);
void EntityInitTreeOrPotDroppable(GBState *gb);
void EntityInitWithShiftedXPosition(GBState *gb, uint16_t bc);
void SetDroppableDefaultTimer(GBState *gb, uint16_t bc);
void EntityInitWithCountdown(GBState *gb);
void EntityInitGhini(GBState *gb);

#endif /* LADX_BANK3_ENTITIES_H */