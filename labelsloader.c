/*
    This file is part of Etripator,
    copyright (c) 2009--2015 Vincent Cruz.

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
#include "message.h"
#include "jsonhelpers.h"
#include "labelsloader.h"

#define MAX_LABEL_NAME 64

/**
 * Load labels from file.
 * \param [in]  filename Input filename.
 * \param [out] reposity Label repository.
 * \return 1 if the labels contained in the file was succesfully added to the repository.
 *         0 if an error occured.
 */
int loadLabels(const char* filename, LabelRepository* repository)
{
    json_t* root;
    json_t* value;
    json_error_t err;
    const char* key;
    int num;
    uint16_t logical;
    uint8_t page;

    root = json_load_file(filename, 0, &err);
    if(!root) {
        ERROR_MSG("Failed to parse %s: %s", filename, err.text);
        return 0;
    }
    if(!json_is_object(root)) {
        ERROR_MSG("Object expected.");
        return 0;
    }
    
    json_object_foreach(root, key, value) {
        json_t* tmp;
        if(!json_is_object(value)) {
            ERROR_MSG("Expected object for %s", key);
            return 0;
        }
        // logical
        tmp = json_object_get(value, "logical");
        if(!json_validateInt(tmp, &num)) {
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
        if(!json_validateInt(tmp, &num)) {
            ERROR_MSG("Invalid or missing page.");
            return 0;
        }
        if((num < 0) || (num > 0xff)) {
            ERROR_MSG("Page value out of range.");
            return 0;
        }
	page = (uint8_t)num;

        if(!addLabel(repository, key, logical, page)) {
            return 0;
        }
    }

    json_decref(root);
    return 1;
}
