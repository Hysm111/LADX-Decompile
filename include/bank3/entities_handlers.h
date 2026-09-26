#ifndef LADX_BANK3_ENTITIES_HANDLERS_H
#define LADX_BANK3_ENTITIES_HANDLERS_H

#include "gb.h"

/* Entity Handlers (03:4C4C-03:4DEF) */
void EntityBurningHandler(GBState *gb, uint16_t bc);
void EntityFallHandler(GBState *gb, uint16_t bc);
void EntityThrownHandler(GBState *gb, uint16_t bc);
void EntityStunnedHandler(GBState *gb, uint16_t bc);
void EntityGetLiftedUp(GBState *gb, uint16_t bc);
void EntityLiftedHandler(GBState *gb, uint16_t bc);
void EntityBecomeStunned(GBState *gb, uint16_t bc);

#endif /* LADX_BANK3_ENTITIES_HANDLERS_H */