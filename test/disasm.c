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

typedef struct {
    MemoryMap map;
    LabelRepository labels;
} Context;

static Context g_context = {0};

TEST_GROUP(disasm);

TEST_SETUP(disasm) {
    RESET_FAKE(message_print);
    FFF_RESET_HISTORY();
    
    g_context = (Context) {0};
    TEST_ASSERT_TRUE_MESSAGE(memory_map_init(&g_context.map), "failed to initialize memory map");
    TEST_ASSERT_TRUE_MESSAGE(rom_create(&g_context.map, 0x60000U), "failed to create ROM memory map");
    TEST_ASSERT_TRUE_MESSAGE(label_repository_create(&g_context.labels), "failed to create label repository");
    g_context.map.mpr[0] = 0xFFU;
    g_context.map.mpr[1] = 0xF8U;
    g_context.map.mpr[7] = 0x00U;
}

TEST_TEAR_DOWN(disasm) {
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

TEST(disasm, pce_disasm) {
    Context *context = (Context*)&g_context;

    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x200AU, 0xF8U, string_view_from_literal("zp0"), string_view_from_literal("zp var 0")));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0x200CU, 0xF8U, string_view_from_literal("zp1"), string_view_from_literal("zp var 1")));
    TEST_ASSERT_TRUE(label_repository_add(&context->labels, 0xEC06U, 0x00U, string_view_from_literal("dat0"), string_view_from_literal("rom data 0")));

    String buf;
    string_init(&buf);

    // brk
    uint16_t logical = 0xE000U;
    TEST_ASSERT_TRUE(pce_disasm(&context->map, &context->labels, &logical, &buf));
    TEST_ASSERT_EQUAL_STRING("bbs7 <$ff,$e000", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xE003U, logical);
    string_release(&buf);

    // bra dat 0
    context->map.memory[PCE_MEMORY_ROM].data[0x0C4AU] = 0x80U;
    context->map.memory[PCE_MEMORY_ROM].data[0x0C4BU] = 0xBBU;

    logical = 0xEC4AU;
    TEST_ASSERT_TRUE(pce_disasm(&context->map, &context->labels, &logical, &buf));
    TEST_ASSERT_EQUAL_STRING("bra  dat0", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xEC4CU, logical);
    string_release(&buf);

    // tam 7
    context->map.memory[PCE_MEMORY_ROM].data[0x0000U] = 0x53U;
    context->map.memory[PCE_MEMORY_ROM].data[0x0001U] = 0x80U;

    logical = 0xE000U;
    TEST_ASSERT_TRUE(pce_disasm(&context->map, &context->labels, &logical, &buf));
    TEST_ASSERT_EQUAL_STRING("tam  #$07", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xE002U, logical);
    string_release(&buf);

    // unknown instruction
    context->map.memory[PCE_MEMORY_ROM].data[0x0002U] = 0xCBU;
    TEST_ASSERT_TRUE(pce_disasm(&context->map, &context->labels, &logical, &buf));
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
    TEST_ASSERT_TRUE(pce_jump_table(&context->map, &context->labels, &logical, &buf));
    TEST_ASSERT_EQUAL_STRING("l02", string_ptr(&buf));
    TEST_ASSERT_EQUAL_UINT16(0xC022U, logical);

    string_release(&buf);

    TEST_ASSERT_TRUE(pce_jump_table(&context->map, &context->labels, &logical, &buf));
    TEST_ASSERT_EQUAL_STRING("$ea70", string_ptr(&buf));
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

TEST_GROUP_RUNNER(disasm) {
    RUN_TEST_CASE(disasm, pce_arg);
    RUN_TEST_CASE(disasm, pce_arg_list);
    RUN_TEST_CASE(disasm, pce_op_local_jump);
    RUN_TEST_CASE(disasm, pce_tag_mmap_test);
    RUN_TEST_CASE(disasm, pce_disasm);
    RUN_TEST_CASE(disasm, pce_jump_table);
    RUN_TEST_CASE(disasm, pce_data_hex);
    RUN_TEST_CASE(disasm, pce_data_string);
}

static void run_all_tests(void) {
    RUN_TEST_GROUP(disasm);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}
