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
  copyright (c) 2009--2023 Vincent Cruz.
 
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
#include "cd.h"
#include "message.h"

/* Adds CD RAM to memory map. */
int cd_memmap(memmap_t *map) {
    int i, ret = 0;
    /* Allocate CD RAM */
    if(!mem_create(&map->mem[PCE_MEM_CD_RAM], 8 * 8192)) {
        ERROR_MSG("Failed to allocate cd memory!\n");
        memmap_destroy(map);
    } else {
        /* CD RAM is mapped to pages 0x80-0x88 (included). */
        for (i = 0; i <= 8; i++) {
            map->page[0x80 + i] = &map->mem[PCE_MEM_CD_RAM].data[i * 8192];
        }
        /* Allocate System Card RAM */
        if (!mem_create(&map->mem[PCE_MEM_SYSCARD_RAM], 24 * 8192)) {
            ERROR_MSG("Failed to allocate system card memory!\n");
            memmap_destroy(map);
        } else {
            /* System Card RAM is mapped to pages 0x68-0x86. */
            for (i = 0; i < 24; i++) {
                map->page[0x68 + i] = &map->mem[PCE_MEM_SYSCARD_RAM].data[i * 8192];
            }
            ret = 1;
        }
    }
    return ret;
}
/* Load CDROM data from file. */
int cd_load(const char* filename, size_t start, size_t len, size_t sector_size, uint8_t page, size_t offset, memmap_t* map) {
    int ret = 0;
    FILE *in = fopen(filename, "rb");
    if(in == NULL) {
        ERROR_MSG("Unable to open %s : %s", filename, strerror(errno));
    } else {
        size_t remaining = len;
        size_t physical = (offset & 0x1FFFU) | (page << 0x0D);
        for(ret=1; ret && remaining; ) {
            size_t count = 2048 - (start % 2048);
            if(count > remaining) {
                count = remaining;
            }

            size_t sector_id = start / 2048;
            size_t sector_offset = start % 2048;
            
            size_t file_offset = (sector_id * sector_size) + sector_offset;

            size_t current_page = physical >> 0x0D;
            size_t current_addr = physical & 0x1FFF;

            ret = 0;
            if(fseek(in, (long int)file_offset, SEEK_SET)) {
                ERROR_MSG("Offset out of bound : %s", strerror(errno));
            } else if(fread(map->page[current_page] + current_addr, 1, count, in) != count) {
                ERROR_MSG("Failed to read %d bytes : %s", count, strerror(errno));
            } else {
                ret = 1;
            }
            start += count;
            physical += count;
            remaining -= count;
        }
  
    }    
    if(in) {
        fclose(in);
    }
    return ret;
}
