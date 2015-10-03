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
#ifndef CD_H
#define CD_H

#include "config.h"
#include "memorymap.h"

/**
 * Load CDROM data from file.
 * \param [in]  filename CDROM data filename.
 * \param [in]  start    CDROM data offset.
 * \param [in]  len      CDROM data length (in bytes).
 * \param [in]  page     Memory page.
 * \param [in]  offset   memory page offset.
 * \param [out] memmap   Memory map.
 * \return 1 upon success, 0 if an error occured.
 */
int loadCD(const char* filename, size_t start, size_t len, uint8_t page, size_t offset, MemoryMap* memmap);

#endif // CD_H
