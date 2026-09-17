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

/**
 * Dispatches the register-A event argument (not wRoomEvent) after storing
 * event & 0x1F in hMultiPurpose0 (02:5F9F-02:5FC4).
 * Verified domain: masked IDs 1..16, as present in all nonzero room events.
 * Returns true after dispatch, including declared Events.return entries.
 * NULL or unsupported IDs return false without writes: this is a C API
 * rejection, NOT emulation of the original unchecked out-of-table jump.
 */
bool CheckTriggersResolution(GBState *gb, uint8_t event);

/**
 * Checks triggers then reloads wRoomEvent to dispatch its effect (02:5D4F).
 * Returns true for event zero (no work), or a completed supported dispatch.
 * Nonzero events require a valid masked ID and BOTH allocator callbacks.
 * Invalid inputs return false before writes; callers must handle rejection.
 * spawn_key follows label_002_5425: slot 0..15 or 0xFFFF for carry/failure,
 * preserving the map ID. spawn_fairy returns raw DE (0..15 or 0x00FF on full).
 * Allocation internals remain external; callbacks run through the existing
 * bank-3 trampoline. This bool API needs an adapter for void callback slots.
 */
bool ExecuteRoomTriggersAndEffects(GBState *gb,
                                   uint16_t (*spawn_key)(GBState *, uint8_t),
                                   uint16_t (*spawn_fairy)(GBState *, uint8_t));

#endif /* LADX_BANK2_ROOM_TRIGGERS_H */
