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
 
  You should have received ax copy of the GNU General Public License
  along with Etripator.  If not, see <http://www.gnu.org/licenses/>.

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯
*/
#include <unity.h>
#include <unity_fixture.h>

#include "../src/string.c"

TEST_GROUP(string);

TEST_SETUP(string) {
}

TEST_TEAR_DOWN(string) {
}

TEST(string, init) {
    String str = {
        .storage = (char*)0xCAFE,
        .length = 1,
        .capacity = 2,
        .buffer = {
            [0 ... (STRING_BUFFER_SIZE-1)] = 0xFFU,
        }
    };
    string_init(&str);

    TEST_ASSERT_NULL(str.storage);
    TEST_ASSERT_EQUAL(str.length, 0);
    TEST_ASSERT_EQUAL(str.capacity, 0);
    for(size_t i=0; i<STRING_BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_UINT8(str.buffer[i], 0);
    }
}

TEST(string, reserve) { 
    String str;
    string_init(&str);
   
    // invalid args
    TEST_ASSERT_FALSE(string_reserve(NULL, 10));
    TEST_ASSERT_FALSE(string_reserve(&str, 0));

    // keep in local storage
    str.length = 2;
    str.buffer[0] = str.buffer[1] = 'A';
    str.buffer[3] = '\0';
    str.length = 2;

    TEST_ASSERT_NOT_NULL(string_reserve(&str, STRING_BUFFER_SIZE/2));
    TEST_ASSERT_NULL(str.storage);
    TEST_ASSERT_EQUAL(str.capacity, 0);
    TEST_ASSERT_EQUAL(str.length, 2);
    TEST_ASSERT_EQUAL_STRING(str.buffer, "AA");

    // dynamic data storage
    TEST_ASSERT_NOT_NULL(string_reserve(&str, STRING_BUFFER_SIZE*2));
    TEST_ASSERT_EQUAL(str.capacity, (STRING_BUFFER_SIZE*2)+1);
    TEST_ASSERT_EQUAL(str.length, 2);
    TEST_ASSERT_NOT_NULL(str.storage);
    TEST_ASSERT_EQUAL_STRING(str.storage, "AA");

    // no need to realloc, keep the same capacity
    TEST_ASSERT_NOT_NULL(string_reserve(&str, (STRING_BUFFER_SIZE*3)/2));
    TEST_ASSERT_EQUAL(str.capacity, (STRING_BUFFER_SIZE*2)+1);
    TEST_ASSERT_EQUAL(str.length, 2);
    TEST_ASSERT_NOT_NULL(str.storage);
    TEST_ASSERT_EQUAL_STRING(str.storage, "AA");

    // realloc
    TEST_ASSERT_NOT_NULL(string_reserve(&str, STRING_BUFFER_SIZE*4));
    TEST_ASSERT_EQUAL(str.capacity, (STRING_BUFFER_SIZE*4)+1);
    TEST_ASSERT_EQUAL(str.length, 2);
    TEST_ASSERT_NOT_NULL(str.storage);
    TEST_ASSERT_EQUAL_STRING(str.storage, "AA");

    // try to reserve back to local storage size -> nothing should change
    TEST_ASSERT_NOT_NULL(string_reserve(&str, STRING_BUFFER_SIZE));
    TEST_ASSERT_EQUAL(str.capacity, (STRING_BUFFER_SIZE*4)+1);
    TEST_ASSERT_EQUAL(str.length, 2);
    TEST_ASSERT_NOT_NULL(str.storage);
    TEST_ASSERT_EQUAL_STRING(str.storage, "AA");

    string_release(&str);
}

TEST(string, ptr) {
    String str;
    string_init(&str);

    TEST_ASSERT_NOT_NULL(string_reserve(&str, STRING_BUFFER_SIZE/4));
    TEST_ASSERT_EQUAL_PTR(string_ptr(&str), str.buffer);

    TEST_ASSERT_NOT_NULL(string_reserve(&str, STRING_BUFFER_SIZE/2));
    TEST_ASSERT_EQUAL_PTR(string_ptr(&str), str.buffer);

    TEST_ASSERT_NOT_NULL(string_reserve(&str, STRING_BUFFER_SIZE*4));
    TEST_ASSERT_EQUAL_PTR(string_ptr(&str), str.storage);
    
    string_release(&str);
}

