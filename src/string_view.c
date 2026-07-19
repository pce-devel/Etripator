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
#include <etripator/string_view.h>
#include <string.h>

StringView string_view_from_literal(const char *ptr) {
    return (StringView) {
        .data = ptr,
        .length = ptr ? strlen(ptr) : 0,
    };
}

StringView string_view_from_substring(const char *ptr, size_t length) {
    return (StringView) {
        .data = ptr,
        .length = length,
    };
}

bool string_view_empty(StringView s) {
    return (s.data == NULL) || (s.length == 0);
}

bool string_view_cmp(StringView s0, StringView s1) {
    assert(s0.data != NULL);
    assert(s1.data != NULL);
    if(s0.length != s1.length) {
        return false;
    }
    for(size_t i=0; i<s0.length; i++) {
        if(s0.data[i] != s1.data[i]) {
            return false;
        }
    }
    return true;
}

bool string_view_case_cmp(StringView s0, StringView s1) {
    assert(s0.data != NULL);
    assert(s1.data != NULL);
    if(s0.length != s1.length) {
        return false;
    }
    for(size_t i=0; i<s0.length; i++) {
        char u = s0.data[i];
        char v = s1.data[i];
        if(u != v) {
            if((u >= 'A') && (u <= 'Z')) {
                u = u + 'a' - 'A';
            }
            if((v >= 'A') && (v <= 'Z')) {
                v = v + 'a' - 'A';
            }
            if(u != v) {
                return false;
            }
        }
    }
    return true;
}

size_t string_view_find_first(const StringView s, char sep) {
    if(s.data == NULL) {
        return 0;
    }
    size_t i;
    for(i=0; (i<s.length) && (s.data[i]!=sep); i++) {
    }
    return i;
}
