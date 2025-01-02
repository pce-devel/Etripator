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
#include "decode.h"
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

MunitResult label_extract_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    const uint8_t rom[] = {
        0xEA,                   // NOP
        0xEA,                   // NOP
        0xEA,                   // NOP
        0x20, 0xB0, 0xEA,       // JMP $EAB0
        0x44, 0xEC,             // BRS -20
    };

    const size_t size = sizeof(rom);

    // [todo::begin] move to setup
    LabelRepository labels = {0};
    munit_assert_true(label_repository_create(&labels));

    MemoryMap map = {0};
    munit_assert_true(memory_map_init(&map));
    // [todo::end] move to setup

    Memory *memory = &map.memory[PCE_MEMORY_ROM];
    munit_assert_true(memory_create(memory, 8192U));

    for (size_t i = 0; i < 128; i++) {
        map.page[i].id = PCE_MEMORY_ROM;
        map.page[i].bank = i % (map.memory[PCE_MEMORY_ROM].length / PCE_BANK_SIZE);
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

    memory_map_mpr(&map, section.mpr);

    munit_assert_true(label_extract(&labels, &map, &section));
    munit_assert_size(2, ==, label_repository_size(&labels));
    
    Label l = {0};
    munit_assert_true(label_repository_get(&labels, 0, &l));
    munit_assert_uint16(0xEAB0, ==, l.logical);
    munit_assert_uint8(0, ==, l.page);
    munit_assert_string_equal("leab0_00", l.name);

    munit_assert_true(label_repository_get(&labels, 1, &l));
    munit_assert_uint16(0xE1F4, ==, l.logical);
    munit_assert_uint8(0, ==, l.page);
    munit_assert_string_equal("le1f4_00", l.name);

    // [todo::begin] move to tear_down
    label_repository_destroy(&labels);
    memory_map_destroy(&map);
    // [todo::end] move to tear_down
    
    return MUNIT_OK;
}

static MunitTest decode_tests[] = {
    { "/label_extract", label_extract_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite decode_suite = {
    "Output test suite", decode_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&decode_suite, NULL, argc, argv);
}