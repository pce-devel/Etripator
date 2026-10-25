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
#include <etripator/memory_map.h>
#include <etripator/rom.h>

#include <etripator/disasm.h>
#include "../src/disasm.c"

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC_VARARG(message_print, MessageType, const char*, size_t, const char*, const char*, ...);

static Context g_context = {0};

TEST_GROUP(disasm);

TEST_SETUP(disasm) {
    RESET_FAKE(message_print);
    FFF_RESET_HISTORY();

    g_context = (Context) {0};
    TEST_ASSERT_TRUE_MESSAGE(memory_map_init(&g_context.map), "failed to initialize memory map");
    TEST_ASSERT_TRUE_MESSAGE(rom_create(&g_context.map, 0x60000U), "failed to create ROM memory map");
    TEST_ASSERT_TRUE_MESSAGE(label_repository_create(&g_context.labels), "failed to create label repository");
    TEST_ASSERT_TRUE_MESSAGE(comment_repository_create(&g_context.comments), "failed to create comment repository");
    section_repository_create(&g_context.sections);

    g_context.map.mpr[0] = 0xFFU;
    g_context.map.mpr[1] = 0xF8U;
    g_context.map.mpr[7] = 0x00U;
}

TEST_TEAR_DOWN(disasm) {
    section_repository_destroy(&g_context.sections);
    comment_repository_destroy(&g_context.comments);
    label_repository_destroy(&g_context.labels);
    memory_map_destroy(&g_context.map);
}

TEST(disasm, pce_arg) {
    Context *context = &g_context;

    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x200AU, 0xF8U, string_view_from_literal("zp0"), string_view_from_literal("zp var 0")));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x200CU, 0xF8U, string_view_from_literal("zp1"), string_view_from_literal("zp var 1")));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xEC06U, 0x00U, string_view_from_literal("dat0"), string_view_from_literal("rom data 0")));

    String buf;
    string_init(&buf);

    uint8_t data[2];

    TEST_ASSERT_TRUE(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_A, data));
    TEST_ASSERT_EQUAL_STRING("A", string_ptr(&buf));
    string_release(&buf);

    TEST_ASSERT_TRUE(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_X, data));
    TEST_ASSERT_EQUAL_STRING("X", string_ptr(&buf));
    string_release(&buf);

    TEST_ASSERT_TRUE(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_Y, data));
    TEST_ASSERT_EQUAL_STRING("Y", string_ptr(&buf));
    string_release(&buf);

    data[0] = 0x5FU;
    TEST_ASSERT_TRUE(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_ABS, data));
    TEST_ASSERT_EQUAL_STRING("#$5f", string_ptr(&buf));
    string_release(&buf);

    data[0] = 0x20U;
    TEST_ASSERT_TRUE(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_ZP, data));
    TEST_ASSERT_EQUAL_STRING("<$20", string_ptr(&buf));
    string_release(&buf);

    data[0] = 0x0AU;
    TEST_ASSERT_TRUE(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_ZP, data));
    TEST_ASSERT_EQUAL_STRING("<zp0", string_ptr(&buf));
    string_release(&buf);

    data[0] = 0x21U;
    data[1] = 0x34U;
    TEST_ASSERT_TRUE(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_HHLL, data));
    TEST_ASSERT_EQUAL_STRING("$3421", string_ptr(&buf));
    string_release(&buf);

    data[0] = 0x06U;
    data[1] = 0xECU;
    TEST_ASSERT_TRUE(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_HHLL, data));
    TEST_ASSERT_EQUAL_STRING("dat0", string_ptr(&buf));
    string_release(&buf);

    TEST_ASSERT_FALSE(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_NONE, data));
    TEST_ASSERT_EQUAL_STRING("", string_ptr(&buf));
    string_release(&buf);
}

