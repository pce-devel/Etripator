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

DEFINE_FFF_GLOBALS;

// [todo] DECLARE + move definition in an object library
FAKE_VOID_FUNC_VARARG(message_print, MessageType, const char*, size_t, const char*, const char*, ...);

TEST_GROUP(buffer);

TEST_SETUP(buffer) {
    RESET_FAKE(message_print);
    FFF_RESET_HISTORY();
}

TEST_TEAR_DOWN(buffer) {
}

TEST(buffer, create) {
    uint8_t buffer[32] = {0};
    TEST_ASSERT_NULL(data_buffer_create(NULL, 8U));
    TEST_ASSERT_NULL(data_buffer_create(buffer, 0));
    
    Data* data = data_buffer_create(buffer, 32);
    TEST_ASSERT_NOT_NULL(data);

    data_release(data);
}

TEST(buffer, open) {
    TEST_ASSERT_FALSE(data_open(NULL));

    uint8_t buffer[32] = {0};
    
    Data* data = data_buffer_create(buffer, 32);
    TEST_ASSERT_NOT_NULL(data);

    // everything should be fine.
    TEST_ASSERT_TRUE(data_open(data));
    // can't reopen an already opened data buffer.
    TEST_ASSERT_FALSE(data_open(data));

    data_release(data);
}

TEST(buffer, close) {
    uint8_t buffer[32] = {0};
    
    Data* data = data_buffer_create(buffer, 32);
    TEST_ASSERT_NOT_NULL(data);

    // open data buffer.
    TEST_ASSERT_TRUE(data_open(data));
    // closing it should work.
    data_close(data);
    // we can re-open the data buffer.
    TEST_ASSERT_TRUE(data_open(data));

    data_release(data);
}

TEST(buffer, size) {
    uint8_t buffer[32] = {0};
    
    Data* data = data_buffer_create(buffer, 32);
    TEST_ASSERT_NOT_NULL(data);

    TEST_ASSERT_EQUAL_size_t(32, data_size(data));

    TEST_ASSERT_TRUE(data_open(data));

    TEST_ASSERT_EQUAL_size_t(32, data_size(data));

    data_close(data);

    TEST_ASSERT_EQUAL_size_t(32, data_size(data));

    data_release(data);
}

TEST(buffer, seek_tell) {
    uint8_t buffer[32] = {0};

    TEST_ASSERT_EQUAL_size_t(0, data_tell(NULL));
    TEST_ASSERT_FALSE(data_seek(NULL, 1));

    Data* data = data_buffer_create(buffer, 32);
    TEST_ASSERT_NOT_NULL(data);

    TEST_ASSERT_EQUAL_size_t(0, data_tell(data));
    TEST_ASSERT_FALSE(data_seek(data, 2));

    TEST_ASSERT_TRUE(data_open(data));

    TEST_ASSERT_EQUAL_size_t(0, data_tell(data));
    TEST_ASSERT_TRUE(data_seek(data, 10));
    TEST_ASSERT_TRUE(data_seek(data, 0));
    TEST_ASSERT_EQUAL_size_t(10, data_tell(data));
    TEST_ASSERT_TRUE(data_seek(data, -4));
    TEST_ASSERT_EQUAL_size_t(6, data_tell(data));
    TEST_ASSERT_FALSE(data_seek(data, 100));
    TEST_ASSERT_EQUAL_size_t(6, data_tell(data));
    TEST_ASSERT_FALSE(data_seek(data, -200));
    TEST_ASSERT_EQUAL_size_t(6, data_tell(data));

    data_close(data);

    TEST_ASSERT_EQUAL_size_t(0, data_tell(data));
    TEST_ASSERT_FALSE(data_seek(data, 1));

    data_release(data);
}


TEST(buffer, jump) {
    uint8_t buffer[32] = {0};

    TEST_ASSERT_FALSE(data_jump(NULL, 1));

    Data* data = data_buffer_create(buffer, 32);
    TEST_ASSERT_NOT_NULL(data);

    TEST_ASSERT_FALSE(data_jump(data, 2));

    TEST_ASSERT_TRUE(data_open(data));

    TEST_ASSERT_TRUE(data_jump(data, 10));
    TEST_ASSERT_EQUAL_size_t(10, data_tell(data));
    
    TEST_ASSERT_FALSE(data_jump(data, 100));
    TEST_ASSERT_EQUAL_size_t(10, data_tell(data));

    TEST_ASSERT_TRUE(data_jump(data, -12));
    TEST_ASSERT_EQUAL_size_t(20, data_tell(data));
    TEST_ASSERT_FALSE(data_jump(data, -200));
    TEST_ASSERT_EQUAL_size_t(20, data_tell(data));

    data_close(data);

    TEST_ASSERT_FALSE(data_jump(data, 1));

    data_release(data);
}

