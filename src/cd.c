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
#include <etripator/cd.h>
#include <etripator/utils.h>
#include <etripator/message.h>

/// Adds CD RAM to memory map.
bool cd_memory_map(MemoryMap *map) {
    bool ret = false;
    // Allocate CD RAM 
    if(!memory_create(&map->memory[PCE_MEMORY_CD_RAM], PCE_CD_RAM_BANK_COUNT * PCE_BANK_SIZE)) {
        ERROR_MSG("Failed to allocate cd memory!");
    // Allocate System Card RAM
    } else if (!memory_create(&map->memory[PCE_MEMORY_SYSCARD_RAM], PCE_SYSCARD_RAM_BANK_COUNT * PCE_BANK_SIZE)) {
        ERROR_MSG("Failed to allocate system card memory!");
    } else {
        size_t i;
        // CD RAM is mapped to pages 0x80-0x88 (included).
        for (i = 0; i <= PCE_CD_RAM_BANK_COUNT; i++) {
            map->page[PCE_CD_RAM_FIRST_PAGE + i].id = PCE_MEMORY_CD_RAM;
            map->page[PCE_CD_RAM_FIRST_PAGE + i].bank = i;
        }
        // System Card RAM is mapped to pages 0x68-0x86.
        for (i = 0; i < PCE_SYSCARD_RAM_BANK_COUNT; i++) {
            map->page[PCE_SYSCARD_RAM_FIRST_PAGE + i].id = PCE_MEMORY_SYSCARD_RAM;
            map->page[PCE_SYSCARD_RAM_FIRST_PAGE + i].bank = i;
        }
        ret = true;
    }

    if(ret == false) {
        memory_map_destroy(map);
    }
    return ret;
}

/// Load CDROM data from file.
bool cd_load(MemoryMap* map, Data *in, size_t start, size_t len, size_t sector_size, uint8_t page, size_t offset) {
    SANITY_CHECK((map != NULL) && (in != NULL), false);
    SANITY_CHECK(len != 0, false);

    if (data_open(in) != true) {
        ERROR_MSG("Failed to CDROM track");
        return false;
    }
    
    bool ret;
    size_t remaining = len;
    size_t physical = (offset & 0x1FFFU) | (page << 0x0D);
    for(ret=true; ret && remaining; ) {
        size_t count = 2048 - (start % 2048);
        if(count > remaining) {
            count = remaining;
        }

        size_t sector_id = start / 2048;
        size_t sector_offset = start % 2048;

        size_t file_offset = (sector_id * sector_size) + sector_offset;

        size_t current_page = physical >> 0x0D;
        size_t current_addr = physical & 0x1FFF;

        size_t bank_offset = current_addr + (map->page[current_page].bank * PCE_BANK_SIZE);

        // [todo] test that map->page[current_page].id != PCE_MEMORY_NONE

        ret = false;
        size_t nread = 0;
        if(data_jump(in, file_offset) != true) {
            ERROR_MSG("Offset out of bound");
        } else if(data_read(in, map->memory[map->page[current_page].id].data+bank_offset, count, &nread) != true) {
            ERROR_MSG("Failed to read %zu bytes", count);
        } else {
            ret = true;
        }
        start += nread;
        physical += nread;
        remaining -= nread;
    }

    data_close(in);
    return ret;
}
