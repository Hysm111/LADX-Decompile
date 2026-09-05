#ifndef LADX_GB_H
#define LADX_GB_H

#include "common.h"
#include "constants/hardware.h"
#include "constants/memory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const uint8_t *rom;        /* Optional pointer to ROM data */
    uint32_t rom_size;         /* Size of ROM in bytes */

    uint8_t wram[8][0x1000];   /* 8 banks of 4KB (0xC000-0xDFFF) */
    uint8_t vram[2][0x2000];   /* 2 banks of 8KB (0x8000-0x9FFF) */
    uint8_t sram[4][0x2000];   /* 4 banks of 8KB (0xA000-0xBFFF) */
    uint8_t oam[160];          /* OAM memory (0xFE00-0xFE9F) */
    uint8_t io[0x80];          /* I/O registers (0xFF00-0xFF7F) */
    uint8_t hram[0x7F];        /* HRAM (0xFF80-0xFFFE) */
    uint8_t ie;                /* Interrupt Enable (0xFFFF) */

    uint8_t rom_bank;          /* Current banked ROM (0x4000-0x7FFF) */
    uint8_t wram_bank;         /* Current WRAM bank (1-7) */
    uint8_t vram_bank;         /* Current VRAM bank (0-1) */
} GBState;

/* Initialize or reset GB state */
void gb_init(GBState *gb);

/* Attach a ROM to the GB state */
void gb_attach_rom(GBState *gb, const uint8_t *rom_data, uint32_t rom_size);

/* Memory read / write */
uint8_t gb_read(const GBState *gb, uint16_t addr);
void gb_write(GBState *gb, uint16_t addr, uint8_t val);

/* High-RAM specific read / write (0xFF80 - 0xFFFE) */
static inline uint8_t gb_read_hram(const GBState *gb, uint16_t addr) {
    if (addr >= 0xFF80 && addr <= 0xFFFE) {
        return gb->hram[addr - 0xFF80];
    }
    return 0;
}

static inline void gb_write_hram(GBState *gb, uint16_t addr, uint8_t val) {
    if (addr >= 0xFF80 && addr <= 0xFFFE) {
        gb->hram[addr - 0xFF80] = val;
    }
}

/* Get pointer to memory for sequential block operations */
uint8_t *gb_get_ptr(GBState *gb, uint16_t addr);
const uint8_t *gb_get_const_ptr(const GBState *gb, uint16_t addr);

#ifdef __cplusplus
}
#endif

#endif /* LADX_GB_H */
