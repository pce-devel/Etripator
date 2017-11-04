/*
    This file is part of Etripator,
    copyright (c) 2009--2017 Vincent Cruz.

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
#include <errno.h>
#include <stdlib.h>

static const char *supported_types[SectionTypeCount] = {"bin_data", "inc_data",
                                                        "code"};

static int json_validateType(const char *str, SectionType *type) {
    int i;
    for (i = 0; i < SectionTypeCount; i++) {
        if (0 == strncmp(supported_types[i], str, strlen(supported_types[i]))) {
            *type = i;
            return 1;
        }
    }
    *type = UnknownSectionType;
    return 0;
}
/**
 * Reset a section to its default values.
 **/
void resetSection(Section *out) {
    out->type = UnknownSectionType;
    out->name = NULL;
    out->bank = 0;
    out->org = 0;
    out->offset = 0;
    out->size = 0;
    out->filename = NULL;
    memset(out->mpr, 0, 8);
}
/**
 * Initializes section from JSON object.
 * @param [in] obj JSON object.
 * @param [out] out Section.
 * @return 1 upon success or 0 if an error occured.
 **/
int parseSection(const json_t *obj, Section *out) {
    const json_t *tmp;
    int num;
    resetSection(out);
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
    if (!json_validateType(json_string_value(tmp), &out->type)) {
        ERROR_MSG("Invalid section type.");
        return 0;
    }
    /* bank */
    tmp = json_object_get(obj, "bank");
    if (!json_validateInt(tmp, &num)) {
        ERROR_MSG("Invalid or missing bank value.");
        return 0;
    }
    if ((num < 0) || (num > 255)) {
        ERROR_MSG("Bank value out of range.");
        return 0;
    }
    out->bank = (uint8_t)num;
    /* org */
    tmp = json_object_get(obj, "org");
    if (!json_validateInt(tmp, &num)) {
        ERROR_MSG("Invalid or missing org value.");
        return 0;
    }
    if ((num < 0) || (num > 0xffff)) {
        ERROR_MSG("Org value out of range.");
        return 0;
    }
    out->org = (uint16_t)num;
    /* offset */
    tmp = json_object_get(obj, "offset");
    if (tmp) {
        if (!json_validateInt(tmp, &num)) {
            ERROR_MSG("Invalid or missing offset value.");
            return 0;
        }
    }
    if (out->offset == 0) {
        out->offset = (out->bank << 13) | (out->org & 0x1fff);
    }
    /* size */
    tmp = json_object_get(obj, "size");
    if (tmp) {
        if (!json_validateInt(tmp, &out->size)) {
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
            if(json_validateInt(value, &num)) {
                if((num < 0) && (num > 0xff)) {
                    ERROR_MSG("Invalid mpr %d value", index);
                    return 0;
                }
                out->mpr[index] = (uint8_t)num;
            }
        }
    }
    /* filename */
    tmp = json_object_get(obj, "filename");
    if (!json_is_string(tmp)) {
        ERROR_MSG("Missing or invalid filename.");
        return 0;
    }
    out->filename = strdup(json_string_value(tmp));
    return 1;
}

int readSections(const char* filename, Section **out, int *count) {
    json_t* root;
    json_t* obj;
    const char* key;
    json_error_t err;
    int ret = 1;
    size_t size;
    Section *ptr;

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
    *out = (Section*)realloc(*out, (*count+size) * sizeof(Section));
    ptr = *out + *count;
    *count += size;
    json_object_foreach(root, key, obj) {
        resetSection(ptr);
        ret = ret && parseSection(obj, ptr);
        if(ret) {
            ptr->name = strdup(key);
        }
        ptr++;
    }

    json_decref(root);
    return ret;
}

