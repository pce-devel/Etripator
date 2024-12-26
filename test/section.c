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
  copyright (c) 2009--2024 Vincent Cruz.
 
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
#include "../section.c"
#include "message.h"
#include "message/console.h"


void* setup(const MunitParameter params[] __attribute__((unused)), void* user_data __attribute__((unused))) {
    message_printer_init();    
    console_message_printer_init();
    return NULL;
}

void tear_down(void* fixture __attribute__((unused))) {
    message_printer_destroy();
}

MunitResult section_load_test(const MunitParameter params[], void* fixture) {
    (void)params;
    (void)fixture;

    static const Section bank0_0[4] = {
        { "cdbios_functions", SECTION_TYPE_CODE, 0, 0xe000, 0x0000, 0x504, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } }, 
        { "unknown.0", SECTION_TYPE_DATA, 0, 0xe504, 0x0504, 0x05, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } },
        { "ex_colorcmd.impl", SECTION_TYPE_CODE, 0, 0xe509, 0x0509, 0xce, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } },
        { "unknown.1", SECTION_TYPE_DATA, 0, 0xe5d7, 0x05d7, 0x03, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } }
    };

    static const Section bank0_1[9] = {
        { "ex_satclr.impl", SECTION_TYPE_CODE, 0, 0xe5da, 0x05da, 0x26, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } }, 
        { "unknown.2", SECTION_TYPE_DATA, 0, 0xf8a9, 0x18a9, 0x0f, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } },
        { "bm_free.impl", SECTION_TYPE_CODE, 0, 0xf8b8, 0x18b8, 0x575, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } },
        { "jump_table.0", SECTION_TYPE_DATA, 0, 0xfe2d, 0x1e2d, 0x2a, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } },
        { "grp_bios.impl", SECTION_TYPE_CODE, 0, 0xfe57, 0x1e57, 0x18, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } },
        { "jump_table.1", SECTION_TYPE_DATA, 0, 0xfe70, 0x1e70, 0x22, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } },
        { "ex_memopen.impl", SECTION_TYPE_CODE, 0, 0xfe92, 0x1e92, 0x30, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } },
        { "unknown.3", SECTION_TYPE_DATA, 0, 0xfec2, 0x1ec2, 0x134, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_UNKNOWN, 8, 16 } },
        { "irq_vectors", SECTION_TYPE_DATA, 0, 0xfff6, 0x1ff6, 0x0a, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { DATA_TYPE_HEX, 2, 1 } }
    };
 
    SectionArray arr = {0};
    section_array_reset(&arr);

    int i, j, k;
    int ret;
    ret = section_load(&arr, "./data/bank0_0.json");
    section_array_tidy(&arr);
    munit_assert_int(ret, !=, 0);
    munit_assert_int(arr.count, ==, 4);

    for(i=0; i<arr.count; i++) {
        munit_assert_string_equal(bank0_0[i].name, arr.data[i].name);
        munit_assert_int(bank0_0[i].type, ==, arr.data[i].type);
        munit_assert_uint8(bank0_0[i].page, ==, arr.data[i].page);
        munit_assert_uint16(bank0_0[i].logical, ==, arr.data[i].logical);
        munit_assert_uint32(bank0_0[i].offset, ==, arr.data[i].offset);
        munit_assert_int32(bank0_0[i].size, ==, arr.data[i].size);
        munit_assert_memory_equal(8, bank0_0[i].mpr, arr.data[i].mpr);
        munit_assert_int32(bank0_0[i].data.element_size, ==, arr.data[i].data.element_size);
        munit_assert_int32(bank0_0[i].data.elements_per_line, ==, arr.data[i].data.elements_per_line);
        munit_assert_string_equal(bank0_0[i].output, arr.data[i].output);
    }

    ret = section_load(&arr, "./data/bank0_1.json");
    section_array_tidy(&arr);
    munit_assert_int(ret, !=, 0);
    munit_assert_int(arr.count, ==, 13);
    for(i=0, k=0; k<4; i++, k++) {
        munit_assert_string_equal(bank0_0[k].name, arr.data[i].name);
        munit_assert_int(bank0_0[k].type, ==, arr.data[i].type);
        munit_assert_uint8(bank0_0[k].page, ==, arr.data[i].page);
        munit_assert_uint16(bank0_0[k].logical, ==, arr.data[i].logical);
        munit_assert_uint32(bank0_0[k].offset, ==, arr.data[i].offset);
        munit_assert_int32(bank0_0[k].size, ==, arr.data[i].size);
        munit_assert_memory_equal(8, bank0_0[k].mpr, arr.data[i].mpr);
        munit_assert_int32(bank0_0[k].data.element_size, ==, arr.data[i].data.element_size);
        munit_assert_int32(bank0_0[k].data.elements_per_line, ==, arr.data[i].data.elements_per_line);
        munit_assert_string_equal(bank0_0[k].output, arr.data[i].output);
    }
    for(k=0; k<9; i++, k++) {
        munit_assert_string_equal(bank0_1[k].name, arr.data[i].name);
        munit_assert_int(bank0_1[k].type, ==, arr.data[i].type);
        munit_assert_uint8(bank0_1[k].page, ==, arr.data[i].page);
        munit_assert_uint16(bank0_1[k].logical, ==, arr.data[i].logical);
        munit_assert_uint32(bank0_1[k].offset, ==, arr.data[i].offset);
        munit_assert_int32(bank0_1[k].size, ==, arr.data[i].size);
        munit_assert_memory_equal(8, bank0_1[k].mpr, arr.data[i].mpr);
        munit_assert_int32(bank0_1[k].data.element_size, ==, arr.data[i].data.element_size);
        munit_assert_int32(bank0_1[k].data.elements_per_line, ==, arr.data[i].data.elements_per_line);
        munit_assert_string_equal(bank0_1[k].output, arr.data[i].output);
    }

    section_array_delete(&arr);
    return MUNIT_OK;
}

MunitResult section_overlap_test(const MunitParameter params[], void* fixture) {
    Section a = {
        .type = SECTION_TYPE_CODE,
        .page = 0x01,
    };
    Section b = {
        .type = SECTION_TYPE_CODE,
        .page = 0x01,
    };

    a.logical = 0xe000;
    a.size = 0x100;

    b.logical = 0xe010;
    b.size = 0x40;
   
    munit_assert_int(section_overlap(&a, &b), ==, 1);

    b.type = SECTION_TYPE_DATA;
    munit_assert_int(section_overlap(&a, &b), ==, -1);

    a.type = SECTION_TYPE_DATA;
    a.logical = 0x0200;
    a.size = 0x10;

    b.logical = 0x01e0;
    b.size = 0x50;

    munit_assert_int(section_overlap(&a, &b), ==, 1);

    b.page = 0x02;
    munit_assert_int(section_overlap(&a, &b), ==, 0);

    b.page = a.page;
    b.size = 0x01;
    munit_assert_int(section_overlap(&a, &b), ==, 0);

    return MUNIT_OK;
}


MunitResult section_add_test(const MunitParameter params[], void* fixture) {
    // [todo] add sections
    // [todo] add one that merge
    // [todo] add one with merge+error
    return MUNIT_OK;
}

static MunitTest section_tests[] = {
    { "/load", section_load_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/overlap", section_overlap_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/add", section_add_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite section_suite = {
    "Section test suite", section_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&section_suite, NULL, argc, argv);
}