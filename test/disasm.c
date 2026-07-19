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
#include <munit.h>
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

static void tear_down(void* fixture) {
    Context *context = (Context*)fixture;
    if(context != NULL) {
        label_repository_destroy(&context->labels);
        memory_map_destroy(&context->map);
    }
    free(fixture);
}

static void* setup(const MunitParameter params[] __unused, void* user_data __unused) {
    RESET_FAKE(message_print);
    FFF_RESET_HISTORY();
    
    Context *context = calloc(1, sizeof(Context));
    if(context == NULL) {
        munit_log(MUNIT_LOG_ERROR, "failed to alloc context");
        return NULL;
    }

    void *out = NULL;
    if(memory_map_init(&context->map) != true) {
        munit_log(MUNIT_LOG_ERROR, "failed to initialize memory map");
    } else if(rom_create(&context->map, 0x60000U) != true) {
        munit_log(MUNIT_LOG_ERROR, "failed to create ROM memory map");
    } else if(label_repository_create(&context->labels) != true) {
        munit_log(MUNIT_LOG_ERROR, "failed to create label repository");
    } else {
        context->map.mpr[0] = 0xFFU;
        context->map.mpr[1] = 0xF8U;
        context->map.mpr[7] = 0x00U;        
        out = context;
    }

    if(out == NULL) {
        tear_down(context);
    }

    return out;
}

MunitResult pce_arg_test(const MunitParameter params[] __unused, void* fixture) {
    Context *context = (Context*)fixture;

    munit_assert_true(label_repository_add(&context->labels, 0x200AU, 0xF8U, "zp0", "zp var 0"));
    munit_assert_true(label_repository_add(&context->labels, 0x200CU, 0xF8U, "zp1", "zp var 1"));
    munit_assert_true(label_repository_add(&context->labels, 0xEC06U, 0x00U, "dat0", "rom data 0"));

    String buf;
    string_init(&buf);

    uint8_t data[2];

    munit_assert_true(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_A, data));
    munit_assert_string_equal(string_ptr(&buf), "A");
    string_release(&buf);

    munit_assert_true(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_X, data));
    munit_assert_string_equal(string_ptr(&buf), "X");
    string_release(&buf);

    munit_assert_true(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_Y, data));
    munit_assert_string_equal(string_ptr(&buf), "Y");
    string_release(&buf);

    data[0] = 0x5FU;
    munit_assert_true(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_ABS, data));
    munit_assert_string_equal(string_ptr(&buf), "#$5f");
    string_release(&buf);

    data[0] = 0x20U;
    munit_assert_true(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_ZP, data));
    munit_assert_string_equal(string_ptr(&buf), "<$20");
    string_release(&buf);

    data[0] = 0x0AU;
    munit_assert_true(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_ZP, data));
    munit_assert_string_equal(string_ptr(&buf), "<zp0");
    string_release(&buf);

    data[0] = 0x21U;
    data[1] = 0x34U;
    munit_assert_true(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_HHLL, data));
    munit_assert_string_equal(string_ptr(&buf), "$3421");
    string_release(&buf);

    data[0] = 0x06U;
    data[1] = 0xECU;
    munit_assert_true(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_HHLL, data));
    munit_assert_string_equal(string_ptr(&buf), "dat0");
    string_release(&buf);

    munit_assert_false(pce_arg(&context->map, &context->labels, &buf, PCE_ARG_NONE, data));
    munit_assert_string_equal(string_ptr(&buf), "");
    string_release(&buf);

    return MUNIT_OK;
}

MunitResult pce_arg_list_test(const MunitParameter params[] __unused, void* fixture) {
    Context *context = (Context*)fixture;

    munit_assert_true(label_repository_add(&context->labels, 0x200AU, 0xF8U, "zp0", "zp var 0"));
    munit_assert_true(label_repository_add(&context->labels, 0x200CU, 0xF8U, "zp1", "zp var 1"));
    munit_assert_true(label_repository_add(&context->labels, 0xEC06U, 0x00U, "dat0", "rom data 0"));

    String buf;
    string_init(&buf);

    uint8_t data[] = { 0x0CU, 0x06U, 0xECU, 0x7aU, 0x00, 0x00, 0x00, 0x00};
    size_t length = 8;

    munit_assert_true(pce_arg_list(&context->map, &context->labels, &buf, PCE_OP_ZZ_IND_Y, data, length));
    munit_assert_string_equal(string_ptr(&buf), "[<zp1],Y");
    string_release(&buf);

    munit_assert_true(pce_arg_list(&context->map, &context->labels, &buf, PCE_OP_HHLL_X_IND, data+1, length));
    munit_assert_string_equal(string_ptr(&buf), "[dat0,X]");
    string_release(&buf);

    munit_assert_true(pce_arg_list(&context->map, &context->labels, &buf, PCE_OP_ABS_HHLL_X, data, length));
    munit_assert_string_equal(string_ptr(&buf), "#$0c,dat0,X");
    string_release(&buf);

    return MUNIT_OK;
}

