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

#include <etripator/label.h>
#include <etripator/message.h>

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC_VARARG(message_print, MessageType, const char*, size_t, const char*, const char*, ...);

TEST_GROUP(label);

TEST_SETUP(label) {
    RESET_FAKE(message_print);
    FFF_RESET_HISTORY();
}

TEST_TEAR_DOWN(label) {
}

TEST(label, add) {
    Label label = {};
    LabelRepository repository = {0};
    TEST_ASSERT_TRUE(label_repository_create(&repository));

    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0001, 0x00, string_view_from_literal("l_0001"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0020, 0x00, string_view_from_literal("l_0020"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x000a, 0xb1, string_view_from_literal("l_000a"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0xcafe, 0xf7, string_view_from_literal("l_cafe"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0001, 0x00, string_view_from_literal("l_0001"), (StringView){0}));

    TEST_ASSERT_EQUAL_INT(4, label_repository_size(&repository));
    
    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x000a, 0xb1, &label));
    TEST_ASSERT_EQUAL_STRING("l_000a", label.name.data);

    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0020, 0x00, &label));
    TEST_ASSERT_EQUAL_STRING("l_0020", label.name.data);

    TEST_ASSERT_TRUE(label_repository_find(&repository, 0xcafe, 0xf7, &label));
    TEST_ASSERT_EQUAL_STRING("l_cafe", label.name.data);

    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0001, 0x00, &label));
    TEST_ASSERT_EQUAL_STRING("l_0001", label.name.data);

    TEST_ASSERT_FALSE(label_repository_find(&repository, 0xbeef, 0xac, &label));
    
    label_repository_destroy(&repository);}

TEST(label, delete) {
    Label label = {};
    LabelRepository repository = {0};
    TEST_ASSERT_TRUE(label_repository_create(&repository));

    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0110, 0x1a, string_view_from_literal("label01"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0220, 0x1a, string_view_from_literal("label02"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0330, 0x1b, string_view_from_literal("label03"), string_view_from_literal("description")));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0440, 0x1a, string_view_from_literal("label04"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0550, 0x1b, string_view_from_literal("label05"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0553, 0x1b, string_view_from_literal("label06"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0555, 0x1b, string_view_from_literal("label07"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0557, 0x1b, string_view_from_literal("label08"), (StringView){0}));
    
    TEST_ASSERT_EQUAL_INT(8, label_repository_size(&repository));

    label_repository_delete(&repository, 0x04a0, 0x0556, 0x1b);

    TEST_ASSERT_EQUAL_INT(5, label_repository_size(&repository));

    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0557, 0x1b, &label));
    TEST_ASSERT_EQUAL_STRING("label08", label.name.data);
    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0440, 0x1a, &label));
    TEST_ASSERT_EQUAL_STRING("label04", label.name.data);
    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0330, 0x1b, &label));
    TEST_ASSERT_EQUAL_STRING("label03", label.name.data);
    TEST_ASSERT_EQUAL_STRING("description", label.description.data);
    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0220, 0x1a, &label));
    TEST_ASSERT_EQUAL_STRING("label02", label.name.data);
    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0110, 0x1a, &label));
    TEST_ASSERT_EQUAL_STRING("label01", label.name.data);

    TEST_ASSERT_FALSE(label_repository_find(&repository, 0x0555, 0x1b, &label));
    TEST_ASSERT_FALSE(label_repository_find(&repository, 0x0553, 0x1b, &label));
    TEST_ASSERT_FALSE(label_repository_find(&repository, 0x0550, 0x1b, &label));

    label_repository_destroy(&repository);    
}

