/*
    This file is part of Etripator,
    copyright (c) 2009--2019 Vincent Cruz.

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
#include <errno.h>
#include <string.h>

#include "writer.h"
#include "../message.h"

/**
 * Write labels to file.
 * \param [in] filename Configuration file.
 * \param [in] reposity Label repository.
 * \return 1 if the labels in the repository were succesfully written to the file.
 *         0 if an error occured.
 */
int label_repository_write(const char* filename, label_repository_t* repository) {
    FILE *stream = fopen(filename, "wb");
    int i, count = label_repository_size(repository);
    if(stream == NULL) {
        ERROR_MSG("Failed to open %s: %s", filename, strerror(errno));
        return 0;
    }
    fprintf(stream, "[\n");
    for(i=0; i<count; i++) {
        uint16_t logical;
        uint8_t page;
        char* name;
        if(label_repository_get(repository, i, &logical, &page, &name)) {
            fprintf(stream, "\t{ \"name\":\"%s\", \"logical\":\"%04x\", \"page\":\"%02x\"}%c\n", name, logical, page, (i<(count-1)) ? ',' : ' ');
        }
    }
    fprintf(stream, "]\n");
    fclose(stream);
    return 1;
}
