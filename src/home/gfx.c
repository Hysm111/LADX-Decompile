#include "home/animated_tiles.h"
#include "constants/gameplay.h"
#include "constants/maps.h"
#include "home/gfx.h"
#include "home/bank.h"
#include "home/copy_data.h"
#include "home/audio.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/memory.h"

void LoadCreditsKoholintDisappearingTiles(GBState *gb) {
    if (!gb) return;

    uint8_t bank = AdjustBankNumberForGBC(gb, BANK_EndingTiles);
    gb_write(gb, rSelectROMBank, bank);

    CopyData(gb, vTiles2, EndingTiles + 0x2800, TILE_SIZE * 0x80);
    CopyData(gb, vTiles1, EndingTiles + 0x3000, TILE_SIZE * 0x80);
}

void LoadTileset15(GBState *gb) {
    if (!gb) return;

    uint8_t bank = AdjustBankNumberForGBC(gb, BANK_EndingTiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, EndingTiles, TILE_SIZE * 0x180);

    bank = AdjustBankNumberForGBC(gb, BANK_Overworld1Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles2 + 0x7F0, Overworld1Tiles + 0x8E0, TILE_SIZE);

    bank = AdjustBankNumberForGBC(gb, BANK_Npc4Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, Npc4Tiles + 0x100, TILE_SIZE * 4);

    CopyData(gb, vTiles1 + 0x500, Npc4Tiles + 0x100, TILE_SIZE * 0x20);
}

void LoadCreditsStairsTiles(GBState *gb) {
    if (!gb) return;

    LoadTileset15(gb);
    CopyData(gb, vTiles0 + 0x400, EndingTiles + 0x3600, TILE_SIZE * 0x10);
}

void LoadCreditsKoholintViewsTiles(GBState *gb) {
    if (!gb) return;

    uint8_t bank = AdjustBankNumberForGBC(gb, BANK_Overworld1Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles2, Overworld1Tiles + 0x100, TILE_SIZE * 0x80);

    bank = AdjustBankNumberForGBC(gb, BANK_Npc3Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, Npc3Tiles + 0x2000, TILE_SIZE * 0x80);

    bank = AdjustBankNumberForGBC(gb, BANK_Overworld2Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles1, Overworld2Tiles + 0x600, TILE_SIZE * 0x80);
}

static void copyLinkOnSeaTiles(GBState *gb, uint16_t src_hl, uint8_t bank, bool adjust_bank) {
    if (adjust_bank) {
        bank = AdjustBankNumberForGBC(gb, bank);
    }
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, src_hl, TILE_SIZE * 0x80);

    uint8_t ending_bank = AdjustBankNumberForGBC(gb, BANK_EndingTiles);
    gb_write(gb, rSelectROMBank, ending_bank);
    CopyData(gb, vTiles0 + TILE_SIZE * 0x80, EndingTiles + 0x1800, TILE_SIZE * 0x100);
}

void LoadCreditsLinkOnSeaCloseTiles(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, hIsGBC) == 0) {
        copyLinkOnSeaTiles(gb, EndingTiles, BANK_EndingTiles, true);
    } else {
        copyLinkOnSeaTiles(gb, PhotoAlbumTiles, BANK_PhotoAlbumTiles, false);
    }
}

void LoadCreditsSunAboveTiles(GBState *gb) {
    if (!gb) return;

    copyLinkOnSeaTiles(gb, EndingTiles + 0x800, BANK_EndingTiles, true);
}

void LoadCreditsLinkOnSeaLargeTiles(GBState *gb) {
    if (!gb) return;

    copyLinkOnSeaTiles(gb, EndingTiles + 0x2000, BANK_EndingTiles, true);
}

static void loadCreditsEndingSceneTiles(GBState *gb, uint16_t src_hl, uint8_t bank, bool adjust_bank) {
    if (adjust_bank) {
        bank = AdjustBankNumberForGBC(gb, bank);
    }
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, src_hl, TILE_SIZE * 0x80);

    uint8_t ending_bank = AdjustBankNumberForGBC(gb, BANK_EndingTiles);
    gb_write(gb, rSelectROMBank, ending_bank);
    CopyData(gb, vTiles1, EndingTiles + 0x3000, TILE_SIZE * 0x80);
    CopyData(gb, vTiles2, EndingTiles + 0x2800, TILE_SIZE * 0x80);
}

