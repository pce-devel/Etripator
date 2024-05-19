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
#include "section.h"
#include "jsonhelpers.h"
#include "message.h"
#include <jansson.h>
#include <errno.h>
#include <stdlib.h>

static const int32_t g_default_element_size = 8;
static const int32_t g_default_elements_per_line = 16;

static const char *g_supported_section_types[SectionTypeCount] = {
    "data",
    "code"
};

static const char *g_supported_data_types[DataTypeCount] = {
    "binary",
    "hex",
    "string",
    "jumptable"
};

/* Retrieves section type name. */
const char* section_type_name(section_type_t type) {
    if((type <= UnknownSectionType) || (type >= SectionTypeCount)) {
        return "unknown";
    }
    return g_supported_section_types[type];
}

/* Retrieves data type name. */
const char* data_type_name(data_type_t type) {
    if((type <= UnknownDataType) || (type >= DataTypeCount)) {
        return "unknown";
    }
    return g_supported_data_types[type];
}

/* Reset a section to its default values. */
void section_reset(section_t *s) {
    s->name = NULL;
    s->type = UnknownSectionType;
    s->page = 0;
    s->logical = 0;
    s->offset = 0;
    s->size = 0;
    memset(s->mpr, 0, 8*sizeof(uint8_t));
    s->output = NULL;
    s->data.type = UnknownDataType;
    s->data.element_size = g_default_element_size;
    s->data.elements_per_line = g_default_elements_per_line;
    s->description = NULL;
}

static int section_compare(const void *a, const void *b) {
    section_t* s0 = (section_t*)a;
    section_t* s1 = (section_t*)b;
    int cmp = s0->page - s1->page;
    if(!cmp) {
        cmp = s0->logical - s1->logical;
    }
    return cmp;
}

/* Group section per output filename and sort them in bank/org order. */
void section_sort(section_t *ptr, size_t n) {
    qsort(ptr, n, sizeof(section_t), &section_compare);
}

/* Delete sections. */
void section_delete(section_t *ptr, int n) {
    int i;
    for(i=0; i<n; i++) {
        if(ptr[i].name) {
            free(ptr[i].name);
        }
        if(ptr[i].output) {
            free(ptr[i].output);
        }
        if(ptr[i].description) {
            free(ptr[i].description);
        }
    }
    free(ptr);
}
