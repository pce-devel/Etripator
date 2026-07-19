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
#include <etripator/memory.h>

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC_VARARG(message_print, MessageType, const char*, size_t, const char*, const char*, ...);

TEST_GROUP(memory);

TEST_SETUP(memory) {
    RESET_FAKE(message_print);
    FFF_RESET_HISTORY();
}

TEST_TEAR_DOWN(memory) {
}

TEST(memory, create) {
    Memory mem = {0};

    mem.data = NULL;
    mem.length = 0xCAFEU;
    TEST_ASSERT_FALSE(memory_create(&mem, 0U));
    TEST_ASSERT_EQUAL_size_t(0xCAFEU, mem.length);
    TEST_ASSERT_NULL(mem.data);

    TEST_ASSERT_TRUE(memory_create(&mem, 32U));
    TEST_ASSERT_EQUAL_size_t(32U, mem.length);
    TEST_ASSERT_NOT_NULL(mem.data);

    memory_destroy(&mem);
    TEST_ASSERT_EQUAL_size_t(0U, mem.length);
    TEST_ASSERT_NULL(mem.data);
}

TEST(memory, fill) {
    Memory mem = {0};

    TEST_ASSERT_FALSE(memory_fill(&mem, 0x7C));

    TEST_ASSERT_TRUE(memory_create(&mem, 256U));
    TEST_ASSERT_EQUAL_size_t(256U, mem.length);
    TEST_ASSERT_NOT_NULL(mem.data);

    TEST_ASSERT_TRUE(memory_fill(&mem, 0x7C));
    TEST_ASSERT_EACH_EQUAL_UINT8(0x7C, mem.data, mem.length);

    TEST_ASSERT_TRUE(memory_fill(&mem, 0xA0));
    TEST_ASSERT_EACH_EQUAL_UINT8(0xA0, mem.data, mem.length);

    memory_destroy(&mem);
}

TEST_GROUP_RUNNER(memory) {
    RUN_TEST_CASE(memory, create);
    RUN_TEST_CASE(memory, fill);
}

static void run_all_tests(void) {
    RUN_TEST_GROUP(memory);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}