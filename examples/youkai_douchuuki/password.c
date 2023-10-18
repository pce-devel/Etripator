#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>
#include <stdbool.h>
uint8_t mem[0x2000];
uint8_t a, x, y;
uint8_t sp;

struct {
    uint8_t n : 1;
    uint8_t v : 1;
    uint8_t t : 1;
    uint8_t d : 1;
    uint8_t i : 1;
    uint8_t z : 1;
    uint8_t c : 1;
} st = {0};

uint8_t *password_str = mem + 0x31DC - 0x2000;
uint8_t *password_len = mem + 0x31F6 - 0x2000;
uint8_t *password_encoded = mem + 0x31F4 - 0x2000;

uint8_t blob[] = {
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

typedef struct {
    off_t offset;
    uint8_t len;
} password_info_t;

password_info_t *password_infos = NULL;
size_t password_count = 0;

void password_extract();
void password_encode();

uint8_t alphabet_code[42] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, // AHOV16
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, // BIPW27
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, // CJQX38
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, // DKRY49
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, // ELSZ50
    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, // FMT-な!
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35  // GNU.むこ
};

wchar_t alphabet_ch[42] = {
    L'A',L'H',L'O',L'V',L'1',L'6',
    L'B',L'I',L'P',L'W',L'2',L'7',
    L'C',L'J',L'Q',L'X',L'3',L'8',
    L'D',L'K',L'R',L'Y',L'4',L'9',
    L'E',L'L',L'S',L'Z',L'5',L'0',
    L'F',L'M',L'T',L'-',L'な',L'!',
    L'G',L'N',L'U',L'.',L'む',L'こ'
};

// taken from https://tcrf.net/Youkai_Douchuki_(TurboGrafx-16)
const wchar_t *known_passwords[] = {
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
    L"なむこむな!756-2311",
    L"KUMI.HANAOKA",
    L"HARUHISA.UDAGAWA",
    L"NEC",
    L"KID",
    L"MIZUNO"
};

const size_t known_passwords_count = sizeof(known_passwords) / sizeof(known_passwords[0]);

void wchar_to_code(const wchar_t *in) {
    *password_len = 0;
    for(size_t i = 0; in[i] != L'\0'; ++i, *password_len+=1) {
        size_t j;
        for(j = 0; (j < 42) && (alphabet_ch[j] != in[i]) ; ++j) {
        }
        if(j < 42) {
            password_str[i] = alphabet_code[j];
        } else {
            // should never happen but who knows?
            fwprintf(stderr, L"Unknown character %lc in %ls\n", in[i], in);
        }
    }
}

int main() {
    uint8_t str[256];
    uint8_t code[8];
    sp = 0xFF;

    password_extract();

    printf("password count %zu\n", password_count);

    // remove known passwords
    for(size_t j=0; j<known_passwords_count; j++) {
        wchar_to_code(known_passwords[j]);
        bool found = false;
        size_t i;
        for(i=0; i<password_count; i++) {
            if(password_infos[i].len == *password_len) {
                password_encode();
                if(memcmp(password_encoded, blob+password_infos[i].offset, 8) == 0) {
                    found = true;
                    break;
                }
            }
        }
        if(found) {
            wprintf(L"%lld \"%ls\" ", password_infos[i].offset, known_passwords[j]);
            for(size_t k=0; k<8; k++) {
                printf("%02x ", password_encoded[k]);
            }
            fputc('\n', stdout);

            if(i < password_count) {
                --password_count;
                memmove(&password_infos[i], &password_infos[password_count], sizeof(password_info_t));
            }
        } else {
            wprintf(L"%ls not found\n", known_passwords[j]);
        }
    }

    printf("remaining entries:\n");
    for(size_t i=0; i<password_count; i++) {
        printf("%lld %d\n", password_infos[i].offset, password_infos[i].len);
    }

    for(size_t i=0; i<password_count; i++) {
        *password_len = password_infos[i].len;
        
        memset(code, 0, 8);
        memset(password_str, 0, *password_len);
        memset(str, 0, *password_len);
        
        size_t attempt = 0;
        uint8_t *last = str + (*password_len - 1);
        bool found = false;
        do {
            for(size_t j=0; j<*password_len; j++) {
                str[j] = (str[j]+1) % 42;
                if(str[j]) {
                    break;
                }
            }
            for(size_t j=0; j<*password_len; j++) {
                password_str[j] = alphabet_code[str[j]];
            }
            memset(&st, 0, sizeof(st));
            password_encode();
            if(memcmp(password_encoded, blob+password_infos[i].offset, 8) == 0) {
                found = true;
                break;
            }
            attempt++;
        } while(*last != 41);
    
        if(found) {
            printf("%zu \"", i);
            for(size_t j=0; j<*password_len; j++) {
                fputwc(alphabet_ch[str[j]], stdout);
            }
            printf("\" %zu ", attempt);
            for(size_t j=0; j<8; j++) {
                printf("%02x ", password_encoded[j]);
            }
        }
        fputc('\n', stdout);
    }
    return EXIT_SUCCESS;
}

