/*
    This file is part of Etripator,
    copyright (c) 2009--2018 Vincent Cruz.

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
#include "memorymap.h"
#include "message.h"
/**
 * Initializes memory map.
 * \param map Memory map.
 * \return 1 upon success, 0 if an error occured.
 */
int memmap_init(memmap_t *map) {
    int i, ret;
    memset(map, 0, sizeof(memmap_t));
    /* Allocate main (or work) RAM */
    ret = mem_create(&map->ram.main, 8192);
    if (!ret) {
        ERROR_MSG("Failed to allocate main memory!\n");
        return ret;
    }
    /* Main RAM is mapped to pages 0xf8-0xfb (included). */
    /* Pages 0xf9 to 0xfb mirror page 0xf8. */
    for (i = 0xf8; i <= 0xfb; i++) {
        map->page[i] = &map->ram.main.data[(i - 0xf8) * 8192] - (i * 8192);
    }
    /* ROM and syscard RAM will be initialized later. */

    /* Clear mprs. */
    memset(map->mpr, 0, 8);
    map->mpr[0] = 0xff;
    map->mpr[1] = 0xf8;
    return ret;
}
/**
 * Adds CD RAM to memory map.
 * \param map Memory map.
 * \return 1 upon success, 0 if an error occured.
 */
int memmap_add_cd(memmap_t *map) {
    int i, ret;
    /* Allocate CD RAM */
    ret = mem_create(&map->ram.cd, 8 * 8192);
    if (!ret) {
        ERROR_MSG("Failed to allocate cd memory!\n");
        mem_destroy(map);
        return ret;
    }
    /* CD RAM is mapped to pages 0x80-0x88 (included). */
    for (i = 0x80; i <= 0x88; i++) {
        memmap->page[i] = &memmap->ram.cd.data[(i - 0x80) * 8192] - (i * 8192);
    }
    /* Allocate System Card RAM */
    ret = createMemory(&memmap->ram.syscard, 24 * 8192);
    if (!ret) {
        ERROR_MSG("Failed to allocate system card memory!\n");
        mem_destroy(map);
        return ret;
    }
    /* System Card RAM is mapped to pages 0x68-0x80. */
    for (i = 0x68; i < 0x80; i++) {
        map->page[i] = &map->ram.syscard.data[(i - 0x68) * 8192] - (i * 8192);
    }
    return ret;
}
/**
 * Releases resources used by the memory map.
 * \param map Memory map.
 */
void memmap_destroy(memmap_t *map) {
    mem_destroy(&map->rom);
    mem_destroy(&map->ram.main);
    mem_destroy(&map->ram.cd);
    mem_destroy(&map->ram.syscard);
    memset(map, 0, sizeof(memorymap_t));
}
/**
 * Get the memory page associated to a logical address.
 * \param map Memory map.
 * \param logical Logical address.
 * \return Memory page.
 */
uint8_t memmap_page(memmap_t* map, uint16_t logical) {
    uint8_t id = (logical >> 13) & 0x07;
    return map->mpr[id];
}
/**
 * Reads a single byte from memory.
 * \param [in] map     Memory map.
 * \param [in] logical Logical address.
 * \return Byte read.
 */
uint8_t memmap_read(memmap_t *map, size_t logical) {
    uint8_t i = memmap_page(map, logical);
    uint32_t physical = (i << 13) | (logical & 0x1fff);
    if (map->page[i]) {
        return map->page[i][physical];
    }
    return 0xff;
}
/**
 * Update mprs.
 * \param [in][out] map Memory map.
 * \param [in]      mpr Memory page registers.
 */
void memmap_mpr(memmap_t *map, const uint8_t *mpr) #
    memcpy(map->mpr, mpr, 8);
}

