#include "test_bank2.h"

#include "gb.h"
#include "bank2/room_effects.h"
#include "bank2/room_triggers.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Batch 68: independently read events.asm:7-29 and 459-484 for routing.
 * The exhaustive routing oracle COMPOSES the real, already VERIFIED checkers
 * and effects. It verifies dispatch/integration, NOT those bodies independently.
 * Literal cases below additionally use events.asm:81-165,182-278,290-315,
 * 588-716; bank0.asm:834-858,926-974; bank2.asm:3123-3176,4515-4546.
 * Unsupported IDs and missing dependencies are explicit C API rejection, NOT
 * claims that the unchecked assembly jump table treats invalid input as no-op.
 * Snapshots cover all GBState bytes (including initialized padding), not CPU
 * registers/flags/cycles or transient writes undone before observation.
 */
#define C(g, a) ((g)->wram[0][(a) - 0xC000])
#define D(g, a) ((g)->wram[(g)->wram_bank][(a) - 0xD000])
#define H(g, a) ((g)->hram[(a) - 0xFF80])

typedef void (*Checker)(GBState *);
typedef uint16_t (*Spawner)(GBState *, uint8_t);

/* Literal entry order from 02:5FA5-5FC4. NULL represents Events.return. */
static const Checker checker_table[16] = {
    CheckKillEnemiesTrigger, NULL, CheckStepOnButtonTrigger, NULL,
    CheckLightTorchesTrigger, CheckKillInOrderTrigger, NULL,
    CheckKillEnemiesTrigger, NULL, CheckKillSidescrollBossTrigger,
    NULL, NULL, NULL, NULL, NULL, CheckAnswerTunicsTrigger
};

static unsigned key_calls, fairy_calls;
static uint16_t key_result, fairy_result;
static bool record_entry, compare_entry;
static GBState key_entry, fairy_entry;

static void same(const GBState *gb, const GBState *expected,
                 const char *phase, unsigned event) {
    if (memcmp(gb, expected, sizeof(*gb)) != 0) {
        const unsigned char *a = (const unsigned char *)gb;
        const unsigned char *b = (const unsigned char *)expected;
        for (size_t i = 0; i < sizeof(*gb); ++i) {
            if (a[i] != b[i]) {
                fprintf(stderr, "room dispatch %s event %02X byte %zu: %02X != %02X\n",
                        phase, event, i, (unsigned)a[i], (unsigned)b[i]);
                break;
            }
        }
        assert(!"room dispatch full-state mismatch");
    }
}

static uint16_t spawn_key(GBState *gb, uint8_t type) {
    assert(gb && gb->rom_bank == 3);
    assert(C(gb, 0xC18E) == 0);
    assert(C(gb, 0xC18F) != 0);
    assert(type == ((H(gb, 0xFFF7) < 0x0A || H(gb, 0xFFF7) == 0xFF)
                    ? 0x30 : 0x3C));
    assert(++key_calls == 1);
    if (record_entry) memcpy(&key_entry, gb, sizeof(*gb));
    if (compare_entry) same(gb, &key_entry, "key callback entry", type);
    assert(key_result < 16 || key_result == 0xFFFF);
    if (key_result != 0xFFFF) {
        C(gb, 0xC280 + key_result) = 2;
        C(gb, 0xC3A0 + key_result) = type;
    }
    return key_result; /* Existing key C convention: FFFF means carry/failure. */
}

static uint16_t spawn_fairy(GBState *gb, uint8_t type) {
    assert(gb && gb->rom_bank == 3);
    assert(C(gb, 0xC18E) == 0);
    assert(C(gb, 0xC18F) != 0);
    assert(type == 0x2F);
    assert(++fairy_calls == 1);
    if (record_entry) memcpy(&fairy_entry, gb, sizeof(*gb));
    if (compare_entry) same(gb, &fairy_entry, "fairy callback entry", type);
    assert(fairy_result < 16 || fairy_result == 0x00FF);
    if (fairy_result != 0x00FF) {
        C(gb, 0xC280 + fairy_result) = 2;
        C(gb, 0xC3A0 + fairy_result) = type;
    }
    return fairy_result; /* Raw DE: 00FF must still receive table writes. */
}