TEST(label, update_name) {
    Label label = {};
    LabelRepository repository = {0};
    TEST_ASSERT_TRUE(label_repository_create(&repository));

    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0110, 0x1a, string_view_from_literal("label01"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0440, 0x1a, (StringView){0}, string_view_from_literal("inline description")));
    
    TEST_ASSERT_EQUAL_INT(2, label_repository_size(&repository));

    TEST_ASSERT_TRUE(label_repository_update_name(&repository, 0x0110, 0x1a, string_view_from_literal("l_0110")));
    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0110, 0x1a, &label));
    TEST_ASSERT_EQUAL_STRING("l_0110", label.name.data);
    TEST_ASSERT_TRUE(string_view_empty(label.description));

    TEST_ASSERT_TRUE(label_repository_update_name(&repository, 0x0440, 0x1a, string_view_from_literal("label02")));
    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0440, 0x1a, &label));
    TEST_ASSERT_EQUAL_STRING("label02", label.name.data);

    TEST_ASSERT_TRUE(label_repository_update_name(&repository, 0x0440, 0x1a, (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0440, 0x1a, &label));
    TEST_ASSERT_TRUE(string_view_empty(label.name));

    TEST_ASSERT_TRUE(label_repository_update_name(&repository, 0x0110, 0x1a, (StringView){0}));
    TEST_ASSERT_FALSE(label_repository_find(&repository, 0x0110, 0x1a, &label));

    label_repository_destroy(&repository);    
}

TEST(label, update_description) {
    Label label = {};
    LabelRepository repository = {0};
    TEST_ASSERT_TRUE(label_repository_create(&repository));

    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0110, 0x1a, string_view_from_literal("label01"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x0440, 0x1a, (StringView){0}, string_view_from_literal("inline description")));
    
    TEST_ASSERT_EQUAL_INT(2, label_repository_size(&repository));

    TEST_ASSERT_TRUE(label_repository_update_description(&repository, 0x0110, 0x1a, string_view_from_literal("a new description")));
    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0110, 0x1a, &label));
    TEST_ASSERT_EQUAL_STRING("a new description", label.description.data);

    TEST_ASSERT_TRUE(label_repository_update_description(&repository, 0x0440, 0x1a, string_view_from_literal("description for 2nd label")));
    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0440, 0x1a, &label));
    TEST_ASSERT_EQUAL_STRING("description for 2nd label", label.description.data);

    TEST_ASSERT_TRUE(label_repository_update_description(&repository, 0x0110, 0x1a, (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x0110, 0x1a, &label));
    TEST_ASSERT_TRUE(string_view_empty(label.description));

    TEST_ASSERT_TRUE(label_repository_update_description(&repository, 0x0440, 0x1a, (StringView){0}));
    TEST_ASSERT_FALSE(label_repository_find(&repository, 0x0440, 0x1a, &label));

    label_repository_destroy(&repository);    
}

TEST(label, load) {
    LabelRepository repository = {0};

    const char *buffer =
    "[\n"
    "    { \"name\": \"var\", \"logical\": \"31dc\", \"page\": \"f8\" },\n"
    "    { \"name\": \"do_something\", \"logical\": \"eabc\", \"page\": \"00\", \"description\": \"do something\" },\n"
    "    { \"name\": \"run\", \"logical\" : \"d6f7\", \"page\": \"1f\", \"description\": [\n"
    "            \"line0\",\n"
    "            \"line1\",\n"
    "            \"line2\",\n"
    "            \"line3\"\n"
    "        ]\n"
    "    }\n"
    "]\n";
    size_t length = strlen(buffer);

    Data *input = data_buffer_create((uint8_t*)buffer, length);
    
    TEST_ASSERT_NOT_NULL(input);
    TEST_ASSERT_TRUE(data_open(input));

    TEST_ASSERT_TRUE(label_repository_create(&repository));
    TEST_ASSERT_TRUE(label_repository_load(&repository, input));
    
    data_close(input);
    data_release(input);

    TEST_ASSERT_EQUAL_INT(3, label_repository_size(&repository));
    
    Label label = {};

    TEST_ASSERT_TRUE(label_repository_find(&repository, 0xD6F7, 0x1F, &label));
    TEST_ASSERT_EQUAL_UINT16(0xD6F7U, label.logical);
    TEST_ASSERT_EQUAL_UINT8(0x1F, label.page);
    TEST_ASSERT_EQUAL_STRING("run", label.name.data);
    TEST_ASSERT_EQUAL_STRING("line0\nline1\nline2\nline3", label.description.data);

    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x31DC, 0xF8, &label));
    TEST_ASSERT_EQUAL_UINT16(0x31DC, label.logical);
    TEST_ASSERT_EQUAL_UINT8(0xF8, label.page);
    TEST_ASSERT_EQUAL_STRING("var", label.name.data);
    TEST_ASSERT_TRUE(string_view_empty(label.description));

    TEST_ASSERT_TRUE(label_repository_find(&repository, 0xEABC, 0x00, &label));
    TEST_ASSERT_EQUAL_UINT16(0xEABC, label.logical);
    TEST_ASSERT_EQUAL_UINT8(0x00, label.page);
    TEST_ASSERT_EQUAL_STRING("do_something", label.name.data);
    TEST_ASSERT_EQUAL_STRING("do something", label.description.data);

    label_repository_destroy(&repository);    
}