TEST(disasm, pce_arg_list) {
    Context *context = (Context*)&g_context;

    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x200AU, 0xF8U, string_view_from_literal("zp0"), string_view_from_literal("zp var 0")));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x200CU, 0xF8U, string_view_from_literal("zp1"), string_view_from_literal("zp var 1")));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xEC06U, 0x00U, string_view_from_literal("dat0"), string_view_from_literal("rom data 0")));

    String buf;
    string_init(&buf);

    uint8_t data[] = { 0x0CU, 0x06U, 0xECU, 0x7aU, 0x00, 0x00, 0x00, 0x00};
    size_t length = 8;

    TEST_ASSERT_TRUE(pce_arg_list(&context->map, &context->labels, &buf, PCE_OP_ZZ_IND_Y, data, length));
    TEST_ASSERT_EQUAL_STRING("[<zp1],Y", string_ptr(&buf));
    string_release(&buf);

    TEST_ASSERT_TRUE(pce_arg_list(&context->map, &context->labels, &buf, PCE_OP_HHLL_X_IND, data+1, length));
    TEST_ASSERT_EQUAL_STRING("[dat0,X]", string_ptr(&buf));
    string_release(&buf);

    TEST_ASSERT_TRUE(pce_arg_list(&context->map, &context->labels, &buf, PCE_OP_ABS_HHLL_X, data, length));
    TEST_ASSERT_EQUAL_STRING("#$0c,dat0,X", string_ptr(&buf));
    string_release(&buf);
}

TEST(disasm, pce_op_local_jump) {
    for(unsigned int op=0; op<256; op++) {
	    bool is_local_jmp = ((op & 0x0FU) == 0x0FU) || // BBR* and BBS*
                            (op           == 0x90U) || // BCC
                            (op           == 0xB0U) || // BCS
                            (op           == 0x80U) || // BRA
                            (op           == 0xF0U) || // BEQ
                            (op           == 0x30U) || // BMI
                            (op           == 0xD0U) || // BNE
                            (op           == 0x10U) || // BPL
                            (op           == 0x44U) || // BSR
                            (op           == 0x50U) || // BVC
                            (op           == 0x70U);   // BVS
        if(is_local_jmp) {
            TEST_ASSERT_TRUE(g_pce_opcode[op].tag == PCE_TAG_LOCAL_JUMP);
        } else {
            TEST_ASSERT_TRUE(g_pce_opcode[op].tag != PCE_TAG_LOCAL_JUMP);
        }
    }
}

TEST(disasm, pce_tag_local_jump) {
    uint8_t data[8] = {0};
    size_t size;

    data[0] = 0x80U;
    data[1] = 0xE8U;
    size = 1;
    pce_tag_local_jump(0xFF7DU, data, &size);
    TEST_ASSERT_EQUAL_UINT8(0x65U, data[1]);
    TEST_ASSERT_EQUAL_UINT8(0xFFU, data[2]);
    TEST_ASSERT_EQUAL_size_t(2, size);

    data[1] = 0x06U;
    size = 1;
    pce_tag_local_jump(0xFF9CU, data, &size);
    TEST_ASSERT_EQUAL_UINT16(0xA2U, data[1]);
    TEST_ASSERT_EQUAL_UINT16(0xFFU, data[2]);
    TEST_ASSERT_EQUAL_size_t(2, size);

    data[1] = 0xBBU;
    size = 1;
    pce_tag_local_jump(0xEC4BU, data, &size);
    TEST_ASSERT_EQUAL_UINT16(0x06U, data[1]);
    TEST_ASSERT_EQUAL_UINT16(0xECU, data[2]);
    TEST_ASSERT_EQUAL_size_t(2, size);
}

TEST(disasm, pce_tag_mmap_test) {
    uint8_t data[8] = {0};
    size_t size = 1;
    pce_tag_mmap(0, data, &size);
    TEST_ASSERT_EQUAL_UINT8(0, data[1]);
    TEST_ASSERT_EQUAL_size_t(1, size);
    for(uint8_t i=0; i<8; i++) {
        uint8_t j = (1<<i);
        uint8_t k = j | (j-1);
        for(uint16_t v=j; v<=k; v++) {
            data[1] = v;
            size = 1;
            pce_tag_mmap(0, data, &size);
            TEST_ASSERT_EQUAL_UINT8(i, data[1]);
            TEST_ASSERT_EQUAL_size_t(1, size);
        }
    }
}

