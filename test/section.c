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

#include "../src/section.c"

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC_VARARG(message_print, MessageType, const char*, size_t, const char*, const char*, ...);

TEST_GROUP(section);

TEST_SETUP(section) {
    RESET_FAKE(message_print);
    FFF_RESET_HISTORY();
}

TEST_TEAR_DOWN(section) {
}

TEST(section, section_type_name) {
    TEST_ASSERT_EQUAL_STRING("data", section_type_name(SECTION_TYPE_DATA).data);
    TEST_ASSERT_EQUAL_STRING("code", section_type_name(SECTION_TYPE_CODE).data);
    TEST_ASSERT_EQUAL_STRING("unknown", section_type_name(SECTION_TYPE_UNKNOWN).data);
    TEST_ASSERT_EQUAL_STRING("unknown", section_type_name(SECTION_TYPE_COUNT).data);
}

TEST(section, data_type_name) {
    TEST_ASSERT_EQUAL_STRING("binary", data_type_name(DATA_TYPE_BINARY).data);
    TEST_ASSERT_EQUAL_STRING("hex", data_type_name(DATA_TYPE_HEX).data);
    TEST_ASSERT_EQUAL_STRING("string", data_type_name(DATA_TYPE_STRING).data);
    TEST_ASSERT_EQUAL_STRING("jumptable", data_type_name(DATA_TYPE_JUMP_TABLE).data);
    TEST_ASSERT_EQUAL_STRING("unknown", data_type_name(DATA_TYPE_UNKNOWN).data);
    TEST_ASSERT_EQUAL_STRING("unknown", data_type_name(DATA_TYPE_COUNT).data);
}

TEST(section, section_compare) {
    Section s[2];
    for(size_t i=0; i<2; i++) {
        section_reset(&s[i]);
    }

    s[0].base.page = 0xF7;
    s[0].base.logical = 0xC59A;

    s[1].base.page = 0xF7;
    s[1].base.logical = 0xC59A;
    TEST_ASSERT_EQUAL(0, section_compare(&s[0], &s[1]));

    s[1].base.page = 0xFF;
    s[1].base.logical = 0xE081;
    TEST_ASSERT_LESS_THAN(0, section_compare(&s[0], &s[1]));

    s[1].base.page = 0xF7;
    s[1].base.logical = 0xC5AD;
    TEST_ASSERT_LESS_THAN(0, section_compare(&s[0], &s[1]));

    s[0].base.logical = 0xD000;
    TEST_ASSERT_GREATER_THAN(0, section_compare(&s[0], &s[1]));

    s[0].base.page = 0xF8;
    s[0].base.logical = 0xC5AD;
    TEST_ASSERT_GREATER_THAN(0, section_compare(&s[0], &s[1]));
}

TEST(section, section_overlap) {
    Section s[2];
     for(size_t i=0; i<2; i++) {
        section_reset(&s[i]);
    }

    s[0].base.type = SECTION_TYPE_DATA;
    s[0].base.logical = 0xE900;
    s[0].base.page = 0xFF;
    s[0].base.size = 0x100;

    s[1].base.type = SECTION_TYPE_CODE;
    s[1].base.logical = s[0].base.logical;
    s[1].base.page = s[0].base.page;
    s[1].base.size = 0x100;
    
    TEST_ASSERT_EQUAL(-1, section_overlap(&s[0], &s[1]));

    s[1].base.logical = 0xE8E0;
    TEST_ASSERT_EQUAL(-1, section_overlap(&s[0], &s[1]));

    s[1].base.logical = 0xF004;
    TEST_ASSERT_EQUAL(0, section_overlap(&s[0], &s[1]));

    s[1] = s[0];
    s[1].base.logical = 0xE8E0;
    TEST_ASSERT_EQUAL(1, section_overlap(&s[0], &s[1]));

    s[1].data.type = DATA_TYPE_STRING;
    TEST_ASSERT_EQUAL(-1, section_overlap(&s[0], &s[1]));

    s[1].base.logical = 0xF004;
    TEST_ASSERT_EQUAL(0, section_overlap(&s[0], &s[1]));

    s[1].base.page = 0xE0;
    TEST_ASSERT_EQUAL(0, section_overlap(&s[0], &s[1]));
}

