#ifndef LADX_BANK2_ROOM_EFFECTS_H
#define LADX_BANK2_ROOM_EFFECTS_H

#include "gb.h"

/**
 * Guards a room effect (02:5DAF-02:5DC1).
 *
 * Returns true and clears wRoomEvent only when hRoomStatus bit EVENT_1 is
 * clear and wRoomEventEffectExecuted is nonzero. Otherwise returns false
 * without changing memory. Callers must return immediately on false to
 * model the assembly's discarded caller return address (pop af; ret).
 */
bool EventEffectGuard(GBState *gb);

/**
 * Explodes non-harmless entities with unsigned status >= ACTIVE, scanning
 * slots 15 through 0, after EventEffectGuard allows the effect (02:5D79-02:5DAE).
 */
void KillAllEnemiesEffectHandler(GBState *gb);

/**
 * Guarded fairy spawn at (0x88, 0x30), slow countdown 0x80, followed by a poof
 * and room-status update (02:5DC2-02:5DE8).
 *
 * The required callback implements SpawnNewEntity in bank 3 and returns raw
 * assembly DE: a slot 0..15 on success or 0x00FF when all slots are occupied.
 * Unlike other spawning callers, this handler does not test carry; it writes
 * at table base + DE even on failure. Do not return a 0xFFFF failure sentinel.
 * A NULL state or callback is a C API no-op, not an emulated allocation failure.
 */
void DropFairyEffectHandler(GBState *gb,
                            uint16_t (*spawn_new_entity)(GBState *, uint8_t));

/**
 * Guarded staircase-appearance VFX at (0x88, 0x20), followed by the shared
 * room-status update (02:5DE9-02:5DF5, falls through to MakeEffectObjectAppear).
 * The VFX, not this handler, later creates the staircase object.
 */
void RevealStaircaseEffectHandler(GBState *gb);

/**
 * Allocates the supplied VFX at hMultiPurpose0/1, then ORs EVENT_1 into the
 * saved room status and copies that byte to hRoomStatus (02:5DF6-02:5E02).
 * This entry point has no event guard; NULL state is a C API no-op.
 */
void MakeEffectObjectAppear(GBState *gb, uint8_t vfx_type);

#endif /* LADX_BANK2_ROOM_EFFECTS_H */
