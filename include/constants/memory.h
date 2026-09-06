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
#define wMusicTrackTiming       0xC10B
#define wLinkMotionState        0xC11C
#define wIsUsingSpinAttack      0xC121
#define wSwordCharge            0xC122
#define wActiveEntityIndex      0xC123
#define wRoomTransitionState    0xC124
#define wRoomTransitionDirection 0xC125
#define wC140                   0xC140
#define wC142                   0xC142
#define wIsRunningWithPegasusBoots 0xC14A
#define wPegasusBootsChargeMeter 0xC14B
#define wTransitionSequenceCounter 0xC16B
#define wC167                   0xC167
#define wC16C                   0xC16C
#define wRoomEventEffectExecuted 0xC18F
#define wC19D                   0xC19D
#define wC1CF                   0xC1CF
#define w2_D16A                 0xD16A
#define wWarp0MapCategory       0xD401
#define wD478                   0xD478
#define wDrawCommand            0xD601
#define wGameplayType           0xDB95
#define wIsIndoor               0xDBA5
#define wCurrentBank            0xDBAF
#define wDrawCommandsVRAM1Size  0xDC90
#define wDrawCommandVRAM1       0xDC91
#define wAnimatedScrollingTilesStorage 0xDCC0
#define wFarcallBank            0xDE01
#define wFarcallAdressHigh      0xDE02
#define wFarcallAdressLow       0xDE03
#define wFarcallReturnBank      0xDE04

/* WRAM Entity Tables */
#define wEntitiesTransitionCountdownTable     0xC2E0
#define wEntitiesPrivateCountdown1Table       0xC2F0
#define wEntitiesIgnoreHitsCountdownTable     0xC410
#define wEntitiesSlowTransitionCountdownTable 0xC450
#define wSwordMoblinAlertingSoundCounter      0xC502

/* WRAM Transient VFX Tables */
#define wTranscientVfxTypeTable      0xC510
#define wTranscientVfxCountdownTable 0xC520
#define wTranscientVfxPosXTable      0xC530
#define wTranscientVfxPosYTable      0xC540
#define wC5A6                        0xC5A6
#define wC5C0                        0xC5C0

/* HRAM Sections & Variables */
#define hGameValuesSection           0xFF90
#define hNeedsUpdatingBGTiles        0xFF90
#define hNeedsUpdatingEntityTilesA   0xFF91
#define hBGTilesLoadingStage         0xFF92
#define hLinkPositionX               0xFF98
#define hLinkPositionY               0xFF99
#define hLinkFinalPositionX          0xFF9F
#define hLinkFinalPositionY          0xFFA0
#define hMusicFadeOutTimer           0xFFA8
#define hContinueMusicAfterWarp      0xFFBC
#define hNextDefaultMusicTrack       0xFFBF
#define hInterrupts                  0xFFD2
#define hMultiPurpose0               0xFFD7
#define hMultiPurpose1               0xFFD8
#define hMultiPurpose2               0xFFD9
#define hMultiPurpose3               0xFFDA
#define hMultiPurposeF               0xFFE6
#define hFrameCounter                0xFFE7
#define hJingle                      0xFFF2
#define hWaveSfx                     0xFFF3
#define hNoiseSfx                    0xFFF4
#define hIsComputingFrame            0xFFFD
#define hIsGBC                       0xFFFE

#endif /* LADX_CONSTANTS_MEMORY_H */
