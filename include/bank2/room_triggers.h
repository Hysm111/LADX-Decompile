#ifndef LADX_BANK2_ROOM_TRIGGERS_H
#define LADX_BANK2_ROOM_TRIGGERS_H

#include "gb.h"

/* All entry points treat NULL as a C API no-op. */

/* Tests saved boss-room EVENT_2: DAE8 for map 06, D9FF otherwise (02:5FC6). */
void CheckKillSidescrollBossTrigger(GBState *gb);

/* Resolves only when wC1A2 is exactly 2 (02:5FDA). */
void CheckLightTorchesTrigger(GBState *gb);

/* Resolves when wSwitchButtonPressed is nonzero (02:5FE3). */
void CheckStepOnButtonTrigger(GBState *gb);

/* Requires the first three wKillOrder bytes to equal 0, 1, 2 (02:5FEB). */
void CheckKillInOrderTrigger(GBState *gb);

/* Scans slots 15..0; any nonzero status without the exclusion option blocks.
 * hMultiPurpose0 is the input trigger ID. ID 8 additionally requires wD460
 * nonzero and wEnemyWasKilled zero (02:5FFC-02:602C). */
void CheckKillEnemiesTrigger(GBState *gb);

/* Counts matching active (nonzero-status) puzzle entities into hMultiPurpose0
 * and performs the room-specific resolution/effect continuation. An already
 * executed effect returns without touching the count (02:602D-02:60D7). */
void CheckAnswerTunicsTrigger(GBState *gb);

#endif /* LADX_BANK2_ROOM_TRIGGERS_H */