TEST(disasm, pce_code) {
    Context *context = (Context*)&g_context;

    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x200AU, 0xF8U, string_view_from_literal("zp0"), string_view_from_literal("zp var 0")));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x200CU, 0xF8U, string_view_from_literal("zp1"), string_view_from_literal("zp var 1")));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xEC06U, 0x00U, string_view_from_literal("dat0"), string_view_from_literal("rom data 0")));

    String buf;
    string_init(&buf);

    // brk
    uint16_t logical = 0xE000U;
    TEST_ASSERT_TRUE(pce_code(&context->map, &context->labels, &logical, &buf));
    TEST_ASSERT_EQUAL_STRING("bbs7 <$ff,$e002", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xE003U, logical);
    string_release(&buf);

    // bra dat 0
    context->map.memory[PCE_MEMORY_ROM].data[0x0C4AU] = 0x80U;
    context->map.memory[PCE_MEMORY_ROM].data[0x0C4BU] = 0xBAU;

    logical = 0xEC4AU;
    TEST_ASSERT_TRUE(pce_code(&context->map, &context->labels, &logical, &buf));
    TEST_ASSERT_EQUAL_STRING("bra  dat0", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xEC4CU, logical);
    string_release(&buf);

    // tam 7
    context->map.memory[PCE_MEMORY_ROM].data[0x0000U] = 0x53U;
    context->map.memory[PCE_MEMORY_ROM].data[0x0001U] = 0x80U;

    logical = 0xE000U;
    TEST_ASSERT_TRUE(pce_code(&context->map, &context->labels, &logical, &buf));
    TEST_ASSERT_EQUAL_STRING("tam  #$07", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xE002U, logical);
    string_release(&buf);

    // unknown instruction
    context->map.memory[PCE_MEMORY_ROM].data[0x0002U] = 0xCBU;
    TEST_ASSERT_TRUE(pce_code(&context->map, &context->labels, &logical, &buf));
    TEST_ASSERT_EQUAL_STRING(".db  #$cb", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xE003U, logical);
    string_release(&buf);

    // [todo] simple program
}

TEST(disasm, pce_jump_table) {
    Context *context = (Context*)&g_context;

    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xFA71U, 0x00U, string_view_from_literal("l00"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xE600U, 0x00U, string_view_from_literal("l01"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xEC06U, 0x00U, string_view_from_literal("l02"), (StringView){0}));

    String buf;
    string_init(&buf);

    context->map.memory[PCE_MEMORY_ROM].data[0x4020U] = 0x06U;
    context->map.memory[PCE_MEMORY_ROM].data[0x4021U] = 0xECU;
    context->map.memory[PCE_MEMORY_ROM].data[0x4022U] = 0x70U;
    context->map.memory[PCE_MEMORY_ROM].data[0x4023U] = 0xEAU;

    g_context.map.mpr[6] = 0x02U;

    uint16_t logical = 0xC020U;
    TEST_ASSERT_TRUE(pce_jump_table(&context->map, &context->labels, &logical, 2, 1, &buf));
    TEST_ASSERT_EQUAL_STRING("l02", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xC022U, logical);

    string_release(&buf);

    TEST_ASSERT_TRUE(pce_jump_table(&context->map, &context->labels, &logical, 2, 1, &buf));
    TEST_ASSERT_EQUAL_STRING("$EA70", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xC024U, logical);

    string_release(&buf);
}

TEST(disasm, pce_data_hex) {
    Context *context = (Context*)&g_context;

    String buf;
    string_init(&buf);

    context->map.memory[PCE_MEMORY_ROM].data[0x4020U] = 0xABU;
    context->map.memory[PCE_MEMORY_ROM].data[0x4021U] = 0x01U;
    context->map.memory[PCE_MEMORY_ROM].data[0x4022U] = 0x7CU;
    context->map.memory[PCE_MEMORY_ROM].data[0x4023U] = 0xE8U;
    context->map.memory[PCE_MEMORY_ROM].data[0x4024U] = 0x23U;
    context->map.memory[PCE_MEMORY_ROM].data[0x4025U] = 0xD5U;

    g_context.map.mpr[6] = 0x02U;

    uint16_t logical = 0xC020U;
    TEST_ASSERT_TRUE(pce_data_hex(&context->map, &context->labels, &logical, 1, 5, &buf));
    TEST_ASSERT_EQUAL_STRING("$ab, $01, $7c, $e8, $23", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xC025U, logical);
    string_release(&buf);

    logical = 0xC020U;
    TEST_ASSERT_TRUE(pce_data_hex(&context->map, &context->labels, &logical, 2, 3, &buf));
    TEST_ASSERT_EQUAL_STRING("$01ab, $e87c, $d523", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xC026U, logical);
    string_release(&buf);
}

