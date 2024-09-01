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
  copyright (c) 2009--2023 Vincent Cruz.
 
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

#include <memory.h>

console_msg_printer_t g_msg_printer;

void* setup(const MunitParameter params[] __attribute__((unused)), void* user_data __attribute__((unused))) {
    return NULL;
}

void tear_down(void* fixture __attribute__((unused))) {
}

MunitResult memory_create_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    Memory mem = {0};
    bool result = false;

    mem.data = NULL;
    mem.length = 0xCAFEU;
    munit_assert_false(memory_create(&mem, 0U));
    munit_assert_size(mem.length, ==, 0xCAFEU);
    munit_assert_ptr_null(mem.data);

    munit_assert_true(memory_create(&mem, 32U));
    munit_assert_size(mem.length, ==, 32U);
    munit_assert_ptr_not_null(mem.data);

    memory_destroy(&mem);
    munit_assert_size(mem.length, ==, 0U);
    munit_assert_ptr_null(mem.data);

    return MUNIT_OK;
}

MunitResult memory_create_fill(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    Memory mem = {0};

    munit_assert_false(memory_fill(&mem, 0x7C));

    munit_assert_true(memory_create(&mem, 256U));
    munit_assert_size(mem.length, ==, 256U);
    munit_assert_ptr_not_null(mem.data);

    munit_assert_true(memory_fill(&mem, 0x7C));
    for(size_t i=0; i<mem.length; i++) {
        munit_assert_uint8(mem.data[i], ==, 0x7C);
    }

    munit_assert_true(memory_fill(&mem, 0xA0));
    for(size_t i=0; i<mem.length; i++) {
        munit_assert_uint8(mem.data[i], ==, 0xA0);
    }

    memory_destroy(&mem);

    return MUNIT_OK;
}


static MunitTest memory_tests[] = {
    { "/create", memory_create_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/fill", memory_create_fill, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite memory_suite = {
    "Memory block test suite", memory_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    msg_printer_init();    
    console_msg_printer_init(&g_msg_printer);
    msg_printer_add((msg_printer_t*)&g_msg_printer);

    int ret = munit_suite_main(&memory_suite, NULL, argc, argv);

    msg_printer_destroy();

    return ret;
}