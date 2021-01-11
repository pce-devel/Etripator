/*
    This file is part of Etripator,
    copyright (c) 2009--2021 Vincent Cruz.

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
#ifndef ETRIPATOR_MEMORY_MAP_H
#define ETRIPATOR_MEMORY_MAP_H

#include "memory.h"

/**
 * PC Engine memory 
 */
enum {
    PCE_MEM_ROM = 0,
    PCE_MEM_BASE_RAM,
    PCE_MEM_CD_RAM,
    PCE_MEM_SYSCARD_RAM,
    PCE_MEM_COUNT
};

/**
 * PC Engine memory map.
 */
typedef struct {
    mem_t mem[PCE_MEM_COUNT];
    uint8_t *page[0x100];
    uint8_t mpr[8];
} memmap_t;

/**
 * Initializes memory map.
 * \param map Memory map.
 * \return 1 upon success, 0 if an error occured.
 */
int memmap_init(memmap_t *map);
/**
 * Releases resources used by the memory map.
 * \param map Memory map.
 */
void memmap_destroy(memmap_t *map);
/**
 * Get the memory page associated to a logical address.
 * \param map Memory map.
 * \param logical Logical address.
 * \return Memory page.
 */
uint8_t memmap_page(memmap_t* map, uint16_t logical);
/**
 * Reads a single byte from memory.
 * \param [in] map     Memory map.
 * \param [in] logical Logical address.
 * \return Byte read.
 */
uint8_t memmap_read(memmap_t *map, size_t logical);
/**
 * Update mprs.
 * \param [in][out] map Memory map.
 * \param [in]      mpr Memory page registers.
 */
void memmap_mpr(memmap_t *map, const uint8_t *mpr);

#endif // ETRIPATOR_MEMORY_MAP_H
