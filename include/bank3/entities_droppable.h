#ifndef LADX_BANK3_ENTITIES_DROPPABLE_H
#define LADX_BANK3_ENTITIES_DROPPABLE_H

#include "gb.h"

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

/* Pickable Item Collection Functions (Stubs) */
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

/* Entity Spawning Functions (Stubs) */
void SpawnNewEntity(GBState *gb, uint16_t bc);
void SpawnNewEntityInRange(GBState *gb, uint16_t bc);
void ConfigureNewEntity_helper(GBState *gb, uint16_t bc);

#endif /* LADX_BANK3_ENTITIES_DROPPABLE_H */