TEST(label, save) {
    LabelRepository repository = {0};

    TEST_ASSERT_TRUE(label_repository_create(&repository));
    
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0x31DC, 0xF8, string_view_from_literal("var"), (StringView){0}));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0xEABC, 0x00, string_view_from_literal("do_something"), string_view_from_literal("do something")));
    TEST_ASSERT_TRUE(label_repository_add(&repository, 0xD6F7, 0x1F, string_view_from_literal("run"), string_view_from_literal("line0\nline1\nline2\nline3")));

    char buffer[512];
    size_t length = sizeof(buffer);
    Data *data = data_buffer_create((uint8_t*)buffer, length);
    
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_TRUE(data_open(data));

    TEST_ASSERT_TRUE(label_repository_save(&repository, data));

    length = data_tell(data);

    data_close(data);
    data_release(data);

    data = data_buffer_create((uint8_t*)buffer, length);

    label_repository_destroy(&repository);    

    TEST_ASSERT_TRUE(data_open(data));

    TEST_ASSERT_TRUE(label_repository_create(&repository));
    TEST_ASSERT_TRUE(label_repository_load(&repository, data));

    data_close(data);
    data_release(data);

    TEST_ASSERT_EQUAL_INT(3, label_repository_size(&repository));
    
    Label label = {};

    TEST_ASSERT_TRUE(label_repository_find(&repository, 0xD6F7, 0x1F, &label));
    TEST_ASSERT_EQUAL_UINT16(0xD6F7U, label.logical);
    TEST_ASSERT_EQUAL_UINT8(0x1F, label.page);
    TEST_ASSERT_EQUAL_STRING("run", label.name.data);
    TEST_ASSERT_EQUAL_STRING("line0\nline1\nline2\nline3", label.description.data);

    TEST_ASSERT_TRUE(label_repository_find(&repository, 0x31DC, 0xF8, &label));
    TEST_ASSERT_EQUAL_UINT16(0x31DC, label.logical);
    TEST_ASSERT_EQUAL_UINT8(0xF8, label.page);
    TEST_ASSERT_EQUAL_STRING("var", label.name.data);
    TEST_ASSERT_TRUE(string_view_empty(label.description));

    TEST_ASSERT_TRUE(label_repository_find(&repository, 0xEABC, 0x00, &label));
    TEST_ASSERT_EQUAL_UINT16(0xEABC, label.logical);
    TEST_ASSERT_EQUAL_UINT8(0x00, label.page);
    TEST_ASSERT_EQUAL_STRING("do_something", label.name.data);
    TEST_ASSERT_EQUAL_STRING("do something", label.description.data);

    label_repository_destroy(&repository);
}

TEST_GROUP_RUNNER(label) {
    RUN_TEST_CASE(label, add);
    RUN_TEST_CASE(label, delete);
    RUN_TEST_CASE(label, update_name);
    RUN_TEST_CASE(label, update_description);
    RUN_TEST_CASE(label, load);
    RUN_TEST_CASE(label, save);
}

static void run_all_tests(void) {
    RUN_TEST_GROUP(label);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}