#ifndef LADX_HOME_COPY_DATA_H
#define LADX_HOME_COPY_DATA_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Copy data between memory locations.
 * Corresponds to CopyData (00:2914) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Destination address
 * @param hl Source address
 * @param bc Number of bytes to copy
 */
void CopyData(GBState *gb, uint16_t de, uint16_t hl, uint16_t bc);

/**
 * Copy data from a specified ROM bank, then restore ROM bank 1.
 * Corresponds to CopyDataFromBank (00:2908) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Target ROM bank to select
 * @param de Destination address
 * @param hl Source address
 * @param bc Number of bytes to copy
 */
void CopyDataFromBank(GBState *gb, uint8_t bank, uint16_t de, uint16_t hl, uint16_t bc);

/**
 * Copy data from a specified ROM bank, then restore ROM bank $28.
 * Corresponds to CopyData_trampoline (00:0B5D) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bank Target ROM bank to select
 * @param de Destination address
 * @param hl Source address
 * @param bc Number of bytes to copy
 */
void CopyData_trampoline(GBState *gb, uint8_t bank, uint16_t de, uint16_t hl, uint16_t bc);

/**
 * Dispatch a draw command to VRAM based on flags in register A.
 * Corresponds to DrawCommandToVRAM (00:2941) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Pointer to source address variable (advanced as data is read)
 * @param hl Destination address in VRAM
 * @param a Data length (bits 0-5) and copy mode (bits 6-7)
 */
void DrawCommandToVRAM(GBState *gb, uint16_t *de, uint16_t hl, uint8_t a);

/**
 * Copy data to VRAM map during room transitions, skipping transparent tile $EE.
 * Corresponds to DrawCommandToVRAMDuringRoomTransition (00:2991) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Pointer to source address variable (advanced as data is read)
 * @param hl Destination address in VRAM
 * @param a Data length (bits 0-5) and copy mode (bit 7: 0 = row, 1 = column)
 */
void DrawCommandToVRAMDuringRoomTransition(GBState *gb, uint16_t *de, uint16_t hl, uint8_t a);

/**
 * Process a series of draw command structures located at DE until a terminator (0 high byte) is reached.
 * Corresponds to ExecuteDrawCommands (00:2927) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Address of the first draw command struct
 */
void ExecuteDrawCommands(GBState *gb, uint16_t de);

/**
 * Inner loop entry for executing draw commands without room transition.
 * Corresponds to NoRoomTransitionDrawLoop in disassembly.
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
 * Corresponds to CopyToBGMap0 (00:0B96) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Pointer to source address variable
 */
void CopyToBGMap0(GBState *gb, uint16_t *hl);

/**
 * Copy $100 (256) bytes to VRAM from ${b}00 to $8000+${c}00 using direct CPU copy,
 * then switch back to return_bank.
 * Corresponds to CopyDataToVRAM_noDMA (00:0A01) in disassembly.
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
 * Corresponds to CopyDataToVRAM (00:0A13) in disassembly.
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
 * Corresponds to func_BB5 (00:0BB5) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Source address
 */
void func_BB5(GBState *gb, uint16_t hl);

/**
 * Copy screen tile data (and on GBC, attributes at hl + $168) from specified ROM bank to vBGMap0.
 * If wGameplayType == GAMEPLAY_PHOTO_ALBUM, also copies tile data to wIsFileSelectionArrowShifted.
 * Restores ROM bank from hMultiPurposeF.
 * Corresponds to CopyBGMapFromBank (00:0B69) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param src_bank Source ROM bank
 * @param hl Source address of screen tile data
 */
void CopyBGMapFromBank(GBState *gb, uint8_t src_bank, uint16_t hl);

/**
 * Copy 4 tiles (64 bytes / $40) of Siren Instruments Tiles from ROM bank $0C to destination DE.
 * Restores ROM bank 1 upon return.
 * Corresponds to CopySirenInstrumentTiles (00:0C3A) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Destination address
 * @param hl Source address in ROM bank $0C
 */
void CopySirenInstrumentTiles(GBState *gb, uint16_t de, uint16_t hl);

/**
 * Copy B bytes from HL to DE using the ROM bank stored at w2_D16A ($D16A),
 * then restore ROM bank $28.
 * Corresponds to func_BC5 (00:0BC5) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Destination address
 * @param hl Source address
 * @param count Number of bytes to copy (register B)
 */
void func_BC5(GBState *gb, uint16_t de, uint16_t hl, uint8_t count);

/**
 * Copy 32 ($20) bytes of Color Dungeon tiles from ColorDungeonNpcTiles + $F00 ($4F00 in bank $35)
 * to wAnimatedScrollingTilesStorage ($DCC0), then restores stacked ROM bank.
 * Corresponds to CopyColorDungeonSymbols (00:0A32) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param stacked_bank Bank to restore upon completion
 */
void CopyColorDungeonSymbols(GBState *gb, uint8_t stacked_bank);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_COPY_DATA_H */