TEST(disasm, pce_data_string) {
    Context *context = (Context*)&g_context;

    String buf;
    string_init(&buf);

    context->map.memory[PCE_MEMORY_ROM].data[0x4020U] = 'h';
    context->map.memory[PCE_MEMORY_ROM].data[0x4021U] = 'e';
    context->map.memory[PCE_MEMORY_ROM].data[0x4022U] = 0xB9;
    context->map.memory[PCE_MEMORY_ROM].data[0x4023U] = 'l';
    context->map.memory[PCE_MEMORY_ROM].data[0x4024U] = 'o';
    context->map.memory[PCE_MEMORY_ROM].data[0x4025U] = 0xC5U;

    g_context.map.mpr[6] = 0x02U;
    uint16_t logical = 0xC020U;

    TEST_ASSERT_TRUE(pce_data_string(&context->map, &context->labels, &logical, 6, &buf));
    TEST_ASSERT_EQUAL_STRING("\"he\",$b9,\"lo\",$c5", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xC026U, logical);

    string_release(&buf);

    context->map.memory[PCE_MEMORY_ROM].data[0x4022U] = 'l';
    context->map.memory[PCE_MEMORY_ROM].data[0x4025U] = '!';
    logical = 0xC020U;
    TEST_ASSERT_TRUE(pce_data_string(&context->map, &context->labels, &logical, 6, &buf));
    TEST_ASSERT_EQUAL_STRING("\"hello!\"", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xC026U, logical);

    string_release(&buf);
}

TEST(disasm, pce_disasm_impl_code) {
    Context *context = (Context*)&g_context;

    Section section = {
        .code = {
            .base = {
                .type = SECTION_TYPE_CODE,
                .page = 0x00,
                .logical = 0xE0F3,
                .mpr = { 0xFF, 0xF8, 1, 2, 3, 4, 5, 0 },
                .offset = 0x0000,
                .size = 123,
                .output = string_view_from_literal("irq_reset.asm"),
                .name = string_view_from_literal("irq_reset"),
                .description = string_view_from_literal("Reset interrupt handler"),
            },
        },
    };

    TEST_ASSERT_EQUAL(1, section_repository_add(&context->sections, &section));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xE13A, 0, string_view_from_literal("init_mmap"), (StringView){0}));

    memory_map_mpr(&context->map, section.base.mpr);

    const uint8_t code[] = {
        0x78, 0xD4, 0xD8, 0xA2, 0xFF, 0x9A, 0x8A, 0x53,
        0x01, 0xA9, 0xF8, 0x53, 0x02, 0x44, 0x38, 0xA9,
        0x01, 0x53, 0x40, 0x20, 0x0F, 0xCB, 0x20, 0x60,
        0xC8, 0xAA, 0xF0, 0x09, 0x44, 0x29, 0xA9, 0x03,
    };
    size_t code_len = sizeof(code) / sizeof(code[0]);
    for(size_t i=0; i<code_len; i++) {
        context->map.memory[PCE_MEMORY_ROM].data[0x00F3+i] = code[i];
    }

    const char *expected =
        "sei  \n"
        "csh  \n"
        "cld  \n"
        "ldx  #$ff\n"
        "txs  \n"
        "txa  \n"
        "tam  #$00\n"
        "lda  #$f8\n"
        "tam  #$01\n"
        "bsr  init_mmap\n";

    String str = {0};
    uint16_t logical = 0xE0F3;

    for(size_t i=0; i<10; i++) {
        TEST_ASSERT_TRUE(pce_disasm_impl(context, 0, &logical, code_len, &str));
        TEST_ASSERT_TRUE(string_append_n(&str, '\n', 1));
    }
    TEST_ASSERT_EQUAL_STRING_LEN(expected, string_const_ptr(&str), str.length);
}

