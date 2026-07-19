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
#include <etripator/message.h>
#include <etripator/comment.h>

#include "../json_helpers.h"

static bool comment_save(Comment *comment, Data *out) {
    bool ret = false;
    String str;
    string_init(&str);
    if(!string_format(&str, "\t{ \"logical\":\"%04x\", \"page\":\"%02x\", ", comment->logical, comment->page)) {
        ERROR_MSG("failed to format output for comment at offset: %04x, page: %02x", comment->logical, comment->page);    
    } else if(!data_print(out, string_get_view(&str))) {
        ERROR_MSG("failed to output comment at offset: %04x, page: %02x", comment->logical, comment->page);    
    } else {
        ret = true;
    }
    string_release(&str);
    
    if(!ret) {
        return false;
    }

    if(!json_print_description(out, "text", comment->text)) {
        ERROR_MSG("failed to write comment test at offset: %04x, page: %02x", comment->logical, comment->page);
        return false;
    }

    uint8_t end = '}';
    if(!data_write(out, &end, 1, NULL)) {
        ERROR_MSG("failed to write commet at offset: %04x, page: %02x", comment->logical, comment->page);
        return false;
    }

    return true;
}

// Save comments to file.
bool comment_repository_save(CommentRepository* repository,  Data* out) {
    assert(out != NULL);
    assert(repository != NULL);

    int count = comment_repository_size(repository);

    if(!data_print(out, string_view_from_literal("[\n"))) {
        return false;
    }

    for(int i=0; i<count; i++) {
        Comment comment = {0};
        if(!comment_repository_get(repository, i, &comment)) {
            ERROR_MSG("failed to retrieve comment #%d", i);
            return false;
        }
        if(!comment_save(&comment, out)) {
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
