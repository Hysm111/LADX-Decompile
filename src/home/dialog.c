#include "home/dialog.h"
#include "constants/hardware.h"
#include "constants/dialog.h"

uint8_t ReadValueInDialogsBank(GBState *gb, uint16_t hl, uint16_t bc) {
    if (!gb) return 0;

    gb_write(gb, rSelectROMBank, BANK_CodepointToTileMap);
    uint8_t val = gb_read(gb, hl + bc);
    gb_write(gb, rSelectROMBank, 0x01);
    return val;
}

uint8_t ReadTileValueFromAsciiTable(GBState *gb, uint16_t bc) {
    return ReadValueInDialogsBank(gb, ADDR_CodepointToTileMap, bc);
}

uint8_t ReadTileValueFromDiacriticsTable(GBState *gb, uint16_t bc) {
    return ReadValueInDialogsBank(gb, ADDR_CodepointToDiacritic, bc);
}
