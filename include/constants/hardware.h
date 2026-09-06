#ifndef LADX_CONSTANTS_HARDWARE_H
#define LADX_CONSTANTS_HARDWARE_H

#include "common.h"

/* Screen dimensions */
#define SCRN_X           160
#define SCRN_Y           144
#define SCRN_X_B         20
#define SCRN_Y_B         18

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

/* VRAM Tile Blocks */
#define vTiles0          0x8000
#define vTiles1          0x8800
#define vTiles2          0x9000

/* VRAM Tilemaps */
#define vBGMap0          0x9800
#define vBGMap1          0x9C00

/* External MBC / Cartridge registers */
#define rRAMG            0x0000
#define rSelectROMBank   0x2100
#define rRAMB            0x4000
#define CART_SRAM_ENABLE 0x0A
#define CART_SRAM_DISABLE 0x00

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

/* rLCDC flags */
#define LCDCF_OFF        0x00
#define LCDCF_ON         0x80

/* rIE flags */
#define IEF_VBLANK       0x01
#define IEF_LCDC         0x02
#define IEF_TIMER        0x04
#define IEF_SERIAL       0x08
#define IEF_HILO         0x10

/* OAM attribute flags */
#define OAMF_PRI         0x80
#define OAMF_YFLIP       0x40
#define OAMF_XFLIP       0x20
#define OAMF_PAL0        0x00
#define OAMF_PAL1        0x10
#define OAMF_BANK0       0x00
#define OAMF_BANK1       0x08
#define OAMF_PALMASK     0x07
#define OAM_GBC_PAL_4    0x04

#endif /* LADX_CONSTANTS_HARDWARE_H */
#define hLinkSpeedX                 0xFF9A
#define hLinkSpeedY                 0xFF9B
#define hLinkDirection              0xFF9E
#define hLinkPositionZ              0xFFA2
#define hLinkVelocityZ              0xFFA3
#define hObjectUnderEntity          0xFFAF
#define hDungeonTitleMessageCountdown   0xFFB4
#define hMultiPurposeD                  0xFFE4
#define hLinkAnimationState          0xFF9D
#define hReplaceTiles                   0xFFA5
#define hAnimatedTilesFrameCount        0xFFA6
#define hAnimatedTilesDataOffset        0xFFA7
#define hMultiPurpose5                  0xFFDC
#define hMultiPurposeE                  0xFFE5
#define hMultiPurposeB                  0xFFE2
#define hMultiPurposeC                  0xFFE3