TEST(string, append_n) {
    String str;
    string_init(&str);
    
    TEST_ASSERT_TRUE(string_append_n(&str, 'a', 8));
    TEST_ASSERT_EQUAL(str.length, 8);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), "aaaaaaaa");
    
    TEST_ASSERT_TRUE(string_append_n(&str, 'b', 12));
    TEST_ASSERT_EQUAL(str.length, 20);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), "aaaaaaaabbbbbbbbbbbb");

    TEST_ASSERT_TRUE(string_append_n(&str, 'c', 24));
    TEST_ASSERT_EQUAL(str.length, 44);
    char *ptr = string_ptr(&str);
    for(size_t i=0; i<8; i++, ptr++) {
        TEST_ASSERT_EQUAL_CHAR(*ptr, 'a');
    } 
    for(size_t i=0; i<12; i++, ptr++) {
        TEST_ASSERT_EQUAL_CHAR(*ptr, 'b');
    } 
    for(size_t i=0; i<24; i++, ptr++) {
        TEST_ASSERT_EQUAL_CHAR(*ptr, 'c');
    } 
    TEST_ASSERT_EQUAL_CHAR(*ptr, '\0');

    string_release(&str);
}

TEST(string, prepend_n) { 
    String str;
    string_init(&str);
    
    TEST_ASSERT_TRUE(string_prepend_n(&str, 'a', 8));
    TEST_ASSERT_EQUAL(str.length, 8);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), "aaaaaaaa");
    
    TEST_ASSERT_TRUE(string_prepend_n(&str, 'b', 12));
    TEST_ASSERT_EQUAL(str.length, 20);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), "bbbbbbbbbbbbaaaaaaaa");

    TEST_ASSERT_TRUE(string_prepend_n(&str, 'c', 24));
    TEST_ASSERT_EQUAL(str.length, 44);
    char *ptr = string_ptr(&str);
    for(size_t i=0; i<24; i++, ptr++) {
        TEST_ASSERT_EQUAL_CHAR(*ptr, 'c');
    } 
    for(size_t i=0; i<12; i++, ptr++) {
        TEST_ASSERT_EQUAL_CHAR(*ptr, 'b');
    } 
    for(size_t i=0; i<8; i++, ptr++) {
        TEST_ASSERT_EQUAL_CHAR(*ptr, 'a');
    } 
    TEST_ASSERT_EQUAL_CHAR(*ptr, '\0');

    string_release(&str);
}

TEST(string, copy) {
    String str;
    string_init(&str);
    
    const char* string[] = {
        "Fit in local storage.",
        "We will need to allocated some storage for this one.",
        "Trigger realloc with a way bigger string that will not fit in his dynamic storage.",
        "This still can't fit in local storage.",
        "Back in local storage.",
    };
    const size_t length[] = {
        strlen(string[0]),
        strlen(string[1]),
        strlen(string[2]),
        strlen(string[3]),
        strlen(string[4]),
    };

    TEST_ASSERT_TRUE(string_copy(&str, string[0], strlen(string[0])));
    TEST_ASSERT_EQUAL(str.length, length[0]);
    TEST_ASSERT_EQUAL_PTR(string_ptr(&str), str.buffer);
    TEST_ASSERT_NULL(str.storage);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), string[0]);
    TEST_ASSERT_EQUAL(str.capacity, 0);

    TEST_ASSERT_TRUE(string_copy(&str, string[1], strlen(string[1])));
    TEST_ASSERT_EQUAL(str.length, length[1]);
    TEST_ASSERT_EQUAL(str.capacity, length[1]+1);
    TEST_ASSERT_EQUAL_PTR(string_ptr(&str), str.storage);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), string[1]);

    TEST_ASSERT_TRUE(string_copy(&str, string[2], strlen(string[2])));
    TEST_ASSERT_EQUAL(str.length, length[2]);
    TEST_ASSERT_EQUAL(str.capacity, length[2]+1);
    TEST_ASSERT_EQUAL_PTR(string_ptr(&str), str.storage);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), string[2]);

    TEST_ASSERT_TRUE(string_copy(&str, string[3], strlen(string[3])));
    TEST_ASSERT_EQUAL(str.length, length[3]);
    TEST_ASSERT_EQUAL(str.capacity, length[2]+1);
    TEST_ASSERT_EQUAL_PTR(string_ptr(&str), str.storage);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), string[3]);

    TEST_ASSERT_TRUE(string_copy(&str, string[4], strlen(string[4])));
    TEST_ASSERT_EQUAL(str.length, length[4]);
    TEST_ASSERT_EQUAL(str.capacity, 0);
    TEST_ASSERT_EQUAL_PTR(string_ptr(&str), str.buffer);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), string[4]);

    string_release(&str);
}

