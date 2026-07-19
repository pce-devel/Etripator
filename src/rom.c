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
  copyright (c) 2009--2026 Vincent Cruz.
 
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
#include <etripator/rom.h>

#include <etripator/message.h>
#include <etripator/utils.h>

bool rom_create(MemoryMap* map, size_t size) {
    // Allocate rom storage
    Memory *memory = &map->memory[PCE_MEMORY_ROM];
    if (!memory_create(memory, (size + 0x1FFFU) & ~0x1FFFU)) {
        ERROR_MSG("Failed to allocate ROM storage : %s", strerror(errno));
        return false;
    }
    // Fill rom with 0xFF
    (void)memory_fill(memory, 0xFFU);        

    /* Initialize ROM pages. */
    if (map->memory[PCE_MEMORY_ROM].length == 0x60000U) {
        for (unsigned int i = 0; i < 64; i++) {
            map->page[i].id = PCE_MEMORY_ROM;
            map->page[i].bank = i & 0x1FU;
        }
        for (unsigned int i = 64; i < 128; i++) {
            map->page[i].id = PCE_MEMORY_ROM;
            map->page[i].bank = (i & 0x0FU) + 32;
        }
    } else if (map->memory[PCE_MEMORY_ROM].length == 0x80000U) {
        for (unsigned int i = 0; i < 64; i++) {
            map->page[i].id = PCE_MEMORY_ROM;
            map->page[i].bank = i & 0x3FU;
        }
        for (unsigned int i = 64; i < 128; i++) {
            map->page[i].id = PCE_MEMORY_ROM;
            map->page[i].bank = (i & 0x1FU) + 32;
        }
    } else {
        for (unsigned int i = 0; i < 128; i++) {
            map->page[i].id = PCE_MEMORY_ROM;
            map->page[i].bank = i % (map->memory[PCE_MEMORY_ROM].length / PCE_BANK_SIZE);
        }
    }
    return true;
}

static size_t rom_size_adjust(Data *input) {
    size_t size = data_size(input);
    if(size == 0) {
        ERROR_MSG("empty rom");
        return 0;
    }
    // Check for possible header
    if (size & 0x200U) {
        // Jump header
        size &= ~0x200U;
        if (data_jump(input, 0x200U) != true) {
            ERROR_MSG("Failed to jump ROM header");
            return 0;
        }
    }
    return size;
}

static bool rom_memory_read(Memory *memory, Data *input, size_t size) {
    size_t count = (size < memory->length) ? size : memory->length;
    size_t nread;
    bool ret = false;
    if(!data_read(input, memory->data, count, &nread)) {
        ERROR_MSG("Failed to read ROM data");
    } else if (nread != count) {
        ERROR_MSG("Failed to read ROM data (expected %zu, read %zu)", count, nread);
    } else {
        ret = true;
    }
    return ret;
} 

// Load ROM from file and update memory map.
bool rom_load(MemoryMap* map, Data *input) {
    SANITY_CHECK(map != NULL, false);
    SANITY_CHECK(input != NULL, false);
    
    if (data_open(input) != true) {
        ERROR_MSG("Failed to open rom data");
        return false;
    }

    bool ret = false;
    size_t size = rom_size_adjust(input);
    if(size) {
        // Allocate rom storage & read rom
        Memory *memory = &map->memory[PCE_MEMORY_ROM];
        if (!rom_create(map, (size + 0x1FFFU) & ~0x1FFFU)) {
            ERROR_MSG("Failed to allocate ROM storage : %s", strerror(errno));
        } else if(!rom_memory_read(memory, input, size)) {
            memory_destroy(memory);
        } else {
            ret = true;
        }
    }
    data_close(input);
    return ret;
}