TEST(section, section_merge) {
    Section s[2];
    for(size_t i=0; i<2; i++) {
        section_reset(&s[i]);
    }

    s[0].base.logical = 0xE900;
    s[0].base.size = 0x100;

    s[1].base.logical = 0xE9A0;
    s[1].base.size = 0x10;

    section_merge(&s[0], &s[1]);
    TEST_ASSERT_EQUAL(0xE900, s[0].base.logical);
    TEST_ASSERT_EQUAL(0x100, s[0].base.size);

    s[1].base.logical = 0xE9E0;
    s[1].base.size = 0x60;

    section_merge(&s[0], &s[1]);
    TEST_ASSERT_EQUAL(0xE900, s[0].base.logical);
    TEST_ASSERT_EQUAL(0x140, s[0].base.size);

    s[1].base.logical = 0xEB00;
    s[1].base.size = 0x20;

    section_merge(&s[0], &s[1]);
    TEST_ASSERT_EQUAL(0xE900, s[0].base.logical);
    TEST_ASSERT_EQUAL(0x220, s[0].base.size);

    s[1].base.logical = 0xE800;
    s[1].base.size = 0x10;

    section_merge(&s[0], &s[1]);
    TEST_ASSERT_EQUAL(0xE800, s[0].base.logical);
    TEST_ASSERT_EQUAL(0x320, s[0].base.size);

    s[1].base.logical = 0xE7C0;
    s[1].base.size = 0x200;

    section_merge(&s[0], &s[1]);
    TEST_ASSERT_EQUAL(0xE7C0, s[0].base.logical);
    TEST_ASSERT_EQUAL(0x360, s[0].base.size);


    s[1].base.logical = 0xE600;
    s[1].base.size = 0x600;

    section_merge(&s[0], &s[1]);
    TEST_ASSERT_EQUAL(0xE600, s[0].base.logical);
    TEST_ASSERT_EQUAL(0x600, s[0].base.size);
}

TEST(section, section_array_add) {
    SectionArray array = {0};

    do {
        Section section = {
            .code = {
                .base = {
                    .type = SECTION_TYPE_CODE,
                    .page = 0x07,
                    .logical = 0xC5A6,
                    .mpr = { 0, 1, 2, 3, 4, 5, 6, 7 },
                    .offset = 0xCDEF,
                    .size = 123,
                    .output = string_view_from_literal("output"),
                    .name = string_view_from_literal("name"),
                    .description = string_view_from_literal("description"),
                },
            },
        };

        TEST_ASSERT_EQUAL(1, section_array_add(&array, &section));
    } while(0);

    do {
        Section section = {
            .data = {
                .base = {
                    .type = SECTION_TYPE_DATA,
                    .page = 0x01,
                    .logical = 0xA0FD,
                    .mpr = { 0xFF, 0xF4, 0, 1, 2, 3, 4, 0x00 },
                    .offset = 0x0000,
                    .size = 32,
                    .output = string_view_from_literal("data.asm"),
                    .name = string_view_from_literal("d00"),
                    .description = string_view_from_literal("data"),
                },
                .hex = {
                    .type = DATA_TYPE_HEX, 
                    .element_size = 98,
                    .elements_per_line = 76,
                },
            },
        };

        TEST_ASSERT_EQUAL(1, section_array_add(&array, &section));
    } while(0);

    TEST_ASSERT_EQUAL(2, array.count);
    TEST_ASSERT_EQUAL(4, array.capacity);
    
    Section value = {0};
    TEST_ASSERT_TRUE(section_array_get(&array, 0, &value));

    TEST_ASSERT_EQUAL(SECTION_TYPE_CODE, value.base.type);
    TEST_ASSERT_EQUAL(0x07, value.base.page);
    TEST_ASSERT_EQUAL(0xC5A6, value.base.logical);
    TEST_ASSERT_EQUAL(0xCDEF, value.base.offset);
    TEST_ASSERT_EQUAL(123, value.base.size);

    TEST_ASSERT_EQUAL(0, value.base.mpr[0]);
    TEST_ASSERT_EQUAL(1, value.base.mpr[1]);
    TEST_ASSERT_EQUAL(2, value.base.mpr[2]);
    TEST_ASSERT_EQUAL(3, value.base.mpr[3]);
    TEST_ASSERT_EQUAL(4, value.base.mpr[4]);
    TEST_ASSERT_EQUAL(5, value.base.mpr[5]);
    TEST_ASSERT_EQUAL(6, value.base.mpr[6]);
    TEST_ASSERT_EQUAL(7, value.base.mpr[7]);

    TEST_ASSERT_EQUAL_STRING("output", value.base.output.data);
    TEST_ASSERT_EQUAL_STRING("name", value.base.name.data);
    TEST_ASSERT_EQUAL_STRING("description", value.base.description.data);

    value = (Section) {0};
    TEST_ASSERT_TRUE(section_array_get(&array, 1, &value));

    TEST_ASSERT_EQUAL(SECTION_TYPE_DATA, value.base.type);
    TEST_ASSERT_EQUAL(0x01, value.base.page);
    TEST_ASSERT_EQUAL(0xA0FD, value.base.logical);
    TEST_ASSERT_EQUAL(0x0000, value.base.offset);
    TEST_ASSERT_EQUAL(32, value.base.size);

    TEST_ASSERT_EQUAL(0xFF, value.base.mpr[0]);
    TEST_ASSERT_EQUAL(0xF4, value.base.mpr[1]);
    TEST_ASSERT_EQUAL(0, value.base.mpr[2]);
    TEST_ASSERT_EQUAL(1, value.base.mpr[3]);
    TEST_ASSERT_EQUAL(2, value.base.mpr[4]);
    TEST_ASSERT_EQUAL(3, value.base.mpr[5]);
    TEST_ASSERT_EQUAL(4, value.base.mpr[6]);
    TEST_ASSERT_EQUAL(0, value.base.mpr[7]);

    TEST_ASSERT_EQUAL_STRING("data.asm", value.base.output.data);
    TEST_ASSERT_EQUAL_STRING("d00", value.base.name.data);
    TEST_ASSERT_EQUAL_STRING("data", value.base.description.data);

    TEST_ASSERT_EQUAL(DATA_TYPE_HEX, value.data.hex.type);
    TEST_ASSERT_EQUAL(98, value.data.hex.element_size);
    TEST_ASSERT_EQUAL(76, value.data.hex.elements_per_line);

    TEST_ASSERT_FALSE(section_array_get(&array, 2, &value));

    section_array_release(&array);
}