void func_2B92(GBState *gb, uint16_t src_hl, uint8_t bank) {
    if (!gb) return;
    loadCreditsEndingSceneTiles(gb, src_hl, bank, true);
}

void LoadCreditsLinkFaceCloseUpTiles(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, hIsGBC) == 0) {
        loadCreditsEndingSceneTiles(gb, EndingTiles + 0x3800, BANK_EndingTiles, true);
    } else {
        loadCreditsEndingSceneTiles(gb, EndingCGBAltTiles, BANK_EndingCGBAltTiles, false);
    }
}

void LoadCreditsLinkSeatedOnLogTiles(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, hIsGBC) == 0) {
        loadCreditsEndingSceneTiles(gb, EndingTiles + 0x800, BANK_EndingTiles, true);
    } else {
        loadCreditsEndingSceneTiles(gb, PhotoAlbumTiles + 0x800, BANK_PhotoAlbumTiles, false);
    }
}

void LoadCreditsRollTiles(GBState *gb) {
    if (!gb) return;

    PlayAudioStep(gb);

    func_2B92(gb, FontLargeTiles + 0x100, BANK_FontLargeTiles);

    PlayAudioStep(gb);

    uint8_t bank = AdjustBankNumberForGBC(gb, BANK_Npc3Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, Npc3Tiles + 0x2600, TILE_SIZE * 0x08);

    PlayAudioStep(gb);

    if (gb_read(gb, hIsGBC) == 0) {
        gb_write(gb, rSelectROMBank, BANK_FontLargeTiles);
        CopyData(gb, vTiles0 + 0x100, FontLargeTiles + 0x200, TILE_SIZE * 0x70);
    } else {
        gb_write(gb, rSelectROMBank, BANK_CreditsRollTiles);
        CopyData(gb, vTiles0, CreditsRollTiles, TILE_SIZE * 0x80);
    }
}

void LoadBaseTiles(GBState *gb) {
    if (!gb) return;

    SwitchAdjustedBank(gb, BANK_LinkCharacterTiles);
    CopyData(gb, vTiles0, LinkCharacterTiles, TILE_SIZE * 0x40);

    SwitchAdjustedBank(gb, BANK_InventoryEquipmentItemsTiles);
    CopyData(gb, vTiles1, InventoryEquipmentItemsTiles, TILE_SIZE * 0x100);

    CopyData(gb, vTiles1 + 0x600, Items1Tiles + 0x3A0, TILE_SIZE * 2);

    SwitchBank(gb, 0x01);
}

void LoadMenuTiles(GBState *gb) {
    if (!gb) return;

    LoadBaseTiles(gb);

    SwitchAdjustedBank(gb, BANK_MenuTiles);
    CopyData(gb, vTiles1, MenuTiles, 0x400);

    SwitchAdjustedBank(gb, BANK_FontTiles);
    CopyData(gb, vTiles2, FontTiles, 0x800);
}

void LoadIntroSequenceTiles(GBState *gb) {
    if (!gb) return;

    SwitchBank(gb, BANK_IntroRainTiles);
    CopyData(gb, vTiles0 + 0x700, IntroRainTiles, TILE_SIZE * 8);

    SwitchAdjustedBank(gb, BANK_IntroTiles);
    CopyData(gb, vTiles0, Intro3Tiles, TILE_SIZE * 0x60);
    CopyData(gb, vTiles1, Intro1Tiles, TILE_SIZE * 0x100);
}

void LoadTitleScreenTiles(GBState *gb) {
    if (!gb) return;

    SwitchAdjustedBank(gb, BANK_TitleLogoTiles);
    CopyData(gb, vTiles1, TitleLogoTiles, TILE_SIZE * 0x70);

    SwitchBank(gb, BANK_TitleDXTiles);

    uint16_t dx_tiles = (gb_read(gb, hIsGBC) != 0) ? TitleDXTilesCGB : TitleDXTilesDMG;
    CopyData(gb, vTiles0 + 0x400, dx_tiles, TILE_SIZE * 0x40);

    uint16_t dx_oam_tiles = (gb_read(gb, hIsGBC) != 0) ? TitleDXOAMTiles : (TitleDXOAMTiles + 0x100);
    CopyData(gb, vTiles0 + 0x200, dx_oam_tiles, TILE_SIZE * 0x10);
}

