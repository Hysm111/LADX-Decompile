#ifndef LADX_BANK2_SHOVEL_H
#define LADX_BANK2_SHOVEL_H

#include "gb.h"

/* Bank 2 Shovel Digging & Animation Tables */
extern const uint8_t LinkDirectionToLinkAnimationState2[8];
extern const int8_t LinkDirectionToAdjacentTileIndexX[4];
extern const int8_t LinkDirectionToAdjacentTileIndexY[4];

/**
 * Prepares DMG draw commands for a dug shovel hole tile at the intersected object address (02:4BD4).
 *
 * @param gb Pointer to Game Boy system state.
 * @param hl Destination pointer in wDrawCommand.
 */
void func_002_4BD4(GBState *gb, uint16_t hl);

/**
 * Prepares CGB draw commands for a dug shovel hole tile in both VRAM0 and VRAM1 (02:4C14).
 *
 * @param gb Pointer to Game Boy system state.
 * @param hl Destination pointer in wDrawCommand.
 */
void func_002_4C14(GBState *gb, uint16_t hl);

/**
 * Places a shovel hole in wRoomObjects, backups to RAM2, issues draw command, and rolls random drop (02:4C92).
 *
 * @param gb Pointer to Game Boy system state.
 * @param spawn_new_entity Optional callback to spawn drop entity.
 * @param apply_vector Optional callback to apply velocity vector towards Link.
 */
void label_002_4C92(GBState *gb,
                    uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                    void (*apply_vector)(GBState *));

/**
 * Validates whether the tile in front of Link can be dug with the shovel (02:4D20).
 *
 * @param gb Pointer to Game Boy system state.
 * @return true if tile can be dug, false if blocked or non-diggable.
 */
bool func_002_4D20(GBState *gb);

/**
 * Attempts to dig with the shovel: validates facing tile and places hole / rolls drop (02:4BC8).
 *
 * @param gb Pointer to Game Boy system state.
 * @param spawn_new_entity Optional callback to spawn drop entity.
 * @param apply_vector Optional callback to apply velocity vector towards Link.
 * @return true if digging succeeded, false otherwise.
 */
bool func_002_4BC8(GBState *gb,
                   uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                   void (*apply_vector)(GBState *));

/**
 * Link shovel usage state handler: advances digging animation, triggers hole placement, Marin scolding (02:4B49).
 *
 * @param gb Pointer to Game Boy system state.
 * @param func_020_4b4a Optional callback for Wind Fish Egg Nightmare shovel interaction.
 * @param shovel_dig_action Optional callback for shovel digging action.
 */
void func_002_4B49(GBState *gb,
                   void (*func_020_4b4a)(GBState *),
                   bool (*shovel_dig_action)(GBState *));

#endif /* LADX_BANK2_SHOVEL_H */
