/*
¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

  __/¯\____ ___/\__   _/\__   _/\_   _/\__   _/\___ ___/\__   __/\_   _/\__   
  \_  ____/_> ____ \_/  _  \_ \  <  /_    \_/     _>> ____ \_ >    \_/  _  \_ 
  _> ___/ ¯>__> <<__// __  _/ |>  ></ _/>  </  ¯  \\__> <<__//  /\  // __  _/ 
 _>  \7   <__/:. \__/:. \>  \_/   L/  _____/.  7> .\_/:. \__/  <_/ </:. \>  \_ 
|:::::::::::::::::::::::/:::::::::::::>::::::::/::::::::::::::::::::::::/:::::|
|¯¯\::::/\:/¯\::::/¯¯¯¯<::::/\::/¯¯\:/¯¯¯¯¯¯\::\::/¯¯\::::/¯¯\::::/¯¯¯¯<::::/¯|
|__ |¯¯|  T _ |¯¯¯| ___ |¯¯|  |¯| _ T ______ |¯¯¯¯| _ |¯¯¯| _ |¯¯¯| ___ |¯¯| _|
   \|__|/\|/ \|___|/   \|__|/\|_|/ \|/      \|    |/ \|___|/ \|___|/dNo\|__|/  

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

  This file is part of Etripator,
  copyright (c) 2009--2024 Vincent Cruz.
 
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

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯
*/
#include "memory_map.h"
#include "message.h"

// Resets memory map.
static void memory_map_clear(MemoryMap *map) {
    for(unsigned int i=0; i<PCE_MEMORY_COUNT; i++) {
        map->memory[i].length = 0;
        map->memory[i].data = NULL;
    }
    for(unsigned int i=0; i<PCE_MPR_COUNT; i++) {
        map->mpr[i] = 0xFFU;
    }
    for(unsigned int i=0; i<PCE_PAGE_COUNT; i++) {
        map->page[i].id = PCE_MEMORY_NONE;
        map->page[i].bank = 0;
    }  
}

// Initializes memory map.
bool memory_map_init(MemoryMap *map) {
    bool ret = false;

    memory_map_clear(map);

    // Allocate main (or work) RAM.
    if(!memory_create(&map->memory[PCE_MEMORY_BASE_RAM], PCE_BANK_SIZE)) {
        ERROR_MSG("Failed to allocate main memory!");
    } else {
        // Main RAM is mapped to pages 0xF8 to 0xFB (included).
        // Pages 0xF9 to 0xFB mirror page 0xF8. */
        for(unsigned int i=0xf8; i<=0xfb; i++) {
            map->page[i].id = PCE_MEMORY_BASE_RAM;
            map->page[i].bank = 0;
        }
        
        // ROM and syscard RAM will be initialized later.
        ret = true;
    }
    return ret;
}

// Releases resources used by the memory map.
void memory_map_destroy(MemoryMap *map) {
    assert(map != NULL);
    for(unsigned i=0; i<PCE_MEMORY_COUNT; i++) {
        memory_destroy(&map->memory[i]);
    }
    memory_map_clear(map);
}

// Get the memory page associated to a logical address.
uint8_t memory_map_page(MemoryMap* map, uint16_t logical) {
    assert(map != NULL);
    uint8_t id = (logical >> 13) & 0x07U;
    return map->mpr[id];
}

// Reads a single byte from memory. 
uint8_t memory_map_read(MemoryMap *map, size_t logical) {
    assert(map != NULL);
    const uint8_t id = memory_map_page(map, (uint16_t)logical);
    const Page *page = &map->page[id];
    
    uint8_t ret = 0xFFU;
    if(page->id != PCE_MEMORY_NONE) {
        const size_t offset = (logical & 0x1FFFU) + (page->bank * PCE_BANK_SIZE);
        const Memory *mem = &map->memory[page->id];
        if(offset < mem->length) {
            ret = mem->data[offset];
        }
    }
    return ret;
}

// Update mprs.
void memory_map_mpr(MemoryMap *map, const uint8_t mpr[PCE_MPR_COUNT]) {
    assert(map != NULL);
    for(unsigned int i=0; i<PCE_MPR_COUNT; i++) {
        map->mpr[i] = mpr[i];
    }
}