void LoadWorldMapTiles(GBState *gb) {
    if (!gb) return;

    SwitchAdjustedBank(gb, BANK_WorldMapTiles);
    CopyData(gb, vTiles1 + 0x700, WorldMapTiles, TILE_SIZE * 0x80);
    CopyData(gb, vTiles0 + 0x200, Overworld1Tiles + 0x100, TILE_SIZE * 0x10);
}

void LoadStaticPictureTiles(GBState *gb, uint16_t src_hl) {
    if (!gb) return;

    SwitchAdjustedBank(gb, BANK_StaticPicturesTiles);
    CopyData(gb, vTiles2, src_hl, TILE_SIZE * 0x80);
}

void LoadFaceShrineReliefTiles(GBState *gb) {
    LoadStaticPictureTiles(gb, ReliefTiles);
}

void LoadSchulePaintingTiles(GBState *gb) {
    LoadStaticPictureTiles(gb, PaintingTiles);
}

void LoadChristinePortraitTiles(GBState *gb) {
    LoadStaticPictureTiles(gb, ChristineTiles);
}

void LoadEaglesTowerTopTiles(GBState *gb) {
    if (!gb) return;

    uint8_t bank = AdjustBankNumberForGBC(gb, BANK_EaglesTowerTop1Tiles);
    gb_write(gb, rSelectROMBank, bank);

    CopyData(gb, vTiles1 + 0x400, EaglesTowerTop2Tiles, TILE_SIZE * 0x40);
    CopyData(gb, vTiles2, EaglesTowerTop1Tiles, TILE_SIZE * 0x40);
}

void LoadMarinBeachTiles(GBState *gb) {
    if (!gb) return;

    SwitchAdjustedBank(gb, BANK_FontLargeTiles);
    CopyData(gb, vTiles0 + 0x400, FontLargeTiles, TILE_SIZE * 0x40);
    CopyData(gb, vTiles2, MarinBeachTiles, TILE_SIZE * 0x60);
}

void LoadSaveMenuTiles(GBState *gb) {
    if (!gb) return;

    SwitchBank(gb, BANK_SaveMenuTiles);
    CopyData(gb, vTiles1, SaveMenuTiles, TILE_SIZE * 0x50);
}

static const uint8_t NpcTilesBankTable[4] = {
    0x00,
    BANK_Npc2Tiles, /* 0x11 */
    BANK_Npc1Tiles, /* 0x0E */
    BANK_Npc3Tiles  /* 0x12 */
};

