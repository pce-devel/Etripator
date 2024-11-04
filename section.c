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

// Reset a section array.
void section_array_reset(SectionArray *arr) {
    assert(arr != NULL);
    if(arr->data != NULL) {
        for(size_t i=0; i<arr->count; i++) {
            free(arr->data[i].name);
            free(arr->data[i].output);
            free(arr->data[i].description);
            section_reset(&arr->data[i]);
        }
    }
    arr->count = 0;
}

// Release section array memory.
void section_array_delete(SectionArray *arr) {
    assert(arr != NULL);
    section_array_reset(arr);
    free(arr->data);
    arr->data = NULL;
    arr->capacity = 0;
}

// Check if 2 sections overlaps.
// \return 1 Both sections has the same time and overlaps
// \return 0 Sections don't overlap
// \return -1 Sections overlap but they are not of the same type.
static int section_overlap(const Section *a, const Section *b) {
    assert(a != NULL);
    assert(b != NULL);

    int ret = 0;

    if(a->page == b->page) {
        if(a->logical > b->logical) {
            const Section *tmp = b;
            b = a;
            a = tmp;
        }
        if(b->logical <= (a->logical + a->size)) {
            if(a->type == b->type) {
                ret = 1;
            } else {
                ret = -1;
            }
        }
    }
    return ret;
}

static inline uint16_t minu16(uint16_t a, uint16_t b) {
    return (a < b) ? a : b;
}

static inline uint16_t maxi32(int32_t a, int32_t b) {
    return (a > b) ? a : b;
}

// Merge 2 sections the 2nd section into the 1st one.
static void section_merge(Section *a, const Section *b) {
    assert(a != NULL);
    assert(b != NULL);

    uint16_t begin = minu16(a->logical, b->logical);
    int32_t end = maxi32(a->logical+a->size, b->logical+b->size);
    a->logical = begin;
    a->size = end - begin;
}

// Add a new section.
int section_array_add(SectionArray *arr, const Section* in) {
    assert(arr != NULL);
    assert(in != NULL);

    int ret = -1;//todo
    size_t i;
    // Search for overlapping section.
    for(i=0; i<arr->count; i++) {
        int overlap = section_overlap(&arr->data[i], in);
        if(overlap ==  1) {
            section_merge(&arr->data[i], in);
            INFO_MSG("Section %s has been merged with %s!",  arr->data[i].name, in->name);
            break;
        } else if(overlap == -1) {
            WARNING_MSG("Section %s and %s overlaps! %x %x.%x", arr->data[i].name, in->name);
            break;
        }
    }

    if(i >= arr->count) {
        // Check if we need to expand section array buffer
        if(i >= arr->capacity) {
            size_t n = arr->capacity + 4U;
            Section *ptr = realloc(arr->data, n*sizeof(Section));
            if(ptr == NULL) {
                ERROR_MSG("Failed to expand section array buffer", strerror(errno));
            } else {
                arr->data = ptr;
                arr->capacity = n;
                ret = true;
            }
        } else {
            ret = true;
        }

        // Append new section.
        if(ret) {
            arr->data[arr->count++] = *in;
        }
    }
    return ret;
}

// Retrieve the ith section from the array.
const Section* section_array_get(SectionArray *arr, size_t i) {
    const Section *ret = NULL;
    if(arr != NULL) {
        if((arr->data != NULL) && (i < arr->count)) {
            ret = &arr->data[i];
        }
    }
    return ret;
}