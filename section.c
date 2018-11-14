/*
    This file is part of Etripator,
    copyright (c) 2009--2018 Vincent Cruz.

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
*/
#include "section.h"
#include "jsonhelpers.h"
#include "message.h"
#include <jansson.h>
#include <errno.h>
#include <stdlib.h>

static const int32_t g_default_element_size = 8;
static const int32_t g_default_elements_per_line = 16;

/**
 * Reset a section to its default values.
 **/
void section_reset(section_t *s) {
    s->name = NULL;
    s->type = UnknownSectionType;
    s->page = 0;
    s->logical = 0;
    s->size = -1;
    memset(s->mpr, 0, 8*sizeof(uint8_t));
    s->output = NULL;
    s->data.element_size = g_default_element_size;
    s->data.elements_per_line = g_default_elements_per_line;
}

static int section_compare(const void *a, const void *b) {
    section_t* s0 = (section_t*)a;
    section_t* s1 = (section_t*)b;
    int cmp = strcmp(s0->output, s1->output);
    if(!cmp) {
        cmp = s0->page - s1->page;
        if(!cmp) {
            cmp = s0->logical - s1->logical;
        }
    }
    return cmp;
}

/**
 * Group section per output filename and sort them in bank/org order.
 * \param [in][out] sections Sections.
 * \param [in] count Number of sections to sort.
 */
void section_sort(section_t *ptr, size_t n) {
    qsort(ptr, n, sizeof(section_t), &section_compare);
}

static const char *g_supported_types[SectionTypeCount] = {
    "binary", 
    "data",
    "code"
};

static section_type_t json_validate_type(const char *str) {
    int i;
    for (i = 0; i < SectionTypeCount; i++) {
        if (strncmp(g_supported_types[i], str, strlen(g_supported_types[i])) == 0) {
            return (section_type_t)i;
        }
    }
    return UnknownSectionType;
}

/**
 * Initializes section from JSON object.
 * @param [in] obj JSON object.
 * @param [out] out Section.
 * @return 1 upon success or 0 if an error occured.
 **/
static int section_parse(const json_t *obj, section_t *out) {
    const json_t *tmp;
    int num;
    section_reset(out);
    if (!json_is_object(obj)) {
        ERROR_MSG("Invalid json element.");
        return 0;
    }
    /* type */
    tmp = json_object_get(obj, "type");
    if (!json_is_string(tmp)) {
        ERROR_MSG("Invalid data for section type.");
        return 0;
    }
    out->type = json_validate_type(json_string_value(tmp));     
    if (out->type == UnknownSectionType) {
        ERROR_MSG("Invalid section type.");
        return 0;
    }
    /* page */
    tmp = json_object_get(obj, "page");
    if (!json_validate_int(tmp, &num)) {
        ERROR_MSG("Invalid or missing memory page.");
        return 0;
    }
    if ((num < 0) || (num > 255)) {
        ERROR_MSG("Page out of range.");
        return 0;
    }
    out->page = (uint8_t)num;
    /* logical address */
    tmp = json_object_get(obj, "logical");
    if (!json_validate_int(tmp, &num)) {
        ERROR_MSG("Invalid or missing logical address.");
        return 0;
    }
    if ((num < 0) || (num > 0xffff)) {
        ERROR_MSG("Org value out of range.");
        return 0;
    }
    out->logical = (uint16_t)num;
    /* offset */
    num = -1;
    tmp = json_object_get(obj, "offset");
    if (tmp) {
        if (!json_validate_int(tmp, &num)) {
            ERROR_MSG("Invalid or missing offset value.");
            return 0;
        }
    }
    if (num >= 0) {
        out->offset = (uint32_t)num;
    }
    else {
        out->offset = (out->page << 13) | (out->logical & 0x1fff);
    }
    /* size */
    tmp = json_object_get(obj, "size");
    if (tmp) {
        if (!json_validate_int(tmp, &out->size)) {
            ERROR_MSG("Invalid section size.");
            return 0;
        }
    }
    /* mpr */
    tmp = json_object_get(obj, "mpr");
    if(json_is_array(tmp)) {
        size_t index;
        json_t* value;
        json_array_foreach(tmp, index, value) {
            if(index > 7) {
                WARNING_MSG("Extra mpr values");
                break;
            }
            if(json_validate_int(value, &num)) {
                if((num < 0) && (num > 0xff)) {
                    ERROR_MSG("Invalid mpr %d value", index);
                    return 0;
                }
                out->mpr[index] = (uint8_t)num;
            }
        }
    }
    /* output */
    tmp = json_object_get(obj, "filename");
    if (!json_is_string(tmp)) {
        ERROR_MSG("Missing or invalid output filename.");
        return 0;
    }
    out->output = strdup(json_string_value(tmp));
    /* data */
    tmp = json_object_get(obj, "data");
    if (tmp) {
        json_t* value;
        value = json_object_get(tmp, "element_size");
        if (!json_validate_int(value, &out->data.element_size) || (out->data.element_size > 2)) {
            ERROR_MSG("Invalid element size.");
            return 0;
        }
        value = json_object_get(tmp, "elements_per_line");
        if (!json_validate_int(value, &out->data.elements_per_line)) {
            ERROR_MSG("Invalid number of elements per line.");
            return 0;
        }
    }
    return 1;
}

