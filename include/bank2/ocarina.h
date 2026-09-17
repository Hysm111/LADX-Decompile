#ifndef LADX_BANK2_OCARINA_H
#define LADX_BANK2_OCARINA_H

#include "gb.h"

/**
 * Plays the currently selected Ocarina song or off-key notes if no songs learned. (02:41FC)
 *
 * @param gb Pointer to Game Boy system state.
 */
void UseOcarina(GBState *gb);

extern const int8_t Data_002_4A12[2];
extern const int8_t Data_002_4A14[2];

/**
 * Link playing ocarina state handler: progresses countdown, note VFX entities, songs, warp (02:4A16).
 *
 * @param gb Pointer to Game Boy system state.
 * @param select_music_track Optional callback to reload soundtrack (00:27DD / 02:4146).
 * @param spawn_new_entity Optional callback to spawn entity (00:3B86).
 */
void LinkPlayingOcarinaHandler(GBState *gb,
                               void (*select_music_track)(GBState *),
                               uint16_t (*spawn_new_entity)(GBState *, uint8_t));

#endif /* LADX_BANK2_OCARINA_H */
