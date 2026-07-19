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
#include <jansson.h>
#include <errno.h>
#include <stdlib.h>

#include <etripator/utils.h>
#include <etripator/section.h>
#include <etripator/message.h>

#include "json_helpers.h"

static const int32_t g_default_element_size = 8;
static const int32_t g_default_elements_per_line = 16;

// Retrieves section type name
StringView section_type_name(SectionType type) {
    const char *str;
    switch(type) {
        case SECTION_TYPE_DATA:
            str = "data";
            break;
        case SECTION_TYPE_CODE:
            str = "code";
            break;
        default:
            str = "unknown";
            break;
    }
    return string_view_from_literal(str);
}

// Retrieves data type name
StringView data_type_name(DataType type) {
    const char *str;
    switch(type) {
        case DATA_TYPE_BINARY:
            str = "binary";
            break;
        case DATA_TYPE_HEX:
            str = "hex";
            break;
        case DATA_TYPE_STRING:
            str = "string";
            break;
        case DATA_TYPE_JUMP_TABLE:
            str = "jumptable";
            break;
        default:
            str = "unknown";
            break;
    };
    return string_view_from_literal(str);
}

// Reset a section to its default values
void section_reset(Section *s) {
    SANITY_CHECK(s != NULL);
    *s = (Section) {
        .base = {
            .type = SECTION_TYPE_UNKNOWN,
        }
    };
}

static int section_compare(const void *a, const void *b) {
    const Section* s0 = (const Section*)a;
    const Section* s1 = (const Section*)b;
   
    int cmp = s0->base.page - s1->base.page;
    if(!cmp) {
        cmp = s0->base.logical - s1->base.logical;
    }
    return cmp;
}

// Check if 2 sections overlaps.
// \return 1 Both sections has the same type and overlaps
// \return 0 Sections don't overlap
// \return -1 Sections overlap but they are not of the same type.
static int section_overlap(const Section *a, const Section *b) {
    int ret = 0;
    if(a->base.page == b->base.page) {
        if(a->base.logical > b->base.logical) {
            const Section *tmp = b;
            b = a;
            a = tmp;
        }
        if(a->base.type != b->base.type) {
            ret = (b->base.logical < (a->base.logical + a->base.size)) ? -1 : 0;
        } else if(b->base.logical <= (a->base.logical + a->base.size)) {
            if(a->base.type == SECTION_TYPE_DATA) {
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
    uint16_t begin = minu16(a->base.logical, b->base.logical);
    int32_t end = maxi32(a->base.logical+a->base.size, b->base.logical+b->base.size);
    a->base.logical = begin;
    a->base.size = end - begin;
}

struct SectionStorage {
    Section impl;
    String output;
    String name;
    String description;
};

// Reset a section array.
void section_array_reset(SectionArray *arr) {
    SANITY_CHECK(arr != NULL);
    if(arr->data != NULL) {
        for(size_t i=0; i<arr->count; i++) {
            section_reset(&arr->data[i].impl);
            string_release(&arr->data[i].output);
            string_release(&arr->data[i].name);
            string_release(&arr->data[i].description);
        }
    }
    arr->count = 0;
}

// Release section array memory.
void section_array_release(SectionArray *arr) {
    SANITY_CHECK(arr != NULL);
    section_array_reset(arr);
    free(arr->data);
    arr->data = NULL;
    arr->capacity = 0;
}

// Add a new section.
int section_array_add(SectionArray *arr, const Section* in) {
    SANITY_CHECK((arr != NULL) && (in != NULL), -1);

    int ret = -1;
    // Check if we need to expand section array buffer
    if(arr->count >= arr->capacity) {
        size_t n = arr->capacity + 4U;
        SectionStorage *ptr = realloc(arr->data, n*sizeof(SectionStorage));
        if(ptr == NULL) {
            ERROR_MSG("Failed to expand section array buffer: %s", strerror(errno));
        } else {
            arr->data = ptr;
            arr->capacity = n;
            ret = 1;
        }
    } else {
        ret = 1;
    }

    if(ret > 0) {
        arr->data[arr->count].impl = *in;
        
        // [todo] need to check'em all ...
        (void)string_from_view(&arr->data[arr->count].output, in->base.output);
        (void)string_from_view(&arr->data[arr->count].name, in->base.name);
        (void)string_from_view(&arr->data[arr->count].description, in->base.description);
        
        arr->count++;
    }
    return ret;
}

// Retrieve the ith section from the array
bool section_array_get(SectionArray *arr, int i, Section *out) {
    SANITY_CHECK(out != NULL, false);
    if((arr == NULL) || (arr->data == NULL) || (i >= (int)arr->count)) {
        *out = (Section){0};
        return false;
        
    }
    arr->data[i].impl.base.output = string_get_view(&arr->data[i].output);
    arr->data[i].impl.base.name = string_get_view(&arr->data[i].name);
    arr->data[i].impl.base.description = string_get_view(&arr->data[i].description);

    *out = arr->data[i].impl;
    
    return true;
}

//
bool section_array_delete(SectionArray *arr, size_t i) {
    SANITY_CHECK(arr != NULL, false);
    if((arr->data == NULL) || (i >= arr->count)) {
        return false;
    }
    
    string_release(&arr->data[i].output);
    string_release(&arr->data[i].name);
    string_release(&arr->data[i].description);

    arr->count--;
    if(i < arr->count) {
        size_t count = arr->count - i;
        memmove(&arr->data[i], &arr->data[i+1], count * sizeof(SectionStorage));
        arr->data[arr->count] = (SectionStorage) {0};
    } else {
        arr->data[i] = (SectionStorage) {0};
    }

    return true;
}

// Merge and sort sections.
void section_array_tidy(SectionArray *arr) {
    SANITY_CHECK((arr != NULL) && (arr->count != 0));
/* [todo]
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
            j++;
            section[j] = arr->data[i];
        }
    }
    free(arr->data);
    arr->data = section;
    arr->count = j+1;
*/
}
