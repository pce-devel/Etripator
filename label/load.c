/*
    This file is part of Etripator,
    copyright (c) 2009--2021 Vincent Cruz.

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
#include <jansson.h>
#include "../message.h"
#include "../jsonhelpers.h"
#include "load.h"

#define MAX_LABEL_NAME 128

/**
 * Load labels from file.
 * \param [in]  filename Input filename.
 * \param [out] repository Label repository.
 * \return 1 if the labels contained in the file were succesfully added to the repository.
 *         0 if an error occured.
 */
int label_repository_load(const char* filename, label_repository_t* repository) {
    json_t* root;
    json_t* value;
    json_error_t err;
    const char* key;
    int index;
    int num;
    uint16_t logical;
    uint8_t page;
    const char *description;

    root = json_load_file(filename, 0, &err);
    if(!root) {
        ERROR_MSG("Failed to parse %s: %s", filename, err.text);
        return 0;
    }
    if(!json_is_array(root)) {
        ERROR_MSG("Array expected.");
        return 0;
    }
    
    json_array_foreach(root, index, value) {
        json_t* tmp;
        if(!json_is_object(value)) {
            ERROR_MSG("Expected object.");
            return 0;
        }
        // name
        tmp = json_object_get(value, "name");
        if (!json_is_string(tmp)) {
            ERROR_MSG("Missing or invalid label name.");
            return 0;
        }
        key = json_string_value(tmp);

        // logical
        tmp = json_object_get(value, "logical");
        if(!json_validate_int(tmp, &num)) {
            ERROR_MSG("Invalid or missing logical address.");
            return 0;
        }
        if((num < 0) || (num > 0xffff)) {
            ERROR_MSG("Logical address out of range.");
            return 0;
        }
        logical = (uint16_t)num;
        // page
        tmp = json_object_get(value, "page");
        if(!json_validate_int(tmp, &num)) {
            ERROR_MSG("Invalid or missing page.");
            return 0;
        }
        // description
        description = json_load_description(value);
        
        if((num < 0) || (num > 0xff)) {
            ERROR_MSG("Page value out of range.");
            return 0;
        }
	    page = (uint8_t)num;

        if(!label_repository_add(repository, key, logical, page, description)) {
            return 0;
        }
    }

    json_decref(root);
    return 1;
}
