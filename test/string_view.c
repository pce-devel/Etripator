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

#include <etripator/string_view.h>

TEST_GROUP(string_view);

TEST_SETUP(string_view) {
}

TEST_TEAR_DOWN(string_view) {
}

TEST(string_view, empty) {
    StringView s;
    s.data = "   something something... ";
    s.length = strlen(s.data);
    TEST_ASSERT_FALSE(string_view_empty(s));

    s.data = NULL;
    TEST_ASSERT_TRUE(string_view_empty(s));

    s.data = "what?";
    s.length = 0;
    TEST_ASSERT_TRUE(string_view_empty(s));

    s = (StringView) {0};
    TEST_ASSERT_TRUE(string_view_empty(s));
}

TEST(string_view, cmp) {
    StringView s0;
    StringView s1;
    
    s0 = string_view_from_literal(" bleep! ");
    s1 = string_view_from_substring(" bleep!  #", 8U);
    TEST_ASSERT_TRUE(string_view_cmp(s0, s1));

    s1 = string_view_from_literal(" bleep? ");
    TEST_ASSERT_FALSE(string_view_cmp(s0, s1));

    s0 = string_view_from_literal(" bleep!  bloop! ");
    TEST_ASSERT_FALSE(string_view_cmp(s0, s1));
}

TEST(string_view, case_cmp) {
    StringView s0;
    StringView s1;
    
    s0 = string_view_from_literal(" bleep! ");
    s1 = string_view_from_substring(" blEeP!  #", 8U);
    TEST_ASSERT_TRUE(string_view_case_cmp(s0, s1));

    s1 = string_view_from_literal(" blEAp! ");
    TEST_ASSERT_FALSE(string_view_case_cmp(s0, s1));

    s0 = string_view_from_literal(" bleep!  bloop! ");
    TEST_ASSERT_FALSE(string_view_case_cmp(s0, s1));
}

TEST(string_view, find_first) {
    StringView s = string_view_from_literal("first;second;third;");
    TEST_ASSERT_EQUAL(string_view_find_first(s, ';'), 5U);
    TEST_ASSERT_EQUAL(string_view_find_first(s, '#'), s.length);
    TEST_ASSERT_EQUAL(string_view_find_first((StringView){0}, '-'), 0);
}

TEST_GROUP_RUNNER(string_view) {
    RUN_TEST_CASE(string_view, empty);
    RUN_TEST_CASE(string_view, cmp);
    RUN_TEST_CASE(string_view, case_cmp);
    RUN_TEST_CASE(string_view, find_first);
}

static void run_all_tests(void) {
    RUN_TEST_GROUP(string_view);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}