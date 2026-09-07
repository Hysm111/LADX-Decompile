#ifndef LADX_BANK1_FACE_SHRINE_MURAL_H
#define LADX_BANK1_FACE_SHRINE_MURAL_H

#include <stdint.h>
#include "gb.h"

/**
 * Face Shrine mural cinematic sequence handlers (01:6AF8-01:6BA7).
 */
void FaceShrineMuralEntryPoint(GBState *gb);
void FaceShrineMuralStage0Handler(GBState *gb);
void FaceShrineMuralStage1Handler(GBState *gb);
void FaceShrineMuralStage2Handler(GBState *gb);
void FaceShrineMuralStage3Handler(GBState *gb);
void FaceShrineMuralStage4Handler(GBState *gb);
void FaceShrineMuralStage5Handler(GBState *gb);

#endif /* LADX_BANK1_FACE_SHRINE_MURAL_H */
