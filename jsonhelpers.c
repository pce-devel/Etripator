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
#include "jsonhelpers.h"

#include <errno.h>
#include <string.h>

int json_validate_int(const json_t* obj, int* out) {
    int ret = 0;
    if(json_is_string(obj)) {
        const char *str = json_string_value(obj);
        for(; (*str!='\0') && isspace(*str); str++) {
        }
        if(*str=='$') {
            str++;
        }
        errno = 0;
        *out = strtoul(str, NULL, 16);
        if(errno == 0) {
            ret = 1;
        }
    } else if(json_is_integer(obj)) {
        *out = (int)json_integer_value(obj);
        ret = 1;
    }
    return ret;
}

char* json_load_description(const json_t* obj, const char *key) {
    json_t *tmp = json_object_get(obj, key);
    char *out = NULL;
    if(json_is_string(tmp)) {
        out = strdup(json_string_value(tmp));
    } else if (json_is_array(tmp)) {
        int index;
        json_t* value;

        size_t len = 0;
        json_array_foreach(tmp, index, value) {
            if(json_is_string(value)) {
                const char *str = json_string_value(value);
                if(out) {
                    out[len-1] = '\n';
                }
                size_t n = len + strlen(str) + 1;
                char *ptr = realloc(out, n);
                memcpy(ptr+len, str, strlen(str));
                ptr[n-1] = '\0';
                out = ptr;
                len = n;
            }
        }
    }
    return out;
}

void json_print_description(FILE *out, const char *key, const char *str) {
    fprintf(out, "\"%s\":[", key);
    while(*str) {
        fprintf(out, "\n\t\t\t\"");
        for(;*str && (*str != '\n'); str++) {
            fputc(*str, out);
        }
        fprintf(out, "\"%c", *str ? ',' : ' ');
        if(*str) str++;
    }
    fprintf(out, "\n\t]");
}
