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
#include "save.h"

#include "../jsonhelpers.h"
#include "../message.h"
#include <jansson.h>
#include <errno.h>
#include <stdlib.h>

/**
 * Save sections to a JSON file.
 * \param [in] filename Output filename.
 * \param [in] sections Sections to be saved.
 * \param [in] count Number of sections. 
 * \return 1 if the sections were succesfully saved.
 *         0 if an error occured.
 */
int section_save(const char *filename, section_t *ptr, int n) {
    int i;
    FILE *out = fopen(filename, "wb");
    if(!out) {
        ERROR_MSG("Failed to open %s: %s", filename, strerror(errno));
        return 0;
    }
    
    fprintf(out, "{\n");
    for(i=0; i<n; i++) {
        int j;
        fprintf(out, "    \"%s\": {\n", ptr[i].name);
        fprintf(out, "        \"type\": \"%s\",\n", section_type_name(ptr[i].type));
        fprintf(out, "        \"page\": \"%02x\",\n", ptr[i].page);
        fprintf(out, "        \"logical\": \"%04x\",\n", ptr[i].logical);
        fprintf(out, "        \"offset\": \"%x\",\n", ptr[i].offset);
        fprintf(out, "        \"size\": %d,\n", ptr[i].size);
        fprintf(out, "        \"mpr\": [");
        for(j=0; j<8; j++) {
            fprintf(out, "\"%02x\"%x", ptr[i].mpr[j], (j<7) ? ',' : ']');
        }
        fprintf(out, "\n");
        fprintf(out, "        \"output\": \"%s\"", ptr[i].output);
        if(ptr[i].type == Data) {
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
    return 1;
}