TEST(buffer, write) {
    uint8_t bitmap[40] = { // openssl rand -hex 40
        0x7e,0x9b,0xfa,0xe5,0x7a,0xfd,0x14,0x1d,0x3f,0x30,
        0x7e,0xcc,0x95,0x83,0x5d,0x4b,0x24,0xc1,0xdb,0xc9,
        0xa2,0xae,0x6a,0x2e,0x21,0x73,0xdb,0xe9,0x0f,0x3f,
        0xde,0x9e,0x9e,0xbe,0x94,0x06,0x6b,0xf8,0x10,0xd7
    };
    uint8_t buffer[32] = {0};

    Data* data = data_buffer_create(buffer, 32);
    TEST_ASSERT_NOT_NULL(data);

    TEST_ASSERT_TRUE(data_open(data));

    size_t nwritten;
    // write the first 8 bytes and check if they landed in the buffer.
    TEST_ASSERT_TRUE(data_write(data, bitmap, 8, &nwritten));
    TEST_ASSERT_EQUAL_size_t(8, nwritten);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bitmap, buffer, 8);
    TEST_ASSERT_EQUAL_size_t(8, data_tell(data));
    // do the same with the next 10 bytes
    TEST_ASSERT_TRUE(data_write(data, bitmap+8, 10, &nwritten));
    TEST_ASSERT_EQUAL_size_t(10, nwritten);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bitmap, buffer, 18);
    TEST_ASSERT_EQUAL_size_t(18, data_tell(data));
    // rewind data buffer to the beginning and write the 6 next bitmap bytes.
    TEST_ASSERT_TRUE(data_seek(data, -18));
    TEST_ASSERT_TRUE(data_write(data, bitmap+18, 6, NULL));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bitmap+18, buffer, 6);
    TEST_ASSERT_EQUAL_size_t(6, data_tell(data));
    // rewind data buffer to the beginning again and try to write the whole bitmap.
    TEST_ASSERT_TRUE(data_seek(data, -data_tell(data))); 
    TEST_ASSERT_TRUE(data_write(data, bitmap, 40, &nwritten));
    // only 32 bytes should have been written.
    TEST_ASSERT_EQUAL_size_t(32, nwritten);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bitmap, buffer, 32);
    TEST_ASSERT_EQUAL_size_t(32, data_tell(data));
    // try to write 4 more bytes, the function should succeed but it will report 0 bytes written.
    TEST_ASSERT_TRUE(data_write(data, bitmap, 4, &nwritten));
    TEST_ASSERT_EQUAL_size_t(0, nwritten);

    data_close(data);

    // can't write to a closed data buffer.
    TEST_ASSERT_FALSE(data_write(data, bitmap, 4, &nwritten));

    data_release(data);
}

TEST(buffer, print) {
    StringView s;
    
    char buffer[32] = {0};

    Data* data = data_buffer_create((uint8_t*)buffer, 32);
    TEST_ASSERT_NOT_NULL(data);

    TEST_ASSERT_TRUE(data_open(data));
    
    s = string_view_from_literal("Here's a string!");
    TEST_ASSERT_TRUE(data_print(data, s));
    TEST_ASSERT_EQUAL_size_t(s.length, data_tell(data));
    TEST_ASSERT_EQUAL_STRING_LEN(s.data, buffer, s.length);

    s = string_view_from_literal("Add some more characters to the string.");
    TEST_ASSERT_FALSE(data_print(data, s));
    TEST_ASSERT_EQUAL_size_t(data_size(data), data_tell(data));
    TEST_ASSERT_EQUAL_STRING_LEN("Here's a string!Add some more charac", buffer, sizeof(buffer));

    data_release(data);
}

TEST(buffer, read) {
    uint8_t buffer[32] = {
        0x65,0x03,0x66,0xfd,0xc0,0xc5,0xbf,0x2f,0x9c,0x15,0x05,0x95,0xf5,0xe6,0x60,0x8e,
        0xa9,0x64,0x9a,0xfa,0xff,0x62,0x34,0xb6,0x0f,0x2d,0xe9,0x1d,0x55,0xf9,0x52,0x79,
    };
    uint8_t tmp[16] = {0}; 
    size_t nread;

    Data* data = data_buffer_create(buffer, 32);
    TEST_ASSERT_NOT_NULL(data);

    // can't read an unopened data buffer.
    TEST_ASSERT_FALSE(data_read(data, tmp, 8, &nread));

    TEST_ASSERT_TRUE(data_open(data));

    // read 8 bytes.
    TEST_ASSERT_TRUE(data_read(data, tmp, 8, &nread));
    TEST_ASSERT_EQUAL_size_t(8, nread);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(tmp, buffer, 8);
    TEST_ASSERT_EQUAL_size_t(8, data_tell(data));
    // read 4 more.
    TEST_ASSERT_TRUE(data_read(data, tmp+8, 4, &nread));
    TEST_ASSERT_EQUAL_size_t(4, nread);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(tmp, buffer, 12);
    TEST_ASSERT_EQUAL_size_t(12, data_tell(data));
    // jump 12 bytes forward.
    TEST_ASSERT_TRUE(data_seek(data, 12)); 
    TEST_ASSERT_EQUAL_size_t(24, data_tell(data));
    // try to read 16 bytes but only 8 are available.
    TEST_ASSERT_TRUE(data_read(data, tmp, 16, &nread));
    TEST_ASSERT_EQUAL_size_t(8, nread);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(tmp, buffer+24, 8);
    // try to read 4 more bytes but there's no data left.
    TEST_ASSERT_TRUE(data_read(data, tmp, 4, &nread));
    TEST_ASSERT_EQUAL_size_t(0, nread);

    data_close(data);

    // can't read a closed data buffer.
    TEST_ASSERT_FALSE(data_read(data, tmp, 8, &nread));

    data_release(data);
}

TEST_GROUP_RUNNER(buffer) {
    RUN_TEST_CASE(buffer, create);
    RUN_TEST_CASE(buffer, open);
    RUN_TEST_CASE(buffer, close);
    RUN_TEST_CASE(buffer, size);
    RUN_TEST_CASE(buffer, seek_tell);
    RUN_TEST_CASE(buffer, jump);
    RUN_TEST_CASE(buffer, write);
    RUN_TEST_CASE(buffer, print);
    RUN_TEST_CASE(buffer, read);
}
