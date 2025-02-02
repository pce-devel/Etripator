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
#include "../section.h"

#include "../jsonhelpers.h"
#include "../message.h"
#include <jansson.h>
#include <errno.h>
#include <stdlib.h>

// Save sections to a JSON file
bool section_save(const Section *ptr, int n, const char *filename) {
    bool ret = false;
    FILE *out = fopen(filename, "wb");
    if(!out) {
        ERROR_MSG("Failed to open %s: %s", filename, strerror(errno));
    } else {   
        fprintf(out, "{\n");
        for(int i=0; i<n; i++) {
            fprintf(out, "    \"%s\": {\n", ptr[i].name);
            fprintf(out, "        \"type\": \"%s\",\n", section_type_name(ptr[i].type));
            fprintf(out, "        \"page\": \"%02x\",\n", ptr[i].page);
            fprintf(out, "        \"logical\": \"%04x\",\n", ptr[i].logical);
            fprintf(out, "        \"offset\": \"%x\",\n", ptr[i].offset);
            fprintf(out, "        \"size\": %d,\n", ptr[i].size);
            fprintf(out, "        \"mpr\": [");
            for(int j=0; j<8; j++) {
                fprintf(out, "\"%02x\"%x", ptr[i].mpr[j], (j<7) ? ',' : ']');
            }
            fprintf(out, "\n");
            fprintf(out, "        \"output\": \"%s\"", ptr[i].output);
            if(ptr[i].type == SECTION_TYPE_DATA) {
                fprintf(out, ",\n        \"data\": {\n");
                fprintf(out, "               \"type\": \"%s\",\n", data_type_name(ptr[i].data.type));
                fprintf(out, "               \"element_size\": %d,\n", ptr[i].data.element_size);
                fprintf(out, "               \"elements_per_line\": %d\n", ptr[i].data.elements_per_line);
                fprintf(out, "            }\n");
            }
            if(ptr[i].description) {
                fputc(',', out);
                json_print_description(out, "description", ptr[i].description);
            }
            fprintf(out, "\n}\n");
        }
        fprintf(out, "}\n");
        fclose(out);

        ret = true;
    }
    return 1;
}
