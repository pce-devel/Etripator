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
#ifndef ROM_H
#define ROM_H

#include <stdint.h>

/**
 * @todo
 */
struct ROM_t
{
    size_t   len;
    uint8_t *data;
    uint8_t *page[0x100];
};

typedef struct ROM_t ROM;

/**
 * Load ROM from file.
 * \param [in]  filename ROM filename.
 * \param [in]  cd       Is it a CD-ROM binary?
 * \param [out] rom      ROM object.
 * \return 1 if an error occured. 0 on success.
 */
int loadROM(const char* filename, int cd, ROM* rom);
/**
 * Destroy ROM object.
 * \param [in] rom ROM object.
 */
void destroyROM(ROM* rom);
/**
 * Read a single byte from ROM.
 * \param [in] rom     ROM object.
 * \param [in] address Physical ROM address.
 * \return byte read.
 */
uint8_t readROM(ROM* rom, off_t address);

#endif // ROM_H
