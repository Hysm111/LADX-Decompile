#ifndef LADX_HOME_ROOM_H
#define LADX_HOME_ROOM_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Mark room trigger event as resolved:
 * If wRoomEventEffectExecuted != 0, returns immediately.
 * Otherwise, clears wC1CF, sets wRoomEventEffectExecuted and wC5A6 to 1,
 * and if wC19D == 0, triggers JINGLE_PUZZLE_SOLVED.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void MarkTriggerAsResolved(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_ROOM_H */
