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
#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include "memory.h"

/**
 * PC Engine memory map.
 */
typedef struct {
    Memory rom;
    struct {
        Memory main;
        Memory cd;
        Memory syscard;
    } ram;
    uint8_t *page[0x100];
    uint8_t mpr[8];
} MemoryMap;

/**
 * Initialize memory map.
 * \param memmap Memory map.
 * \return 1 upon success, 0 if an error occured.
 */
int initializeMemoryMap(MemoryMap *memmap);

/**
 * Add CD RAM to memory map.
 * \param memmap Memory map.
 * \return 1 upon success, 0 if an error occured.
 */
int addCDRAMMemoryMap(MemoryMap *memmap);

/**
 * Release resources used by the memory map.
 * \param memmap Memory map.
 */
void destroyMemoryMap(MemoryMap *memmap);

/**
 * Memory page associated to the logical address.
 */
uint8_t getPage(MemoryMap* memmap, size_t address);

/**
 * Read a single byte from memory.
 * \param [in] memmap  Memory map.
 * \param [in] address Logical address.
 * \return byte read.
 */
uint8_t readByte(MemoryMap *memmap, size_t address);

/**
 * Update mprs
 * \param [in][out] memmap Memory mao.
 * \param [in]      mpr    Memory page registers.
 */
void updateMPRs(MemoryMap *memmap, uint8_t *mpr);

#endif // MEMORY_MAP_H
