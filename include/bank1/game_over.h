#ifndef LADX_BANK1_GAME_OVER_H
#define LADX_BANK1_GAME_OVER_H

#include "common.h"
#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Data tables */
extern const uint8_t Data_001_41CF[24];
extern const uint8_t Data_001_41E7[28];
extern const uint8_t Data_001_42BA[16];
extern const uint8_t Data_001_4336[3];

/**
 * LinkPassOut (01:41C2)
 * Dispatches the Game Over state machine according to hGameOverStage.
 */
void LinkPassOut(GBState *gb);

/**
 * LinkPassOutHandler (01:4203)
 * Handles Link's passing out animation and transition to Game Over screen.
 * Increments death counter in BCD.
 */
void LinkPassOutHandler(GBState *gb);

/**
 * LoadGameOverStage1Handler (01:4291)
 * Loads save menu tileset and advances stage.
 */
void LoadGameOverStage1Handler(GBState *gb);

/**
 * LoadGameOverStage2Handler (01:429B)
 * Configures Game Over BG map, palettes, and scroll.
 */
void LoadGameOverStage2Handler(GBState *gb);

/**
 * LoadGameOverStage3Handler (01:42CA)
 * Waits for countdown and starts Game Over music.
 */
void LoadGameOverStage3Handler(GBState *gb);

/**
 * GameOverInteractiveHandler (01:42D9)
 * Handles menu options selection (Continue, Save & Continue, Save & Quit).
 */
void GameOverInteractiveHandler(GBState *gb);

/**
 * func_001_4339 (01:4339)
 * Navigation for 3 Game Over options and cursor positioning.
 */
void func_001_4339(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_BANK1_GAME_OVER_H */