static void reset_callbacks(void) {
    key_calls = fairy_calls = 0;
    key_result = 3;
    fairy_result = 7;
    record_entry = compare_entry = false;
}

static void fill_bytes(uint8_t *p, size_t n, unsigned salt) {
    for (size_t i = 0; i < n; ++i)
        p[i] = (uint8_t)(1u + (i * 37u + i / 251u + salt) % 255u);
}

static void init_seed(GBState *gb) {
    static const uint8_t rom[] = {0x31, 0x73, 0xA5, 0xFE};
    memset(gb, 0, sizeof(*gb));
    gb->rom = rom;
    gb->rom_size = sizeof(rom);
    for (unsigned i = 0; i < 8; ++i)
        fill_bytes(gb->wram[i], sizeof(gb->wram[i]), i);
    for (unsigned i = 0; i < 2; ++i)
        fill_bytes(gb->vram[i], sizeof(gb->vram[i]), i + 19);
    for (unsigned i = 0; i < 4; ++i)
        fill_bytes(gb->sram[i], sizeof(gb->sram[i]), i + 41);
    fill_bytes(gb->oam, sizeof(gb->oam), 61);
    fill_bytes(gb->io, sizeof(gb->io), 83);
    fill_bytes(gb->hram, sizeof(gb->hram), 107);
    gb->rom_bank = 0x35;
    gb->wram_bank = 5;
    gb->vram_bank = 1;
    gb->sram_bank = 3;
    gb->sram_enabled = true;
    gb->joypad_input = 0xA6;
    gb->ie = 0x1B;
    C(gb, 0xC18E) = 0xC1;
    C(gb, 0xC18F) = C(gb, 0xC19D) = C(gb, 0xC113) = 0;
    C(gb, 0xC190) = 1;
    C(gb, 0xC18C) = C(gb, 0xC18D) = 0;
    C(gb, 0xC1A2) = 2;
    C(gb, 0xC1CB) = 0x80;
    D(gb, 0xD460) = 1;
    D(gb, 0xDBB6) = 0;
    D(gb, 0xDBB7) = 1;
    D(gb, 0xDBB8) = 2;
    D(gb, 0xD9FF) = D(gb, 0xDAE8) = 0x20;
    D(gb, 0xDBA5) = 1;
    D(gb, 0xDBAF) = 2;
    D(gb, 0xDB68) = 0; /* Map 3 instrument byte. */
    D(gb, 0xD937) = 0x42;
    H(gb, 0xFFF6) = 0x37;
    H(gb, 0xFFF7) = 3;
    H(gb, 0xFFF8) = 0x42;
    H(gb, 0xFFD7) = 0xED;
    H(gb, 0xFFD8) = 0xDA;
    H(gb, 0xFF98) = 0x88;
    H(gb, 0xFF99) = 0x30;
    for (unsigned i = 0; i < 16; ++i) {
        C(gb, 0xC280 + i) = 0;
        C(gb, 0xC430 + i) = 0;
        C(gb, 0xC3A0 + i) = 0;
        C(gb, 0xC510 + i) = 0;
    }
    C(gb, 0xC5C0) = 7;
}

static void tunics(GBState *gb, uint8_t room, unsigned count) {
    H(gb, 0xFFF6) = room;
    H(gb, 0xFFF7) = 0xFF;
    for (unsigned i = 0; i < count; ++i) {
        C(gb, 0xC280 + i) = 1;
        C(gb, 0xC3A0 + i) = room == 0x12 ? 0xF6 : 0xEF;
        C(gb, 0xC3B0 + i) = 8;
        C(gb, 0xC290 + i) = 4;
    }
}

/* Distinguishable positive/negative checker outcomes, not just one fixture
 * where all six checkers happen to resolve and could be swapped unnoticed. */
