#ifndef LADX_CONSTANTS_GFX_H
#define LADX_CONSTANTS_GFX_H

#include "common.h"

/* Draw command copy modes (bits 6-7 of length/mode byte) */
#define DC_COPY_ROW                 0x00
#define DC_FILL_ROW                 0x40
#define DC_COPY_COLUMN              0x80
#define DC_FILL_COLUMN              0xC0

/* Special tile during room transitions treated as transparent */
#define TRANSITION_TRANSPARENT_TILE 0xEE

/* Graphic asset ROM addresses in Bank $0C */
#define InventoryEquipmentItemsTiles 0x4800
#define PieceOfHeartMeterTiles       0x6900

#endif /* LADX_CONSTANTS_GFX_H */
