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
#ifndef ETRIPATOR_STRING_H
#define ETRIPATOR_STRING_H

#include "config.h"
#include "string_view.h"

/// @defgroup StringGroup String API
/// @{

/// Size of the static storage buffer
#define STRING_BUFFER_SIZE (32U)

/// String buffer.
/// Static buffer is used when the string length is less than STRING_BUFFER_SIZE.
/// Otherwise a new storage buffer is dynamically allocated and used instead.
/// When the string is slice or stripped, if its size becomes length than
/// STRING_BUFFER_SIZE the content will be transfered to the static buffer storage
/// and the dynamically allocated storage will be freed.
typedef struct {
    /// Pointer to the dynamically allocated storage buffer.
    char *storage;
    /// String length.
    size_t length;
    /// Size of the dynamically allocated storage.
    size_t capacity;
    /// String buffer storage
    char buffer[STRING_BUFFER_SIZE];
} String;

/// Initialize string buffer.
/// All members will be set to default values.
/// Note that no memory release will be performed. This means that this function should be
/// called to initialize a newly instanced string buffer, or that it must have been freed
/// before.
void string_init(String *str);

/// Release any allocated resources.
void string_release(String *str);

/// Get string length.
size_t string_length(const String *str);

/// Get pointer to the string buffer.
char* string_ptr(String *str);

/// Get pointer to the string buffer.
const char* string_const_ptr(const String *str);

/// Get string view.
StringView string_get_view(const String *str);

/// Create string from string view.
bool string_from_view(String *str, const StringView view);

/// Copy string.
bool string_copy(String *str, const char *data, size_t length);

/// Append n times the character ch to the string.
bool string_append_n(String *str, char ch, size_t n);

/// Add n times the character ch before the string.
bool string_prepend_n(String *str, char ch, size_t n);

/// Append string.
bool string_append(String *str, const char *data, size_t length);

/// Add string before.
bool string_prepend(String *str, const char *data, size_t length);

/// Formatted string output.
bool string_format(String *str, const char *fmt, ...) __printflike(2, 3);

/// Formatted string append.
bool string_append_format(String *str, const char *fmt, ...) __printflike(2, 3);

/// Formatted string prepend.
bool string_prepend_format(String *str, const char *fmt, ...) __printflike(2, 3);

/// Clone string.
bool string_clone(String *dst, const String *src);

// [todo] eq

/// @}

#endif // ETRIPATOR_STRING_H