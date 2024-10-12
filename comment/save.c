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
  copyright (c) 2009--2024 Vincent Cruz.
 
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
#include <errno.h>
#include <string.h>

#include "../message.h"
#include "../jsonhelpers.h"
#include "../comment.h"

// Save comments to file.
bool comment_repository_save(CommentRepository* repository, const char* filename) {
    assert(filename != NULL);
    assert(repository != NULL);

    bool ret = false;

    FILE *stream = fopen(filename, "wb");
    if(stream == NULL) {
        ERROR_MSG("Failed to open %s: %s", filename, strerror(errno));
    } else {
        int count = comment_repository_size(repository);
        fprintf(stream, "[\n");
        for(int i=0; i<count; i++) {
            Comment entry;
            if(comment_repository_get(repository, i, &entry)) {
                fprintf(stream, "\t{ \"logical\":\"%04x\", \"page\":\"%02x\", \"text\": ", entry.logical, entry.page);
                
                bool multiline = false;
                if(strchr(entry.text, '\n') != NULL) {
                    multiline = true;
                }

                if(multiline) {
                    fprintf(stream, "[\n");
                    for(char *ptr=entry.text, *next=NULL; ptr!=NULL; ptr=next) {
                        next = strchr(ptr, '\n');
                        if(next != NULL) {
                            fprintf(stream, "\t\"");
                            fwrite(ptr, 1, next-ptr, stream);
                            fprintf(stream, "\",\n");
                        } else {
                            fprintf(stream, "\"%s\"\n", ptr);
                        }
                    }
                    fprintf(stream, "]\n");
                } else {
                    fprintf(stream, "\"%s\"", entry.text);
                }

                fprintf(stream,"}%c\n", (i<(count-1)) ? ',' : ' ');
            }
        }
        fprintf(stream, "]\n");
        fclose(stream);
        ret = true;
    }
    return ret;
}