TEST(disasm, pce_disasm_impl_data_hex) {
    Context *context = (Context*)&g_context;
    Section section = {
        .data = {
            .base = {
                .type = SECTION_TYPE_DATA,
                .page = 0x01,
                .logical = 0xC06A,
                .mpr = { 0xFF, 0xF4, 0, 1, 2, 3, 4, 0x00 },
                .offset = 0x0000,
                .size = 32,
                .output = string_view_from_literal("data.asm"),
                .name = string_view_from_literal("d00"),
                .description = string_view_from_literal("data"),
            },
            .hex = {
                .type = DATA_TYPE_HEX,
                .element_size = 2,
                .elements_per_line = 4,
            },
        },
    };

    TEST_ASSERT_EQUAL(1, section_repository_add(&context->sections, &section));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xC06A, 4, string_view_from_literal("d00"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xC07C, 4, string_view_from_literal("d01"), (StringView){0}));

    memory_map_mpr(&context->map, section.base.mpr);

    const uint8_t data[] = {
        0x78, 0xD4, 0xD8, 0xA2, 0xFF, 0x9A, 0x8A, 0x53,
        0x01, 0xA9, 0xF8, 0x53, 0x02, 0x44, 0x38, 0xA9,
        0x01, 0x53, 0x40, 0x20, 0x0F, 0xCB, 0x20, 0x60,
        0xC8, 0xAA, 0xF0, 0x09, 0x44, 0x29, 0xA9, 0x03,
    };
    size_t data_len = sizeof(data) / sizeof(data[0]);
    for(size_t i=0; i<data_len; i++) {
        context->map.memory[PCE_MEMORY_ROM].data[(4 << 13) + (0xC06AU & 0x1FFFU) + i] = data[i];
    }

    const char *expected = "$d478, $a2d8, $9aff, $538a, $a901";

    String str = {0};
    uint16_t logical = 0xC06A;

    TEST_ASSERT_TRUE(pce_disasm_impl(context, 0, &logical, 5, &str));
    TEST_ASSERT_EQUAL_STRING_LEN(expected, string_const_ptr(&str), str.length);
}

TEST(disasm, pce_disasm_impl_data_string) {
    Context *context = (Context*)&g_context;
    Section section = {
        .data = {
            .base = {
                .type = SECTION_TYPE_DATA,
                .page = 0x01,
                .logical = 0xC06A,
                .mpr = { 0xFF, 0xF4, 0, 1, 2, 3, 4, 0x00 },
                .offset = 0x0000,
                .size = 32,
                .output = string_view_from_literal("data.asm"),
                .name = string_view_from_literal("s00"),
                .description = string_view_from_literal("data"),
            },
            .string = {
                .type = DATA_TYPE_STRING,
                .char_per_line = 10,
                .delimiter_size = 2,
                .delimiter = { [0]=0xFEU, [1]=0xFF },
            },
        },
    };

    TEST_ASSERT_EQUAL(1, section_repository_add(&context->sections, &section));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xC06A, 4, string_view_from_literal("s00"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xC07C, 4, string_view_from_literal("s01"), (StringView){0}));

    memory_map_mpr(&context->map, section.base.mpr);

    const uint8_t data[] = {
        'H', 'e', 'l', 'l', 'o', 0xFE, 0xFF, 'w', 'o', 'r', 'l', 'd', '!'
    };
    size_t data_len = sizeof(data) / sizeof(data[0]);
    for(size_t i=0; i<data_len; i++) {
        context->map.memory[PCE_MEMORY_ROM].data[(4 << 13) + (0xC06AU & 0x1FFFU) + i] = data[i];
    }

    const char *expected = "\"Hello\",$fe,$ff,\"wo\"";

    String str = {0};
    uint16_t logical = 0xC06A;

    TEST_ASSERT_TRUE(pce_disasm_impl(context, 0, &logical, 9, &str));
    TEST_ASSERT_EQUAL_STRING_LEN(expected, string_const_ptr(&str), str.length);
}