void LoadRoomSpecificTiles(GBState *gb, void (*load_color_dungeon_tiles)(GBState *)) {
    if (!gb) return;

    if (gb_read(gb, hMapId) == MAP_COLOR_DUNGEON) {
        gb_write(gb, rSelectROMBank, 0x20);
        if (load_color_dungeon_tiles) {
            load_color_dungeon_tiles(gb);
        }
        goto oamTilesEnd;
    }

    for (uint8_t row = 0; row < 4; row++) {
        gb_write(gb, hMultiPurpose0, row);

        uint8_t spritesheet = gb_read(gb, wLoadedEntitySpritesheets + row);

        if (row == 0) {
            bool use_predefined = false;
            if (gb_read(gb, wIsIndoor) != 0) {
                if (gb_read(gb, hIsSideScrolling) != 0) {
                    use_predefined = true;
                } else if (gb_read(gb, hMapId) == MAP_KANALET) {
                    use_predefined = true;
                } else if (gb_read(gb, hMapId) < MAP_CAVE_B) {
                    use_predefined = true;
                } else if (gb_read(gb, hMapRoom) == ROOM_INDOOR_B_MANBO) {
                    use_predefined = true;
                } else if (gb_read(gb, hMapRoom) == ROOM_INDOOR_B_FISHING_MINIGAME) {
                    use_predefined = true;
                }
            }

            if (!use_predefined) {
                if (gb_read(gb, wIsBowWowFollowingLink) == BOW_WOW_FOLLOWING) {
                    spritesheet = 0xA4;
                } else if (gb_read(gb, wIsGhostFollowingLink) != 0) {
                    spritesheet = 0xD8;
                } else if (gb_read(gb, wIsRoosterFollowingLink) != 0) {
                    spritesheet = 0xDD;
                } else if (gb_read(gb, wIsMarinFollowingLink) != 0) {
                    spritesheet = 0x8F;
                }
            }
        }

        if (spritesheet == 0) {
            continue;
        }

        uint16_t bc = (spritesheet & 0x3F) << 8;
        uint8_t bank_index = (spritesheet >> 6) & 0x03;
        uint8_t bank = NpcTilesBankTable[bank_index];
        if (bank != 0) {
            bank = AdjustBankNumberForGBC(gb, bank);
        }
        gb_write(gb, rSelectROMBank, bank);

        uint16_t dest = (vTiles0 + 0x400) + (row << 8);
        uint16_t src = NpcTilesDataStart + bc;
        CopyData(gb, dest, src, TILE_SIZE * 0x10);
    }

oamTilesEnd:
    if (gb_read(gb, wIsIndoor) == 0) {
        uint8_t bank = AdjustBankNumberForGBC(gb, BANK_Overworld2Tiles);
        gb_write(gb, rSelectROMBank, bank);

        uint8_t world_tileset = gb_read(gb, hWorldTileset);
        if (world_tileset == W_TILESET_KEEP) {
            return;
        }

        uint16_t src = (0x40 + world_tileset) << 8;
        CopyData(gb, vTiles2, src, TILE_SIZE * 0x20);
        return;
    }

    uint8_t dungeon_bank = AdjustBankNumberForGBC(gb, BANK_DungeonsTiles);
    gb_write(gb, rSelectROMBank, dungeon_bank);

    if (gb_read(gb, hIsSideScrolling) != 0) {
        uint16_t sideview_tiles = DungeonSideview1Tiles;
        uint8_t map_id = gb_read(gb, hMapId);
        if (map_id == MAP_EAGLES_TOWER) {
            sideview_tiles = DungeonSideview1Tiles;
        } else if (map_id >= MAP_CAVE_B) {
            if (gb_read(gb, hMapRoom) == ROOM_INDOOR_B_SEASHELL_MANSION) {
                sideview_tiles = DungeonSideview2Tiles;
            }
        } else {
            sideview_tiles = DungeonSideview2Tiles;
        }
        CopyData(gb, vTiles2, sideview_tiles, TILE_SIZE * 0x80);
        return;
    }

    uint8_t map_id = gb_read(gb, hMapId);
    bool skip_bg_loading = false;
    if (map_id == MAP_COLOR_DUNGEON) {
        if (gb_read(gb, hMapRoom) != UNKNOWN_ROOM_12) {
            skip_bg_loading = true;
        }
    }

    if (!skip_bg_loading) {
        uint8_t world_tileset = gb_read(gb, hWorldTileset);
        if (world_tileset != W_TILESET_NO_UPDATE) {
            uint16_t src = (0x50 + world_tileset) << 8;
            CopyData(gb, vTiles2, src, TILE_SIZE * 0x10);
        }
    }

    if (map_id == MAP_HOUSE && gb_read(gb, hMapRoom) == ROOM_INDOOR_B_CAMERA_SHOP) {
        gb_write(gb, rSelectROMBank, BANK_CameraShopIndoorTiles);
        CopyData(gb, vTiles1 + 0x700, CameraShopIndoorTiles, TILE_SIZE * 0x20);
        return;
    }

    if (gb_read(gb, hIsGBC) == 0) {
        return;
    }
    if (map_id != 0) {
        return;
    }

    gb_write(gb, rSelectROMBank, BANK_PhotoAlbumTiles);
    CopyData(gb, vTiles2 + 0x690, PhotoAlbumTiles + 0x600, TILE_SIZE);
    CopyData(gb, vTiles2 + 0x790, PhotoAlbumTiles + 0x610, TILE_SIZE);
}

void CopyWord(GBState *gb, uint16_t de, uint16_t hl) {
    if (!gb) return;
    gb_write(gb, de, gb_read(gb, hl));
    gb_write(gb, de + 1, gb_read(gb, hl + 1));
}

