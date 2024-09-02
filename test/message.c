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

#include "../message.c"

void* setup(const MunitParameter params[] __attribute__((unused)), void* user_data __attribute__((unused))) {
    return NULL;
}

void tear_down(void* fixture __attribute__((unused))) {
}

MunitResult message_init_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    g_message_printer_head = (MessagePrinter*)0xBEEF;
    message_printer_init();
    munit_assert_ptr_null(g_message_printer_head);
    
    return MUNIT_OK;
}

static unsigned int dummy_open_0_call_count;
static bool dummy_open_0(MessagePrinter *printer) {
    dummy_open_0_call_count++;
    return true;
}
static bool dummy_open_1(MessagePrinter *printer) {
    dummy_open_0_call_count++;
    return false;
}

MunitResult message_add_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    message_printer_init();
    munit_assert_ptr_null(g_message_printer_head);
    
    MessagePrinter printer[4] = {
        [0] = { .open = dummy_open_0 },
        [1] = { .open = dummy_open_0 },
        [2] = { .open = dummy_open_1 },
        [3] = { .open = dummy_open_0 },
    };

    dummy_open_0_call_count = 0;
    munit_assert_true(message_printer_add(&printer[0]));
    munit_assert_uint(dummy_open_0_call_count, ==, 1);
    munit_assert_ptr_equal(g_message_printer_head, &printer[0]);
    munit_assert_null(g_message_printer_head->next);

    munit_assert_true(message_printer_add(&printer[1]));
    munit_assert_uint(dummy_open_0_call_count, ==, 2);
    munit_assert_ptr_equal(g_message_printer_head, &printer[1]);
    munit_assert_ptr_equal(g_message_printer_head->next, &printer[0]);
    munit_assert_null(printer[0].next);

    munit_assert_false(message_printer_add(&printer[2]));
    munit_assert_uint(dummy_open_0_call_count, ==, 3);
    munit_assert_ptr_equal(g_message_printer_head, &printer[1]);
    munit_assert_ptr_equal(g_message_printer_head->next, &printer[0]);
    munit_assert_null(g_message_printer_head->next->next);

    munit_assert_true(message_printer_add(&printer[3]));
    munit_assert_uint(dummy_open_0_call_count, ==, 4);
    munit_assert_ptr_equal(g_message_printer_head, &printer[3]);
    munit_assert_ptr_equal(g_message_printer_head->next, &printer[1]);
    munit_assert_ptr_equal(g_message_printer_head->next->next, &printer[0]);
    munit_assert_null(g_message_printer_head->next->next->next);

    return MUNIT_OK;
}

static MunitTest message_tests[] = {
    { "/init", message_init_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/add", message_add_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    // [todo] destroy
    // [todo] print
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite message_suite = {
    "message testt suite", message_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&message_suite, NULL, argc, argv);
}