static void fixture(GBState *gb, const GBState *seed, unsigned variant) {
    memcpy(gb, seed, sizeof(*gb));
    if (variant == 1 || (variant >= 4 && variant <= 7)) {
        C(gb, 0xC280 + 15) = 5;
        C(gb, 0xC340 + 15) = 0x13;
        C(gb, 0xC1A2) = 3;
        C(gb, 0xC1CB) = 0;
        D(gb, 0xDBB7) = 7;
        D(gb, 0xD9FF) = D(gb, 0xDAE8) = 0;
    }
    switch (variant) {
    case 2: C(gb, 0xC18F) = 0x80; break;
    case 3: H(gb, 0xFFF8) = 0x52; break;
    case 4: C(gb, 0xC1CB) = 0x80; break;
    case 5: C(gb, 0xC1A2) = 2; break;
    case 6: D(gb, 0xDBB7) = 1; break;
    case 7:
        H(gb, 0xFFF7) = 6;
        D(gb, 0xDAE8) = 0x20;
        break;
    case 8: tunics(gb, 8, 4); break;
    case 9: C(gb, 0xC113) = 1; break;
    case 10: tunics(gb, 0x0A, 9); break;
    default: break;
    }
}

static void composed_check(GBState *gb, uint8_t event) {
    unsigned id = event & 0x1F;
    assert(id >= 1 && id <= 16);
    H(gb, 0xFFD7) = (uint8_t)id;
    if (checker_table[id - 1]) checker_table[id - 1](gb);
}

static void composed_outer(GBState *gb) {
    uint8_t event = C(gb, 0xC18E);
    if (!event) return;
    composed_check(gb, event);
    /* Literal order from 02:5D69-5D78, using the POST-checker event. */
    switch ((C(gb, 0xC18E) & 0xE0) >> 5) {
    case 0: break;
    case 1: OpenShutterDoorsEffectHandler(gb); break;
    case 2: KillAllEnemiesEffectHandler(gb); break;
    case 3: RevealChestEffectHandler(gb); break;
    case 4: DropKeyEffectHandler(gb, spawn_key); break;
    case 5: RevealStaircaseEffectHandler(gb); break;
    case 6: ClearMidbossEffectHandler(gb); break;
    case 7: DropFairyEffectHandler(gb, spawn_fairy); break;
    }
}

static void test_check_domain_and_routes(const GBState *seed) {
    GBState gb, expected;
    unsigned accepted = 0;
    for (unsigned event = 0; event < 256; ++event) {
        assert(!CheckTriggersResolution(NULL, (uint8_t)event));
        unsigned id = event & 0x1F;
        bool valid = id >= 1 && id <= 16;
        accepted += valid;
        for (unsigned variant = 0; variant < 11; ++variant) {
            fixture(&gb, seed, variant);
            /* Register A argument must not be replaced by wRoomEvent, even
             * when that memory byte is zero or has a different trigger ID. */
            if (variant & 1) C(&gb, 0xC18E) = 0;
            memcpy(&expected, &gb, sizeof(gb));
            if (valid) composed_check(&expected, (uint8_t)event);
            assert(CheckTriggersResolution(&gb, (uint8_t)event) == valid);
            same(&gb, &expected, "checker composition/domain", event);
        }
    }
    assert(accepted == 128);
}

static void test_outer_routes(const GBState *seed) {
    GBState gb, expected;
    unsigned routes = 0;
    for (unsigned effect = 0; effect < 8; ++effect) {
        for (unsigned id = 1; id <= 16; ++id) {
            unsigned event = (effect << 5) | id;
            ++routes;
            for (unsigned variant = 0; variant < 11; ++variant) {
                fixture(&gb, seed, variant);
                C(&gb, 0xC18E) = (uint8_t)event;
                memcpy(&expected, &gb, sizeof(gb));
                for (unsigned repeat = 0; repeat < 2; ++repeat) {
                    reset_callbacks();
                    if (variant & 1) {
                        key_result = 0xFFFF;
                        fairy_result = 0x00FF;
                    }
                    record_entry = true;
                    composed_outer(&expected);
                    unsigned keys = key_calls, fairies = fairy_calls;
                    record_entry = false;
                    compare_entry = true;
                    key_calls = fairy_calls = 0;
                    assert(ExecuteRoomTriggersAndEffects(&gb, spawn_key, spawn_fairy));
                    assert(key_calls == keys && fairy_calls == fairies);
                    same(&gb, &expected, "outer composition/repeat", event);
                }
            }
        }
    }
    assert(routes == 128);
}

