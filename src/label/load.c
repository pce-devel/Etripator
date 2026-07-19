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

#include <etripator/label.h>
#include <etripator/message.h>

#include "../json_helpers.h"

#define MAX_LABEL_NAME 128

static bool load_label(LabelRepository* repository, json_t* node) {
    if(!json_is_object(node)) {
        ERROR_MSG("Expected object.");
        return false;
    }
    // Retrieve label name.
    json_t* tmp = json_object_get(node, "name");
    if (!json_is_string(tmp)) {
        ERROR_MSG("Missing or invalid label name.");
        return false;
    }
    StringView name = string_view_from_literal(json_string_value(tmp));
 
    // Retrieve logical address.
    uint16_t logical = 0;
    if(!json_load_logical(node, &logical)) {
        return false;
    }

    // Retrieve page.
    uint8_t page = 0;
    if(!json_load_page(node, &page)) {
        return false;
    }

    bool ret = false; 

    // Retrieve description and add label to repository.
    String description;
    string_init(&description);
    if(json_load_description(node, "description", &description) != true) {
        ERROR_MSG("Failed to load label description");
    } else if(!label_repository_add(repository, logical, page, name, string_get_view(&description))) {
        ERROR_MSG("Failed to add label named %s (logical: %04x, page: %02x)", name.data, logical, page);
    } else {
        ret = true;
    }
    string_release(&description);
    return ret; 
}

// Load labels from file.
bool label_repository_load(LabelRepository* repository, Data *in) {
    assert(repository != NULL);
    assert(in != NULL);

    json_error_t err;

    json_t* root = json_load_callback(json_load_callback_impl, in, 0, &err);
    if(!root) {
        ERROR_MSG("Failed to parse %d:%d: %s", err.line, err.column, err.text);
        return false;
    }

    bool ret = false;
    if(!json_is_array(root)) {
        ERROR_MSG("Array expected.");
    } else {
        const size_t count = json_array_size(root);
        size_t index;
        for (index = 0, ret = true; ret && (index < count); index++) {
            ret = false;
            json_t *value = json_array_get(root, index);
            if(value == NULL) {
                ERROR_MSG("Failed to retrieve object %zu", index);
            } else if(!json_is_object(value)) {
                ERROR_MSG("Expected object.");
            } else {
                ret = load_label(repository, value);
            }
        }
    }
    json_decref(root);

    return ret;
}
