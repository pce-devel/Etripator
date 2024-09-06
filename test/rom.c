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
  copyright (c) 2009--2023 Vincent Cruz.
 
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

#include <message.h>
#include <message/console.h>

#include <rom.h>

DEFINE_FFF_GLOBALS;

// [NOTE] munit is using fileno. If a test fails, munit will loop indefinitely :/

FAKE_VALUE_FUNC(FILE*, __wrap_fopen, const char*, const char*)
FAKE_VALUE_FUNC(int, __wrap_fileno, FILE*)
FAKE_VALUE_FUNC(int, __wrap_fstat, int, struct stat*)
FAKE_VALUE_FUNC(int, __wrap_fseek, FILE*, long, int)
FAKE_VALUE_FUNC(size_t, __wrap_fread, void*, size_t, size_t, FILE*)
FAKE_VALUE_FUNC(int, __wrap_fclose, FILE*)

static int fstat_empty_file(int fd __attribute__((unused)), struct stat* infos) {
    memset(infos, 0, sizeof(struct stat));
    infos->st_size = 0;
    return 0;
}

static size_t g_dummy_file_size;

static int fstat_dummy_file(int fd __attribute__((unused)), struct stat* infos) {
    infos->st_size = g_dummy_file_size;
    return 0;
}

static size_t fread_dummy(void* out, size_t size, size_t nmemb, FILE* in __attribute__((unused))) {
    uint8_t *ptr = (uint8_t*)out;
    uint8_t b = 0;
    for(size_t j=0; j<nmemb; j++) {
        for(size_t i=0; i<size; i++) {
            *ptr++ = b++;
        }
    }
    return size*nmemb;
}

MemoryMap g_map;

void* setup(const MunitParameter params[] __attribute__((unused)), void* user_data __attribute__((unused))) {
    RESET_FAKE(__wrap_fopen);
    RESET_FAKE(__wrap_fileno);
    RESET_FAKE(__wrap_fstat);
    RESET_FAKE(__wrap_fseek);
    RESET_FAKE(__wrap_fread);
    RESET_FAKE(__wrap_fclose);

    munit_assert_true(memory_map_init(&g_map));

    return NULL;
}

void tear_down(void* fixture __attribute__((unused))) {
    memory_map_destroy(&g_map);
}

MunitResult rom_load_small_test(const MunitParameter params[] __attribute__((unused)), void* fixture __attribute__((unused))) {
    __wrap_fopen_fake.return_val = NULL;
    __wrap_fileno_fake.return_val = -1;
    __wrap_fstat_fake.return_val = -1;
    __wrap_fseek_fake.return_val = -1;
    __wrap_fread_fake.return_val = 0;
    __wrap_fclose_fake.return_val = -1;
    
    munit_assert_false(rom_load("dummy.pce", &g_map));

    __wrap_fopen_fake.return_val = stdin;
    munit_assert_false(rom_load("dummy.pce", &g_map));

    __wrap_fileno_fake.return_val = 0;
    munit_assert_false(rom_load("dummy.pce", &g_map));

    __wrap_fstat_fake.custom_fake = fstat_empty_file;
    munit_assert_false(rom_load("dummy.pce", &g_map));

    g_dummy_file_size = 10U;
    __wrap_fstat_fake.custom_fake = fstat_dummy_file;
    munit_assert_false(rom_load("dummy.pce", &g_map));

    __wrap_fseek_fake.return_val = 0;
    munit_assert_false(rom_load("dummy.pce", &g_map));

    __wrap_fread_fake.custom_fake = fread_dummy;
    __wrap_fclose_fake.return_val = 0;
    munit_assert_true(rom_load("dummy.pce", &g_map));
    munit_assert_not_null(g_map.memory[PCE_MEMORY_ROM].data);
    munit_assert_size(g_map.memory[PCE_MEMORY_ROM].length, ==, PCE_BANK_SIZE);
    for (size_t i = 0; i < 128; i++) {
        munit_assert_int(g_map.page[i].id, ==, PCE_MEMORY_ROM);
        munit_assert_size(g_map.page[i].bank, ==, 0);
    }
 
    return MUNIT_OK;
}

// [todo] test header jump
// [todo] test ROM > 8kb

static MunitTest rom_tests[] = {
    { "/load/small", rom_load_small_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite rom_suite = {
    "ROM test suite", rom_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    message_printer_init();    
    console_message_printer_init();

    int ret = munit_suite_main(&rom_suite, NULL, argc, argv);

    message_printer_destroy();

    return ret;
}