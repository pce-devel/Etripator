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
#include "cd.h"
#include "message.h"

/**
 * Adds CD RAM to memory map.
 * \param map Memory map.
 * \return 1 upon success, 0 if an error occured.
 */
int cd_memmap(memmap_t *map) {
    int i, ret;
    /* Allocate CD RAM */
    ret = mem_create(&map->mem[PCE_MEM_CD_RAM], 8 * 8192);
    if(!ret) {
        ERROR_MSG("Failed to allocate cd memory!\n");
        memmap_destroy(map);
        return ret;
    } 
    /* CD RAM is mapped to pages 0x80-0x88 (included). */
    for (i = 0; i <= 8; i++) {
        map->page[0x80 + i] = &map->mem[PCE_MEM_CD_RAM].data[i * 8192];
    }
    /* Allocate System Card RAM */
    ret = mem_create(&map->mem[PCE_MEM_SYSCARD_RAM], 24 * 8192);
    if (!ret) {
        ERROR_MSG("Failed to allocate system card memory!\n");
        memmap_destroy(map);
        return ret;
    }
    /* System Card RAM is mapped to pages 0x68-0x80. */
    for (i = 0; i < 24; i++) {
        map->page[0x68 + i] = &map->mem[PCE_MEM_SYSCARD_RAM].data[i * 8192];
    }
    return ret;
}
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
int cd_load(const char* filename, size_t start, size_t len, uint8_t page, size_t offset, memmap_t* map) {
    FILE *in;
    int ret;

    size_t addr;
    size_t nRead;

    in = fopen(filename, "rb");
    if(in == NULL) {
        ERROR_MSG("Unable to open %s : %s", filename, strerror(errno));
        return 0;
    }

    ret = fseek(in, start, SEEK_SET);
    if(ret) {
        ERROR_MSG("Offset out of bound : %s", strerror(errno));
        fclose(in);
        return 0;
    }

    addr = offset & 0x1fff;
    nRead = fread(map->page[page] + addr, 1, len, in);    
    if(nRead != len) {
        ERROR_MSG("Failed to read %d bytes : %s", len, strerror(errno));
        fclose(in);
        return 0;
    }
    
    fclose(in);
    return 1;
}
