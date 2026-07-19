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
#include <unity.h>
#include <unity_fixture.h>

#include <fff.h>

#include <etripator/message.h>
#include "../src/json_helpers.h"

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC_VARARG(message_print, MessageType, const char*, size_t, const char*, const char*, ...);

TEST_GROUP(json_helpers);

TEST_SETUP(json_helpers) {
    RESET_FAKE(message_print);
    FFF_RESET_HISTORY();
}

TEST_TEAR_DOWN(json_helpers) {
}

TEST(json_helpers, validate_int) {
    int n = 0;

    json_t *node;

    node = json_integer(65535);
    TEST_ASSERT_TRUE(json_validate_int(node, &n));
    TEST_ASSERT_EQUAL_INT(n, 65535);
    json_decref(node);

    node = json_string("0xE07C");
    TEST_ASSERT_TRUE(json_validate_int(node, &n));
    TEST_ASSERT_EQUAL_INT(n, 0xE07C);
    json_decref(node);

    node = json_string("bozo");
    TEST_ASSERT_FALSE(json_validate_int(node, &n));
    json_decref(node);

    node = json_array();
    TEST_ASSERT_FALSE(json_validate_int(node, &n));
    json_decref(node);
}

TEST(json_helpers, load_description) {
    json_t *node;
    json_error_t error;

    String out = {0};

    node = json_integer(65535);
    string_init(&out);
    TEST_ASSERT_TRUE(json_load_description(node, "key", &out));
    TEST_ASSERT_EQUAL(0, string_length(&out));
    json_decref(node);

    node = json_loads("{\"desc\": \"description\"}", 0, &error);
    TEST_ASSERT_TRUE(json_load_description(node, "desc", &out));
    TEST_ASSERT_EQUAL_STRING(string_ptr(&out), "description");
    json_decref(node);
    string_release(&out);

    node = json_loads("{\"desc\": [\"line 0\",\"line 1\",\"line 2\",\"line 3\"] }", 0, &error);
    TEST_ASSERT_TRUE(json_load_description(node, "desc", &out));
    TEST_ASSERT_EQUAL_STRING(string_ptr(&out), "line 0\nline 1\nline 2\nline 3");
    json_decref(node);
    string_release(&out);
}

TEST(json_helpers, print_description) {
    char buffer[512];
    size_t size = sizeof(buffer);
    
    Data* data = data_buffer_create((uint8_t*)buffer, size);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_TRUE(data_open(data));
    TEST_ASSERT_TRUE(json_print_description(data, "desc", string_view_from_literal("")));
    data_close(data);
    data_release(data);
    
    TEST_ASSERT_EQUAL_MEMORY("\"desc\":[]", buffer, 9U);

    data = data_buffer_create((uint8_t*)buffer, size);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_TRUE(data_open(data));
    TEST_ASSERT_TRUE(json_print_description(data, "desc", string_view_from_literal("line 0\nline 1\nline 2")));
    data_close(data);
    data_release(data);
    
    TEST_ASSERT_EQUAL_MEMORY("\"desc\":[\n\t\t\"line 0\",\n\t\t\"line 1\",\n\t\t\"line 2\" \n\t]", buffer, 47U);
}

TEST_GROUP_RUNNER(json_helpers) {
    RUN_TEST_CASE(json_helpers, load_description);
    RUN_TEST_CASE(json_helpers, print_description);
    RUN_TEST_CASE(json_helpers, validate_int);
}

static void run_all_tests(void) {
    RUN_TEST_GROUP(json_helpers);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}