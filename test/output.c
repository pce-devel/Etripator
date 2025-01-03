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
#include "output.h"
#include "message.h"
#include "message/console.h"

static char g_buffer[1024];

void* open_mem_output(const MunitParameter params[] __attribute__((unused)), void* user_data __attribute__((unused))) {
    message_printer_init();    
    console_message_printer_init();
    memset(g_buffer, 0, sizeof(g_buffer));
    return fmemopen(g_buffer, sizeof(g_buffer), "wb");
}

void close_mem_output(void* fixture) {
    message_printer_destroy();
    fclose((FILE*)fixture);
}

MunitResult output_char_test(const MunitParameter params[] __attribute__((unused)), void* fixture) {
    Output output = {
        .filename = "ram0",
        .stream = (FILE*)fixture
    };

    munit_assert_not_null(output.stream);
    
    munit_assert_true(output_char(&output, '@'));
    munit_assert_size(1, ==, output.column);
    munit_assert_size(0, ==, output.line);
    
    munit_assert_true(output_char(&output, '='));
    munit_assert_size(2, ==, output.column);
    munit_assert_size(0, ==, output.line);

    fflush(output.stream);
    munit_assert_char('@', ==, g_buffer[0]);
    munit_assert_char('=', ==, g_buffer[1]);
    
    return MUNIT_OK;
}

MunitResult output_newline_test(const MunitParameter params[] __attribute__((unused)), void* fixture) {
    Output output = {
        .filename = "ram0",
        .stream = (FILE*)fixture
    };

    munit_assert_not_null(output.stream);

    munit_assert_true(output_newline(&output));
    munit_assert_size(0, ==, output.column);
    munit_assert_size(1, ==, output.line);
    
    munit_assert_true(output_char(&output, '+'));
    munit_assert_size(1, ==, output.column);
    munit_assert_size(1, ==, output.line);

    munit_assert_true(output_newline(&output));
    munit_assert_size(0, ==, output.column);
    munit_assert_size(2, ==, output.line);

    fflush(output.stream);
    munit_assert_char('\n', ==, g_buffer[0]);
    munit_assert_char('+', ==, g_buffer[1]);
    munit_assert_char('\n', ==, g_buffer[2]);

    return MUNIT_OK;
}

MunitResult output_string_test(const MunitParameter params[] __attribute__((unused)), void* fixture) {
    Output output = {
        .filename = "ram0",
        .stream = (FILE*)fixture
    };

    const char expected[] = "this is a test\nfoobar";

    munit_assert_not_null(output.stream);

    munit_assert_true(output_string(&output, "this is a test"));
    munit_assert_size(14, ==, output.column);
    munit_assert_size(0, ==, output.line);

    munit_assert_true(output_newline(&output));

    munit_assert_true(output_string(&output, "foo"));
    munit_assert_size(3, ==, output.column);
    munit_assert_size(1, ==, output.line);

    munit_assert_true(output_string(&output, "bar"));
    munit_assert_size(6, ==, output.column);
    munit_assert_size(1, ==, output.line);

    fflush(output.stream);
    munit_assert_memory_equal(sizeof(expected)-1, expected, g_buffer);

    return MUNIT_OK;
}

MunitResult output_inline_comment_test(const MunitParameter params[] __attribute__((unused)), void* fixture) {
    Output output = {
        .filename = "ram0",
        .stream = (FILE*)fixture
    };

    const char expected[] = 
    "[^_^]                                                                           ; line#0\n"
    "                                                                                ; line#1\n"
    "                                                                                ; line#2";

    munit_assert_not_null(output.stream);

    munit_assert_true(output_char(&output, '['));
    munit_assert_true(output_char(&output, '^'));
    munit_assert_true(output_char(&output, '_'));
    munit_assert_true(output_char(&output, '^'));
    munit_assert_true(output_char(&output, ']'));

    munit_assert_true(output_inline_comment(&output, NULL));
    munit_assert_size(5, ==, output.column);
    munit_assert_size(0, ==, output.line);

    munit_assert_true(output_inline_comment(&output, "line#0\nline#1\nline#2"));
    munit_assert_size(88, ==, output.column);
    munit_assert_size(2, ==, output.line);
    
    fflush(output.stream);
    munit_assert_memory_equal(sizeof(expected)-1, expected, g_buffer);

    return MUNIT_OK;
}