void in_(uint8_t *ptr) {
    *ptr += 1;
    st.t = 0;
    st.n = (*ptr & 0x80) ? 1 : 0;
    st.z = (*ptr == 0) ? 1 : 0;
}
void ina() { in_(&a); }
void inx() { in_(&x); }
void iny() { in_(&y); }
void inc(uint16_t addr) { in_(&mem[addr-0x2000]); }

void eor_(uint8_t val) {
    a ^= val;
    st.t = 0;
    st.n = (a & 0x80) ? 1 : 0;
    st.z = (a == 0) ? 1 : 0;
}
void eori(uint8_t val) { eor_(val); }
void eor(uint16_t addr) { eor_(mem[addr-0x2000]); }

void and_(uint8_t val) {
    a &= val;
    st.t = 0;
    st.n = (a & 0x80) ? 1 : 0;
    st.z = (a == 0) ? 1 : 0;
}
void andi(uint8_t val) { and_(val); }
void and(uint16_t addr) { and_(mem[addr-0x2000]); }

void adc_(uint8_t val) {
    uint16_t r = a + val + st.c;
    a = (uint8_t)r;
    st.t = 0;
    st.c = (r >= 256) ? 1 : 0;
    st.n = (r & 0x80) ? 1 : 0;
    st.z = (a == 0) ? 1 : 0;
}
void adci(uint8_t val) { adc_(val); }
void adc(uint16_t addr) { adc_(mem[addr-0x2000]); }

void cmp_(uint8_t *ptr, uint8_t val) {
    st.t = 0;
    st.z = (*ptr == val) ? 1 : 0;
    st.c = (*ptr >= val) ? 1 : 0;
    st.n = ((*ptr - val) & 0x80) ? 1 : 0;
}

void cmpi(uint8_t val) { cmp_(&a, val); }
void cpix(uint8_t val) { cmp_(&x, val); }
void cpiy(uint8_t val) { cmp_(&y, val); }
void cmp(uint16_t addr) { cmp_(&a, mem[addr-0x2000]); }
void cpx(uint16_t addr) { cmp_(&x, mem[addr-0x2000]); }
void cpy(uint16_t addr) { cmp_(&y, mem[addr-0x2000]); }

void cl_(uint8_t *reg) {
    *reg = 0;
    st.t = 0;
}
void cla() { cl_(&a); }
void clx() { cl_(&x); }
void cly() { cl_(&y); }

void ld_(uint8_t val, uint8_t *dst) {
    *dst = val;
    st.t = 0;
    st.n = (*dst & 0x80) ? 1 : 0;
    st.z = (*dst == 0) ? 1 : 0;
}
void ldia(uint8_t val) { ld_(val, &a); }
void ldix(uint8_t val) { ld_(val, &x); }
void ldiy(uint8_t val) { ld_(val, &y); }
void lda(uint16_t addr) { ld_(mem[addr-0x2000], &a); }
void ldx(uint16_t addr) { ld_(mem[addr-0x2000], &x); }
void ldy(uint16_t addr) { ld_(mem[addr-0x2000], &y); }

void st_(uint8_t *src, uint16_t dst) {
    mem[dst-0x2000] = *src;
    st.t = 0;
}
void sta(uint16_t addr) { st_(&a, addr); }
void stx(uint16_t addr) { st_(&x, addr); }
void sty(uint16_t addr) { st_(&y, addr); }

void ph_(uint8_t *src) {
    mem[0x100+sp] = *src;
    --sp;
    st.t = 0;
}
void pha() { ph_(&a); }
void phx() { ph_(&x); }
void phy() { ph_(&y); }

