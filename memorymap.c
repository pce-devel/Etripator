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
#include "memorymap.h"
#include "message.h"

/**
 * Initialize memory map.
 * \param memmap Memory map.
 * \return 1 upon success, 0 if an error occured.
 */
int initializeMemoryMap(MemoryMap *memmap) {
    int i, err;
    memset(memmap, 0, sizeof(MemoryMap));

    /* Allocate main (or work) RAM */
    err = createMemory(&memmap->ram.main, 8192);
    if (0 == err) {
        ERROR_MSG("Failed to allocate main memory!\n");
        return err;
    }
    /* Main RAM is mapped to pages 0xf8-0xfb (included). */
    /* Pages 0xf9 to 0xfb mirror page 0xf8. */
    for (i = 0xf8; i <= 0xfb; i++) {
        memmap->page[i] = &memmap->ram.main.data[(i - 0xf8) * 8192] - (i * 8192);
    }
    /* ROM and syscard RAM will be initialized later. */

    /* Clear mprs. */
    memset(memmap->mpr, 0, 8);
    memmap->mpr[0] = 0xff;
    memmap->mpr[1] = 0xf8;

    return err;
}

/**
 * Add CD RAM to memory map.
 * \param memmap Memory map.
 * \return 1 upon success, 0 if an error occured.
 */
int addCDRAMMemoryMap(MemoryMap *memmap) {
    int i, err;

    /* Allocate CD RAM */
    err = createMemory(&memmap->ram.cd, 8 * 8192);
    if (0 == err) {
        ERROR_MSG("Failed to allocate cd memory!\n");
        destroyMemoryMap(memmap);
        return err;
    }
    /* CD RAM is mapped to pages 0x80-0x88 (included). */
    for (i = 0x80; i <= 0x88; i++) {
        memmap->page[i] = &memmap->ram.cd.data[(i - 0x80) * 8192] - (i * 8192);
    }

    /* Allocate System Card RAM */
    err = createMemory(&memmap->ram.syscard, 24 * 8192);
    if (0 == err) {
        ERROR_MSG("Failed to allocate system card memory!\n");
        destroyMemoryMap(memmap);
        return err;
    }
    /* System Card RAM is mapped to pages 0x68-0x80. */
    for (i = 0x68; i < 0x80; i++) {
        memmap->page[i] = &memmap->ram.syscard.data[(i - 0x68) * 8192] - (i * 8192);
    }
    return err;
}

/**
 * Release resources used by the memory map.
 * \param memmap Memory map.
 */
void destroyMemoryMap(MemoryMap *memmap) {
    destroyMemory(&memmap->rom);
    destroyMemory(&memmap->ram.main);
    destroyMemory(&memmap->ram.cd);
    destroyMemory(&memmap->ram.syscard);
    memset(memmap, 0, sizeof(MemoryMap));
}

/**
 * Memory page associated to the logical address.
 */
uint8_t getPage(MemoryMap *memmap, size_t address) {
    uint8_t id = (address >> 13) & 0x07;
    return memmap->mpr[id];
}

/**
 * Read a single byte from memory.
 * \param [in] memmap  Memory map.
 * \param [in] address Logical address.
 * \return byte read.
 */
uint8_t readByte(MemoryMap *memmap, size_t address) {
    uint8_t i = getPage(memmap, address);
    uint32_t physical = (i << 13) | (address & 0x1fff);
    if (memmap->page[i]) {
        return memmap->page[i][physical];
    }
    return 0xff;
}

/**
 * Update mprs
 * \param [in][out] memmap Memory mao.
 * \param [in]      mpr    Memory page registers.
 */
void updateMPRs(MemoryMap *memmap, uint8_t *mpr) {
    memcpy(memmap->mpr, mpr, 8);
}

