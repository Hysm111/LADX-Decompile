#include "gb.h"
#include "constants/hardware.h"
#include <string.h>

void gb_init(GBState *gb) {
    if (!gb) return;
    memset(gb, 0, sizeof(GBState));
    gb->rom_bank = 1;
    gb->wram_bank = 1;
    gb->vram_bank = 0;
}

void gb_attach_rom(GBState *gb, const uint8_t *rom_data, uint32_t rom_size) {
    if (!gb) return;
    gb->rom = rom_data;
    gb->rom_size = rom_size;
}

uint8_t *gb_get_ptr(GBState *gb, uint16_t addr) {
    if (addr < 0x8000) {
        return NULL; /* ROM is read-only */
    } else if (addr < 0xA000) {
        return &gb->vram[gb->vram_bank][addr - 0x8000];
    } else if (addr < 0xC000) {
        return &gb->sram[0][addr - 0xA000];
    } else if (addr < 0xD000) {
        return &gb->wram[0][addr - 0xC000];
    } else if (addr < 0xE000) {
        return &gb->wram[gb->wram_bank][addr - 0xD000];
    } else if (addr < 0xFE00) {
        /* Echo RAM */
        uint16_t echo_addr = addr - 0x2000;
        return gb_get_ptr(gb, echo_addr);
    } else if (addr < 0xFEA0) {
        return &gb->oam[addr - 0xFE00];
    } else if (addr < 0xFF00) {
        return NULL; /* Unusable */
    } else if (addr < 0xFF80) {
        return &gb->io[addr - 0xFF00];
    } else if (addr < 0xFFFF) {
        return &gb->hram[addr - 0xFF80];
    } else {
        return &gb->ie;
    }
}

const uint8_t *gb_get_const_ptr(const GBState *gb, uint16_t addr) {
    if (addr < 0x4000) {
        if (gb->rom && gb->rom_size > addr) {
            return &gb->rom[addr];
        }
        return NULL;
    } else if (addr < 0x8000) {
        size_t rom_offset = (size_t)gb->rom_bank * 0x4000 + (addr - 0x4000);
        if (gb->rom && gb->rom_size > rom_offset) {
            return &gb->rom[rom_offset];
        }
        return NULL;
    }

    return gb_get_ptr((GBState *)gb, addr);
}

uint8_t gb_read(const GBState *gb, uint16_t addr) {
    const uint8_t *ptr = gb_get_const_ptr(gb, addr);
    return ptr ? *ptr : 0xFF;
}

void gb_write(GBState *gb, uint16_t addr, uint8_t val) {
    if (addr == rSelectROMBank) {
        gb->rom_bank = (val == 0) ? 1 : (val & 0x7F);
        return;
    }
    if (addr == rSVBK) {
        gb->wram_bank = (val & 0x07) == 0 ? 1 : (val & 0x07);
        gb->io[addr - 0xFF00] = val;
        return;
    }
    if (addr == rVBK) {
        gb->vram_bank = val & 0x01;
        gb->io[addr - 0xFF00] = val;
        return;
    }
    if (addr == rHDMA5) {
        gb->io[addr - 0xFF00] = val;
        /* If bit 7 is 0, General Purpose DMA */
        if ((val & 0x80) == 0) {
            uint16_t length = ((val & 0x7F) + 1) * 16;
            uint16_t src = ((uint16_t)gb->io[rHDMA1 - 0xFF00] << 8) | (gb->io[rHDMA2 - 0xFF00] & 0xF0);
            uint16_t dst = 0x8000 | (((uint16_t)gb->io[rHDMA3 - 0xFF00] & 0x1F) << 8) | (gb->io[rHDMA4 - 0xFF00] & 0xF0);
            for (uint16_t i = 0; i < length; i++) {
                uint8_t byte = gb_read(gb, src + i);
                gb_write(gb, dst + i, byte);
            }
            gb->io[rHDMA5 - 0xFF00] = 0xFF; /* 0xFF indicates DMA complete */
        }
        return;
    }

    uint8_t *ptr = gb_get_ptr(gb, addr);
    if (ptr) {
        *ptr = val;
    }
}
