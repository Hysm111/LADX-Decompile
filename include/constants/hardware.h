#ifndef LADX_CONSTANTS_HARDWARE_H
#define LADX_CONSTANTS_HARDWARE_H

#include "common.h"

/* Memory map base addresses */
#define ADDR_VRAM        0x8000
#define ADDR_SCRN0       0x9800
#define ADDR_SCRN1       0x9C00
#define ADDR_SRAM        0xA000
#define ADDR_RAM         0xC000
#define ADDR_RAMBANK     0xD000
#define ADDR_OAMRAM      0xFE00
#define ADDR_IO          0xFF00
#define ADDR_AUD3WAVERAM 0xFF30
#define ADDR_HRAM        0xFF80

/* VRAM Tilemaps */
#define vBGMap0          0x9800
#define vBGMap1          0x9C00

/* Hardware I/O registers */
#define rP1              0xFF00
#define rSB              0xFF01
#define rSC              0xFF02
#define rDIV             0xFF04
#define rTIMA            0xFF05
#define rTMA             0xFF06
#define rTAC             0xFF07
#define rIF              0xFF0F
#define rNR10            0xFF10
#define rNR11            0xFF11
#define rNR12            0xFF12
#define rNR13            0xFF13
#define rNR14            0xFF14
#define rNR21            0xFF16
#define rNR22            0xFF17
#define rNR23            0xFF18
#define rNR24            0xFF19
#define rNR30            0xFF1A
#define rNR31            0xFF1B
#define rNR32            0xFF1C
#define rNR33            0xFF1D
#define rNR34            0xFF1E
#define rNR41            0xFF20
#define rNR42            0xFF21
#define rNR43            0xFF22
#define rNR44            0xFF23
#define rNR50            0xFF24
#define rNR51            0xFF25
#define rNR52            0xFF26
#define rLCDC            0xFF40
#define rSTAT            0xFF41
#define rSCY             0xFF42
#define rSCX             0xFF43
#define rLY              0xFF44
#define rLYC             0xFF45
#define rDMA             0xFF46
#define rBGP             0xFF47
#define rOBP0            0xFF48
#define rOBP1            0xFF49
#define rWY              0xFF4A
#define rWX              0xFF4B
#define rKEY1            0xFF4D
#define rVBK             0xFF4F
#define rHDMA1           0xFF51
#define rHDMA2           0xFF52
#define rHDMA3           0xFF53
#define rHDMA4           0xFF54
#define rHDMA5           0xFF55
#define rRP              0xFF56
#define rBCPS            0xFF68
#define rBCPD            0xFF69
#define rOCPS            0xFF6A
#define rOCPD            0xFF6B
#define rSVBK            0xFF70
#define rIE              0xFFFF

/* MBC Bank selection registers */
#define rSelectROMBank   0x2100

#endif /* LADX_CONSTANTS_HARDWARE_H */
