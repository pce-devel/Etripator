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
#include "message.h"
#include "rom.h"

/**
 * Load ROM from file.
 * \param [in]  filename ROM filename.
 * \param [out] memmap   Memory map.
 * \return 1 upon success, 0 if an error occured.
 */
int load_rom(const char* filename, memmap_t* map) {
    FILE   *in;
    int i;
    size_t size;
    size_t count, nread;
    /* Open file */
    in = fopen(filename, "rb");
    if(!in) {
        ERROR_MSG("Unable to open %s : %s", filename, strerror(errno));
        return 0;
    }
    /* Compute file size. */
    fseek(in, 0, SEEK_END);
    size  = ftell(in);
    fseek(in, 0, SEEK_SET);
    size -= ftell(in);
    /* Check size */
    if(!size) {
        ERROR_MSG("Empty file: %s", filename);
        goto err_0;
    }
    /* Check for possible header */
    if(size & 0x200) {
        /* Jump header */
        size &= ~0x200;
        if(fseek(in, 0x200, SEEK_SET)) {
            ERROR_MSG("Failed to jump rom header in %s: %s", filename, strerror(errno));
            goto err_0;
        }
    }
    /* Allocate rom storage */
    if(!mem_create(&map->rom, (size + 0x1fff) & ~0x1fff)) {
        ERROR_MSG("Failed to allocate ROM storage : %s", strerror(errno));
        goto err_0;
    }
    /* Fill rom with 0xff */
    memset(map->rom.data, 0xff, map->rom.len);
    /* Read ROM data */
    count = (size < map->rom.len) ? size : map->rom.len;
    nread = fread(map->rom.data, 1, count, in);
    if(nread != count) {
        ERROR_MSG("Failed to read ROM data from %s : %s", filename, strerror(errno));
        goto err_1;
    }
    fclose(in);
    /* Initialize ROM pages (from mednafen source code). */
    /* Note : the decrement by (i*8192) is a trick to avoid doing a 
     *        bitwise with the address when reading a byte from that
     *        page. */
    if(map->rom.len == 0x60000) {
        for(i=0; i<64; i++) {
            map->page[i] = &map->rom.data[(i & 0x1f) * 8192] - (i*8192);
        }
        for(i=64; i<128; i++) {
            map->page[i] = &map->rom.data[((i & 0x0f) + 32) * 8192] - (i*8192);
        }
    }
    else if(map->rom.len == 0x80000) {
        for(i=0; i<64; i++) {
            map->page[i] = &map->rom.data[(i & 0x3f) * 8192] - (i*8192);
        }
        for(i=64; i<128; i++) {
            map->page[i] = &map->rom.data[((i & 0x1f) + 32) * 8192] - (i*8192);
        }
    }
    else {
        for(i=0; i<128; i++) {
            uint8_t bank = i % (memmap->rom.len / 8192);
            memmap->page[i] = &memmap->rom.data[bank * 8192] - (i*8192);
        }
    }
    return 1;
err_1:
    mem_destroy(&map->rom);
err_0:
    fclose(in);
    return 0;
}
