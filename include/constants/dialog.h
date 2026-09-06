#ifndef LADX_CONSTANTS_DIALOG_H
#define LADX_CONSTANTS_DIALOG_H

#include "common.h"

/* Dialog background tile constants */
#define Dialog026                  0x26
#define Dialog0DA                  0xDA
#define DIALOG_BG_TILE_DARK        0x7E
#define DIALOG_BG_TILE_LIGHT       0x7F

/* Dialog Codepoint Conversion Tables in ROM Bank $1C */
#define BANK_CodepointToTileMap    0x1C
#define ADDR_CodepointToTileMap    0x4641
#define ADDR_CodepointToDiacritic  0x4741

#endif /* LADX_CONSTANTS_DIALOG_H */
