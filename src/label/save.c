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

#include <etripator/message.h>
#include <etripator/label.h>

#include "../json_helpers.h"

static bool label_save(Label *label, Data *out) {
    bool ret = false;
    String str;
    string_init(&str);
    if(!string_format(&str, "\t{ \"name\":\"%s\", \"logical\":\"%04x\", \"page\":\"%02x\"", label->name.data, label->logical, label->page)) {
        ERROR_MSG("failed to format output for label %s", label->name.data);    
    } else if(!data_print(out, string_get_view(&str))) {
        ERROR_MSG("failed to output label %s", label->name.data);
    } else {
        ret = true;
    }
    string_release(&str);
    
    if(!ret) {
        return false;
    }
    
    if(!string_view_empty(label->description)) {
        uint8_t sep = ',';
        if(!data_write(out, &sep, 1, NULL)) {
            ERROR_MSG("failed to write description for label %s", label->name.data);
            return false;
        }
        if(!json_print_description(out, "description", label->description)) {
            ERROR_MSG("failed to write description for label %s", label->name.data);
            return false;
        }
    }
    uint8_t end = '}';
    if(!data_write(out, &end, 1, NULL)) {
        ERROR_MSG("failed to write label %s", label->name.data);
        return false;
    }
    return true;
}

// Save labels to file
bool label_repository_save(LabelRepository* repository, Data *out) {
    assert(repository != NULL);
    assert(out != NULL);

    int count = label_repository_size(repository);

    if(!data_print(out, string_view_from_literal("[\n"))) {
        return false;
    }

    for(int i=0; i<count; i++) {
        Label label = {0};
        if(!label_repository_get(repository, i, &label)) {
            ERROR_MSG("failed to retrieve label #%d", i);
            return false;
        }
        if(!label_save(&label, out)) {
            return false;
        }
        char buffer[2] = {
            [0] = (i<(count-1)) ? ',' : ' ',
            [1] = '\n'
        };
        if(!data_write(out, (const uint8_t*)buffer, sizeof(buffer), NULL)) {
            return false;
        }
    }

    if(!data_print(out, string_view_from_literal("]\n"))) {
        return false;
    }

    return true;
}
