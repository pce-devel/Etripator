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

#include "../src/message.c"

TEST_GROUP(message);

TEST_SETUP(message) {
}

TEST_TEAR_DOWN(message) {
}

TEST(message, init) {
    g_message_printer_head = (MessagePrinter*)0xBEEF;
    message_printer_init();
    TEST_ASSERT_NULL(g_message_printer_head);
}

static unsigned int dummy_open_0_call_count;
static bool dummy_open_0(void) {
    dummy_open_0_call_count++;
    return true;
}
static bool dummy_open_1(void) {
    dummy_open_0_call_count++;
    return false;
}

TEST(message, add) {
    message_printer_init();
    TEST_ASSERT_NULL(g_message_printer_head);
    
    MessagePrinter printer[4] = {
        [0] = { .open = dummy_open_0 },
        [1] = { .open = dummy_open_0 },
        [2] = { .open = dummy_open_1 },
        [3] = { .open = dummy_open_0 },
    };

    dummy_open_0_call_count = 0;
    TEST_ASSERT_TRUE(message_printer_add(&printer[0]));
    TEST_ASSERT_EQUAL_UINT(1, dummy_open_0_call_count);
    TEST_ASSERT_EQUAL_PTR(&printer[0], g_message_printer_head);
    TEST_ASSERT_NULL(g_message_printer_head->next);

    TEST_ASSERT_TRUE(message_printer_add(&printer[1]));
    TEST_ASSERT_EQUAL_UINT(2, dummy_open_0_call_count);
    TEST_ASSERT_EQUAL_PTR(&printer[1], g_message_printer_head);
    TEST_ASSERT_EQUAL_PTR(&printer[0], g_message_printer_head->next);
    TEST_ASSERT_NULL(printer[0].next);

    TEST_ASSERT_FALSE(message_printer_add(&printer[2]));
    TEST_ASSERT_EQUAL_UINT(3, dummy_open_0_call_count);
    TEST_ASSERT_EQUAL_PTR(&printer[1], g_message_printer_head);
    TEST_ASSERT_EQUAL_PTR(&printer[0], g_message_printer_head->next);
    TEST_ASSERT_NULL(g_message_printer_head->next->next);

    TEST_ASSERT_TRUE(message_printer_add(&printer[3]));
    TEST_ASSERT_EQUAL_UINT(4, dummy_open_0_call_count);
    TEST_ASSERT_EQUAL_PTR(&printer[3], g_message_printer_head);
    TEST_ASSERT_EQUAL_PTR(&printer[1], g_message_printer_head->next);
    TEST_ASSERT_EQUAL_PTR(&printer[0], g_message_printer_head->next->next);
    TEST_ASSERT_NULL(g_message_printer_head->next->next->next);
}

static int dummy_close_call_count;

static bool dummy_close(void) {
    dummy_close_call_count++;
    return true;
}

TEST(message, destroy) {
    message_printer_init();
    TEST_ASSERT_NULL(g_message_printer_head);
    
    MessagePrinter printer[4] = {
        [0] = { .open = dummy_open_0, .close = dummy_close },
        [1] = { .open = dummy_open_0, .close = dummy_close  },
        [2] = { .open = dummy_open_0, .close = dummy_close  },
        [3] = { .open = dummy_open_0, .close = dummy_close  },
    };

    dummy_open_0_call_count = 0;
    dummy_close_call_count = 0;

    message_printer_init();
    TEST_ASSERT_TRUE(message_printer_add(&printer[0]));
    TEST_ASSERT_TRUE(message_printer_add(&printer[1]));
    TEST_ASSERT_TRUE(message_printer_add(&printer[2]));
    TEST_ASSERT_TRUE(message_printer_add(&printer[3]));

    TEST_ASSERT_EQUAL_PTR(&printer[3], g_message_printer_head);

    TEST_ASSERT_EQUAL_UINT(4, dummy_open_0_call_count);
    TEST_ASSERT_EQUAL_UINT(0, dummy_close_call_count);

    message_printer_destroy();
    TEST_ASSERT_EQUAL_UINT(4, dummy_close_call_count);
    TEST_ASSERT_NULL(g_message_printer_head);
}

