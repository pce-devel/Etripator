/*
¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

  __/¯\____ ___/\__   _/\__   _/\_   _/\__   _/\___ ___/\__   __/\_   _/\__   
  \_  ____/_> ____ \_/  _  \_ \  <  /_    \_/     _>> ____ \_ >    \_/  _  \_ 
  _> ___/ ¯>__> <<__// __  _/ |>  ></ _/>  </  ¯  \\__> <<__//  /\  // __  _/ 
 _>  \7   <__/:. \__/:. \>  \_/   L/  _____/.  7> .\_/:. \__/  <_/ </:. \>  \_ 
|:::::::::::::::::::::::/:::::::::::::>::::::::/::::::::::::::::::::::::/:::::|
|¯¯\::::/\:/¯\::::/¯¯¯¯<::::/\::/¯¯\:/¯¯¯¯¯¯\::\::/¯¯\::::/¯¯\::::/¯¯¯¯<::::/¯|
|__ |¯¯|  T _ |¯¯¯| ___ |¯¯|  |¯| _ T ______ |¯¯¯¯| _ |¯¯¯| _ |¯¯¯| ___ |¯¯| _|
   \|__|/\|/ \|___|/   \|__|/\|_|/ \|/      \|    |/ \|___|/ \|___|/dNo\|__|/  

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

  This file is part of Etripator,
  copyright (c) 2009--2026 Vincent Cruz.
 
  Etripator is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  Etripator is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with Etripator.  If not, see <http://www.gnu.org/licenses/>.

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯
*/
#include <munit.h>
#include "decode.h"
#include "message.h"
#include "message/console.h"

typedef struct {
    MemoryMap map;
    LabelRepository labels;
    Output output;
} Decoder;

static char g_buffer[1024U];

void* setup(const MunitParameter params[] __unused, void* user_data __unused) {
    message_printer_init();    
    console_message_printer_init();

    Decoder *decoder = calloc(1, sizeof(Decoder));
    munit_assert_ptr_not_null(decoder);
    munit_assert_true(label_repository_create(&decoder->labels));
    munit_assert_true(memory_map_init(&decoder->map));

    memset(g_buffer, 0, sizeof(g_buffer));

    decoder->output.filename = "ram0";
    decoder->output.stream = fmemopen(g_buffer, sizeof(g_buffer), "wb");
    munit_assert_ptr_not_null(decoder->output.stream);

    return decoder;
}

void tear_down(void* fixture) {
    Decoder *decoder = (Decoder*)fixture;
    label_repository_destroy(&decoder->labels);
    memory_map_destroy(&decoder->map);
    output_end(&decoder->output);

    free(decoder);
    
    message_printer_destroy();
}

MunitResult label_extract_test(const MunitParameter params[] __unused, void* fixture) {
    const uint8_t rom[] = {
        0xEA,                   // NOP
        0xEA,                   // NOP
        0xEA,                   // NOP
        0x20, 0xB0, 0xEA,       // JMP $EAB0
        0x44, 0xEC,             // BRS -20
    };

    const size_t size = sizeof(rom);

    Decoder *decoder = (Decoder*)fixture;

    Memory *memory = &decoder->map.memory[PCE_MEMORY_ROM];
    munit_assert_true(memory_create(memory, 8192U));

    for (size_t i = 0; i < 128; i++) {
        decoder->map.page[i].id = PCE_MEMORY_ROM;
        decoder->map.page[i].bank = i % (decoder->map.memory[PCE_MEMORY_ROM].length / PCE_BANK_SIZE);
    }

    memcpy(memory->data+0x200, rom, size);

    Section section = {
        .name = "",
        .type = SECTION_TYPE_CODE,
        .page = 0,
        .logical = 0xE200U,
        .size = size,
        .mpr = { [7] = 0x00, }
    };

    memory_map_mpr(&decoder->map, section.mpr);

    munit_assert_true(label_extract(&decoder->labels, &decoder->map, &section));
    munit_assert_size(2, ==, label_repository_size(&decoder->labels));
    
    Label l = {0};
    munit_assert_true(label_repository_get(&decoder->labels, 0, &l));
    munit_assert_uint16(0xEAB0, ==, l.logical);
    munit_assert_uint8(0, ==, l.page);
    munit_assert_string_equal("leab0_00", l.name);

    munit_assert_true(label_repository_get(&decoder->labels, 1, &l));
    munit_assert_uint16(0xE1F4, ==, l.logical);
    munit_assert_uint8(0, ==, l.page);
    munit_assert_string_equal("le1f4_00", l.name);

    return MUNIT_OK;
}

