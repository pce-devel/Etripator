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

#include <message.h>
#include <message/console.h>

#include <cd.h>

void* setup(const MunitParameter params[] __attribute__((unused)), void* user_data __attribute__((unused))) {
    return NULL;
}

void tear_down(void* fixture __attribute__((unused))) {
}

MunitResult cd_memory_map_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    MemoryMap map = {};

    munit_assert_true(memory_map_init(&map));
    munit_assert_true(cd_memory_map(&map));

    munit_assert_not_null(map.memory[PCE_MEMORY_CD_RAM].data);
    munit_assert_size(map.memory[PCE_MEMORY_CD_RAM].length, ==, PCE_CD_RAM_BANK_COUNT*PCE_BANK_SIZE);
    for(size_t i=0; i<PCE_CD_RAM_BANK_COUNT; i++) {
        munit_assert_int(map.page[PCE_CD_RAM_FIRST_PAGE+i].id, ==, PCE_MEMORY_CD_RAM);
        munit_assert_size(map.page[PCE_CD_RAM_FIRST_PAGE+i].bank, ==, i);
    }

    munit_assert_not_null(map.memory[PCE_MEMORY_SYSCARD_RAM].data);
    munit_assert_size(map.memory[PCE_MEMORY_SYSCARD_RAM].length, ==, PCE_SYSCARD_RAM_BANK_COUNT*PCE_BANK_SIZE);
    for(size_t i=0; i<PCE_SYSCARD_RAM_BANK_COUNT; i++) {
        munit_assert_int(map.page[PCE_SYSCARD_RAM_FIRST_PAGE+i].id, ==, PCE_MEMORY_SYSCARD_RAM);
        munit_assert_size(map.page[PCE_SYSCARD_RAM_FIRST_PAGE+i].bank, ==, i);
    }

    memory_map_destroy(&map);

    return MUNIT_OK;
}

// [todo] cd_load

static MunitTest cd_tests[] = {
    { "/memory_map", cd_memory_map_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite cd_suite = {
    "CDROM test suite", cd_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    message_printer_init();    
    console_message_printer_init();

    int ret = munit_suite_main(&cd_suite, NULL, argc, argv);

    message_printer_destroy();

    return ret;
}