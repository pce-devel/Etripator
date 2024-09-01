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
#include "memorymap.h"
#include "message.h"
/* Initializes memory map.  */
int memmap_init(memmap_t *map) {
    int i, ret = 0;
    
    memset(map, 0, sizeof(memmap_t)); // [todo] function


    /* Allocate main (or work) RAM */
    if(!memory_create(&map->mem[PCE_MEM_BASE_RAM], 8192)) {
        ERROR_MSG("Failed to allocate main memory!\n");
    } else {
        /* Main RAM is mapped to pages 0xf8-0xfb (included). */
        /* Pages 0xf9 to 0xfb mirror page 0xf8. */
        for(i=0xf8; i<=0xfb; i++) {
            map->page[i] = &(map->mem[PCE_MEM_BASE_RAM].data[0]);
        }
        
        /* ROM and syscard RAM will be initialized later. */

        /* Clear mprs. */
        memset(map->mpr, 0, 8);
        map->mpr[0] = 0xff;
        map->mpr[1] = 0xf8;
    
        ret = 1;
    }
    return ret;
}
/* Releases resources used by the memory map. */
void memmap_destroy(memmap_t *map) {
    int i;
    for(i=0; i<PCE_MEM_COUNT; i++) {
        memory_destroy(&map->mem[i]);
    }
    memset(map, 0, sizeof(memmap_t));
}
/* Get the memory page associated to a logical address. */
uint8_t memmap_page(memmap_t* map, uint16_t logical) {
    uint8_t id = (logical >> 13) & 0x07;
    return map->mpr[id];
}
/* Reads a single byte from memory. */
uint8_t memmap_read(memmap_t *map, size_t logical) {
    uint8_t i = memmap_page(map, (uint16_t)logical);
    return (map->page[i]) ? map->page[i][logical & 0x1fff] : 0xff;
}
/* Update mprs. */
void memmap_mpr(memmap_t *map, const uint8_t *mpr) {
    memcpy(map->mpr, mpr, 8);
}

