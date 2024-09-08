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
#include "comment.h"
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

MunitResult comment_add_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    Comment comment = {0};
    CommentRepository *repository = comment_repository_create();
    munit_assert_not_null(repository);

    munit_assert_true(comment_repository_add(repository, 0x0002, 0x00, "comment #0"));
    munit_assert_true(comment_repository_add(repository, 0x0001, 0x00, "comment #1"));
    munit_assert_true(comment_repository_add(repository, 0x0003, 0x00, "comment #3"));
    munit_assert_true(comment_repository_add(repository, 0x0001, 0x01, "comment #2"));
    
    munit_assert_true(comment_repository_find(repository, 0x0001, 0x01, &comment));
    munit_assert_uint16(comment.logical, ==, 0x0001);
    munit_assert_uint8(comment.page, ==, 0x01);
    munit_assert_string_equal(comment.text, "comment #2");

    munit_assert_true(comment_repository_find(repository, 0x0001, 0x00, &comment));
    munit_assert_uint16(comment.logical, ==, 0x0001);
    munit_assert_uint8(comment.page, ==, 0x00);
    munit_assert_string_equal(comment.text, "comment #1");

    munit_assert_true(comment_repository_find(repository, 0x0003, 0x00, &comment));
    munit_assert_uint16(comment.logical, ==, 0x0003);
    munit_assert_uint8(comment.page, ==, 0x00);
    munit_assert_string_equal(comment.text, "comment #3");

    munit_assert_int(comment_repository_size(repository), ==, 4);

    comment_repository_destroy(repository);
    
    return MUNIT_OK;
}

MunitResult comment_delete_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    Comment comment = {0};
    CommentRepository *repository = comment_repository_create();
    munit_assert_not_null(repository);

    munit_assert_true(comment_repository_add(repository, 0x0002, 0x03, "comment #7"));
    munit_assert_true(comment_repository_add(repository, 0x0002, 0x01, "comment #6"));
    munit_assert_true(comment_repository_add(repository, 0x0002, 0x02, "comment #5"));
    munit_assert_true(comment_repository_add(repository, 0x000a, 0x00, "comment #4"));
    munit_assert_true(comment_repository_add(repository, 0x0008, 0x00, "comment #3"));
    munit_assert_true(comment_repository_add(repository, 0x0004, 0x00, "comment #2"));
    munit_assert_true(comment_repository_add(repository, 0x0002, 0x00, "comment #1"));
    munit_assert_true(comment_repository_add(repository, 0x0000, 0x00, "comment #0"));
    munit_assert_int(comment_repository_size(repository), ==, 8);

    comment_repository_delete(repository, 0x0001, 0x0009, 0x00);

    munit_assert_int(comment_repository_size(repository), ==, 5);

    comment_repository_destroy(repository);
    
    return MUNIT_OK;
}

MunitResult comment_load_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    Comment comment = {0};
    CommentRepository *repository = comment_repository_create();

    munit_assert_false(comment_repository_load(repository, "/not_here/comment.json"));
    munit_assert_int(comment_repository_size(repository), ==, 0);

    munit_assert_false(comment_repository_load(repository, "data/comment_1.json"));
    munit_assert_int(comment_repository_size(repository), ==, 1);

    munit_assert_true(comment_repository_get(repository, 0, &comment));
    munit_assert_uint16(comment.logical, ==, 0xCAFEU);
    munit_assert_uint8(comment.page, ==, 0x0AU);
    munit_assert_string_equal(comment.text, "hello!");

    comment_repository_destroy(repository);

    munit_assert_true(comment_repository_load(repository, "data/comment_0.json"));
    munit_assert_int(comment_repository_size(repository), ==, 2);

    munit_assert_true(comment_repository_find(repository, 0xC105U, 3, &comment));
    munit_assert_uint16(comment.logical, ==, 0xC105U);
    munit_assert_uint8(comment.page, ==, 3);
    munit_assert_string_equal(comment.text, "line 0\nline 1\nline 2");

    munit_assert_true(comment_repository_find(repository, 0xEABCU, 0, &comment));
    munit_assert_uint16(comment.logical, ==, 0xEABCU);
    munit_assert_uint8(comment.page, ==, 0);
    munit_assert_string_equal(comment.text, "single line comment");

    comment_repository_destroy(repository);

    return MUNIT_OK;
}

static MunitTest comment_tests[] = {
    { "/add", comment_add_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/delete", comment_delete_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/load", comment_load_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite comment_suite = {
    "Comment test suite", comment_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&comment_suite, NULL, argc, argv);
}