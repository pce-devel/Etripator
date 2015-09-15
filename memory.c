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
#include "memory.h"
#include "message.h"

/**
 * Create memory unit.
 * \param [out] mem Memory unit.
 * \param [in]  len Memory size (in bytes).
 */
int createMemory(Memory *mem, size_t len)
{
    mem->data = (uint8_t*)malloc(len);
    if(NULL == mem->data)
    {
        ERROR_MSG("Unable to allocate memory : %s.\n", strerror(errno));
        mem->len = 0;
        return 1;
    }
    mem->len = len;
    return 0;
}

/**
 * Destroy memory.
 * \param [in] mem Memory unit.
 */
void destroyMemory(Memory *mem)
{
    if(NULL == mem)
    {
        return;
    }
    if(NULL != mem->data)
    {
        free(mem->data);
        mem->data = NULL;
    }
    mem->len = 0;
}