static void test_outer_preconditions(const GBState *seed) {
    GBState gb, expected;
    for (unsigned callbacks = 0; callbacks < 4; ++callbacks) {
        Spawner key = callbacks & 1 ? spawn_key : NULL;
        Spawner fairy = callbacks & 2 ? spawn_fairy : NULL;
        reset_callbacks();
        assert(!ExecuteRoomTriggersAndEffects(NULL, key, fairy));
        for (unsigned event = 0; event < 256; ++event) {
            unsigned id = event & 0x1F;
            if (event && id >= 1 && id <= 16 && callbacks == 3) continue;
            memcpy(&gb, seed, sizeof(gb));
            C(&gb, 0xC18E) = (uint8_t)event;
            /* Ready checkers would write if validation were done too late. */
            if (id == 16) tunics(&gb, 0x0A, 9);
            memcpy(&expected, &gb, sizeof(gb));
            for (unsigned repeat = 0; repeat < 2; ++repeat) {
                assert(ExecuteRoomTriggersAndEffects(&gb, key, fairy) == (event == 0));
                same(&gb, &expected, "precondition/zero", event);
                assert(key_calls == 0 && fairy_calls == 0);
            }
        }
    }
}

/* Literal expected writes: no production helpers in the remaining oracles. */
static void expect_mark(GBState *gb) {
    C(gb, 0xC1CF) = 0;
    C(gb, 0xC18F) = 1;
    C(gb, 0xC5A6) = 1;
    if (!C(gb, 0xC19D)) H(gb, 0xFFF2) = 2;
}

static void expect_vfx15(GBState *gb, uint8_t type, uint8_t y) {
    H(gb, 0xFFD7) = 0x88;
    H(gb, 0xFFD8) = y;
    C(gb, 0xC51F) = type;
    C(gb, 0xC52F) = 0x0F;
    C(gb, 0xC53F) = 0x88;
    C(gb, 0xC54F) = y;
}

static void test_literal_marking(const GBState *seed) {
    static const uint8_t ids[] = {1, 3, 5, 6, 8, 10};
    GBState gb, expected;
    for (unsigned i = 0; i < sizeof(ids); ++i) {
        for (unsigned muted = 0; muted < 2; ++muted) {
            memcpy(&gb, seed, sizeof(gb));
            C(&gb, 0xC19D) = muted ? 0x80 : 0;
            C(&gb, 0xC18E) = 0xE0; /* Unsupported memory event is irrelevant. */
            memcpy(&expected, &gb, sizeof(gb));
            H(&expected, 0xFFD7) = ids[i];
            expect_mark(&expected);
            assert(CheckTriggersResolution(&gb, (uint8_t)(0xE0 | ids[i])));
            same(&gb, &expected, "literal mark", ids[i]);
            assert(CheckTriggersResolution(&gb, ids[i]));
            same(&gb, &expected, "literal mark repeat", ids[i]);
        }
    }
    /* IDs 1 and 8 share a body but must receive different MP0 values. */
    for (unsigned killed = 0; killed < 2; ++killed) {
        memcpy(&gb, seed, sizeof(gb));
        D(&gb, 0xD460) = killed ? 1 : 0;
        C(&gb, 0xC113) = (uint8_t)killed;
        memcpy(&expected, &gb, sizeof(gb));
        H(&expected, 0xFFD7) = 8;
        assert(CheckTriggersResolution(&gb, 0xA8));
        same(&gb, &expected, "special trigger blocked", 0xA8);
        H(&expected, 0xFFD7) = 1;
        expect_mark(&expected);
        assert(CheckTriggersResolution(&gb, 0xA1));
        same(&gb, &expected, "ordinary trigger ignores special guards", 0xA1);
    }
}