void WriteObjectToBG_DMG(GBState *gb, uint16_t de, uint16_t hl) {
    if (!gb) return;

    uint8_t obj_id = gb_read(gb, hl);
    uint16_t offset = (uint16_t)obj_id * 4;

    uint16_t table;
    uint8_t map_id = gb_read(gb, hMapId);
    if (map_id == MAP_COLOR_DUNGEON ||
        (map_id == MAP_HOUSE && gb_read(gb, hMapRoom) == ROOM_INDOOR_B_CAMERA_SHOP)) {
        table = ColorDungeonObjectsTilemap;
    } else if (gb_read(gb, wIsIndoor) != 0) {
        table = IndoorObjectsTilemapDMG;
    } else {
        table = OverworldObjectsTilemapDMG;
    }

    uint16_t src = table + offset;

    gb_write(gb, de, gb_read(gb, src));
    gb_write(gb, de + 1, gb_read(gb, src + 1));
    gb_write(gb, de + 0x20, gb_read(gb, src + 2));
    gb_write(gb, de + 0x21, gb_read(gb, src + 3));
}

uint8_t SwitchToObjectsTilemapBank(GBState *gb) {
    if (!gb) return 0;
    uint8_t bank = (gb_read(gb, wIsIndoor) != 0) ? BANK_IndoorObjectsTilemapDMG : BANK_OverworldObjectsTilemapDMG;
    gb_write(gb, rSelectROMBank, bank);
    return bank;
}

void WriteOverworldObjectToBG(GBState *gb, uint16_t de, uint16_t hl,
                              void (*get_bg_attributes)(GBState *, uint16_t, uint16_t)) {
    if (!gb) return;

    gb_write(gb, rSVBK, 2);
    uint8_t c = gb_read(gb, hl);
    gb_write(gb, rSVBK, 0);

    doCopyObjectToBG(gb, de, hl, c, get_bg_attributes);
}

void WriteIndoorObjectToBG(GBState *gb, uint16_t de, uint16_t hl,
                            void (*get_bg_attributes)(GBState *, uint16_t, uint16_t)) {
    if (!gb) return;

    uint8_t c = gb_read(gb, hl);
    doCopyObjectToBG(gb, de, hl, c, get_bg_attributes);
}

void doCopyObjectToBG(GBState *gb, uint16_t de, uint16_t hl, uint8_t obj_attr_val,
                      void (*get_bg_attributes)(GBState *, uint16_t, uint16_t)) {
    if (!gb) return;

    uint16_t bc = (uint16_t)obj_attr_val * 4;

    gb_write(gb, rSelectROMBank, 0x1A);
    if (get_bg_attributes) {
        get_bg_attributes(gb, hl, bc);
    }

    SwitchToObjectsTilemapBank(gb);

    uint16_t tilemap;
    uint8_t map_id = gb_read(gb, hMapId);
    if (gb_read(gb, wIsIndoor) != 0) {
        if (map_id == MAP_COLOR_DUNGEON ||
            (map_id == MAP_HOUSE && gb_read(gb, hMapRoom) == ROOM_INDOOR_B_CAMERA_SHOP)) {
            tilemap = ColorDungeonObjectsTilemap;
        } else {
            tilemap = IndoorObjectsTilemapCGB;
        }
    } else {
        tilemap = OverworldObjectsTilemapCGB;
    }

    uint16_t src_tile = tilemap + bc;

    /* Copy tile numbers to BG map for tiles on the upper row */
    CopyWord(gb, de, src_tile);
    src_tile += 2;

    /* Copy tile attributes to BG map for tiles on the upper row */
    uint8_t attr_bank = gb_read(gb, hMultiPurpose8);
    gb_write(gb, rSelectROMBank, attr_bank);
    uint16_t attr_addr = ((uint16_t)gb_read(gb, hMultiPurpose9) << 8) | gb_read(gb, hMultiPurposeA);
    gb_write(gb, rVBK, 1);
    CopyWord(gb, de, attr_addr);
    attr_addr += 2;

    /* Restore RAM and ROM banks */
    gb_write(gb, rVBK, 0);
    SwitchToObjectsTilemapBank(gb);

    /* Update palette offset */
    gb_write(gb, hMultiPurpose9, (uint8_t)(attr_addr >> 8));
    gb_write(gb, hMultiPurposeA, (uint8_t)(attr_addr & 0xFF));

    /* Move BG target down by one row (+ 0x20) */
    uint16_t de_row2 = de + 0x20;

    /* Copy tile numbers for tiles on the lower row */
    CopyWord(gb, de_row2, src_tile);

    /* Copy palettes for tiles on the lower row */
    gb_write(gb, rSelectROMBank, attr_bank);
    attr_addr = ((uint16_t)gb_read(gb, hMultiPurpose9) << 8) | gb_read(gb, hMultiPurposeA);
    gb_write(gb, rVBK, 1);
    CopyWord(gb, de_row2, attr_addr);

    /* Restore RAM and ROM banks */
    gb_write(gb, rVBK, 0);
    SwitchToObjectsTilemapBank(gb);
}

