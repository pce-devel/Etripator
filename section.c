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

// Delete section.
void section_delete(Section *ptr) {
    assert(ptr != NULL);
    free(ptr->name);
    free(ptr->output);
    free(ptr->description);
    section_reset(ptr);
}

// Reset a section array.
void section_array_reset(SectionArray *arr) {
    assert(arr != NULL);
    if(arr->data != NULL) {
        for(size_t i=0; i<arr->count; i++) {
            section_delete(&arr->data[i]);
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
        if(a->type != b->type) {
            ret = (b->logical < (a->logical + a->size)) ? -1 : 0;
        } else if(b->logical <= (a->logical + a->size)) {
            if(a->type == SECTION_TYPE_DATA) {
                ret =  (a->data.type == b->data.type) ? 1 : -1;
            } else {
                ret = 1;
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

    int ret = -1;
    // Check if we need to expand section array buffer
    if(arr->count >= arr->capacity) {
        size_t n = arr->capacity + 4U;
        Section *ptr = realloc(arr->data, n*sizeof(Section));
        if(ptr == NULL) {
            ERROR_MSG("Failed to expand section array buffer", strerror(errno));
        } else {
            arr->data = ptr;
            arr->capacity = n;
            ret = 1;
        }
    } else {
        ret = 1;
    }

    if(ret > 0) {
        arr->data[arr->count] = *in;   
        arr->count++;
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

// Merge and sort sections.
void section_array_tidy(SectionArray *arr) {
    assert(arr != NULL);
    assert(arr->count != 0);

    Section *section = calloc(arr->count, sizeof(Section));
    
    qsort(arr->data, arr->count, sizeof(Section), section_compare);

    size_t j = 0;
    section[0] = arr->data[0];
    for(size_t i=1; i<arr->count; i++) {
        int overlap = section_overlap(&section[j], &arr->data[i]);
        if(overlap == 1) {
            section_merge(&section[j], &arr->data[i]);
            INFO_MSG("Section %s has been merged with %s!",  arr->data[i].name, section[j].name);
            section_delete(&arr->data[i]);
        } else {
            if(overlap == -1) {
                WARNING_MSG("Section %s and %s overlaps!", arr->data[i].name, section[j].name);
            }
            section[++j] = arr->data[i];
        }
    }
    free(arr->data);
    arr->data = section;
    arr->count = j+1;
}
