#ifndef LADX_HOME_COPY_DATA_H
#define LADX_HOME_COPY_DATA_H

#include "gb.h"
#include "constants/gfx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Copy BC bytes from HL (source) to DE (destination).
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Destination address
 * @param hl Source address
 * @param bc Number of bytes to copy
 */
void CopyData(GBState *gb, uint16_t de, uint16_t hl, uint16_t bc);

/**
 * Copy data from specified ROM bank.
 * Switches ROM bank via rSelectROMBank, performs CopyData, and restores bank 1.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Source ROM bank
 * @param de Destination address
 * @param hl Source address
 * @param bc Number of bytes to copy
 */
void CopyDataFromBank(GBState *gb, uint8_t bank, uint16_t de, uint16_t hl, uint16_t bc);

/**
 * Copy BC bytes from HL to DE using ROM bank 'bank', then switch back to bank $28.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Source ROM bank
 * @param de Destination address
 * @param hl Source address
 * @param bc Number of bytes to copy
 */
void CopyData_trampoline(GBState *gb, uint8_t bank, uint16_t de, uint16_t hl, uint16_t bc);

/**
 * Copy data to VRAM map with support for row or column copying/filling.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Pointer to source address variable (advanced as data is read)
 * @param hl Destination address in VRAM
 * @param a Data length (bits 0-5) and copy mode (bits 6-7)
 */
void DrawCommandToVRAM(GBState *gb, uint16_t *de, uint16_t hl, uint8_t a);

/**
 * Copy data to VRAM map during room transitions, skipping transparent tile $EE.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Pointer to source address variable (advanced as data is read)
 * @param hl Destination address in VRAM
 * @param a Data length (bits 0-5) and copy mode (bit 7: 0 = row, 1 = column)
 */
void DrawCommandToVRAMDuringRoomTransition(GBState *gb, uint16_t *de, uint16_t hl, uint8_t a);

/**
 * Process a series of draw command structures located at DE until a terminator (0 high byte) is reached.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Address of the first draw command struct
 */
void ExecuteDrawCommands(GBState *gb, uint16_t de);

/**
 * Inner loop entry for executing draw commands without room transition.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Address of the next draw command structure (pointing to destinationLow)
 * @param a_dest_high Destination address high byte
 */
void NoRoomTransitionDrawLoop(GBState *gb, uint16_t de, uint8_t a_dest_high);

/**
 * Copy 20x18 tiles (360 bytes) from source address (*hl) to vBGMap0 ($9800),
 * stepping 20 bytes per row and skipping the remaining 12 bytes of each 32-byte VRAM row.
 * Source pointer *hl is advanced by 360 bytes.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Pointer to source address variable
 */
void CopyToBGMap0(GBState *gb, uint16_t *hl);

/**
 * Copy $100 (256) bytes to VRAM from ${b}00 to $8000+${c}00 using direct CPU copy,
 * then switch back to return_bank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param b Source address high byte
 * @param c Destination address offset high byte (dest is $8000 + (c << 8))
 * @param return_bank Bank to restore
 */
void CopyDataToVRAM_noDMA(GBState *gb, uint8_t b, uint8_t c, uint8_t return_bank);

/**
 * Copy $100 (256) bytes to VRAM from ${b}00 to $8000+${c}00, then switch back to return_bank.
 * Uses GDMA on Game Boy Color, or direct CPU copy on DMG.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param src_bank Source ROM bank to select
 * @param b Source address high byte
 * @param c Destination address offset high byte (dest is $8000 + (c << 8))
 * @param return_bank Bank to restore
 */
void CopyDataToVRAM(GBState *gb, uint8_t src_bank, uint8_t b, uint8_t c, uint8_t return_bank);

/**
 * Copy $168 (360) bytes from HL to wIsFileSelectionArrowShifted ($D000).
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Source address
 */
void func_BB5(GBState *gb, uint16_t hl);

/**
 * Copy screen tile data (and on GBC, attributes at hl + $168) from specified ROM bank to vBGMap0.
 * If wGameplayType == GAMEPLAY_PHOTO_ALBUM, also copies tile data to wIsFileSelectionArrowShifted.
 * Restores ROM bank from hMultiPurposeF.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param src_bank Source ROM bank
 * @param hl Source address of screen tile data
 */
void CopyBGMapFromBank(GBState *gb, uint8_t src_bank, uint16_t hl);

/**
 * Copy 4 tiles (64 bytes / $40) of Siren Instruments Tiles from ROM bank $0C to destination DE.
 * Restores ROM bank 1 upon return.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Destination address
 * @param hl Source address in ROM bank $0C
 */
void CopySirenInstrumentTiles(GBState *gb, uint16_t de, uint16_t hl);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_COPY_DATA_H */