MunitResult label_dump_test(const MunitParameter params[] __unused, void* fixture) {
    Decoder *decoder = (Decoder*)fixture;

    const char *expected = 
    "_ptr .equ $2000\n"
    "; Joypad port\n"
    "joyport .equ $1000\n"
    "; VCE Control register\n"
    "color_ctrl .equ $1000\n"
    "; VCE control table index register\n"
    "color_reg .equ $1000\n"
    "; VCE Color table data register\n"
    "color_data .equ $1000\n";

    munit_assert_true(label_repository_add(&decoder->labels, "_ptr", 0x2000, 0xF8U, NULL));
    munit_assert_true(label_repository_add(&decoder->labels, "joyport", 0x1000U, 0xFFU, "Joypad port"));
    munit_assert_true(label_repository_add(&decoder->labels, "color_ctrl", 0x0400, 0xFFU, "VCE Control register"));
    munit_assert_true(label_repository_add(&decoder->labels, "color_reg", 0x0402, 0xFFU, "VCE control table index register"));
    munit_assert_true(label_repository_add(&decoder->labels, "color_data", 0x0404, 0xFFU, "VCE Color table data register"));

    label_dump(&decoder->output, &decoder->map, &decoder->labels);

    fflush(decoder->output.stream);
    munit_assert_memory_equal(sizeof(expected)-1, expected, g_buffer);

    return MUNIT_OK;
}

MunitResult compute_size_test(const MunitParameter params[] __unused, void* fixture) {
/*
	.code
	.bank 0
	.org $e000
  lda #$c7
  ldx #$a2
  sta <$01
  stx <$02
  rts
l0:
  nop
  nop
  inc A
  bra l0
l1:
  sei
  stz $1403
  rti
  nop
  nop
  nop
  nop
  brk
  .db "end",0
*/
    const uint8_t rom[] = {
        0xA9,0xC7,0xA2,0xA2,0x85,0x01,0x86,0x02,
        0x60,0xEA,0xEA,0x1A,0x80,0xFB,0x78,0x9C,
        0x03,0x14,0x40,0xEA,0xEA,0xEA,0xEA,0x00,
        0x65,0x6E,0x64,0x00,
    };

    Decoder *decoder = (Decoder*)fixture;
    Memory *memory = &decoder->map.memory[PCE_MEMORY_ROM];
    munit_assert_true(memory_create(memory, 8192U));
    for (size_t i = 0; i < 128; i++) {
        decoder->map.page[i].id = PCE_MEMORY_ROM;
        decoder->map.page[i].bank = i % (decoder->map.memory[PCE_MEMORY_ROM].length / PCE_BANK_SIZE);
    }

    memcpy(memory->data+0x0E7, rom, sizeof(rom));

    Section section_buffer[] = {
        [0] = {
            .name = "section #0",
            .type = SECTION_TYPE_CODE,
            .page = 0,
            .logical = 0xE0E7U,
            .size = -1,
            .mpr = { [7] = 0x00, }
        },
        [1] = {
            .name = "section #1",
            .type = SECTION_TYPE_CODE,
            .page = 0,
            .logical = 0xE0E7U + 0x09U,
            .size = -1,
            .mpr = { [7] = 0x00, }
        },
        [2] = {
            .name = "section #2",
            .type = SECTION_TYPE_CODE,
            .page = 0,
            .logical = 0xE0E7U + 0x0EU,
            .size = -1,
            .mpr = { [7] = 0x00, }
        },
        [3] = {
            .name = "section #3",
            .type = SECTION_TYPE_CODE,
            .page = 0,
            .logical = 0xE0E7U + 0x13U,
            .size = -1,
            .mpr = { [7] = 0x00, }
        }
    };
    size_t count = sizeof(section_buffer) / sizeof(section_buffer[0]);
    // don't do this at home
    SectionArray sections = {
        .capacity = count,
        .count = count,
        .data = &section_buffer[0]
    };
 
    memory_map_mpr(&decoder->map, sections.data[0].mpr);

    int32_t size;
    size = compute_size(&decoder->map, &sections, 0);
    munit_assert_int32(9, ==, size);

    size = compute_size(&decoder->map, &sections, 1);
    munit_assert_int32(5, ==, size);

    size = compute_size(&decoder->map, &sections, 2);
    munit_assert_int32(5, ==, size);

    size = compute_size(&decoder->map, &sections, 3);
    munit_assert_int32(5, ==, size);

    return MUNIT_OK;
}

static MunitTest decode_tests[] = {
    { "/label_extract", label_extract_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/label_dump", label_dump_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/compute_size", compute_size_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite decode_suite = {
    "Output test suite", decode_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&decode_suite, NULL, argc, argv);
}