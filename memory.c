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
#include "memory.h"
#include "message.h"
/**
 * Create memory block.
 * \param [out] mem Memory block.
 * \param [in]  len Memory block size (in bytes).
 * \return 1 upon success, 0 if an error occured.
 */
int mem_create(mem_t *mem, size_t len) {
    mem->data = (uint8_t*)malloc(len);
    if(!mem->data) {
        ERROR_MSG("Unable to allocate memory : %s.\n", strerror(errno));
        mem->len = 0;
        return 0;
    }
    mem->len = len;
    return 1;
}
/**
 * Destroy memory block.
 * \param [in] mem Memory block.
 */
void mem_destroy(mem_t *mem) {
    if(mem) {
        mem->len = 0;
        if(mem->data) {
            free(mem->data);
            mem->data = NULL;
        }
    }
}
/**
 * Fill memory block bytes with a given byte value.
 * \param [in] mem Memory block.
 * \param [in] c Byte value.
 */
void mem_fill(mem_t *mem, uint8_t c) {
    if(mem->data && mem->len) {
        memset(mem->data, mem->len, c);
    }
}