static void test_literal_tunics(const GBState *seed) {
    static const uint8_t rooms[] = {8, 0x0A, 0x12, 0x37};
    static const unsigned counts[] = {4, 9, 2, 4};
    GBState gb, expected;
    for (unsigned r = 0; r < 4; ++r) {
        for (unsigned effect = 0; effect < 8; ++effect) {
            /* Room 8 deliberately leaves the event alone; the other three
             * execute an internal effect and clear it before outer dispatch. */
            if (r == 0 && effect != 0) continue;
            memcpy(&gb, seed, sizeof(gb));
            tunics(&gb, rooms[r], counts[r]);
            C(&gb, 0xC18E) = (uint8_t)((effect << 5) | 16);
            D(&gb, 0xDDEA) = D(&gb, 0xDDF2) = 0x42;
            memcpy(&expected, &gb, sizeof(gb));
            H(&expected, 0xFFD7) = (uint8_t)counts[r];
            expect_mark(&expected);
            if (r != 0) C(&expected, 0xC18E) = 0;
            if (r == 1) {
                C(&expected, 0xC18C) = 1;
                H(&expected, 0xFFF4) = 4;
                D(&expected, 0xDDEA) = 0x52;
            } else if (r == 2) {
                D(&expected, 0xDDF2) = H(&expected, 0xFFF8) = 0x52;
            } else if (r == 3) {
                expect_vfx15(&expected, 3, 0x40);
            }
            reset_callbacks();
            assert(ExecuteRoomTriggersAndEffects(&gb, spawn_key, spawn_fairy));
            same(&gb, &expected, "literal tunic post-checker event", effect << 5);
            assert(key_calls == 0 && fairy_calls == 0);
            /* For cleared events the zero shortcut must even preserve MP0.
             * Room 8 remains pending: dispatcher rewrites MP0 to ID 16. */
            if (r == 0) H(&expected, 0xFFD7) = 16;
            assert(ExecuteRoomTriggersAndEffects(&gb, spawn_key, spawn_fairy));
            same(&gb, &expected, "literal tunic repeat", effect << 5);
            assert(key_calls == 0 && fairy_calls == 0);
        }
    }

    /* Register A=10 selects tunics independently of memory C1. Its guard
     * clears C1 before shutter handling: no miniboss flag/warp jingle. */
    memcpy(&gb, seed, sizeof(gb));
    tunics(&gb, 0x0A, 9);
    C(&gb, 0xC18E) = 0xC1;
    D(&gb, 0xDDEA) = 0x42;
    memcpy(&expected, &gb, sizeof(gb));
    H(&expected, 0xFFD7) = 9;
    expect_mark(&expected);
    C(&expected, 0xC18E) = 0;
    C(&expected, 0xC18C) = 1;
    H(&expected, 0xFFF4) = 4;
    D(&expected, 0xDDEA) = 0x52;
    assert(CheckTriggersResolution(&gb, 0x10));
    same(&gb, &expected, "tunic clears independent C1", 0x10);
    reset_callbacks();
    assert(ExecuteRoomTriggersAndEffects(&gb, NULL, NULL));
    same(&gb, &expected, "cleared C1 means effect none", 0);

    /* An unsolved tunic scan overwrites the dispatch ID with the real count. */
    memcpy(&gb, seed, sizeof(gb));
    tunics(&gb, 8, 3);
    memcpy(&expected, &gb, sizeof(gb));
    H(&expected, 0xFFD7) = 3;
    assert(CheckTriggersResolution(&gb, 0xF0));
    same(&gb, &expected, "unsolved tunic count", 0xF0);
}