void pl_(uint8_t *addr) {
    ++sp;
    *addr = mem[0x100+sp];
    st.t = 0;
    st.n = (*addr & 0x80) ? 1 : 0;
    st.z = (*addr == 0) ? 1 : 0;
}
void pla() { pl_(&a); }
void plx() { pl_(&x); }
void ply() { pl_(&y); }

void dec(uint8_t *addr) {
    *addr -= 1;
    st.t = 0;
    st.n = (*addr & 0x80) ? 1 : 0;   
    st.z = (*addr == 0) ? 1 : 0;
}
void dea() { dec(&a); }
void dex() { dec(&x); }
void dey() { dec(&y); }

void asla() {
    st.c = (a & 0x80) ? 1 : 0;
    a <<= 1;
    st.n = (a & 0x80) ? 1 : 0;
    st.z = (a == 0) ? 1 : 0;
    st.t = 0;
}

void rol_(uint8_t *ptr) {
    uint8_t tmp = st.c ? 0x01 : 0x00;
    st.c = (*ptr & 0x80) ? 1 : 0;
    a = (*ptr << 1) | tmp;
    st.n = (*ptr & 0x80) ? 1 : 0;
    st.z = (*ptr == 0) ? 1 : 0;
    st.t = 0;
}
void rola() { rol_(&a); }
void rol(uint16_t addr) { rol_(&mem[addr-0x2000]); }

void ror_(uint8_t *ptr) {
    uint8_t tmp = st.c ? 0x80 : 0x00;
    st.c = (*ptr & 0x01) ? 1 : 0;
    *ptr = (*ptr >> 1) | tmp;
    st.n = (*ptr & 0x80) ? 1 : 0;
    st.z = (*ptr == 0) ? 1 : 0;
    st.t = 0;
}
void rora() { ror_(&a); }
void ror(uint16_t addr) { ror_(&mem[addr-0x2000]); }

void ld8bd() {
    pha();
    for(y = 0x08; y!=0; y--) {
        asla();
        pha();
        ror(0x31F4);
        ror(0x31F5);
        ldia(0x00);
        adci(0xFF);
        eori(0xFF);
        pha();
        andi(0x84);
        eor(0x31F4);
        sta(0x31F4);
        pla();
        andi(0x08);
        eor(0x31F5);
        sta(0x31F5);
        pla();
    }
    pla();
}

void ld8a4() {
    pha();
    pha();
    lda(0x31F4);
    cmpi(0xe5);
    pla();
    adc(0x31F7);
    sta(0x31F7);
    lda(0x31f8);
    adc(0x31F5);
    sta(0x31F8);
    pla();
}

void ld89b() {
    pha();
    eor(0x31F9);
    sta(0x31F9);
    pla();
}

void ld88f() {
    pha();
    ror(0x31FA);
    adc(0x31FA);
    sta(0x31FA);
    pla();
}

void ld87f() {
    pha();
    do {
        rola();
        pha();
        lda(0x31FB);
        adci(0x00); 
        sta(0x31FB);
        pla();
    } while(!st.z);
    pla();
}

void password_encode() {
    clx();
    stx(0x31F4);
    stx(0x31F5);
    stx(0x31F7);
    stx(0x31F8);
    stx(0x31F9);
    stx(0x31FB);
    ldia(0x01);
    sta(0x31FA); 
    do {
        ldia(password_str[x]);
        ld8bd();
        ld8a4();
        ld89b();
        ld88f();
        ld87f();
        inx();
    } while(x < *password_len);
}

void password_extract() {
    const size_t total_size = sizeof(blob)/sizeof(blob[0]);
    const uint8_t *end = blob + total_size;

    size_t capacity = 32;
    password_infos = (password_info_t*)realloc(password_infos, capacity * sizeof(password_info_t));

    for(uint8_t *ptr = blob; ptr < end; ++ptr, ++password_count) {
        if(ptr[2] == 0) {
            break;
        } else {
            if(password_count > capacity) {
                capacity *= 2;
                password_infos = (password_info_t*)realloc(password_infos, capacity * sizeof(password_info_t));
            }
            password_info_t *infos = password_infos + password_count;
            infos->offset = ptr - blob;
            infos->len = ptr[2];
            ptr+=9;
            for(; (ptr < end) && (*ptr != 0x2a); ++ptr) {
            }
        }
    }
}