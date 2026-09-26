#ifndef LADX_BANK3_ENTITIES_PUSHED_BLOCK_H
#define LADX_BANK3_ENTITIES_PUSHED_BLOCK_H

#include "gb.h"

/* Pushed Block Entity Handler (03:5249) */
void PushedBlockEntityHandler(GBState *gb, uint16_t bc);

/* func_003_52D4 (03:52D4) - Helper for pushed blocks */
void func_003_52D4(GBState *gb, uint16_t bc);

#endif /* LADX_BANK3_ENTITIES_PUSHED_BLOCK_H */