static unsigned int dummy_print_index = 0;
static size_t dummy_print_line = 0;
static unsigned int dummy_print_history[4] = {-1};

static bool dummy_print_0(MessageType type, const char* file, size_t line, const char* function, const char* format, va_list args) {
    (void)file;
    (void)function;
    (void)format;
    (void)args;
    dummy_print_history[dummy_print_index++] = 0;
    TEST_ASSERT_EQUAL_UINT(MESSAGE_TYPE_INFO, type);
    TEST_ASSERT_EQUAL_size_t(dummy_print_line, line);
    return true;
}
static bool dummy_print_1(MessageType type, const char* file, size_t line, const char* function, const char* format, va_list args) {
    (void)file;
    (void)function;
    (void)format;
    (void)args;
    dummy_print_history[dummy_print_index++] = 1;
    TEST_ASSERT_EQUAL_UINT(MESSAGE_TYPE_INFO, type);
    TEST_ASSERT_EQUAL_size_t(dummy_print_line, line);
    return true;
}
static bool dummy_print_2(MessageType type, const char* file, size_t line, const char* function, const char* format, va_list args) {
    (void)file;
    (void)function;
    (void)format;
    (void)args;
    dummy_print_history[dummy_print_index++] = 2;
    TEST_ASSERT_EQUAL_UINT(MESSAGE_TYPE_INFO, type);
    TEST_ASSERT_EQUAL_size_t(dummy_print_line, line);
    return true;
}
static bool dummy_print_3(MessageType type, const char* file, size_t line, const char* function, const char* format, va_list args) {
    (void)file;
    (void)function;
    (void)format;
    (void)args;
    dummy_print_history[dummy_print_index++] = 3;
    TEST_ASSERT_EQUAL_UINT(MESSAGE_TYPE_INFO, type);
    TEST_ASSERT_EQUAL_size_t(dummy_print_line, line);
    return true;
}

TEST(message, print) {
    message_printer_init();
    TEST_ASSERT_NULL(g_message_printer_head);
    
    MessagePrinter printer[4] = {
        [0] = { .open = dummy_open_0, .close = dummy_close, .output = dummy_print_0 },
        [1] = { .open = dummy_open_0, .close = dummy_close, .output = dummy_print_1 },
        [2] = { .open = dummy_open_0, .close = dummy_close, .output = dummy_print_2 },
        [3] = { .open = dummy_open_0, .close = dummy_close, .output = dummy_print_3 },
    };

    dummy_open_0_call_count = 0;
    dummy_close_call_count = 0;
    dummy_print_index = 0;

    for(unsigned int i=0; i<4; i++) {
        dummy_print_history[i] = -1;
    }

    message_printer_init();
    TEST_ASSERT_TRUE(message_printer_add(&printer[0]));
    TEST_ASSERT_TRUE(message_printer_add(&printer[1]));
    TEST_ASSERT_TRUE(message_printer_add(&printer[2]));
    TEST_ASSERT_TRUE(message_printer_add(&printer[3]));

    TEST_ASSERT_EQUAL_PTR(&printer[3], g_message_printer_head);

    TEST_ASSERT_EQUAL_UINT(4, dummy_open_0_call_count);
    TEST_ASSERT_EQUAL_UINT(0, dummy_close_call_count);

    dummy_print_line = __LINE__; INFO_MSG("test");
    TEST_ASSERT_EQUAL_UINT(4, dummy_print_index);
    TEST_ASSERT_EQUAL_UINT(3, dummy_print_history[0]);
    TEST_ASSERT_EQUAL_UINT(2, dummy_print_history[1]);
    TEST_ASSERT_EQUAL_UINT(1, dummy_print_history[2]);
    TEST_ASSERT_EQUAL_UINT(0, dummy_print_history[3]);

    message_printer_destroy();
    TEST_ASSERT_EQUAL_UINT(4, dummy_close_call_count);
    TEST_ASSERT_NULL(g_message_printer_head);
}


TEST_GROUP_RUNNER(message) {
    RUN_TEST_CASE(message, init);
    RUN_TEST_CASE(message, add);
    RUN_TEST_CASE(message, destroy);
    RUN_TEST_CASE(message, print);
}

static void run_all_tests(void) {
    RUN_TEST_GROUP(message);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}