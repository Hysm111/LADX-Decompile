#ifndef LADX_CONSTANTS_MEMORY_H
#define LADX_CONSTANTS_MEMORY_H

#include "common.h"

/* WRAM Sections */
#define wram0Section            0xC000
#define wIsFileSelectionArrowShifted 0xD000
#define wAudioSection           0xD300
#define wDrawCommandsSection    0xD600
#define wStackTop               0xDFFF

/* WRAM Gameplay Variables */
#define wActiveEntityIndex      0xC123
#define wRoomTransitionState    0xC124
#define wRoomTransitionDirection 0xC125
#define wGameplayType           0xDB95
#define wCurrentBank            0xDBAF

/* HRAM Sections & Variables */
#define hGameValuesSection      0xFF90
#define hNextDefaultMusicTrack  0xFFBF
#define hMultiPurpose0          0xFFD7
#define hMultiPurpose1          0xFFD8
#define hMultiPurpose2          0xFFD9
#define hMultiPurpose3          0xFFDA
#define hMultiPurposeF          0xFFE6
#define hIsComputingFrame       0xFFFD
#define hIsGBC                  0xFFFE

#endif /* LADX_CONSTANTS_MEMORY_H */
