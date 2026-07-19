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

#include <etripator/comment.h>
#include <etripator/message.h>

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC_VARARG(message_print, MessageType, const char*, size_t, const char*, const char*, ...);

TEST_GROUP(comment);

TEST_SETUP(comment) {
    RESET_FAKE(message_print);
    FFF_RESET_HISTORY();
}

TEST_TEAR_DOWN(comment) {
}

TEST(comment, add) {
    Comment comment = {0};
    CommentRepository repository = {0};
    TEST_ASSERT_TRUE(comment_repository_create(&repository));

    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0002, 0x00, string_view_from_literal("comment #0")));
    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0001, 0x00, string_view_from_literal("comment #1")));
    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0003, 0x00, string_view_from_literal("comment #3")));
    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0001, 0x01, string_view_from_literal("comment #2")));
    
    TEST_ASSERT_TRUE(comment_repository_find(&repository, 0x0001, 0x01, &comment));
    TEST_ASSERT_EQUAL_UINT16(0x0001, comment.logical);
    TEST_ASSERT_EQUAL_UINT8(0x01, comment.page);
    TEST_ASSERT_EQUAL_STRING("comment #2", comment.text.data);

    TEST_ASSERT_TRUE(comment_repository_find(&repository, 0x0001, 0x00, &comment));
    TEST_ASSERT_EQUAL_UINT16(0x0001, comment.logical);
    TEST_ASSERT_EQUAL_UINT8(0x00, comment.page);
    TEST_ASSERT_EQUAL_STRING("comment #1", comment.text.data);

    TEST_ASSERT_TRUE(comment_repository_find(&repository, 0x0003, 0x00, &comment));
    TEST_ASSERT_EQUAL_UINT16(0x0003, comment.logical);
    TEST_ASSERT_EQUAL_UINT8(0x00, comment.page);
    TEST_ASSERT_EQUAL_STRING("comment #3", comment.text.data);

    TEST_ASSERT_EQUAL_INT(4, comment_repository_size(&repository));

    comment_repository_destroy(&repository);
}

TEST(comment, delete) {
    CommentRepository repository = {0};
    TEST_ASSERT_TRUE(comment_repository_create(&repository));

    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0002, 0x03, string_view_from_literal("comment #7")));
    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0002, 0x01, string_view_from_literal("comment #6")));
    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0002, 0x02, string_view_from_literal("comment #5")));
    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x000a, 0x00, string_view_from_literal("comment #4")));
    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0008, 0x00, string_view_from_literal("comment #3")));
    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0004, 0x00, string_view_from_literal("comment #2")));
    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0002, 0x00, string_view_from_literal("comment #1")));
    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0000, 0x00, string_view_from_literal("comment #0")));
    TEST_ASSERT_EQUAL_INT(8, comment_repository_size(&repository));

    comment_repository_delete(&repository, 0x0001, 0x0009, 0x00);

    TEST_ASSERT_EQUAL_INT(5, comment_repository_size(&repository));

    comment_repository_destroy(&repository);
}

TEST(comment, update) {
    Comment comment = {0};
    CommentRepository repository = {0};
    TEST_ASSERT_TRUE(comment_repository_create(&repository));

    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0002, 0x00, string_view_from_literal("comment #0")));
    TEST_ASSERT_TRUE(comment_repository_add(&repository, 0x0001, 0x00, string_view_from_literal("comment #1")));

    TEST_ASSERT_FALSE(comment_repository_update(&repository, 0x0003, 0x01, string_view_from_literal("( _-_)")));

    TEST_ASSERT_TRUE(comment_repository_update(&repository, 0x0002, 0x00, string_view_from_literal("updated")));
    TEST_ASSERT_TRUE(comment_repository_find(&repository, 0x0002, 0x00, &comment));
    TEST_ASSERT_EQUAL_UINT16(0x0002, comment.logical);
    TEST_ASSERT_EQUAL_UINT8(0x00, comment.page);
    TEST_ASSERT_EQUAL_STRING("updated", comment.text.data);

    TEST_ASSERT_TRUE(comment_repository_update(&repository, 0x0001, 0x00, (StringView){0}));
    TEST_ASSERT_FALSE(comment_repository_find(&repository, 0x0001, 0x00, &comment));
    TEST_ASSERT_EQUAL_INT(1, comment_repository_size(&repository));

    comment_repository_destroy(&repository);
}

/*
TEST(comment, load) {
    Comment comment = {0};
    CommentRepository repository = {0};
    
    TEST_ASSERT_TRUE(comment_repository_create(&repository));

    TEST_ASSERT_FALSE(comment_repository_load(&repository, "/not_here/comment.json"));
    TEST_ASSERT_EQUAL_INT(0, comment_repository_size(&repository));

    TEST_ASSERT_FALSE(comment_repository_load(&repository, "data/comment_1.json"));
    TEST_ASSERT_EQUAL_INT(1, comment_repository_size(&repository));

    TEST_ASSERT_TRUE(comment_repository_get(&repository, 0, &comment));
    TEST_ASSERT_EQUAL_UINT16(0xCAFEU, comment.logical);
    TEST_ASSERT_EQUAL_UINT8(0x0AU, comment.page);
    TEST_ASSERT_EQUAL_STRING("hello!", comment.text);

    comment_repository_destroy(&repository);

    TEST_ASSERT_TRUE(comment_repository_load(&repository, "data/comment_0.json"));
    TEST_ASSERT_EQUAL_INT(2, comment_repository_size(&repository));

    TEST_ASSERT_TRUE(comment_repository_find(&repository, 0xC105U, 3, &comment));
    TEST_ASSERT_EQUAL_UINT16(0xC105U, comment.logical);
    TEST_ASSERT_EQUAL_UINT8(3, comment.page);
    TEST_ASSERT_EQUAL_STRING("line 0\nline 1\nline 2", comment.text);

    TEST_ASSERT_TRUE(comment_repository_find(&repository, 0xEABCU, 0, &comment));
    TEST_ASSERT_EQUAL_UINT16(0xEABCU, comment.logical);
    TEST_ASSERT_EQUAL_UINT8(0, comment.page);
    TEST_ASSERT_EQUAL_STRING("single line comment", comment.text);

    comment_repository_destroy(&repository);

    return MUNIT_OK;
}
*/

TEST_GROUP_RUNNER(comment) {
    RUN_TEST_CASE(comment, add);
    RUN_TEST_CASE(comment, delete);
    RUN_TEST_CASE(comment, update);
//    RUN_TEST_CASE(comment, load);
}

static void run_all_tests(void) {
    RUN_TEST_GROUP(comment);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}