MunitResult pce_op_local_jump_test(const MunitParameter params[] __unused, void* fixture __unused) {
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
            munit_assert_true(g_pce_opcode[op].tag == PCE_TAG_LOCAL_JUMP);
        } else {
            munit_assert_true(g_pce_opcode[op].tag != PCE_TAG_LOCAL_JUMP);
        }
    }
    return MUNIT_OK;
}

MunitResult pce_tag_local_jump_test(const MunitParameter params[] __unused, void* fixture __unused) {
    uint8_t data[8] = {0};
    size_t size;

    data[0] = 0x80U;
    data[1] = 0xE8U;
    size = 1;
    pce_tag_local_jump(0xFF7DU, data, &size);
    munit_assert_uint8(data[1], ==, 0x65U);
    munit_assert_uint8(data[2], ==, 0xFFU);
    munit_assert_size(size, ==, 2);

    data[1] = 0x06U;
    size = 1;
    pce_tag_local_jump(0xFF9CU, data, &size);
    munit_assert_uint16(data[1], ==, 0xA2U);
    munit_assert_uint16(data[2], ==, 0xFFU);
    munit_assert_size(size, ==, 2);
    
    data[1] = 0xBBU;
    size = 1;
    pce_tag_local_jump(0xEC4BU, data, &size);
    munit_assert_uint16(data[1], ==, 0x06U);
    munit_assert_uint16(data[2], ==, 0xECU);
    munit_assert_size(size, ==, 2);

    return MUNIT_OK;
}

MunitResult pce_tag_mmap_test(const MunitParameter params[] __unused, void* fixture __unused) {
    uint8_t data[8] = {0};
    size_t size = 1;
    pce_tag_mmap(0, data, &size);
    munit_assert_uint8(data[1], ==, 0);
    munit_assert_size(size, ==, 1);
    for(uint8_t i=0; i<8; i++) {
        uint8_t j = (1<<i);
        uint8_t k = j | (j-1);
        for(uint16_t v=j; v<=k; v++) {
            data[1] = v;
            size = 1;
            pce_tag_mmap(0, data, &size);
            munit_assert_uint8(data[1], ==, i);
            munit_assert_size(size, ==, 1);
        }
    }
    return MUNIT_OK;
}

MunitResult pce_disasm_test(const MunitParameter params[] __unused, void* fixture) {
    Context *context = (Context*)fixture;

    munit_assert_true(label_repository_add(&context->labels, 0x200AU, 0xF8U, "zp0", "zp var 0"));
    munit_assert_true(label_repository_add(&context->labels, 0x200CU, 0xF8U, "zp1", "zp var 1"));
    munit_assert_true(label_repository_add(&context->labels, 0xEC06U, 0x00U, "dat0", "rom data 0"));

    String buf;
    string_init(&buf);

    // brk
    uint16_t logical = 0xE000U;
    munit_assert_true(pce_disasm(&context->map, &context->labels, &logical, &buf));
    munit_assert_string_equal(string_ptr(&buf), "bbs7 <$ff,$e000");
    munit_assert_uint16(logical, ==, 0xE003U);
    string_release(&buf);

    // bra dat 0
    context->map.memory[PCE_MEMORY_ROM].data[0x0C4AU] = 0x80U;
    context->map.memory[PCE_MEMORY_ROM].data[0x0C4BU] = 0xBBU;

    logical = 0xEC4AU;
    munit_assert_true(pce_disasm(&context->map, &context->labels, &logical, &buf));
    munit_assert_string_equal(string_ptr(&buf), "bra  dat0");
    munit_assert_uint16(logical, ==, 0xEC4CU);
    string_release(&buf);


    // tam 7
    context->map.memory[PCE_MEMORY_ROM].data[0x0000U] = 0x53U;
    context->map.memory[PCE_MEMORY_ROM].data[0x0001U] = 0x80U;

    logical = 0xE000U;
    munit_assert_true(pce_disasm(&context->map, &context->labels, &logical, &buf));
    munit_assert_string_equal(string_ptr(&buf), "tam  #$07");
    munit_assert_uint16(logical, ==, 0xE002U);
    string_release(&buf);

    // unknown instruction
    context->map.memory[PCE_MEMORY_ROM].data[0x0002U] = 0xCBU;
    munit_assert_true(pce_disasm(&context->map, &context->labels, &logical, &buf));
    munit_assert_string_equal(string_ptr(&buf), ".db  #$cb");
    munit_assert_uint16(logical, ==, 0xE003U);
    string_release(&buf);

    // [todo] simple program

    return MUNIT_OK;
}

static MunitTest opcodes_tests[] = {
    { "/pce/arg", pce_arg_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/pce/arg_list", pce_arg_list_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL},
    { "/pce/tag_local_jump", pce_tag_local_jump_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    { "/pce/tag_mmap", pce_tag_mmap_test, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    { "/pce/disasm", pce_disasm_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL},
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite opcodes_suite = {
    "Opcodes test suite", opcodes_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&opcodes_suite, NULL, argc, argv);
}