static void test_literal_guards(const GBState *seed) {
    static const uint8_t guarded_events[] = {0x42, 0x62, 0x82, 0xA2, 0xE2};
    GBState gb, expected;
    for (unsigned i = 0; i < sizeof(guarded_events); ++i) {
        for (unsigned status = 0; status < 2; ++status) {
            for (unsigned resolved = 0; resolved < 2; ++resolved) {
                if (!status && resolved) continue;
                memcpy(&gb, seed, sizeof(gb));
                C(&gb, 0xC18E) = guarded_events[i];
                C(&gb, 0xC18F) = resolved ? 0x80 : 0;
                H(&gb, 0xFFF8) = status ? 0x52 : 0x42;
                memcpy(&expected, &gb, sizeof(gb));
                H(&expected, 0xFFD7) = 2; /* No-op checker still assigns MP0. */
                reset_callbacks();
                for (unsigned repeat = 0; repeat < 2; ++repeat) {
                    assert(ExecuteRoomTriggersAndEffects(&gb, spawn_key, spawn_fairy));
                    same(&gb, &expected, "literal effect guard", guarded_events[i]);
                    assert(key_calls == 0 && fairy_calls == 0);
                }
            }
        }
    }
}

static void test_literal_spawns(const GBState *seed) {
    GBState gb, expected;
    for (unsigned fairy = 0; fairy < 2; ++fairy) {
        for (unsigned fail = 0; fail < 2; ++fail) {
            for (unsigned map = 0; map < 3; ++map) {
                memcpy(&gb, seed, sizeof(gb));
                H(&gb, 0xFFF7) = map == 0 ? 3 : map == 1 ? 0x0A : 0xFF;
                H(&gb, 0xFFF6) = 0x69;
                D(&gb, 0xD969) = D(&gb, 0xDA69) = D(&gb, 0xDE49) = 0x42;
                C(&gb, 0xC18E) = fairy ? 0xE1 : 0x81;
                memcpy(&expected, &gb, sizeof(gb));
                H(&expected, 0xFFD7) = 1;
                expect_mark(&expected); /* Empty entity table resolves ID 1. */
                C(&expected, 0xC18E) = 0;
                uint16_t saved = map == 0 ? 0xD969 : map == 1 ? 0xDA69 : 0xDE49;
                D(&expected, saved) = H(&expected, 0xFFF8) = 0x52;
                expected.rom_bank = 2;
                reset_callbacks();
                if (fairy) {
                    fairy_result = fail ? 0x00FF : 7;
                    if (!fail) {
                        C(&expected, 0xC287) = 2;
                        C(&expected, 0xC3A7) = 0x2F;
                    }
                    /* Failure deliberately writes C2FF/C30F/C54F. The last
                     * overlaps VFX slot 15 Y and is subsequently overwritten. */
                    C(&expected, 0xC200 + fairy_result) = 0x88;
                    C(&expected, 0xC210 + fairy_result) = 0x30;
                    C(&expected, 0xC450 + fairy_result) = 0x80;
                    expect_vfx15(&expected, 2, 0x30);
                } else {
                    key_result = fail ? 0xFFFF : 3;
                    if (!fail) {
                        C(&expected, 0xC283) = 2;
                        C(&expected, 0xC3A3) = map == 1 ? 0x3C : 0x30;
                        C(&expected, 0xC203) = map == 2 ? 0x48 : 0x28;
                        C(&expected, 0xC213) = 0x3C;
                        C(&expected, 0xC313) = 0x70;
                    }
                }
                assert(ExecuteRoomTriggersAndEffects(&gb, spawn_key, spawn_fairy));
                assert(key_calls == !fairy && fairy_calls == fairy);
                same(&gb, &expected, "literal spawn success/failure", fairy ? 0xE1 : 0x81);
                assert(ExecuteRoomTriggersAndEffects(&gb, spawn_key, spawn_fairy));
                assert(key_calls == !fairy && fairy_calls == fairy);
                same(&gb, &expected, "spawn not repeated", 0);
            }
        }
    }
}

void test_bank2_room_dispatch(void) {
    GBState seed;
    init_seed(&seed);
    test_check_domain_and_routes(&seed);
    test_outer_routes(&seed);
    test_outer_preconditions(&seed);
    test_literal_marking(&seed);
    test_literal_tunics(&seed);
    test_literal_guards(&seed);
    test_literal_spawns(&seed);
}
