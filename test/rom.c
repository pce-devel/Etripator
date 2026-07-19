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
#include <etripator/data.h>
#include <etripator/rom.h>

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC_VARARG(message_print, MessageType, const char*, size_t, const char*, const char*, ...);

MemoryMap g_map;

TEST_GROUP(rom);

TEST_SETUP(rom) {
    RESET_FAKE(message_print);
    FFF_RESET_HISTORY();
    TEST_ASSERT_TRUE(memory_map_init(&g_map));
}

TEST_TEAR_DOWN(rom) {
    memory_map_destroy(&g_map);
}

TEST(rom, create_0x60000) {
    TEST_ASSERT_FALSE(rom_create(&g_map, 0));
    TEST_ASSERT_TRUE(rom_create(&g_map, 0x60000U));
    for (unsigned int i = 0; i < 64; i++) {
        TEST_ASSERT_EQUAL_INT(PCE_MEMORY_ROM, g_map.page[i].id);
        TEST_ASSERT_EQUAL_size_t(i & 0x1FU, g_map.page[i].bank);
    }
    for (unsigned int i = 64; i < 128; i++) {
        TEST_ASSERT_EQUAL_INT(PCE_MEMORY_ROM, g_map.page[i].id);
        TEST_ASSERT_EQUAL_size_t((i & 0x0FU) + 32, g_map.page[i].bank);
    }
}

TEST(rom, create_0x80000) {
    TEST_ASSERT_FALSE(rom_create(&g_map, 0));
    TEST_ASSERT_TRUE(rom_create(&g_map, 0x80000U));
    for (unsigned int i = 0; i < 64; i++) {
        TEST_ASSERT_EQUAL_INT(PCE_MEMORY_ROM, g_map.page[i].id);
        TEST_ASSERT_EQUAL_size_t(i & 0x3FU, g_map.page[i].bank);
    }
    for (unsigned int i = 64; i < 128; i++) {
        TEST_ASSERT_EQUAL_INT(PCE_MEMORY_ROM, g_map.page[i].id);
        TEST_ASSERT_EQUAL_size_t((i & 0x1FU) + 32, g_map.page[i].bank);
    }
}

TEST(rom, create_0xA0000) { 
    TEST_ASSERT_FALSE(rom_create(&g_map, 0));
    TEST_ASSERT_TRUE(rom_create(&g_map, 0xA0000U));
    for (unsigned int i = 0; i < 128; i++) {
        TEST_ASSERT_EQUAL_INT(PCE_MEMORY_ROM, g_map.page[i].id);
        TEST_ASSERT_EQUAL_size_t(i % (g_map.memory[PCE_MEMORY_ROM].length / PCE_BANK_SIZE), g_map.page[i].bank);
    }
}

TEST(rom, load) { 
    size_t rom_size = 512 + (4*8192); // 32KB + 512B header
    uint8_t *rom_data = (uint8_t*)malloc(rom_size);

    // fake header
    memset(rom_data, 'H', 512U);
    // fake rom data
    for(int i=0; i<4; i++) {
        memset(rom_data+512+(i*8192), '0'+i, 8192); 
    }

    Data *data = data_buffer_create(rom_data, rom_size);

    TEST_ASSERT_TRUE(rom_load(&g_map, data));

    data_release(data);
    free(rom_data);

    g_map.mpr[0] = 0xFFU;
    g_map.mpr[1] = 0xF8U;
    g_map.mpr[2] = 0x01U;
    g_map.mpr[3] = 0x02U;
    g_map.mpr[4] = 0x03U;
    g_map.mpr[7] = 0x00U;  
    
    for(uint16_t addr = 0; addr<0x2000; addr++) {
        TEST_ASSERT_EQUAL_UINT8('0', memory_map_read(&g_map, 0xE000U + addr));
        TEST_ASSERT_EQUAL_UINT8('1', memory_map_read(&g_map, 0x4000U + addr));
        TEST_ASSERT_EQUAL_UINT8('2', memory_map_read(&g_map, 0x6000U + addr));
        TEST_ASSERT_EQUAL_UINT8('3', memory_map_read(&g_map, 0x8000U + addr));
    }
}

TEST_GROUP_RUNNER(rom) {
    RUN_TEST_CASE(rom, create_0x60000);
    RUN_TEST_CASE(rom, create_0x80000);
    RUN_TEST_CASE(rom, create_0xA0000);
    RUN_TEST_CASE(rom, load);
}

static void run_all_tests(void) {
    RUN_TEST_GROUP(rom);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}