TEST(string, append) {
    String str;
    string_init(&str);
    
    const char* string[] = {
        "Started small.",
        "Add some more strings to get past the local static storage at some point.",
        "Some more to trigger reallocation."
    };
    const size_t length[] = {
        strlen(string[0]),
        strlen(string[1]),
        strlen(string[2]),
    };
    TEST_ASSERT_TRUE(string_append(&str, string[0], strlen(string[0])));
    TEST_ASSERT_EQUAL(str.length, length[0]);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), string[0]);

    TEST_ASSERT_TRUE(string_append(&str, string[1], strlen(string[1])));
    TEST_ASSERT_EQUAL(str.length, length[0] + length[1]);
    TEST_ASSERT_EQUAL_MEMORY(string[0], string_ptr(&str), length[0]);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str) + length[0], string[1]);

    TEST_ASSERT_TRUE(string_append(&str, string[2], strlen(string[2])));
    TEST_ASSERT_EQUAL(str.length, length[0] + length[1] + length[2]);
    TEST_ASSERT_EQUAL_MEMORY(string[0], string_ptr(&str), length[0]);
    TEST_ASSERT_EQUAL_MEMORY(string[1], string_ptr(&str) + length[0], length[1]);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str) + length[0] + length[1], string[2]);

    string_release(&str);
}

TEST(string, prepend) { 
    String str;
    string_init(&str);
    
    const char* string[] = {
        "Started small.",
        "Add some more strings to get past the local static storage at some point.",
        "Some more to trigger reallocation."
    };
    const size_t length[] = {
        strlen(string[0]),
        strlen(string[1]),
        strlen(string[2]),
    };
    TEST_ASSERT_TRUE(string_prepend(&str, string[0], strlen(string[0])));
    TEST_ASSERT_EQUAL(str.length, length[0]);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), string[0]);

    TEST_ASSERT_TRUE(string_prepend(&str, string[1], strlen(string[1])));
    TEST_ASSERT_EQUAL(str.length, length[0] + length[1]);
    TEST_ASSERT_EQUAL_MEMORY(string[1], string_ptr(&str), length[1]);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str) + length[1], string[0]);

    TEST_ASSERT_TRUE(string_prepend(&str, string[2], strlen(string[2])));
    TEST_ASSERT_EQUAL(str.length, length[0] + length[1] + length[2]);
    TEST_ASSERT_EQUAL_MEMORY(string[2], string_ptr(&str), length[2]);
    TEST_ASSERT_EQUAL_MEMORY(string[1], string_ptr(&str) + length[2], length[1]);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str) + length[2] + length[1], string[0]);

    string_release(&str);
}

TEST(string, format) {
    String str;
    string_init(&str);
    
    TEST_ASSERT_TRUE(string_format(&str, "0x%04x", 0x08ACU));
    TEST_ASSERT_EQUAL(str.length, 6U);
    TEST_ASSERT_EQUAL_STRING("0x08ac", string_ptr(&str));

    TEST_ASSERT_TRUE(string_format(&str, "lda [$%04x], %c ; Do stuff", 0xE60BU, 'X'));
    TEST_ASSERT_EQUAL(str.length, 25U);
    TEST_ASSERT_EQUAL_STRING("lda [$e60b], X ; Do stuff", string_ptr(&str));

    TEST_ASSERT_TRUE(string_format(&str, "Don't run with a stick or you may %s", "lose an eye"));
    TEST_ASSERT_EQUAL(str.length, 45U);
    TEST_ASSERT_EQUAL_STRING("Don't run with a stick or you may lose an eye", string_ptr(&str));

    string_release(&str);
}

