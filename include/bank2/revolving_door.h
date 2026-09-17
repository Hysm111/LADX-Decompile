#ifndef LADX_BANK2_REVOLVING_DOOR_H
#define LADX_BANK2_REVOLVING_DOOR_H

#include "gb.h"

/* Bank 2 Revolving Door & Tile/Palette Data Tables */
extern const uint8_t Data_002_4E1C[16];
extern const uint8_t LinkRevolvingDoorAnimation[7];

/**
 * Replaces a room object tile with 0xAE, writes GBC attributes, and emits 10-byte draw command (02:4D97).
 *
 * @param gb Pointer to Game Boy system state.
 * @param get_bg_attr_addr Optional callback for GBC attribute lookup.
 */
void label_002_4D97(GBState *gb, void (*get_bg_attr_addr)(GBState *));

/**
 * Copies 8 bytes of object palette 1 from WRAM bank 1 to WRAM bank 2 (02:4DFC).
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_4DFC(GBState *gb);

/**
 * Copies 8 bytes from Data_002_4E1C + de into wObjPal8 and flags palette update (02:4E2C).
 *
 * @param gb Pointer to Game Boy system state.
 * @param de Byte offset into Data_002_4E1C (0 or 8).
 */
void func_002_4E2C(GBState *gb, uint16_t de);

/**
 * Restores 8 bytes of wObjPal8 from WRAM bank 2 to WRAM bank 1 and flags palette update (02:4E48).
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_4E48(GBState *gb);

/**
 * Resets revolving door animation and state, returning Link to default motion (02:4EDD).
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_4EDD(GBState *gb);

/**
 * Handles Link moving through the revolving door in Eagle's Tower (02:4E6D).
 *
 * @param gb Pointer to Game Boy system state.
 */
void LinkMotionRevolvingDoorHandler(GBState *gb);

#endif /* LADX_BANK2_REVOLVING_DOOR_H */