void LoadRoomTilemap(GBState *gb,
                     void (*get_bg_attributes)(GBState *, uint16_t, uint16_t),
                     void (*update_minimap_arrow)(GBState *)) {
    if (!gb) return;

    uint8_t bank = SwitchToObjectsTilemapBank(gb);
    SwitchBank(gb, bank);

    uint16_t de = vBGMap0;
    uint16_t hl = wRoomObjects;
    uint8_t c = 0x80;

    while (c > 0) {
        if (gb_read(gb, hIsGBC) == 0) {
            WriteObjectToBG_DMG(gb, de, hl);
        } else {
            if (gb_read(gb, wIsIndoor) != 0) {
                WriteIndoorObjectToBG(gb, de, hl, get_bg_attributes);
            } else {
                WriteOverworldObjectToBG(gb, de, hl, get_bg_attributes);
            }
        }

        hl++;
        if ((hl & 0x0F) == (OBJECTS_PER_ROW + 1)) {
            hl = (hl & 0xFFF0) + 0x11;
        }

        uint8_t e = (uint8_t)(de & 0xFF);
        uint8_t d = (uint8_t)(de >> 8);
        e += 2;
        if ((e & 0x1F) == (SCRN_X / 8)) {
            uint16_t new_e = (uint16_t)(e & 0xE0) + 0x40;
            e = (uint8_t)new_e;
            d += (uint8_t)(new_e >> 8);
        }
        de = ((uint16_t)d << 8) | e;

        c--;
    }

    gb_write(gb, rSelectROMBank, 1);
    if (update_minimap_arrow) {
        update_minimap_arrow(gb);
    }
}


void LoadCreditsMarinPortraitTiles_trampoline(GBState *gb, void (*load_marin_portrait)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x27);
    if (load_marin_portrait) {
        load_marin_portrait(gb);
    }
}

void LoadThanksForPlayingTiles_trampoline(GBState *gb, void (*load_thanks)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x20);
    if (load_thanks) {
        load_thanks(gb);
    }
}

void func_2D50(GBState *gb) {
    if (!gb) return;

    gb_write(gb, hAnimatedTilesFrameCount, 0);
    gb_write(gb, hAnimatedTilesDataOffset, 0);
    AnimateTilesGroup(gb, NULL, NULL, NULL, NULL);

    uint8_t bank = AdjustBankNumberForGBC(gb, BANK_InventoryEquipmentItemsTiles);
    gb_write(gb, rSelectROMBank, bank);

    CopyData(gb, vTiles1, InventoryEquipmentItemsTiles, TILE_SIZE * 0x80);
    CopyData(gb, vTiles0 + 0x200, LinkCharacterTiles + 0x200, TILE_SIZE * 0x10);
}

void PatchInventoryTiles(GBState *gb) {
    if (!gb) return;

    /* Replace Magic Powder tile by Toadstool if needed */
    if (gb_read(gb, wHasToadstool) != 0) {
        ReplaceMagicPowderTilesByToadstool(gb);
    }

    /* Replace Slime Key tile by Golden Leaf if needed */
    bool should_check_leaves = true;
    if (gb_read(gb, wIsIndoor) != 0) {
        uint8_t map_id = gb_read(gb, hMapId);
        if (map_id == MAP_COLOR_DUNGEON || map_id < MAP_CAVE_B) {
            should_check_leaves = false;
        }
    }
    if (should_check_leaves) {
        if (gb_read(gb, wGoldenLeavesCount) >= SLIME_KEY) {
            ReplaceSlimeKeyTilesByGoldenLeaf(gb);
        }
    }

    /* Update the trading sequence item tile if needed */
    if (gb_read(gb, wTradeSequenceItem) >= TRADING_ITEM_RIBBON) {
        gb_write(gb, hReplaceTiles, REPLACE_TILES_TRADING_ITEM);
    }
}

