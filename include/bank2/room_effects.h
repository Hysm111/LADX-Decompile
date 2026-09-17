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

#endif /* LADX_BANK2_ROOM_EFFECTS_H */