MunitResult output_comment_test(const MunitParameter params[] __attribute__((unused)), void* fixture) {
    Output output = {
        .filename = "ram0",
        .stream = (FILE*)fixture
    };

    const char expected[] = 
    "[^_^]\n" 
    "; line#0\n"
    "; line#1\n"
    "; line#2\n";

    munit_assert_not_null(output.stream);

    munit_assert_true(output_char(&output, '['));
    munit_assert_true(output_char(&output, '^'));
    munit_assert_true(output_char(&output, '_'));
    munit_assert_true(output_char(&output, '^'));
    munit_assert_true(output_char(&output, ']'));

    munit_assert_true(output_comment(&output, NULL));
    munit_assert_size(5, ==, output.column);
    munit_assert_size(0, ==, output.line);

    munit_assert_true(output_newline(&output));

    munit_assert_true(output_comment(&output, "line#0\nline#1\nline#2"));
    munit_assert_size(0, ==, output.column);
    munit_assert_size(4, ==, output.line);
    
    fflush(output.stream);
    munit_assert_memory_equal(sizeof(expected)-1, expected, g_buffer);

    return MUNIT_OK;
}

MunitResult output_comment_address_test(const MunitParameter params[] __attribute__((unused)), void* fixture) {
    Output output = {
        .filename = "ram0",
        .stream = (FILE*)fixture
    };

    const char expected[] =
    "[=_o]                                                                           ; page: $000 logical: $e082\n"
    "                                                                                ; page: $1a5 logical: $94c0";

    munit_assert_not_null(output.stream);

    munit_assert_true(output_char(&output, '['));
    munit_assert_true(output_char(&output, '='));
    munit_assert_true(output_char(&output, '_'));
    munit_assert_true(output_char(&output, 'o'));
    munit_assert_true(output_char(&output, ']'));

    munit_assert_size(5, ==, output.column);
    munit_assert_size(0, ==, output.line);

    munit_assert_true(output_address_comment(&output, 0x000, 0xE082));
    munit_assert_size(107, ==, output.column);
    munit_assert_size(0, ==, output.line);

    munit_assert_true(output_newline(&output));

    munit_assert_true(output_address_comment(&output, 0x1A5, 0x94C0));

    munit_assert_size(107, ==, output.column);
    munit_assert_size(1, ==, output.line);
    
    fflush(output.stream);
    munit_assert_memory_equal(sizeof(expected)-1, expected, g_buffer);
    
    return MUNIT_OK;
}

MunitResult output_label_test(const MunitParameter params[] __attribute__((unused)), void* fixture) {
    Output output = {
        .filename = "ram0",
        .stream = (FILE*)fixture
    };

    Label label[] = {
        [0] = {
            .name = "routine",
            .page = 0x00,
            .logical = 0xE082
        },
        [1] = {
            .name = "function",
            .description = "function\ndo something",
            .page = 0x05,
            .logical = 0x94C0
        }
    };

    const char expected[] =
    "routine:                                                                        ; page: $000 logical: $e082\n"
    "; function\n"
    "; do something\n"
    "function:                                                                       ; page: $005 logical: $94c0";

    munit_assert_not_null(output.stream);

    munit_assert_true(output_label(&output, &label[0]));
    munit_assert_size(107, ==, output.column);
    munit_assert_size(0, ==, output.line);

    munit_assert_true(output_newline(&output));

    munit_assert_true(output_label(&output, &label[1]));
    munit_assert_size(107, ==, output.column);
    munit_assert_size(3, ==, output.line);
    
    fflush(output.stream);
    munit_assert_memory_equal(sizeof(expected)-1, expected, g_buffer);
    
    return MUNIT_OK;
}

MunitResult output_fill_to_test(const MunitParameter params[] __attribute__((unused)), void* fixture) {
    Output output = {
        .filename = "ram0",
        .stream = (FILE*)fixture
    };

    munit_assert_not_null(output.stream);

    munit_assert_true(output_fill_to(&output, 'x', 30));
    munit_assert_size(30, ==, output.column);
    munit_assert_size(0, ==, output.line);

    munit_assert_true(output_fill_to(&output, '@', 60));
    munit_assert_size(60, ==, output.column);
    munit_assert_size(0, ==, output.line);

    fflush(output.stream);
    for(size_t i=0; i<30; i++) {
        munit_assert_char('x', ==, g_buffer[i]);
    }
    for(size_t i=30; i<60; i++) {
        munit_assert_char('@', ==, g_buffer[i]);
    }

    return MUNIT_OK;
}

