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

/* Arrow Entity Handlers (03:6A34-03:6B71) */
void ArrowEntityHandler(GBState *gb, uint16_t bc);
void BombArrowHandler(GBState *gb, uint16_t bc);
void MoblinArrowEntityHandler(GBState *gb, uint16_t bc);
void ArrowRenderAndMove(GBState *gb, uint16_t bc);
void ArrowRenderAndMove_skipRendering(GBState *gb, uint16_t bc);
void EntityBounceOffWallX(GBState *gb, uint16_t bc);
void EntityBounceOffWallY(GBState *gb, uint16_t bc);
void ArrowRockAfterHittingWall(GBState *gb, uint16_t bc);

/* Octorok Entity Handler (03:57E9) */
void OctorokEntityHandler(GBState *gb, uint16_t bc);

/* Collision and Damage Handlers (03:6C72-03:7267) */
bool CheckLinkCollisionWithEnemy(GBState *gb, uint16_t bc);
void ApplyLinkCollisionWithEnemy(GBState *gb, uint16_t bc);
void DefaultEnemyDamageCollisionHandler(GBState *gb, uint16_t bc);
void ApplySwordDamagesToEnemy(GBState *gb, uint16_t bc);
void func_003_6B7B(GBState *gb, uint16_t bc);

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

/* Droppable Item Handlers (03:6057-03:60B0) */
void DroppableMagicPowderEntityHandler(GBState *gb, uint16_t bc);
void DroppableArrowsEntityHandler(GBState *gb, uint16_t bc);
void DroppableRupeeEntityHandler(GBState *gb, uint16_t bc);
void PickableHandler(GBState *gb, uint16_t bc);

/* Droppable Helper Functions (03:608C-03:629D) */
void DroppableDisappearIfNeeded(GBState *gb, uint16_t bc);
void func_003_61C0(GBState *gb, uint16_t bc);
void DroppableRevealOrReturnIfNeeded(GBState *gb, uint16_t bc);
void func_003_7E0E(GBState *gb, uint16_t bc);
void PickableCanBeCollectedBySwordTable(GBState *gb);
void PickableHandleGrabbedByItemIfNeeded(GBState *gb, uint16_t bc);
void PickableCollectIfNeeded(GBState *gb, uint16_t bc);
void PickDroppableMagicPowder(GBState *gb, uint16_t bc);
void PickSecretSeashell(GBState *gb, uint16_t bc);
void IncreaseValueAtHLClampAt99(GBState *gb);
void PickDroppableArrows(GBState *gb, uint16_t bc);
void PickDroppableBombs(GBState *gb, uint16_t bc);
void PickSirensInstrument(GBState *gb, uint16_t bc);
void HoldPickupInTheAir(GBState *gb, uint16_t bc);
void PickHeartContainer(GBState *gb, uint16_t bc);
void PickToadstoolOrDungeonKey(GBState *gb, uint16_t bc);
void PickHeartPiece(GBState *gb, uint16_t bc);
void PickGuardianAcorn(GBState *gb, uint16_t bc);
void PickPieceOfPower(GBState *gb, uint16_t bc);
void ProcessPowerUp(GBState *gb, uint16_t bc);
void MovePickupInTheAir(GBState *gb, uint16_t bc);
void PickSword(GBState *gb, uint16_t bc);
void GiveInventoryItem(GBState *gb, uint16_t bc);
void PickDroppableKey(GBState *gb, uint16_t bc);
void PickDroppableHeart(GBState *gb, uint16_t bc);
void PickDroppableRupee(GBState *gb, uint16_t bc);
void PickDroppableFairy(GBState *gb, uint16_t bc);

/* Entity Spawning (03:4F68-03:4FD0) */
void SpawnNewEntity(GBState *gb, uint16_t bc);
void SpawnNewEntityInRange(GBState *gb, uint16_t bc);
void ConfigureNewEntity_helper(GBState *gb, uint16_t bc);

/* Moblin/Roaming Enemy Handlers (03:5827-05:59D6) */
void MoblinEntityHandler(GBState *gb, uint16_t bc);
void AnimateRoamingEnemy(GBState *gb, uint16_t bc);
void RoamingEnemyState0Handler(GBState *gb, uint16_t bc);
void SetEntityVariantForDirection_03(GBState *gb, uint16_t bc);
void SpawnMoblinArrow(GBState *gb, uint16_t bc);
void SpawnOctorokRock(GBState *gb, uint16_t bc);

/* Bomb Entity Handlers (03:65E2-03:68F0) */
void BombExplosionHandler(GBState *gb, uint16_t bc);
void BombExplosionVisuals(GBState *gb, uint16_t bc);
void RenderBombExplosion(GBState *gb, uint16_t bc);
void BombEntityHandler(GBState *gb, uint16_t bc);
void RenderBomb(GBState *gb, uint16_t bc);
void CheckForBombDestroyableObjectPuzzle(GBState *gb, uint16_t bc);
void CheckForBombDestroyableObjectBasic(GBState *gb, uint16_t bc);
void CheckExplosionInteractionWithEntities(GBState *gb, uint16_t bc);

/* Magic Rod Fireball Handler (03:69B2-03:6A1D) */
void MagicRodFireballEntityHandler(GBState *gb, uint16_t bc);

#endif /* LADX_BANK3_ENTITIES_H */