TEST(string, append_format) {
    String str;
    string_init(&str);
    
    TEST_ASSERT_TRUE(string_append_format(&str, "lda #$%02x", 0x5CU));
    TEST_ASSERT_EQUAL(str.length, 8U);
    TEST_ASSERT_EQUAL_STRING("lda #$5c", string_ptr(&str));

    TEST_ASSERT_TRUE(string_append_format(&str, "%s; load 0x%02x into A register", "        ", 0x5CU));
    TEST_ASSERT_EQUAL(str.length, 43U);
    TEST_ASSERT_EQUAL_STRING("lda #$5c        ; load 0x5c into A register", string_ptr(&str));

    TEST_ASSERT_TRUE(string_append_format(&str, "\n;%s", "-------------------------------"));
    TEST_ASSERT_EQUAL(str.length, 76U);
    TEST_ASSERT_EQUAL_STRING("lda #$5c        ; load 0x5c into A register\n;-------------------------------", string_ptr(&str));

    string_release(&str);
}

TEST(string, prepend_format) {
    String str;
    string_init(&str);
    
    TEST_ASSERT_TRUE(string_prepend_format(&str, "%s", "-------------------------------"));
    TEST_ASSERT_EQUAL(str.length, 31U);
    TEST_ASSERT_EQUAL_STRING(string_ptr(&str), "-------------------------------");

    TEST_ASSERT_TRUE(string_prepend_format(&str, "%s; load 0x%02x into A register\n;", "        ", 0x5CU));
    TEST_ASSERT_EQUAL(str.length, 68U);
    TEST_ASSERT_EQUAL_STRING("        ; load 0x5c into A register\n;-------------------------------", string_ptr(&str));

    TEST_ASSERT_TRUE(string_prepend_format(&str, "lda #$%02x", 0x5CU));
    TEST_ASSERT_EQUAL(str.length, 76U);
    TEST_ASSERT_EQUAL_STRING("lda #$5c        ; load 0x5c into A register\n;-------------------------------", string_ptr(&str));

    string_release(&str);
}

TEST(string, clone) {
    String s0, s1;
    string_init(&s0);
    string_init(&s1);
    
    TEST_ASSERT_TRUE(string_append(&s0, "Once upon a time ...", 20U));
    TEST_ASSERT_TRUE(string_clone(&s1, &s0));
    TEST_ASSERT_NOT_EQUAL_PTR(string_ptr(&s1), string_ptr(&s0));
    TEST_ASSERT_EQUAL(string_length(&s1), string_length(&s0));
    TEST_ASSERT_EQUAL_STRING(string_ptr(&s1), string_ptr(&s0));
    string_release(&s0);
    string_release(&s1);

    TEST_ASSERT_TRUE(string_append_n(&s0, '#', 512U));
    TEST_ASSERT_TRUE(string_clone(&s1, &s0));
    TEST_ASSERT_NOT_EQUAL_PTR(string_ptr(&s1), string_ptr(&s0));
    TEST_ASSERT_EQUAL(string_length(&s1), string_length(&s0));
    TEST_ASSERT_EQUAL_STRING(string_ptr(&s1), string_ptr(&s0));
    string_release(&s0);
    string_release(&s1);
}

TEST_GROUP_RUNNER(string) {
    RUN_TEST_CASE(string, init); 
    RUN_TEST_CASE(string, reserve); 
    RUN_TEST_CASE(string, ptr); 
    RUN_TEST_CASE(string, append_n); 
    RUN_TEST_CASE(string, prepend_n); 
    RUN_TEST_CASE(string, copy); 
    RUN_TEST_CASE(string, append); 
    RUN_TEST_CASE(string, prepend); 
    RUN_TEST_CASE(string, format); 
    RUN_TEST_CASE(string, append_format); 
    RUN_TEST_CASE(string, prepend_format); 
    RUN_TEST_CASE(string, clone);
}

static void run_all_tests(void) {
    RUN_TEST_GROUP(string);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}