MunitResult output_fill_n_test(const MunitParameter params[] __attribute__((unused)), void* fixture) {
    Output output = {
        .filename = "ram0",
        .stream = (FILE*)fixture
    };

    munit_assert_not_null(output.stream);

    munit_assert_true(output_fill_n(&output, '.', 10));
    munit_assert_size(10, ==, output.column);
    munit_assert_size(0, ==, output.line);

    munit_assert_true(output_fill_n(&output, 'e', 7));
    munit_assert_size(17, ==, output.column);
    munit_assert_size(0, ==, output.line);

    fflush(output.stream);
    for(size_t i=0; i<10; i++) {
        munit_assert_char('.', ==, g_buffer[i]);
    }
    for(size_t i=10; i<17; i++) {
        munit_assert_char('e', ==, g_buffer[i]);
    }

    return MUNIT_OK;
}

MunitResult output_byte_test(const MunitParameter params[] __attribute__((unused)), void* fixture) {
    Output output = {
        .filename = "ram0",
        .stream = (FILE*)fixture
    };

    const char expected[] = "e79a05c000";

    munit_assert_not_null(output.stream);
    
    munit_assert_true(output_byte(&output, 0xE7));
    munit_assert_size(2, ==, output.column);
    munit_assert_size(0, ==, output.line);
    
    munit_assert_true(output_byte(&output, 0x9A));
    munit_assert_size(4, ==, output.column);
    munit_assert_size(0, ==, output.line);
    
    munit_assert_true(output_byte(&output, 0x05));
    munit_assert_size(6, ==, output.column);
    munit_assert_size(0, ==, output.line);
    
    munit_assert_true(output_byte(&output, 0xC0));
    munit_assert_size(8, ==, output.column);
    munit_assert_size(0, ==, output.line);
 
    munit_assert_true(output_byte(&output, 0x00));
    munit_assert_size(10, ==, output.column);
    munit_assert_size(0, ==, output.line);
 
    fflush(output.stream);
    munit_assert_memory_equal(sizeof(expected)-1, expected, g_buffer);

    return MUNIT_OK;
}

MunitResult output_word_test(const MunitParameter params[] __attribute__((unused)), void* fixture) {
    Output output = {
        .filename = "ram0",
        .stream = (FILE*)fixture
    };

    const char expected[] = "a45c07010000";

    munit_assert_not_null(output.stream);
    
    munit_assert_true(output_word(&output, 0xA45C));
    munit_assert_size(4, ==, output.column);
    munit_assert_size(0, ==, output.line);
    
    munit_assert_true(output_word(&output, 0x0701));
    munit_assert_size(8, ==, output.column);
    munit_assert_size(0, ==, output.line);

    munit_assert_true(output_word(&output, 0x0000));
    munit_assert_size(12, ==, output.column);
    munit_assert_size(0, ==, output.line);

    fflush(output.stream);
    munit_assert_memory_equal(sizeof(expected)-1, expected, g_buffer);

    return MUNIT_OK;
}

static MunitTest output_tests[] = {
    { "/char", output_char_test, open_mem_output, close_mem_output, MUNIT_TEST_OPTION_NONE, NULL }, 
    { "/newline", output_newline_test, open_mem_output, close_mem_output, MUNIT_TEST_OPTION_NONE, NULL },
    { "/inline", output_inline_comment_test, open_mem_output, close_mem_output, MUNIT_TEST_OPTION_NONE, NULL },
    { "/comment", output_comment_test, open_mem_output, close_mem_output, MUNIT_TEST_OPTION_NONE, NULL },
    { "/address", output_comment_address_test, open_mem_output, close_mem_output, MUNIT_TEST_OPTION_NONE, NULL },
    { "/label", output_label_test, open_mem_output, close_mem_output, MUNIT_TEST_OPTION_NONE, NULL },
    { "/fill_to", output_fill_to_test, open_mem_output, close_mem_output, MUNIT_TEST_OPTION_NONE, NULL },
    { "/fill_n", output_fill_n_test, open_mem_output, close_mem_output, MUNIT_TEST_OPTION_NONE, NULL },
    { "/string", output_string_test, open_mem_output, close_mem_output, MUNIT_TEST_OPTION_NONE, NULL },
    { "/byte", output_byte_test, open_mem_output, close_mem_output, MUNIT_TEST_OPTION_NONE, NULL },
    { "/word", output_word_test, open_mem_output, close_mem_output, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite output_suite = {
    "Output test suite", output_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&output_suite, NULL, argc, argv);
}