/**
 * Load sections from a JSON file.
 * \param [in]  filename Input filename.
 * \param [out] sections Loaded sections.
 * \param [out] count Number of loaded sections. 
 * \return 1 if the sections contained in the file were succesfully loaded.
 *         0 if an error occured.
 */
int section_load(const char *filename, section_t **out, int *n) {
    json_t* root;
    json_t* obj;
    json_error_t err;
    section_t *ptr;
    const char* key;
    size_t size;
    int ret = 1;

    root = json_load_file(filename, 0, &err);
    if(!root) {
        ERROR_MSG("Failed to parse %s: %s", filename, err.text);
        return 0;
    }

    if(!json_is_object(root)) {
        ERROR_MSG("Invalid root element");
        json_decref(root);
        return ret;
    }

    size = json_object_size(root);
    *out = (section_t*)realloc(*out, (*n+size) * sizeof(section_t));
    ptr = *out + *n;
    *n += size;
    json_object_foreach(root, key, obj) {
        section_reset(ptr);
        ret = ret && section_parse(obj, ptr);
        if(ret) {
            ptr->name = strdup(key);
        }
        ptr++;
    }

    json_decref(root);
    return ret;
}

/**
 * Save sections to a JSON file.
 * \param [in] filename Output filename.
 * \param [in] sections Sections to be saved.
 * \param [in] count Number of sections. 
 * \return 1 if the sections were succesfully saved.
 *         0 if an error occured.
 */
int section_save(const char *filename, section_t *ptr, int n) {
    int i;
    FILE *out = fopen(filename, "wb");
    if(!out) {
        ERROR_MSG("Failed to open %s: %s", filename, strerror(errno));
        return 0;
    }
    
    fprintf(out, "{\n");
    for(i=0; i<n; i++) {
        int j;
        fprintf(out, "    \"%s\": {\n", ptr[i].name);
        fprintf(out, "        \"type\": \"%s\",\n", g_supported_types[ptr[i].type]);
        fprintf(out, "        \"page\": \"%02x\",\n", ptr[i].page);
        fprintf(out, "        \"logical\": \"%04x\",\n", ptr[i].logical);
        fprintf(out, "        \"offset\": \"%x\",\n", ptr[i].offset);
        fprintf(out, "        \"size\": %d,\n", ptr[i].size);
        fprintf(out, "        \"mpr\": [");
        for(j=0; j<8; j++) {
            fprintf(out, "\"%02x\"%x", ptr[i].mpr[j], (j<7) ? ',' : ']');
        }
        fprintf(out, "\n");
        fprintf(out, "        \"output\": \"%s\"", ptr[i].output);
        if(ptr[i].type == Data) {
            fprintf(out, ",\n        \"data\": { \"element_size\": %d, \"elements_per_line\": %d }", ptr[i].data.element_size, ptr[i].data.elements_per_line);
        }
        fprintf(out, "\n}\n");
    }
    fprintf(out, "}\n");
    
    fclose(out);
    return 1;
}

/**
 * Delete sections.
 */
void section_delete(section_t *ptr, int n) {
    int i;
    for(i=0; i<n; i++) {
        if(ptr[i].name) {
            free(ptr[i].name);
        }
        if(ptr[i].output) {
            free(ptr[i].output);
        }
    }
    free(ptr);
}
