#ifndef LADX_BANK1_MARIN_BEACH_H
#define LADX_BANK1_MARIN_BEACH_H

#include <stdint.h>
#include "gb.h"

extern const uint8_t Data_001_63AA[16];
extern const uint8_t Data_001_63BA[16];
extern const uint8_t Data_001_650A[4];
extern const uint8_t Data_001_650E[4];
extern const uint8_t Data_001_6512[4];
extern const uint8_t Data_001_6516[4];
extern const uint8_t Data_001_651A[4];
extern const uint8_t Data_001_6607[108];
extern const uint8_t Data_001_66F0[8];
extern const uint8_t Data_001_66F6[7];

/**
 * Marin Beach cinematic master entry point (01:6203).
 */
void MarinBeachEntryPoint(GBState *gb);

void MarinBeachPrepare0(GBState *gb);
void MarinBeachPrepare1(GBState *gb);
void MarinBeachPrepare2(GBState *gb);
void MarinBeachPrepare3(GBState *gb);
void MarinBeachScroll1(GBState *gb);
void MarinBeachScroll2(GBState *gb);
void MarinBeachScrollStop(GBState *gb);
void MarinBeachDialog1(GBState *gb);
void MarinBeachPause1(GBState *gb);
void MarinBeachDialog2(GBState *gb);
void MarinBeachPause2(GBState *gb);
void MarinBeachAreYouListening(GBState *gb);
void MarinBeachDialog3(GBState *gb);
void MarinBeachDialog4(GBState *gb);

void OpenMarinBeachDialog(GBState *gb, uint8_t dialog_low);
void func_001_64FF(GBState *gb);
void func_001_651E(GBState *gb);
void func_001_658B(GBState *gb, uint16_t hl, const uint8_t *de);
void func_001_65AE(GBState *gb);
void func_001_65DD(GBState *gb, uint8_t type, uint8_t bc_idx);
void func_001_6673(GBState *gb, uint8_t bc_idx);
void func_001_66FD(GBState *gb, uint8_t bc_idx);
void func_001_67A8(GBState *gb, uint8_t bc_idx);
void func_001_67B5(GBState *gb, uint8_t bc_idx);

#endif /* LADX_BANK1_MARIN_BEACH_H */
