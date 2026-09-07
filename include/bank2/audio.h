#ifndef LADX_BANK2_AUDIO_H
#define LADX_BANK2_AUDIO_H

#include "gb.h"

extern const uint8_t OverworldMusicTracks[256];
extern const uint8_t HouseMusicTracks[32];
extern const uint8_t MusicOverridesPowerUpTrack[38];

/**
 * Selects and sets the appropriate background music track after a room or area transition.
 * Handles swordless overworld state, dungeon/house rooms, boss defeat, 2D side-scrolling,
 * and active power-up track override logic. (02:4146)
 *
 * @param gb Pointer to Game Boy system state.
 */
void SelectMusicTrackAfterTransition(GBState *gb);

#endif /* LADX_BANK2_AUDIO_H */
