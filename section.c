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
  copyright (c) 2009--2024 Vincent Cruz.
 
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
#include "section.h"
#include "jsonhelpers.h"
#include "message.h"
#include <jansson.h>
#include <errno.h>
#include <stdlib.h>

static const int32_t g_default_element_size = 8;
static const int32_t g_default_elements_per_line = 16;

// Retrieves section type name
const char* section_type_name(SectionType type) {
    static const char *name[SECTION_TYPE_COUNT] = {
        [SECTION_TYPE_DATA] = "data",
        [SECTION_TYPE_CODE] = "code"
    };

    if((type <= SECTION_TYPE_UNKNOWN) || (type >= SECTION_TYPE_COUNT)) {
        return "unknown";
    }
    return name[type];
}

// Retrieves data type name
const char* data_type_name(DataType type) {
    static const char *name[DATA_TYPE_COUNT] = {
        [DATA_TYPE_BINARY] = "binary",
        [DATA_TYPE_HEX] = "hex",
        [DATA_TYPE_STRING] = "string",
        [DATA_TYPE_JUMP_TABLE] = "jumptable"
    };

    if((type <= DATA_TYPE_UNKNOWN) || (type >= DATA_TYPE_COUNT)) {
        return "unknown";
    }
    return name[type];
}

// Reset a section to its default values
void section_reset(Section *s) {
    assert(s != NULL);
    s->name = NULL;
    s->type = SECTION_TYPE_UNKNOWN;
    s->page = 0;
    s->logical = 0;
    s->offset = 0;
    s->size = 0;
    for(unsigned int i=0; i<8U; i++) {
        s->mpr[i] = 0;
    }
    s->output = NULL;
    s->data.type = DATA_TYPE_UNKNOWN;
    s->data.element_size = g_default_element_size;
    s->data.elements_per_line = g_default_elements_per_line;
    s->description = NULL;
}

static int section_compare(const void *a, const void *b) {
    Section* s0 = (Section*)a;
    Section* s1 = (Section*)b;
   
    int cmp = s0->page - s1->page;
    if(!cmp) {
        cmp = s0->logical - s1->logical;
    }
    return cmp;
}

// Group section per output filename and sort them in bank/org order.
void section_sort(Section *ptr, size_t n) {
    qsort(ptr, n, sizeof(Section), &section_compare);
}

// Delete sections.
void section_delete(Section *ptr, int n) {
    assert(ptr != NULL);
    for(int i=0; i<n; i++) {
        free(ptr[i].name);
        free(ptr[i].output);
        free(ptr[i].description);
    }
    free(ptr);
}
