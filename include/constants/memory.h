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
#define wEntityTilesSpriteslotIndexB 0xC10D
#define wNeedsUpdatingEntityTilesB 0xC10E
#define wLinkMotionState        0xC11C
#define wIsUsingSpinAttack      0xC121
#define wSwordCharge            0xC122
#define wActiveEntityIndex      0xC123
#define wRoomTransitionState    0xC124
#define wRoomTransitionDirection 0xC125
#define wRandomSeed             0xC13D
#define wC140                   0xC140
#define wC142                   0xC142
#define wIsRunningWithPegasusBoots 0xC14A
#define wPegasusBootsChargeMeter 0xC14B
#define wTransitionSequenceCounter 0xC16B
#define wC167                   0xC167
#define wC16C                   0xC16C
#define wRoomEventEffectExecuted 0xC18F
#define wLoadedEntitySpritesheets 0xC193
#define wEntityTilesSpriteslotIndexA 0xC197
#define wC19C                   0xC19C
#define wC19D                   0xC19D
#define wDialogState            0xC19F
#define wC1CF                   0xC1CF
#define wMusicTrackToPlay       0xD368
#define w2_D16A                 0xD16A
#define wWarp0MapCategory       0xD401
#define wWarpPositions          0xD416
#define wD464                   0xD464
#define wD474                   0xD474
#define wD478                   0xD478
#define wDrawCommand            0xD601
#define wOverworldRoomStatus    0xD800
#define wGameplayType           0xDB95
#define wGameplaySubtype        0xDB96
#define wIsIndoor               0xDBA5
#define wCurrentBank            0xDBAF
#define wDrawCommandsVRAM1Size  0xDC90
#define wDrawCommandVRAM1       0xDC91
#define wAnimatedScrollingTilesStorage 0xDCC0
#define wDDD8                   0xDDD8
#define wPaletteUnknownE        0xDDD5
#define wFarcallBank            0xDE01
#define wFarcallAdressHigh      0xDE02
#define wFarcallAdressLow       0xDE03
#define wFarcallReturnBank      0xDE04

/* Followers in WRAM */
#define wIsBowWowFollowingLink  0xDB56
#define wIsMarinFollowingLink   0xDB73
#define wIsGhostFollowingLink   0xDB79
#define wIsRoosterFollowingLink 0xDB7B
#define BOW_WOW_FOLLOWING       0x01

/* Room Objects in WRAM */
#define wRoomObjectsArea             0xD700
#define wRoomObjects                 0xD711
#define OBJECTS_PER_ROW              10

/* WRAM Entity Tables */
#define wEntitiesPhysicsFlagsTable            0xC120
#define wEntitiesPosXTable                    0xC200
#define wEntitiesPosYTable                    0xC210
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
#define hWorldTileset                0xFF94
#define W_TILESET_KEEP               0x0F
#define W_TILESET_NO_UPDATE          0xFF
#define hBaseScrollX                 0xFF96
#define hBaseScrollY                 0xFF97
#define hLinkPositionX               0xFF98
#define hLinkPositionY               0xFF99
#define hLinkPhysicsModifier         0xFF9C
#define hLinkFinalPositionX          0xFF9F
#define hLinkFinalPositionY          0xFFA0
#define hMusicFadeOutTimer           0xFFA8
#define hMusicFadeInTimer            0xFFAB
#define hContinueMusicAfterWarp      0xFFBC
#define hNextDefaultMusicTrack       0xFFBF
#define hPressedButtonsMask          0xFFCB
#define hJoypadState                 0xFFCC
#define hIntersectedObjectTop        0xFFCD
#define hIntersectedObjectLeft       0xFFCE
#define hIntersectedObjectBGAddressHigh 0xFFCF
#define hIntersectedObjectBGAddressLow  0xFFD0
#define hInterrupts                  0xFFD2
#define hMultiPurpose0               0xFFD7
#define hMultiPurpose1               0xFFD8
#define hMultiPurpose2               0xFFD9
#define hMultiPurpose3               0xFFDA
#define hMultiPurpose8               0xFFDF
#define hMultiPurpose9               0xFFE0
#define hMultiPurposeA               0xFFE1
#define hMultiPurposeF               0xFFE6
#define hFrameCounter                0xFFE7
#define hMultiPurposeG               0xFFE8
#define hRoomBank                    0xFFE8
#define hMultiPurposeH               0xFFE9
#define hJingle                      0xFFF2
#define hWaveSfx                     0xFFF3
#define hNoiseSfx                    0xFFF4
#define hMapRoom                     0xFFF6
#define hMapId                       0xFFF7
#define hIsSideScrolling             0xFFF9
#define hIsComputingFrame            0xFFFD
#define hIsGBC                       0xFFFE

/* Map ID Constants */
#define MAP_EAGLES_TOWER             0x06
#define MAP_CAVE_B                   0x0A
#define MAP_HOUSE                    0x10
#define MAP_KANALET                  0x14
#define MAP_COLOR_DUNGEON            0xFF

/* Room Constants */
#define ROOM_INDOOR_B_FISHING_MINIGAME 0xB1
#define ROOM_INDOOR_B_CAMERA_SHOP      0xB5
#define ROOM_INDOOR_B_SEASHELL_MANSION 0xE9
#define ROOM_INDOOR_B_MANBO            0xFD
#define UNKNOWN_ROOM_12                0x12

#endif /* LADX_CONSTANTS_MEMORY_H */
