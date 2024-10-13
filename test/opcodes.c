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
#include "opcodes.h"
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

MunitResult opcodes_get_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    const Opcode* op;
    
    op = opcode_get(0x00);
    munit_assert_not_null(op);
    munit_assert_string_equal(op->name, "brk ");
    munit_assert_uint8(op->size, ==, 1U);
    munit_assert_uint8(op->type, ==, PCE_OP);

    op = opcode_get(0xFF);
    munit_assert_not_null(op);
    munit_assert_string_equal(op->name, "bbs7");
    munit_assert_uint8(op->size, ==, 3U);
    munit_assert_uint8(op->type, ==, PCE_OP_ZZ_lbl);

    op = opcode_get(0x2B);
    munit_assert_not_null(op);
    munit_assert_string_equal(op->name, ".db ");
    munit_assert_uint8(op->size, ==, 1U);
    munit_assert_uint8(op->type, ==, PCE_unknown);

    return MUNIT_OK;
}

MunitResult opcodes_format_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    const Opcode* op;
    unsigned int i;

    // tii
    op = opcode_get(0x73);
    munit_assert_uint8(op->type, ==, PCE_OP_shsl_dhdl_hhll);
    for(i=0; i<(op->size-1); i++) { // skip opcode
        munit_assert_not_null(opcode_format(op, i));
    }
    for(; i<256; i++) {
        munit_assert_null(opcode_format(op, i));
    }

    return MUNIT_OK;
}

MunitResult opcodes_jump_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    // local jump
    for(unsigned int i=0x0FU; i<0x100U; i+=0x10U) {
        munit_assert_true(opcode_is_local_jump(i)); // BBR* and BBS*
    }
    munit_assert_true(opcode_is_local_jump(0x90U)); // BCC
    munit_assert_true(opcode_is_local_jump(0xB0U)); // BCS
    munit_assert_true(opcode_is_local_jump(0x80U)); // BRA
    munit_assert_true(opcode_is_local_jump(0xF0U)); // BEQ
    munit_assert_true(opcode_is_local_jump(0x30U)); // BMI
    munit_assert_true(opcode_is_local_jump(0xD0U)); // BNE
    munit_assert_true(opcode_is_local_jump(0x10U)); // BPL
    munit_assert_true(opcode_is_local_jump(0x44U)); // BSR
    munit_assert_true(opcode_is_local_jump(0x50U)); // BVC
    munit_assert_true(opcode_is_local_jump(0x70U)); // BVS

    munit_assert_false(opcode_is_local_jump(0xEAU)); // NO
    munit_assert_false(opcode_is_local_jump(0x4CU)); // JMP
    munit_assert_false(opcode_is_local_jump(0x20U)); // JSP

    // far jump
    for(unsigned int i=0x0FU; i<0x100U; i+=0x10U) {
        munit_assert_false(opcode_is_far_jump(i)); // BBR* and BBS*
    }
    munit_assert_false(opcode_is_far_jump(0x90U)); // BCC
    munit_assert_false(opcode_is_far_jump(0xB0U)); // BCS
    munit_assert_false(opcode_is_far_jump(0x80U)); // BRA
    munit_assert_false(opcode_is_far_jump(0xF0U)); // BEQ
    munit_assert_false(opcode_is_far_jump(0x30U)); // BMI
    munit_assert_false(opcode_is_far_jump(0xD0U)); // BNE
    munit_assert_false(opcode_is_far_jump(0x10U)); // BPL
    munit_assert_false(opcode_is_far_jump(0x44U)); // BSR
    munit_assert_false(opcode_is_far_jump(0x50U)); // BVC
    munit_assert_false(opcode_is_far_jump(0x70U)); // BVS

    munit_assert_false(opcode_is_far_jump(0xEAU)); // NOP
    munit_assert_true(opcode_is_far_jump(0x4CU)); // JMP
    munit_assert_true(opcode_is_far_jump(0x20U)); // JSR
    
    return MUNIT_OK;
}

static MunitTest opcodes_tests[] = {
    { "/get", opcodes_get_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/format", opcodes_format_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/jump", opcodes_jump_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite opcodes_suite = {
    "Opcodes test suite", opcodes_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&opcodes_suite, NULL, argc, argv);
}