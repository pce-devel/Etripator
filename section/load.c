/*
 * This file is part of Etripator,
 * copyright (c) 2009--2023 Vincent Cruz.
 *
 * Etripator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Etripator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Etripator.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "load.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <jansson.h>

#include "../jsonhelpers.h"
#include "../message.h"

static section_type_t json_validate_section_type(const char *str) {
    int i;
    for (i = 0; i < SectionTypeCount; i++) {
        if (strncmp(section_type_name(i), str, strlen(section_type_name(i))) == 0) {
            return (section_type_t)i;
        }
    }
    return UnknownSectionType;
}

static data_type_t json_validate_data_type(const char *str) {
    int i;
    for (i = 0; i < DataTypeCount; i++) {
        if (strncmp(data_type_name(i), str, strlen(data_type_name(i))) == 0) {
            return (data_type_t)i;
        }
    }
    return UnknownDataType;
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
    out->type = json_validate_section_type(json_string_value(tmp));     
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
                if((num < 0) || (num > 0xff)) {
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
        value = json_object_get(tmp, "type");
        if (!json_is_string(value)) {
            ERROR_MSG("Invalid data for data type.");
            return 0;
        }
        out->data.type = json_validate_data_type(json_string_value(value));     
        if (out->data.type == UnknownDataType) {
            ERROR_MSG("Invalid data type.");
            return 0;
        }        
        value = json_object_get(tmp, "element_size");
        if(value && (out->data.type == Hex)) { 
            if (!json_validate_int(value, &out->data.element_size) || (out->data.element_size > 2)) {
                ERROR_MSG("Invalid element size.");
                return 0;
            }
        }
        value = json_object_get(tmp, "elements_per_line");
        if(value && (out->data.type != Binary)) {
            if (!json_validate_int(value, &out->data.elements_per_line)) {
                ERROR_MSG("Invalid number of elements per line.");
                return 0;
            }
        }
    }
    /* description (optional) */
    out->description = json_load_description(obj, "description");

    return 1;
}

/* Load sections from a JSON file. */
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
    *n += (int)size;
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
