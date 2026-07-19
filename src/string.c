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
  copyright s(c) 2009--2026 Vincent Cruz.
 
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
#include <etripator/string.h>

#include <assert.h>

void string_init(String *str) {
    assert(str != NULL);
    *str = (String) {0};
}

/// Expand string buffer storage.
/// This function can only expand the size of the string buffer storage.
static char* string_reserve(String *str, size_t length) {
    if((str == NULL) || (length == 0)) {
        return NULL;
    }
    // Here we only reserve storage space. We don't modify the actual string length.

    // If it fits in the local buffer.
    if((str->storage == NULL) && (length < STRING_BUFFER_SIZE)) {
        return str->buffer;
    }
    // If it fits in the storage buffer.
    if((str->storage != NULL) && (length < str->capacity)) {
        return str->storage;
    }
    // The requested size is bigger than the local static buffer and the currently allocated storage.
    char *storage = realloc(str->storage, length+1U); // We add 1 to stock any extra '\0'.
    if(storage == NULL) {
        return NULL;
    }
    // Copy the whole local static buffer into the dynamic storage if it was previously in use.
    if(str->storage == NULL) {
        memcpy(storage, str->buffer, STRING_BUFFER_SIZE);
    }
    str->storage = storage;
    str->capacity = length + 1U;
    // Keep length as is as it may reflect the size of the currently stored string.
    return storage;
}

void string_release(String *str) {
    if(str == NULL) {
        return;
    }
    free(str->storage);
    string_init(str);
}

size_t string_length(const String *str) {
    assert(str != NULL);
    return str->length;
}

char* string_ptr(String *str) {
    if(str == NULL) {
        return NULL;
    }
    return (str->storage != NULL) ? str->storage : str->buffer;
}

const char* string_const_ptr(const String *str) {
    return (const char*)string_ptr((String*)str);
}

// Resize string buffer to the specified length.
static char* string_adjust(String *str, size_t length) {
    char *out = NULL;
    if(length < STRING_BUFFER_SIZE) {
        // The requested length fits into internal buffer.
        // First check if we can release dynamically allocated storage.
        if(str->storage != NULL) {
            free(str->storage);
            str->storage = NULL;
            str->capacity = 0;
        }
        out = str->buffer;
    } else if(length >= str->capacity) {
        // It can't fit in actual storage. We need to reallocate it.
        out = realloc(str->storage, length+1U);
        if(out != NULL) {
            str->storage = out;
            str->capacity = length+1U;
        }
    } else {
        out = str->storage;
    }
    str->length = length;
    return out;
}

bool string_copy(String *str, const char *data, size_t length) {
    if((str == NULL) || (data == NULL) || (length == 0)) {
        return false;
    }

    char *ptr = string_adjust(str, length);
    if(ptr == NULL) {
        return false;
    }

    memcpy(ptr, data, length);
    ptr[length] = '\0';
    str->length = length;
    return true;
}

bool string_append_n(String *str, char ch, size_t n) {
    if(str == NULL) {
        return false;
    }
    // Allocate storage if needed.
    size_t requested = str->length + n;
    char *ptr = string_reserve(str, requested);
    if(ptr == NULL) {
        return false;
    }
    // Copy n times character ch to the string
    ptr += str->length;
    for(size_t i=0; i<n; i++) {
        *ptr++ = ch;
    }
    *ptr = '\0';
    str->length = requested;
    return true;
}

bool string_prepend_n(String *str, char ch, size_t n) {
    if(str == NULL) {
        return false;
    }
    // Allocate storage if needed.
    size_t requested = str->length + n;
    char *ptr = string_reserve(str, requested);
    if(ptr == NULL) {
        return false;
    }
    // Move current string up to n characters
    memmove(ptr+n, ptr, str->length);
    ptr[requested] = '\0';
    // Copy n times character ch at the beginning of the string
    for(size_t i=0; i<n; i++) {
        *ptr++ = ch;
    }
    str->length = requested;
    return true;
}

bool string_append(String *str, const char *data, size_t length) {
    if(str == NULL) {
        return false;
    }
    // Allocate storage if needed.
    size_t requested = str->length + length;
    char *ptr = string_reserve(str, requested);
    if(ptr == NULL) {
        return false;
    }
    // Copy the string to the end of the buffer
    memcpy(ptr+str->length, data, length);
    ptr[requested] = '\0';
    str->length = requested;
    return true;
}

bool string_prepend(String *str, const char *data, size_t length) {
    if(str == NULL) {
        return false;
    }
    // Allocate storage if needed.
    size_t requested = str->length + length;
    char *ptr = string_reserve(str, requested);
    if(ptr == NULL) {
        return false;
    }
    // Copy the string at the beginning of the buffer
    memmove(ptr+length, ptr, str->length);
    memcpy(ptr, data, length);
    ptr[requested] = '\0';
    str->length = requested;
    return true;
}

bool string_format(String *str, const char *fmt, ...) {
    if((str == NULL) || (fmt == NULL)) {
        return false;
    }
    
    va_list args, backup;
    va_start(args, fmt);
    va_copy(backup, args);

    // compute size
    size_t length = vsnprintf(NULL, 0, fmt, args);
    
    // fit storage size
    char *ptr = string_adjust(str, length);
    size_t nwritten = 0;
    if(ptr != NULL) {
        nwritten = vsnprintf(string_ptr(str), length+1, fmt, backup);
    }

    va_end(backup);
    va_end(args);
    return (length == nwritten);
}

bool string_append_format(String *str, const char *fmt, ...) {
    if((str == NULL) || (fmt == NULL)) {
        return false;
    }
    
    va_list args, backup;
    va_start(args, fmt);
    va_copy(backup, args);

    // compute size
    size_t length = vsnprintf(NULL, 0, fmt, args);
    
    // expand storage if needed
    char *ptr = string_reserve(str, str->length + length);
    size_t nwritten = 0;
    if(ptr != NULL) {
        nwritten = vsnprintf(string_ptr(str) + str->length, length+1, fmt, backup);
    }
    str->length += nwritten;

    va_end(backup);
    va_end(args);
    return (length == nwritten);
}

bool string_prepend_format(String *str, const char *fmt, ...) {
    if((str == NULL) || (fmt == NULL)) {
        return false;
    }
    
    va_list args, backup;
    va_start(args, fmt);
    va_copy(backup, args);

    // compute size
    size_t length = vsnprintf(NULL, 0, fmt, args);
    
    // expand storage if needed
    char *ptr = string_reserve(str, str->length + length);
    size_t nwritten = 0;
    if(ptr != NULL) {
        char first = ptr[0];
        memmove(ptr+length, ptr, str->length);
        nwritten = vsnprintf(ptr, length+1, fmt, backup);
        ptr[length] = first;
    }
    str->length += nwritten;
    ptr[str->length] = '\0';

    va_end(backup);
    va_end(args);
    return (length == nwritten);
}

bool string_clone(String *dst, const String *src) {
    if((dst == NULL) || (src == NULL)) {
       return false;
    }
    return string_copy(dst, (src->storage != NULL) ? src->storage : src->buffer, src->length);
}

StringView string_get_view(const String *in) {
    return (StringView) {
        .data = string_const_ptr(in),
        .length = string_length(in),
    };
}

bool string_from_view(String *str, const StringView view) {
    if(str == NULL) {
        return false;
    }
    if((view.data == NULL) || (view.length == 0)) {
        return false;
    }
    char *ptr = string_adjust(str, view.length);
    if(ptr == NULL) {
        string_release(str);
        return false;
    }
    memcpy(ptr, view.data, view.length);
    ptr[view.length] = '\0';
    return true;
}
