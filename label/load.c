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
#include <jansson.h>
#include "../message.h"
#include "../jsonhelpers.h"
#include "../label.h"

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
    json_error_t err;
    json_t* value;
    int ret = 0, index = 0;
    root = json_load_file(filename, 0, &err);
    if(!root) {
        ERROR_MSG("Failed to parse %s: %s", filename, err.text);
    } else {
        if(!json_is_array(root)) {
            ERROR_MSG("Array expected.");
        } else for (index = 0, ret = 1; ret && (index < json_array_size(root)) && (value = json_array_get(root, index)); index++) {
            ret = 0;
            if(!json_is_object(value)) {
                ERROR_MSG("Expected object.");
            } else {
                // name
                json_t* tmp = json_object_get(value, "name");
                if (!json_is_string(tmp)) {
                    ERROR_MSG("Missing or invalid label name.");
                } else {
                    int num;
                    const char* key = json_string_value(tmp);
                    // logical
                    tmp = json_object_get(value, "logical");
                    if(!json_validate_int(tmp, &num)) {
                        ERROR_MSG("Invalid or missing logical address.");
                    } else if((num < 0) || (num > 0xffff)) {
                        ERROR_MSG("Logical address out of range.");
                    } else {
                        uint16_t logical = (uint16_t)num;
                        // page
                        tmp = json_object_get(value, "page");
                        if(!json_validate_int(tmp, &num)) {
                            ERROR_MSG("Invalid or missing page.");
                        } else {
                            // description
                            char* description = json_load_description(value, "description");
                            if((num < 0) || (num > 0xff)) {
                                ERROR_MSG("Page value out of range.");
                            } else if(label_repository_add(repository, key, logical, (uint8_t)num, description)) {
                                ret = 1;
                            }
                            if(description) {
                                free(description);
                            }
                        }
                    }
                }
            }
        }
        json_decref(root);
    }
    return ret;
}
