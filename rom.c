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
#include <stdio.h>
#include <errno.h>
#include "message.h"
#include "rom.h"

/**
 * Load ROM from file.
 * \param [in]  filename ROM filename.
 * \param [in]  cd       Is it a CD-ROM binary?
 * \param [out] rom      ROM object.
 * \return 1 if an error occured. 0 on success.
 */
int loadROM(const char* filename, int cd, ROM* rom)
{
    FILE   *in;
    size_t size;
    size_t count, nRead;
    
    /* Reset ROM object */
    memset(rom, 0, sizeof(ROM));

    /* Open file */
    in = fopen(filename, "rb");
    if(NULL == in)
    {
        ERROR_MSG("Unable to open %s : %s", filename, strerror(errno));
        return 1;
    }

    /* Compute file size. */
    fseek(in, 0, SEEK_END);
    size  = ftell(in);
    fseek(in, 0, SEEK_SET);
    size -= ftell(in);

    if(!cd)
    {
        /* Check for possible header */
        if(size & 0x200)
        {
            /* Jump header */
            size &= ~0x200;
            fseek(in, 0x200, SEEK_SET);
        }
    }

    /* Adjust rom size if needed */
    rom->len = (size + 0x1fff) & ~0x1fff;

    /* Allocate rom storage */
    rom->data = (uint8_t*)malloc(rom->len);
    if(NULL == rom->data)
    {
        ERROR_MSG("Failed to allocate ROM storage : %s", strerror(errno));
        goto err_0;
    }
    /* Fill rom with 0xff */
    memset(rom->data, 0xff, rom->len);
    
    /* Read ROM data */
    count = (size < rom->len) ? size : rom->len;
    nRead = fread(rom->data, 1, count, in);
    if(nRead != count)
    {
        ERROR_MSG("Failed to read ROM data from %s : %s", filename, strerror(errno));
        goto err_1;
    }
    fclose(in);

    /* Initialize ROM pages (from mednafen source code). */
    /* Note : the decrement by (i*8192) is a trick to avoid doing a 
     *        bitwise with the address when reading a byte from that
     *        page. */
    if(0x60000 == rom->len)
    {
        int i;
        for(i=0; i<64; i++)
        {
            rom->page[i] = &rom->data[(i & 0x1f) * 8192] - (i*8192);
        }
        for(i=64; i<128; i++)
        {
            rom->page[i] = &rom->data[((i & 0x0f) + 32) * 8192] - (i*8192);
        }
    }
    else if(0x80000 == rom->len)
    {
        int i;
        for(i=0; i<64; i++)
        {
            rom->page[i] = &rom->data[(i & 0x3f) * 8192] - (i*8192);
        }
        for(i=64; i<128; i++)
        {
            rom->page[i] = &rom->data[((i & 0x1f) + 32) * 8192] - (i*8192);
        }
    }
    else
    {
        int i;
        for(i=0; i<128; i++)
        {
            uint8_t bank = i % (rom->len / 8192);
            rom->page[i] = &rom->data[bank * 8192] - (i*8192);
        }
    }

    return 0;

err_1:
    destroyROM(rom);
err_0:
    fclose(in);
    return 1;
}
/**
 * Destroy ROM object.
 * \param [in] rom ROM object.
 */
void destroyROM(ROM* rom)
{
    if(NULL == rom)
    {
        return;
    }
    if(NULL != rom->data)
    {
        free(rom->data);
    }
    memset(rom, 0, sizeof(ROM));
}
/**
 * Read a single byte from ROM.
 * \param [in] rom     ROM object.
 * \param [in] address Physical ROM address.
 * \return byte read.
 */
uint8_t readROM(ROM* rom, off_t address)
{
    int bank = address >> 13;
    if((bank < 0x100) && rom->page[bank])
    {
        return rom->page[bank][address];
    }
    return 0xff;
}