TEST(disasm, pce_disasm_impl_data_jump_table) {
    Context *context = (Context*)&g_context;
    Section section = {
        .data = {
            .base = {
                .type = SECTION_TYPE_DATA,
                .page = 0x01,
                .logical = 0xC06A,
                .mpr = { 0xFF, 0xF4, 0, 1, 2, 3, 4, 0x00 },
                .offset = 0x0000,
                .size = 12,
                .output = string_view_from_literal("data.asm"),
                .name = string_view_from_literal("j00"),
                .description = string_view_from_literal("data"),
            },
            .jump_table = {
                .type = DATA_TYPE_JUMP_TABLE,
                .elements_per_line = 10,
                .element_size = 2,
            },
        },
    };

    TEST_ASSERT_EQUAL(1, section_repository_add(&context->sections, &section));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xC06A, 4, string_view_from_literal("j00"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x612A, 1, string_view_from_literal("l_0"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x6135, 1, string_view_from_literal("l_1"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x6140, 1, string_view_from_literal("l_2"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x615F, 1, string_view_from_literal("l_3"), (StringView){0}));

    memory_map_mpr(&context->map, section.base.mpr);

    const uint8_t data[] = {
        0x2A, 0x61, 0x08, 0x22,
        0x35, 0x61, 0xc6, 0x30,
        0x40, 0x61, 0x5F, 0x61,
    };
    size_t data_len = sizeof(data) / sizeof(data[0]);
    for(size_t i=0; i<data_len; i++) {
        context->map.memory[PCE_MEMORY_ROM].data[(4 << 13) + (0xC06AU & 0x1FFFU) + i] = data[i];
    }

    const char *expected = "l_0, $2208, l_1, $30C6, l_2, l_3";

    String str = {0};
    uint16_t logical = 0xC06A;

    TEST_ASSERT_TRUE(pce_disasm_impl(context, 0, &logical, 6, &str));
    TEST_ASSERT_EQUAL_STRING_LEN(expected, string_const_ptr(&str), str.length);
}

#if 0
TEST(disasm, pce_data_count) {
    Context *context = (Context*)&g_context;

    const uint16_t start = 0x1000U;
    const uint8_t page = start >> 13U;
    const uint8_t mpr = 6;
    uint16_t logical = (mpr << 13) + (start & 0x1FFFFU);
    uint8_t byte;
    for(byte=0x20U; byte<=0x7EU; byte++) {
        context->map.memory[PCE_MEMORY_ROM].data[start+byte] = byte;
    }

    g_context.map.mpr[mpr] = page;

    TEST_ASSERT_TRUE(label_repository_add(&context->labels, logical, page, string_view_from_literal("l_0000"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, logical+'A', page, string_view_from_literal("l_0001"), string_view_from_literal("uppercase")));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, logical+'a', page, string_view_from_literal("l_0002"), string_view_from_literal("lowercase")));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, logical+'0', page, string_view_from_literal("l_0003"), string_view_from_literal("numbers")));

    TEST_ASSERT_TRUE(comment_repository_add(&context->comments, logical + 'A', page, string_view_from_literal("comment 0!")));
    TEST_ASSERT_TRUE(comment_repository_add(&context->comments, logical + 'G', page, string_view_from_literal("comment 1!")));
    TEST_ASSERT_TRUE(comment_repository_add(&context->comments, logical + 'M', page, string_view_from_literal("comment 2!")));

    int count = label_repository_size(&context->labels);

    printf("\n");

    uint16_t first = logical + 'H';
    uint16_t length = 10U; // [todo] compute from section
    uint16_t end;
    {
        Comment comment = {0};
        Label label = {0};

        bool has_label = label_repository_find(&context->labels, first, page, &label);
        bool has_comment = comment_repository_find(&context->comments, first, page, &comment);

        if(has_label) {
            printf("%.*s:", (int)label.name.length, label.name.data);
            if(has_comment) {
                printf("                ; %.*s", (int)comment.text.length, comment.text.data);
            }
            printf("\n");
        }

        end = first;
        if(has_label || has_comment) {
            label = (Label){0};
            comment = (Comment){0};
            end++;
        }

        uint16_t limit = first + length;
        for(; end<limit; end++) {
            Comment tmp = {0};
            if(comment_repository_find(&context->comments, end, page, &tmp)) {
                if(string_view_empty(comment.text)) {
                    comment = tmp;
                }
                break;
            }
            if(label_repository_find(&context->labels, end, page, &label)) {
                break;
            }
        }

        // [todo] output from section type
        for(uint16_t k=first; k<end; k++) {
            printf("%c", memory_map_read(&context->map, k));
        }

        if(!string_view_empty(comment.text)) {
            printf("                ; %.*s", (int)comment.text.length, comment.text.data);
        }
        printf("\n");
    }
}
#endif

