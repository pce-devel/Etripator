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

#include "../ipl.c"

#include "message.h"
#include "message/console.h"

static uint8_t g_hcd8004_ipl[] = {
    0x00, 0x00, 0x02, 0x04, 0x00, 0x40, 0x00, 0x40, 0x00, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x50, 0x43, 0x20, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65, 0x20, 0x43, 0x44, 0x2D, 0x52, 0x4F, 0x4D, 
    0x20, 0x53, 0x59, 0x53, 0x54, 0x45, 0x4D, 0x00, 0x43, 0x6F, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68, 
    0x74, 0x20, 0x48, 0x55, 0x44, 0x53, 0x4F, 0x4E, 0x20, 0x53, 0x4F, 0x46, 0x54, 0x20, 0x2F, 0x20, 
    0x4E, 0x45, 0x43, 0x20, 0x48, 0x6F, 0x6D, 0x65, 0x20, 0x45, 0x6C, 0x65, 0x63, 0x74, 0x72, 0x6F, 
    0x6E, 0x69, 0x63, 0x73, 0x2C, 0x4C, 0x74, 0x64, 0x2E, 0x00, 0x53, 0x70, 0x61, 0x63, 0x65, 0x20, 
    0x41, 0x64, 0x76, 0x65, 0x6E, 0x74, 0x75, 0x72, 0x65, 0x20, 0x43, 0x4F, 0x42, 0x52, 0x41, 0x20, 
};

void* setup(const MunitParameter params[] __attribute__((unused)), void* user_data __attribute__((unused))) {
    message_printer_init();    
    console_message_printer_init();
    return NULL;
}

void tear_down(void* fixture __attribute__((unused))) {
    message_printer_destroy();
}

MunitResult ipl_read_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    IPL ipl = {0};
    const char *filename = "data/HCD8004.ipl";

    FILE *in = fmemopen(g_hcd8004_ipl, sizeof(g_hcd8004_ipl), "r");
    
    munit_assert_not_null(in);
    munit_assert_true(ipl_read_header(&ipl, in, filename));
    munit_assert_int(fclose(in), ==, 0);

    munit_assert_uint8(ipl.load_start_record[0], ==, 0x00U);
    munit_assert_uint8(ipl.load_start_record[1], ==, 0x00U);
    munit_assert_uint8(ipl.load_start_record[2], ==, 0x02U);

    munit_assert_uint8(ipl.load_sector_count, ==, 0x04U);

    munit_assert_uint8(ipl.load_store_address[0], ==, 0x00U);
    munit_assert_uint8(ipl.load_store_address[1], ==, 0x40U);

    munit_assert_uint8(ipl.load_exec_address[0], ==, 0x00U);
    munit_assert_uint8(ipl.load_exec_address[1], ==, 0x40U);

    munit_assert_uint8(ipl.mpr[0], ==, 0x00U);
    munit_assert_uint8(ipl.mpr[1], ==, 0x01U);
    munit_assert_uint8(ipl.mpr[2], ==, 0x02U);
    munit_assert_uint8(ipl.mpr[3], ==, 0x03U);
    munit_assert_uint8(ipl.mpr[4], ==, 0x04U);

    munit_assert_uint8(ipl.opening_mode, ==, 0x00U);

    munit_assert_uint8(ipl.opening_gfx_record[0], ==, 0x00U);
    munit_assert_uint8(ipl.opening_gfx_record[1], ==, 0x00U);
    munit_assert_uint8(ipl.opening_gfx_record[2], ==, 0x00U);

    munit_assert_uint8(ipl.opening_gfx_sector_count, ==, 0x00U);

    munit_assert_uint8(ipl.opening_gfx_read_address[0], ==, 0x00U);
    munit_assert_uint8(ipl.opening_gfx_read_address[1], ==, 0x00U);

    munit_assert_uint8(ipl.opening_adpcm_record[0], ==, 0x00U);
    munit_assert_uint8(ipl.opening_adpcm_record[1], ==, 0x00U);
    munit_assert_uint8(ipl.opening_adpcm_record[2], ==, 0x00U);

    munit_assert_uint8(ipl.opening_adpcm_sector_count, ==, 0x00U);

    munit_assert_uint8(ipl.opening_adpcm_sampling_rate, ==, 0x00U);

    for(unsigned int i=0; i<7; i++) {
        munit_assert_uint8(ipl.reserved[i], ==, 0x00U);
    }

    munit_assert_memory_equal(24U, ipl.id, "PC Engine CD-ROM SYSTEM");

    munit_assert_memory_equal(50U, ipl.legal, "Copyright HUDSON SOFT / NEC Home Electronics,Ltd.");

    munit_assert_memory_equal(16U, ipl.program_name, "Space Adventure ");

    munit_assert_memory_equal(6U, ipl.extra, "COBRA ");

    return MUNIT_OK;
}

static MunitTest g_ipl_tests[] = {
    { "/read", ipl_read_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite g_ipl_test_suite = {
    "Label test suite", g_ipl_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&g_ipl_test_suite, NULL, argc, argv);
}