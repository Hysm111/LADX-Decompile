#ifndef LADX_BANK3_ENTITIES_INIT_EXTENDED_H
#define LADX_BANK3_ENTITIES_INIT_EXTENDED_H

#include "gb.h"

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

#endif /* LADX_BANK3_ENTITIES_INIT_EXTENDED_H */