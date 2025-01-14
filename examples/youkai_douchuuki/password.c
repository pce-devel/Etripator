/*
 * This file is part of Etripator,
 * copyright (c) 2009--2024 Vincent Cruz.
 *
 * Etripator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Etripator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Etripator.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>

#define ALPHABET_SIZE 42
#define MAX_STRING_SIZE 0x11

// list of passwords from bank 0x01f at offset 0x1198
static const uint8_t g_blob[] = {
    0xed,0x26,0x08,0xee,0x3d,0x23,0x1d,0x12,
    0x23,0xba,0xcf,0xb2,0x20,0xb6,0xbd,0xde,
    0xcb,0xba,0x20,0xc5,0x20,0xb6,0xde,0xd2,
    0xdd,0xc6,0x20,0xc5,0xd8,0xcf,0xbc,0xc0,
    0xa1,0x2a,0x64,0x98,0x0b,0x15,0x91,0x18,
    0xb1,0x15,0x12,0xb2,0xc2,0xc3,0xde,0xd3,
    0x20,0x32,0xd2,0xdd,0xb6,0xd7,0x20,0xbd,
    0xc0,0xb0,0xc4,0x20,0xbc,0xcf,0xbd,0x2a,
    0x5f,0x65,0x0c,0x8f,0x8b,0x22,0x76,0x13,
    0x14,0xb2,0xc2,0xc3,0xde,0xd3,0x20,0x33,
    0xd2,0xdd,0xb6,0xd7,0x20,0xbd,0xc0,0xb0,
    0xc4,0x20,0xbc,0xcf,0xbd,0x2a,0x9a,0x7d,
    0x09,0x9e,0xb5,0x09,0x55,0x0c,0x16,0xb2,
    0xc2,0xc3,0xde,0xd3,0x20,0x34,0xd2,0xdd,
    0xb6,0xd7,0x20,0xbd,0xc0,0xb0,0xc4,0x20,
    0xbc,0xcf,0xbd,0x2a,0x4a,0x89,0x0a,0x1f,
    0xf5,0x28,0x7b,0x1c,0x18,0xb2,0xc2,0xc3,
    0xde,0xd3,0x20,0x35,0xd2,0xdd,0xb6,0xd7,
    0x20,0xbd,0xc0,0xb0,0xc4,0x20,0xbc,0xcf,
    0xbd,0x2a,0x65,0x94,0x0e,0xac,0xe9,0x07,
    0x33,0x25,0x30,0xd3,0xb3,0x20,0xba,0xda,
    0xc3,0xde,0x20,0xba,0xdc,0xb2,0xd3,0xc9,
    0x20,0xc5,0xdd,0xb6,0x20,0xb1,0xd8,0xcf,
    0xbe,0xdd,0x2a,0xdc,0xd9,0x08,0xa3,0xe3,
    0x17,0x28,0x15,0x00,0xba,0xc9,0x20,0xcc,
    0xdf,0xdb,0xb8,0xde,0xd7,0xd1,0xc9,0x20,
    0xbc,0xc2,0xd3,0xdd,0xc6,0xca,0x20,0xb5,
    0xba,0xc0,0xb4,0xc3,0xde,0xb7,0xcf,0xbe,
    0xdd,0x2a,0x86,0xc4,0x09,0x6b,0x32,0x22,
    0xe6,0x0e,0x00,0xce,0xde,0xb8,0xca,0x20,
    0xd0,0xad,0xb0,0xbc,0xde,0xaf,0xb8,0xc9,
    0x20,0xcc,0xdf,0xdb,0xb8,0xde,0xd7,0xd1,
    0xa6,0x20,0xc2,0xb8,0xaf,0xc0,0xc9,0xc0,
    0xde,0x2a,0x54,0x34,0x06,0x3c,0x10,0x0a,
    0x9f,0x08,0x00,0xba,0xc9,0xb7,0xae,0xb8,
    0xca,0x20,0xc4,0xaf,0xc3,0xd3,0x20,0xb8,
    0xdb,0xb3,0xbc,0xc3,0x20,0xc2,0xb8,0xd8,
    0xcf,0xbc,0xc0,0x2a,0x63,0xb6,0x09,0x20,
    0xb4,0x08,0xea,0x13,0x00,0x4d,0x41,0x44,
    0x45,0x20,0x49,0x4e,0x20,0x4a,0x41,0x50,
    0x41,0x4e,0x2a,0x46,0x60,0x03,0x61,0xec,
    0x01,0x2d,0x05,0x00,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x2a,0x70,
    0x74,0x06,0xbb,0xd6,0x02,0x4e,0x0f,0x00,
    0xdc,0xc0,0xbc,0xc9,0x20,0xb7,0xb6,0xb8,
    0xbc,0xc0,0x20,0xba,0xc9,0xb9,0xde,0xb0,
    0xd1,0x20,0xb2,0xb6,0xb6,0xde,0xc3,0xde,
    0xbd,0xb6,0x3f,0x2a,0x3e,0x19,0x04,0x5c,
    0x5b,0x28,0x76,0x0c,0x00,0x50,0x4c,0x45,
    0x41,0x53,0x45,0x20,0x49,0x4e,0x53,0x45,
    0x52,0x54,0x20,0x36,0x38,0x30,0x39,0x20,
    0x43,0x41,0x52,0x44,0x2a,0xf9,0x41,0x07,
    0xac,0xe5,0x21,0x83,0x10,0x00,0x32,0x44,
    0x38,0x38,0x20,0x35,0x31,0x20,0x36,0x32,
    0x20,0x30,0x45,0x20,0x46,0x44,0x20,0x33,
    0x39,0x20,0x30,0x33,0x20,0x43,0x42,0x20,
    0x32,0x36,0x2a,0x44,0x3d,0x09,0x9a,0x57,
    0x32,0x71,0x0f,0x00,0xdc,0xc0,0xbc,0xca,
    0x20,0xb6,0xd6,0xb3,0xb7,0xae,0xb8,0xb6,
    0xde,0x20,0xc0,0xde,0xb2,0xb7,0xd7,0xb2,
    0x2a,0x63,0x95,0x07,0x6a,0xb1,0x21,0x44,
    0x0e,0x00,0x33,0xd2,0xdd,0xb6,0xde,0x20,
    0xb8,0xd8,0xb1,0x20,0xc3,0xde,0xb7,0xc5,
    0xb2,0xd6,0xb0,0x21,0x2a,0x01,0xf7,0x04,
    0x54,0x94,0x22,0xe5,0x08,0x00,0xbf,0xba,
    0xc9,0xb6,0xc9,0xbc,0xde,0xae,0x20,0xc2,
    0xb3,0xd8,0xdd,0xb8,0xde,0x20,0xb2,0xba,
    0xb3,0xd6,0xb0,0x2a,0x08,0x3a,0x08,0x94,
    0xcb,0x2a,0x78,0x13,0x00,0xca,0xde,0xb2,
    0xb8,0xa6,0x20,0xb1,0xdd,0xb6,0xc3,0xde,
    0x20,0xd5,0xbd,0xde,0xaf,0xc3,0x20,0xb8,
    0xc0,0xde,0xbb,0xb2,0x2a,0x4b,0x70,0x05,
    0x4d,0x96,0x3b,0x36,0x09,0x00,0xc0,0xaf,
    0xb7,0xad,0xb3,0x20,0xbc,0xd6,0xb3,0xd6,
    0xb0,0x2a,0xd4,0x77,0x08,0x94,0xcd,0x1b,
    0x6a,0x10,0x00,0xb4,0xdd,0xc1,0xac,0xdd,
    0x20,0xd0,0xc3,0xd9,0xb0,0xaf,0x2a,0xfa,
    0x68,0x08,0x8a,0x3a,0x1f,0x8d,0x12,0x00,
    0xca,0xb2,0x2e,0xc5,0xd1,0xba,0xc3,0xde,
    0xbd,0x2a,0x3b,0xea,0x05,0x6c,0xd2,0x0a,
    0xd8,0x08,0x00,0xb9,0xde,0xb0,0xd1,0xbe,
    0xdd,0xc0,0xb0,0xc3,0xde,0xd3,0x20,0xb1,
    0xbf,0xdd,0xc3,0xde,0xb8,0xc0,0xde,0xbb,
    0xb2,0xc8,0x2a,0x1c,0x6f,0x06,0x96,0x41,
    0x20,0x16,0x0c,0x00,0xc5,0xd1,0xba,0xaf,
    0xc4,0x20,0xbc,0xde,0xae,0xb3,0xce,0xb3,
    0xb7,0xae,0xb8,0x20,0xb2,0xc2,0xd3,0xb7,
    0xb2,0xc3,0xb8,0xda,0xc3,0xd9,0xb6,0xc5,
    0x3f,0x2a,0xf2,0x4a,0x09,0x0a,0xa2,0x12,
    0x76,0x10,0x00,0xce,0xde,0xb8,0xca,0x20,
    0xc3,0xde,0xa8,0xcc,0xaa,0xdd,0xc0,0xde,
    0xb0,0xb6,0xde,0x20,0xbd,0xb7,0xc5,0xdd,
    0xc0,0xde,0x2a,0x43,0x4c,0x05,0x3c,0x35,
    0x0a,0x93,0x08,0x00,0xb4,0xcd,0x20,0xb4,
    0xcd,0xaf,0x2a,0x45,0xd7,0x08,0x9f,0xad,
    0x38,0x92,0x0b,0x00,0xd7,0xcb,0xdf,0xad,
    0xc0,0xd3,0x20,0xca,0xb2,0xaf,0xc3,0xb2,
    0xcf,0xbd,0x20,0xbb,0xb6,0xde,0xbc,0xc3,
    0xb8,0xc0,0xde,0xbb,0xb2,0x2a,0x04,0x49,
    0x06,0x87,0x39,0x33,0xc4,0x0a,0x00,0xce,
    0xdd,0xc4,0xb3,0xc6,0x20,0xb1,0xd9,0xc4,
    0xb5,0xd3,0xaf,0xc0,0x3f,0x20,0xb1,0xdd,
    0xc0,0xd3,0x20,0xcb,0xcf,0xc0,0xde,0xc8,
    0xb0,0x2a,0x29,0x62,0x05,0x89,0xbd,0x35,
    0x64,0x0e,0x00,0x42,0x52,0x45,0x41,0x4b,
    0x20,0x41,0x57,0x41,0x59,0x20,0x46,0x52,
    0x4f,0x4d,0x20,0x54,0x48,0x45,0x20,0x50,
    0x41,0x53,0x54,0x2a,0xf8,0x7d,0x04,0x5b,
    0x4b,0x08,0xa9,0x09,0x00,0x36,0x38,0x30,
    0x39,0xc5,0xdd,0xb6,0xc6,0x20,0xcf,0xb9,
    0xcf,0xbe,0xdd,0xd6,0x2a,0x00,0x08,0x03,
    0x22,0x88,0x20,0x22,0x03,0x00,0xce,0xde,
    0xb8,0xca,0x20,0x46,0x43,0xc9,0x47,0x41,
    0x4c,0x41,0x58,0x49,0x41,0x4e,0xd3,0x20,
    0xc2,0xb8,0xd8,0xcf,0xbc,0xc0,0xa1,0x2a,
    0x00,0x51,0x03,0x3a,0xe9,0x08,0x23,0x07,
    0x00,0xce,0xde,0xb8,0xc9,0x20,0xc2,0xb8,
    0xd9,0x20,0xc4,0xde,0xd7,0xba,0xde,0xdd,
    0xbd,0xcb,0xdf,0xd8,0xaf,0xc4,0x20,0xb6,
    0xaf,0xc3,0xc8,0x21,0x2a,0x5d,0x50,0x0c,
    0xfd,0xb4,0x13,0x63,0x17,0x00,0x2a,0x04,
    0x76,0x10,0x32,0xd1,0x21,0x27,0x1b,0x00,
    0x2a,0x00,0x00,0x00,0x00,0x00
};

static const size_t g_blob_size = sizeof(g_blob) / sizeof(g_blob[0]);

// password alphabhet (internal code numbers)
static const uint8_t g_alphabet_code[ALPHABET_SIZE] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, // AHOV16
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, // BIPW27
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, // CJQX38
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, // DKRY49
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, // ELSZ50
    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, // FMT-な!
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35  // GNU.むこ
};

static const wchar_t g_alphabet_ch[ALPHABET_SIZE] = {
    L'A',L'H',L'O',L'V',L'1',L'6',
    L'B',L'I',L'P',L'W',L'2',L'7',
    L'C',L'J',L'Q',L'X',L'3',L'8',
    L'D',L'K',L'R',L'Y',L'4',L'9',
    L'E',L'L',L'S',L'Z',L'5',L'0',
    L'F',L'M',L'T',L'-',L'な',L'!',
    L'G',L'N',L'U',L'.',L'む',L'こ'
};

// list of known passwords taken from:
//  * https://tcrf.net/Youkai_Douchuki_(TurboGrafx-16)
//  * https://ameblo.jp/gzzms1126/entry-12432089291.html
//  * https://www.youtube.com/watch?v=xoe9yWfjTzU
static const wchar_t *g_known_passwords[] = {
    L"HARUHISA.UDAGAWA",
    L"PC-ENGINE",
    L"NAMCO",
    L"NAMCOT",
    L"6502",
    L"6809",
    L"68000",
    L"756-2311",
    L"YAMASHITA",
    L"AKIRA",
    L"KOMAI",
    L"KAZUHIKO",
    L"KAWADA",
    L"HAL",
    L"NAUSICAA",
    L"LAPUTA",
    L"MICHIYO",    
    L"MONITOR",    
    L"YAGI",    
    L"YUKIHIKO",    
    L"NAGAMATSU",
    L"UDADAGAWA",    
    L"818-6104",    
    L"HENTAIOSUGI",    
    L"KUMI.HANAOKA",
    L"NEC",
    L"KID",
    L"MIZUNO",
    L"なむこむな!756-2311",
    L"SPEED-UP",            // found this one after running the above code for 16 hours... Unfortunately it's been known for years :S
    L"KOBAYASHI",
    L"OHAYOUKAWADA",
    L"S.62.08.22"
};

static const size_t g_known_passwords_count = sizeof(g_known_passwords) / sizeof(g_known_passwords[0]);

// PC Engine "core"

// status register
typedef struct {
    uint8_t n : 1;
    uint8_t v : 1;
    uint8_t t : 1;
    uint8_t d : 1;
    uint8_t i : 1;
    uint8_t z : 1;
    uint8_t c : 1;
} status_t;

// PC Engine "core"
typedef struct {
    uint8_t mem[0x2000];
    uint8_t a, x, y;
    uint8_t sp;
    status_t st;
} core_t;

#define RAM_START 0x2000
#define RAM_STACK_START 0x0100
#define RAM_STACK_END 0x0200

#define PASSWORD_STRING_ADDR 0x31DC
#define PASSWORD_LENGTH_ADDR 0x31F6
#define PASSWORD_HASH_ADDR 0x31F4

void core_reset(core_t *core) {
    memset(core, 0, sizeof(core_t));
    core->sp = 0xff;
}

void core_clear_registers(core_t *core) {
    core->a = core->x = core->y = 0;
    memset(&core->st, 0, sizeof(status_t));
    core->sp = 0xff;
}

typedef struct {
    off_t offset;   // offset of the password in the raw password blob
    uint8_t len;    // password length
} password_info_t;

// go through the blob and extract password informations
void password_extract(const uint8_t *data, size_t size, password_info_t **out, size_t *count) {
    const uint8_t *end = data + size;
    password_info_t *infos;
    size_t n = 0;

    size_t capacity = 32;
    infos = (password_info_t*)malloc(capacity * sizeof(password_info_t));

    for(const uint8_t *ptr = data; ptr < end; ++ptr, ++n) {
        if(ptr[2] == 0) {
            break;
        } else {
            if(n >= capacity) {
                capacity *= 2;
                infos = (password_info_t*)realloc(infos, capacity * sizeof(password_info_t));
            }
            password_info_t *last = infos + n;
            last->offset = ptr - data;
            last->len = ptr[2];
            ptr+=9;
            for(; (ptr < end) && (*ptr != 0x2a); ++ptr) {
            }
        }
    }

    *out = infos;
    *count = n;
}

// convert string to internal encoding
void string_to_code(const wchar_t *in, uint8_t *out, uint8_t *len) {
    *len = 0;
    for(size_t i = 0; in[i] != L'\0'; ++i, *len+=1) {
        size_t j;
        for(j = 0; (j < 42) && (g_alphabet_ch[j] != in[i]) ; ++j) {
        }
        if(j < 42) {
            out[i] = g_alphabet_code[j];
        } else {
            // should never happen but who knows?
            fwprintf(stderr, L"Unknown character %lc in %ls\n", in[i], in);
        }
    }
}

// returns a pointer to the core RAM.
// the address should be a logical one within the RAM address range.
uint8_t* mem_addr(core_t *core, uint16_t addr) { return &core->mem[addr-RAM_START]; }

// PC Engine instructions

void in_(core_t *core, uint8_t *ptr) {
    *ptr += 1;
    core->st.n = (*ptr & 0x80) ? 1 : 0;
    core->st.z = (*ptr == 0) ? 1 : 0;
}
void ina(core_t *core) { in_(core, &core->a); }
void inx(core_t *core) { in_(core, &core->x); }
void iny(core_t *core) { in_(core, &core->y); }
void inc(core_t *core, uint16_t addr) { in_(core, mem_addr(core, addr)); }

void eor_(core_t *core, uint8_t val) {
    core->a ^= val;
    core->st.n = (core->a & 0x80) ? 1 : 0;
    core->st.z = (core->a == 0) ? 1 : 0;
}
void eori(core_t *core, uint8_t val) { eor_(core, val); }
void eor(core_t *core, uint16_t addr) { eor_(core, *mem_addr(core, addr)); }

void and_(core_t *core, uint8_t val) {
    core->a &= val;
    core->st.n = (core->a & 0x80) ? 1 : 0;
    core->st.z = (core->a == 0) ? 1 : 0;
}
void andi(core_t *core, uint8_t val) { and_(core, val); }
void and(core_t *core, uint16_t addr) { and_(core, *mem_addr(core, addr)); }

void adc_(core_t *core, uint8_t val) {
    uint16_t r = core->a + val + core->st.c;
    core->a = (uint8_t)r;
    core->st.c = (r >= 256) ? 1 : 0;
    core->st.n = (r & 0x80) ? 1 : 0;
    core->st.z = (core->a == 0) ? 1 : 0;
}
void adci(core_t *core, uint8_t val) { adc_(core, val); }
void adc(core_t *core, uint16_t addr) { adc_(core, *mem_addr(core, addr)); }

void cmp_(core_t *core, uint8_t *ptr, uint8_t val) {
    core->st.z = (*ptr == val) ? 1 : 0;
    core->st.c = (*ptr >= val) ? 1 : 0;
    core->st.n = ((*ptr - val) & 0x80) ? 1 : 0;
}

void cmpi(core_t *core, uint8_t val) { cmp_(core, &core->a, val); }
void cpix(core_t *core, uint8_t val) { cmp_(core, &core->x, val); }
void cpiy(core_t *core, uint8_t val) { cmp_(core, &core->y, val); }
void cmp(core_t *core, uint16_t addr) { cmp_(core, &core->a, *mem_addr(core, addr)); }
void cpx(core_t *core, uint16_t addr) { cmp_(core, &core->x, *mem_addr(core, addr)); }
void cpy(core_t *core, uint16_t addr) { cmp_(core, &core->y, *mem_addr(core, addr)); }

void cl_(core_t *core, uint8_t *reg) {
    *reg = 0;
}
void cla(core_t *core) { cl_(core, &core->a); }
void clx(core_t *core) { cl_(core, &core->x); }
void cly(core_t *core) { cl_(core, &core->y); }

void ld_(core_t *core, uint8_t val, uint8_t *dst) {
    *dst = val;
    core->st.n = (*dst & 0x80) ? 1 : 0;
    core->st.z = (*dst == 0) ? 1 : 0;
}
void ldia(core_t *core, uint8_t val) { ld_(core, val, &core->a); }
void ldix(core_t *core, uint8_t val) { ld_(core, val, &core->x); }
void ldiy(core_t *core, uint8_t val) { ld_(core, val, &core->y); }
void lda(core_t *core, uint16_t addr) { ld_(core, *mem_addr(core, addr), &core->a); }
void ldx(core_t *core, uint16_t addr) { ld_(core, *mem_addr(core, addr), &core->x); }
void ldy(core_t *core, uint16_t addr) { ld_(core, *mem_addr(core, addr), &core->y); }

void st_(core_t *core, uint8_t *src, uint16_t dst) {
    uint8_t *ptr = mem_addr(core, dst);
    *ptr = *src;
}
void sta(core_t *core, uint16_t addr) { st_(core, &core->a, addr); }
void stx(core_t *core, uint16_t addr) { st_(core, &core->x, addr); }
void sty(core_t *core, uint16_t addr) { st_(core, &core->y, addr); }

void ph_(core_t *core, uint8_t *src) {
    core->mem[RAM_STACK_START+core->sp] = *src;
    --core->sp;
}
void pha(core_t *core) { ph_(core, &core->a); }
void phx(core_t *core) { ph_(core, &core->x); }
void phy(core_t *core) { ph_(core, &core->y); }

void pl_(core_t *core, uint8_t *addr) {
    ++core->sp;
    *addr = core->mem[RAM_STACK_START+core->sp];
    core->st.n = (*addr & 0x80) ? 1 : 0;
    core->st.z = (*addr == 0) ? 1 : 0;
}
void pla(core_t *core) { pl_(core, &core->a); }
void plx(core_t *core) { pl_(core, &core->x); }
void ply(core_t *core) { pl_(core, &core->y); }

void dec(core_t *core, uint8_t *addr) {
    *addr -= 1;
    core->st.n = (*addr & 0x80) ? 1 : 0;   
    core->st.z = (*addr == 0) ? 1 : 0;
}
void dea(core_t *core) { dec(core, &core->a); }
void dex(core_t *core) { dec(core, &core->x); }
void dey(core_t *core) { dec(core, &core->y); }

void asla(core_t *core) {
    core->st.c = (core->a & 0x80) ? 1 : 0;
    core->a <<= 1;
    core->st.n = (core->a & 0x80) ? 1 : 0;
    core->st.z = (core->a == 0) ? 1 : 0;
}

void rol_(core_t *core, uint8_t *ptr) {
    uint8_t tmp = core->st.c ? 0x01 : 0x00;
    core->st.c = (*ptr & 0x80) ? 1 : 0;
    core->a = (*ptr << 1) | tmp;
    core->st.n = (*ptr & 0x80) ? 1 : 0;
    core->st.z = (*ptr == 0) ? 1 : 0;
}
void rola(core_t *core) { rol_(core, &core->a); }
void rol(core_t *core, uint16_t addr) { rol_(core, mem_addr(core, addr)); }

void ror_(core_t *core, uint8_t *ptr) {
    uint8_t tmp = core->st.c ? 0x80 : 0x00;
    core->st.c = (*ptr & 0x01) ? 1 : 0;
    *ptr = (*ptr >> 1) | tmp;
    core->st.n = (*ptr & 0x80) ? 1 : 0;
    core->st.z = (*ptr == 0) ? 1 : 0;
}
void rora(core_t *core) { ror_(core, &core->a); }
void ror(core_t *core, uint16_t addr) { ror_(core, mem_addr(core, addr)); }

// password.encode routines and subroutines

void ld8bd(core_t *core) {
    pha(core);
    for(core->y = 0x08; core->y!=0; core->y--) {
        asla(core);
        pha(core);
        ror(core, 0x31F4);
        ror(core, 0x31F5);
        ldia(core, 0x00);
        adci(core, 0xFF);
        eori(core, 0xFF);
        pha(core);
        andi(core, 0x84);
        eor(core, 0x31F4);
        sta(core, 0x31F4);
        pla(core);
        andi(core, 0x08);
        eor(core, 0x31F5);
        sta(core, 0x31F5);
        pla(core);
    }
    pla(core);
}

void ld8a4(core_t *core) {
    pha(core);
    pha(core);
    lda(core, 0x31F4);
    cmpi(core, 0xe5);
    pla(core);
    adc(core, 0x31F7);
    sta(core, 0x31F7);
    lda(core, 0x31f8);
    adc(core, 0x31F5);
    sta(core, 0x31F8);
    pla(core);
}

void ld89b(core_t *core) {
    pha(core);
    eor(core, 0x31F9);
    sta(core, 0x31F9);
    pla(core);
}

void ld88f(core_t *core) {
    pha(core);
    ror(core, 0x31FA);
    adc(core, 0x31FA);
    sta(core, 0x31FA);
    pla(core);
}

void ld87f(core_t *core) {
    pha(core);
    do {
        rola(core);
        pha(core);
        lda(core, 0x31FB);
        adci(core, 0x00); 
        sta(core, 0x31FB);
        pla(core);
    } while(!core->st.z);
    pla(core);
}

void password_encode(core_t *core) {
    const uint8_t *str = mem_addr(core, PASSWORD_STRING_ADDR);
    const uint8_t len = *mem_addr(core, PASSWORD_LENGTH_ADDR);
    clx(core);
    stx(core, 0x31F4);
    stx(core, 0x31F5);
    stx(core, 0x31F7);
    stx(core, 0x31F8);
    stx(core, 0x31F9);
    stx(core, 0x31FB);
    ldia(core, 0x01);
    sta(core, 0x31FA); 
    do {
        ldia(core, str[core->x]);
        ld8bd(core);
        ld8a4(core);
        ld89b(core);
        ld88f(core);
        ld87f(core);
        inx(core);
    } while(core->x < len);
}

// bruteforce password search 

#define JOB_COUNT 16

typedef struct {
    uint8_t id;
    const password_info_t *info;
} payload_t;

static uint8_t g_matches[JOB_COUNT][MAX_STRING_SIZE];
static atomic_bool g_search = true;

void* job_routine(void *arg) {
    static const float delta = (float)ALPHABET_SIZE / (float)JOB_COUNT;
    payload_t *payload = (payload_t*)arg;
    const password_info_t *password_info = payload->info;

    uint8_t input[MAX_STRING_SIZE] = {0};
    uint8_t *str;
    uint8_t *len;
    uint8_t *hash;

    float u = delta * (float)payload->id;
    uint8_t start = (uint8_t)floor(u);
    uint8_t end = ((uint8_t)floor(u+delta)) % ALPHABET_SIZE;

    core_t core = {0};    
    uint8_t *match = NULL;

    input[0] = start;
    while((input[0] != end) && (g_search)) {
        core_reset(&core);
        str = mem_addr(&core, PASSWORD_STRING_ADDR);
        len = mem_addr(&core, PASSWORD_LENGTH_ADDR);
        hash = mem_addr(&core, PASSWORD_HASH_ADDR);

        for(unsigned int j=0; j<password_info->len; j++) {
            str[j] = g_alphabet_code[input[j]];
        }

        *len = password_info->len;
    
        password_encode(&core);
        if(memcmp(hash, g_blob+password_info->offset, 8) == 0) {
            g_search = false;
            match = g_matches[payload->id];
            memcpy(match, input, password_info->len);
        } else {
            uint8_t i = password_info->len;
            do {
                --i;
                input[i] = (input[i] + 1) % ALPHABET_SIZE;
            } while((i>0) && (input[i] == 0));
        }
    }
    return match;
}

int main() {
    core_t core = {0};

    password_info_t *password_infos = NULL;
    size_t password_count = 0;
    
    password_extract(g_blob, g_blob_size, &password_infos, &password_count); 

    // remove known passwords
    for(size_t j=0; j<g_known_passwords_count; j++) {
        uint8_t *str = mem_addr(&core, PASSWORD_STRING_ADDR);
        uint8_t *len = mem_addr(&core, PASSWORD_LENGTH_ADDR);
        uint8_t *hash = mem_addr(&core, PASSWORD_HASH_ADDR);

        core_reset(&core);

        string_to_code(g_known_passwords[j], str, len);

        bool found = false;
        size_t i;
        for(i=0; i<password_count; i++) {
            if(password_infos[i].len == *len) {
                core_clear_registers(&core);
                password_encode(&core);
                if(memcmp(hash, g_blob+password_infos[i].offset, 8) == 0) {
                    found = true;
                    break;
                }
            }
        }
        if(found) {
            wprintf(L"%zd \"%ls\" - ", password_infos[i].offset, g_known_passwords[j]);
            for(unsigned int k=0; k<password_infos[i].len; k++) {
                wprintf(L"%02x ", str[k]);
            }
            wprintf(L"- ");
            for(size_t k=0; k<8; k++) {
                wprintf(L"%02x ", hash[k]);
            }
            fputwc(L'\n', stdout);

            if(i < password_count) {
                --password_count;
                memmove(password_infos+i, password_infos+password_count, sizeof(password_info_t));
            }
        } else {
            wprintf(L"%ls not found\n", g_known_passwords[j]);
        }
    }

    // display remaining entries infos
    wprintf(L"remaining entries:\n");
    for(unsigned int i=0; i<password_count; i++) {
        wprintf(L"%zd %d\n", password_infos[i].offset, password_infos[i].len);
    }

    fflush(stdout);

    // try to find them
    pthread_t jobs[JOB_COUNT];
    payload_t payload[JOB_COUNT];
    for(unsigned int j=0; j<password_count; j++) {
        g_search = true;
        for(unsigned int i=0; i<JOB_COUNT; i++) {
            payload[i].id = i;
            payload[i].info = password_infos + j;
            pthread_create(jobs+i, NULL, job_routine, payload+i);
        }

        for(unsigned int i=0; i<JOB_COUNT; i++) {
            uint8_t *match;
            pthread_join(jobs[i], (void**)&match);
            if(match != NULL) {
                wprintf(L"%zd ",password_infos[j].offset);
                for(unsigned int k=0; k<password_infos[j].len; k++) {
                    fputwc(g_alphabet_ch[match[k]], stdout);
                }
                wprintf(L" - ");
                for(unsigned int k=0; k<password_infos[j].len; k++) {
                    wprintf(L"%02x ", match[k]);
                }
                wprintf(L"- ");
                for(size_t k=0; k<8; k++) {
                    wprintf(L"%02x ", g_blob[password_infos[j].offset+k]);
                }
                fputwc(L'\n', stdout);
            }
        }
    }
    free(password_infos);

    return EXIT_SUCCESS;
}