TEST(section, section_array_delete) {
    SectionArray array = {0};
    Section section[5] = {
        { .base = { .type = SECTION_TYPE_CODE, .page = 0xA0, .output = string_view_from_literal("out0"), .name = string_view_from_literal("name0"), .description = string_view_from_literal("desc0") } },
        { .base = { .type = SECTION_TYPE_CODE, .page = 0xA1, .output = string_view_from_literal("out1"), .name = string_view_from_literal("name1"), .description = string_view_from_literal("desc1") } },
        { .base = { .type = SECTION_TYPE_CODE, .page = 0xA2, .output = string_view_from_literal("out2"), .name = string_view_from_literal("name2"), .description = string_view_from_literal("desc2") } },
        { .base = { .type = SECTION_TYPE_CODE, .page = 0xA3, .output = string_view_from_literal("out3"), .name = string_view_from_literal("name3"), .description = string_view_from_literal("desc3") } },
        { .base = { .type = SECTION_TYPE_CODE, .page = 0xA4, .output = string_view_from_literal("out4"), .name = string_view_from_literal("name4"), .description = string_view_from_literal("desc4") } },
    };

    for(size_t i=0; i<5; i++) {
        TEST_ASSERT_EQUAL(1, section_array_add(&array, &section[i]));
    }
    TEST_ASSERT_EQUAL(5, array.count);

    TEST_ASSERT_FALSE(section_array_delete(&array, 7));
    TEST_ASSERT_TRUE(section_array_delete(&array, 2));

    TEST_ASSERT_EQUAL(4, array.count);

    Section value = {0};
    TEST_ASSERT_TRUE(section_array_get(&array, 2, &value));

    TEST_ASSERT_EQUAL_STRING("out3", value.base.output.data);
    TEST_ASSERT_EQUAL_STRING("name3", value.base.name.data);
    TEST_ASSERT_EQUAL_STRING("desc3", value.base.description.data);

    value = (Section){0};
    TEST_ASSERT_TRUE(section_array_get(&array, 3, &value));

    TEST_ASSERT_EQUAL_STRING("out4", value.base.output.data);
    TEST_ASSERT_EQUAL_STRING("name4", value.base.name.data);
    TEST_ASSERT_EQUAL_STRING("desc4", value.base.description.data);
}

TEST_GROUP_RUNNER(section) {
    RUN_TEST_CASE(section, section_type_name);
    RUN_TEST_CASE(section, data_type_name);
    RUN_TEST_CASE(section, section_compare);
    RUN_TEST_CASE(section, section_overlap);
    RUN_TEST_CASE(section, section_merge);
    RUN_TEST_CASE(section, section_array_add);
    RUN_TEST_CASE(section, section_array_delete);
// [todo]    RUN_TEST_CASE(section, section_tidy);
}

static void run_all_tests(void) {
    RUN_TEST_GROUP(section);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}
