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
#ifndef MEMORY_H
#define MEMORY_H

#include "config.h"

/**
 * Byte array.
 */
typedef struct
{
    size_t   len;
    uint8_t *data;
} Memory;

/**
 * Create memory unit.
 * \param [out] mem Memory unit.
 * \param [in]  len Memory size (in bytes).
 * \return 1 upon success, 0 if an error occured.
 */
int createMemory(Memory *mem, size_t len);
/**
 * Destroy memory.
 * \param [in] mem Memory unit.
 */
void destroyMemory(Memory *mem);

#endif // MEMORY_H
