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
#include "label.h"
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

MunitResult label_add_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    Label label = {};
    LabelRepository* repository = label_repository_create();
    munit_assert_not_null(repository);

    munit_assert_true(label_repository_add(repository, "l_0001", 0x0001, 0x00, NULL));
    munit_assert_true(label_repository_add(repository, "l_0020", 0x0020, 0x00, NULL));
    munit_assert_true(label_repository_add(repository, "l_000a", 0x000a, 0xb1, NULL));
    munit_assert_true(label_repository_add(repository, "l_cafe", 0xcafe, 0xf7, NULL));
    munit_assert_true(label_repository_add(repository, "l_0001", 0x0001, 0x00, NULL));

    munit_assert_int(label_repository_size(repository), ==, 4);
    
    munit_assert_true(label_repository_find(repository, 0x000a, 0xb1, &label));
    munit_assert_string_equal(label.name, "l_000a");

    munit_assert_true(label_repository_find(repository, 0x0020, 0x00, &label));
    munit_assert_string_equal(label.name, "l_0020");

    munit_assert_true(label_repository_find(repository, 0xcafe, 0xf7, &label));
    munit_assert_string_equal(label.name, "l_cafe");

    munit_assert_true(label_repository_find(repository, 0x0001, 0x00, &label));
    munit_assert_string_equal(label.name, "l_0001");

    munit_assert_false(label_repository_find(repository, 0xbeef, 0xac, &label));
    
    label_repository_destroy(repository);
    return MUNIT_OK;
}

MunitResult label_delete_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    Label label = {};
    LabelRepository* repository = label_repository_create();
    munit_assert_not_null(repository);

    munit_assert_true(label_repository_add(repository, "label01", 0x0110, 0x1a, NULL));
    munit_assert_true(label_repository_add(repository, "label02", 0x0220, 0x1a, NULL));
    munit_assert_true(label_repository_add(repository, "label03", 0x0330, 0x1b, "description"));
    munit_assert_true(label_repository_add(repository, "label04", 0x0440, 0x1a, NULL));
    munit_assert_true(label_repository_add(repository, "label05", 0x0550, 0x1b, NULL));
    munit_assert_true(label_repository_add(repository, "label06", 0x0553, 0x1b, NULL));
    munit_assert_true(label_repository_add(repository, "label07", 0x0555, 0x1b, NULL));
    munit_assert_true(label_repository_add(repository, "label08", 0x0557, 0x1b, NULL));
    
    munit_assert_int(label_repository_size(repository), ==, 8);

    label_repository_delete(repository, 0x04a0, 0x0556, 0x1b);

    munit_assert_int(label_repository_size(repository), ==, 5);

    munit_assert_true(label_repository_find(repository, 0x0557, 0x1b, &label));
    munit_assert_string_equal(label.name, "label08");
    munit_assert_true(label_repository_find(repository, 0x0440, 0x1a, &label));
    munit_assert_string_equal(label.name, "label04");
    munit_assert_true(label_repository_find(repository, 0x0330, 0x1b, &label));
    munit_assert_string_equal(label.name, "label03");
    munit_assert_string_equal(label.description, "description");
    munit_assert_true(label_repository_find(repository, 0x0220, 0x1a, &label));
    munit_assert_string_equal(label.name, "label02");
    munit_assert_true(label_repository_find(repository, 0x0110, 0x1a, &label));
    munit_assert_string_equal(label.name, "label01");

    munit_assert_false(label_repository_find(repository, 0x0555, 0x1b, &label));
    munit_assert_false(label_repository_find(repository, 0x0553, 0x1b, &label));
    munit_assert_false(label_repository_find(repository, 0x0550, 0x1b, &label));

    label_repository_destroy(repository);    

    return MUNIT_OK;
}

MunitResult label_load_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    Label label = {};
    LabelRepository* repository = label_repository_create();
    munit_assert_not_null(repository);

    munit_assert_false(label_repository_load(repository, "/not_here/label.json"));
    munit_assert_int(label_repository_size(repository), ==, 0);

    munit_assert_false(label_repository_load(repository, "data/label_1.json"));
    munit_assert_int(label_repository_size(repository), ==, 1);
    
    munit_assert_true(label_repository_get(repository, 0, &label));
    munit_assert_uint16(label.logical, ==, 0x5030U);
    munit_assert_uint8(label.page, ==, 4);
    munit_assert_string_equal(label.name, "ok");
    munit_assert_null(label.description);

    label_repository_destroy(repository);    

    munit_assert_true(label_repository_load(repository, "data/label_0.json"));
    munit_assert_int(label_repository_size(repository), ==, 3);

    munit_assert_true(label_repository_find(repository, 0x31DC, 0xF8, &label));
    munit_assert_uint16(label.logical, ==, 0x31DCU);
    munit_assert_uint8(label.page, ==, 0xF8);
    munit_assert_string_equal(label.name, "var");
    munit_assert_null(label.description);

    munit_assert_true(label_repository_find(repository, 0xEABC, 0x00, &label));
    munit_assert_uint16(label.logical, ==, 0xEABCU);
    munit_assert_uint8(label.page, ==, 0x00);
    munit_assert_string_equal(label.name, "do_something");
    munit_assert_string_equal(label.description, "do something");
    
    munit_assert_true(label_repository_find(repository, 0xD6F7, 0x1F, &label));
    munit_assert_uint16(label.logical, ==, 0xD6F7U);
    munit_assert_uint8(label.page, ==, 0x1F);
    munit_assert_string_equal(label.name, "run");
    munit_assert_string_equal(label.description, "line0\nline1\nline2\nline3");

    label_repository_destroy(repository);    

    return MUNIT_OK;
}

MunitResult label_save_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    // [todo]
    return MUNIT_OK;
}


static MunitTest Labelests[] = {
    { "/add", label_add_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/delete", label_delete_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/load", label_load_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/save", label_save_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite label_suite = {
    "Label test suite", Labelests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&label_suite, NULL, argc, argv);
}