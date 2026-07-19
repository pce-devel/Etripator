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
#include "json_helpers.h"

#include <etripator/utils.h>

bool json_validate_int(const json_t* obj, int* out) {
    SANITY_CHECK(obj != NULL, false);
    SANITY_CHECK(out != NULL, false);

    bool ret = false;
    if(json_is_string(obj)) {
        const char *str = json_string_value(obj);
        for(; (*str!='\0') && isspace(*str); str++) {
        }
        if(*str=='$') {
            str++;
        }
        errno = 0;
        char *end;
        *out = strtoul(str, &end, 16);
        if((errno == 0) && (*end == '\0')) {
            ret = true;
        }
    } else if(json_is_integer(obj)) {
        *out = (int)json_integer_value(obj);
        ret = true;
    }
    return ret;
}

bool json_load_description(const json_t* obj, const char *key, String *out) {
    SANITY_CHECK(out != NULL, false);

    json_t *tmp = json_object_get(obj, key);
    if(tmp == NULL) {
       return true;
    }

    if(json_is_string(tmp)) {
        return string_copy(out, json_string_value(tmp), json_string_length(tmp));
    } 
    
    if (json_is_array(tmp)) {
        const size_t len = json_array_size(tmp);
        size_t index;
        json_t* value;
        json_array_foreach(tmp, index, value) {
            if(!json_is_string(value)) {
                continue;
            }
            if(!string_append(out, json_string_value(value), json_string_length(value))) {
                return false;
            }
            if(index < (len-1)) {
                if(!string_append_n(out, '\n', 1)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool json_print_description(Data *out, const char *key, StringView description) {
    SANITY_CHECK(out != NULL, false);
    SANITY_CHECK(key != NULL, false);
    
    String s;
    string_init(&s);

    (void)string_append_format(&s, "\"%s\":[", key);
    if(!string_view_empty(description)) {
        for(const char *str=description.data; *str; ) {
            (void)string_append(&s, "\n\t\t\"", 4U);
            for(;*str && (*str != '\n'); str++) {
                (void)string_append_n(&s, *str, 1U);
            }
            (void)string_append_format(&s, "\"%c", *str ? ',' : ' ');
            if(*str) {
                str++;
            }
        }
        (void)string_append(&s, "\n\t", 2U);
    }
    (void)string_append_n(&s, ']', 1U);

    size_t nwritten = 0;
    size_t len = string_length(&s);
    bool ret = data_write(out, (const uint8_t*)string_const_ptr(&s), len, &nwritten);
    string_release(&s);
    return ret && (nwritten == len);
}
