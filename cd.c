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
#include "cd.h"
#include "message.h"

/**
 * Load CDROM data from file.
 * \param [in]  filename CDROM data filename.
 * \param [in]  start    CDROM data offset.
 * \param [in]  len      CDROM data length (in bytes).
 * \param [in]  page     Memory page.
 * \param [in]  offset   memory page offset.
 * \param [out] memmap   Memory map.
 * \return 1 if an error occured. 0 on success.
 */
int loadCD(const char* filename, size_t start, size_t len, uint8_t page, size_t offset, MemoryMap* memmap)
{
    FILE *in;
    int ret;

    size_t addr;
    size_t nRead;

    in = fopen(filename, "rb");
    if(NULL == in)
    {
        ERROR_MSG("Unable to open %s : %s", filename, strerror(errno));
        return 0;
    }

    ret = fseek(in, start, SEEK_SET);
    if(ret)
    {
        ERROR_MSG("Offset out of bound : %s", strerror(errno));
        fclose(in);
        return 0;
    }

    addr = (page << 13) | (offset & 0x1fff);
    nRead = fread(memmap->page[page] + addr, 1, len, in);    
    if(nRead != len)
    {
        ERROR_MSG("Failed to read %d bytes : %s", len, strerror(errno));
        fclose(in);
        return 0;
    }
    
    fclose(in);
    return 1;
}