void LoadBaseOverworldTiles(GBState *gb) {
    if (!gb) return;

    SwitchAdjustedBank(gb, BANK_OverworldLandscapeTiles);
    CopyData(gb, vTiles2 + 0x200, OverworldLandscapeTiles, TILE_SIZE * 0x60);

    CopyData(gb, vTiles1 + 0x400, InventoryOverworldItemsTiles, TILE_SIZE * 0x40);

    func_2D50(gb);
    PatchInventoryTiles(gb);
}

void LoadIndoorTiles(GBState *gb) {
    if (!gb) return;

    /* 1. Floor tiles */
    SwitchBank(gb, BANK_DungeonFloorTilesPointers);
    uint8_t map_id = gb_read(gb, hMapId);
    uint16_t floor_hl = 0;

    if (map_id == MAP_COLOR_DUNGEON) {
        gb_write(gb, rSelectROMBank, BANK_ColorDungeonTiles);
        CopyData(gb, vTiles2, ColorDungeonTiles + 0x200, TILE_SIZE * 0x10);
        floor_hl = ColorDungeonTiles;
    } else {
        uint8_t ptr_high = gb_read(gb, DungeonFloorTilesPointers + map_id);
        floor_hl = (uint16_t)(ptr_high << 8);
        SwitchAdjustedBank(gb, BANK_DungeonsTiles);
    }

    CopyData(gb, vTiles2 + 0x100, floor_hl, TILE_SIZE * 0x10);

    /* 2. Dungeon shared objects (doors, stairs, torches, etc.) */
    SwitchAdjustedBank(gb, BANK_DungeonsTiles);
    CopyData(gb, vTiles2 + 0x200, DungeonsTiles, TILE_SIZE * 0x60);

    /* 3. Indoor walls */
    gb_write(gb, rSelectROMBank, BANK_DungeonWallsTilesPointers);
    uint16_t walls_table = (map_id == MAP_COLOR_DUNGEON) ? ColorDungeonWallsTilesPointers : DungeonWallsTilesPointers;
    uint8_t walls_high = gb_read(gb, walls_table + map_id);
    uint16_t walls_hl = (uint16_t)(walls_high << 8);
    ReloadSavedBank(gb);
    CopyData(gb, vTiles2 + 0x200, walls_hl, TILE_SIZE * 0x20);

    uint8_t items1_bank = AdjustBankNumberForGBC(gb, BANK_Items1Tiles);
    gb_write(gb, rSelectROMBank, items1_bank);
    CopyData(gb, wAnimatedScrollingTilesStorage, Items1Tiles + 0x3C0, TILE_SIZE * 4);

    func_2D50(gb);

    /* 4. Indoor objects */
    gb_write(gb, rSelectROMBank, BANK_DungeonItemsTilesPointers);
    uint8_t items_high = gb_read(gb, DungeonItemsTilesPointers + map_id);
    uint16_t items_hl = (uint16_t)(items_high << 8);

    SwitchAdjustedBank(gb, BANK_DungeonItemsTiles);

    if (map_id == MAP_COLOR_DUNGEON) {
        items_hl = ColorDungeonTiles + 0x100;
        gb_write(gb, rSelectROMBank, BANK_ColorDungeonTiles);
    }

    CopyData(gb, vTiles1 + 0x700, items_hl, TILE_SIZE * 0x10);

    /* 5. Indoor items (map, compass, keys, etc.) */
    gb_write(gb, rSelectROMBank, gb_read(gb, wCurrentBank));
    uint16_t inv_hl = InventoryIndoorItemsTiles;

    if (map_id != MAP_COLOR_DUNGEON && map_id >= MAP_CAVE_B) {
        SwitchAdjustedBank(gb, BANK_InventoryOverworldItemsTiles);
        inv_hl = InventoryOverworldItemsTiles;
    }

    CopyData(gb, vTiles1 + 0x400, inv_hl, TILE_SIZE * 0x30);

    /* 6. Patch inventory tiles */
    PatchInventoryTiles(gb);
}

void ReloadColorDungeonNpcTiles(GBState *gb) {
    if (!gb) return;
    uint8_t bank = (gb_read(gb, hIsGBC) != 0) ? 0x35 : 0x34;
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, (uint16_t)(vTiles0 + 0x400), ColorDungeonNpcTiles, 0x400);
    gb_write(gb, rSelectROMBank, 0x20); /* BANK(InventoryEntryPoint) */
}