TEST(disasm, pce_code_size) {
    Context *context = (Context*)&g_context;
    Section section[2] = {
        [0] = {
            .code = {
                .base = {
                    .type = SECTION_TYPE_CODE,
                    .page = 0x01,
                    .logical = 0xC000,
                    .mpr = {0xFF, 0xF4, 0, 1, 2, 3, 4, 0x0},
                    .offset = 0x0000,
                    .size = -1,
                    .output = string_view_from_literal("output"),
                    .name = string_view_from_literal("l00"),
                    .description = string_view_from_literal("description"),
                },
            },
        },
        [1] = {
            .code = {
                .base = {
                    .type = SECTION_TYPE_CODE,
                    .page = 0x01,
                    .logical = 0xC010,
                    .mpr = { 0, 1, 2, 3, 4, 5, 6, 7 },
                    .offset = 0x0000,
                    .size = -1,
                    .output = string_view_from_literal("output"),
                    .name = string_view_from_literal("l01"),
                    .description = string_view_from_literal("description"),
                },
            }
        },
    };

    TEST_ASSERT_EQUAL(1, section_repository_add(&context->sections, &section[0]));
    TEST_ASSERT_EQUAL(1, section_repository_add(&context->sections, &section[1]));

    memory_map_mpr(&context->map, section[0].base.mpr);

    const uint8_t data[] = {
        0x9F, 0x00, 0x06,           // BBS1 $00, ???
        0x60,                       // RTS
        0xEA,                       // NOP
        0x40,                       // RTI
        0xEA,                       // NOP
        0xEA,                       // NOP
        0x00,                       // BRK
        0x4C, 0x00, 0xC0,           // JMP 0xC000
        0xEA, 0xEA, 0xEA, 0xEA,     // NOP * 4
        0xEA, 0xEA, 0xEA, 0xEA,     // NOP * 4
        0x60,                       // RTS
        0xEA, 0xEA, 0xEA, 0xEA,     // NOP * 4
    };
    size_t data_len = sizeof(data) / sizeof(data[0]);
    for(size_t i=0; i<data_len; i++) {
        context->map.memory[PCE_MEMORY_ROM].data[(4 << 13) + (0xC000 & 0x1FFFU) + i] = data[i];
    }
    TEST_ASSERT_EQUAL_size_t(12, pce_code_size(context, 0));
    TEST_ASSERT_EQUAL_size_t(5, pce_code_size(context, 1));
}

TEST_GROUP_RUNNER(disasm) {
    RUN_TEST_CASE(disasm, pce_arg);
    RUN_TEST_CASE(disasm, pce_arg_list);
    RUN_TEST_CASE(disasm, pce_op_local_jump);
    RUN_TEST_CASE(disasm, pce_tag_mmap_test);
    RUN_TEST_CASE(disasm, pce_code);
    RUN_TEST_CASE(disasm, pce_jump_table);
    RUN_TEST_CASE(disasm, pce_data_hex);
    RUN_TEST_CASE(disasm, pce_data_string);
    RUN_TEST_CASE(disasm, pce_disasm_impl_code);
    RUN_TEST_CASE(disasm, pce_disasm_impl_data_hex);
    RUN_TEST_CASE(disasm, pce_disasm_impl_data_string);
    RUN_TEST_CASE(disasm, pce_disasm_impl_data_jump_table);
// [todo]    RUN_TEST_CASE(disasm, pce_data_count);
    RUN_TEST_CASE(disasm, pce_code_size);
}

static void run_all_tests(void) {
    RUN_TEST_GROUP(disasm);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}
