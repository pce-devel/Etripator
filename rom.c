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
#include "rom.h"
#include "message.h"

static bool rom_load_data(const char *filename, MemoryMap *map) {
    bool ret = false;

    size_t size = 0;
    FILE *in = fopen(filename, "rb");
    if (in == NULL) {
        ERROR_MSG("Unable to open %s : %s", filename, strerror(errno));
    } else {
        // Compute file size.
        struct stat infos;
        int fd = fileno(in);
        if (fd < 0) {
            ERROR_MSG("Failed to retrieve file descriptior for %s : %s", filename, strerror(errno));
        } else if (fstat(fd, &infos) < 0) {
            ERROR_MSG("Failed to retrieve file informations of %s : %s", filename, strerror(errno));
        } else if (infos.st_size == 0) {
            ERROR_MSG("Empty file: %s", filename);
        } else {
            size = infos.st_size;
            // Check for possible header
            if (size & 0x200U) {
                // Jump header
                size &= ~0x200U;
                if (fseek(in, 0x200U, SEEK_SET)) {
                    ERROR_MSG("Failed to jump rom header in %s: %s", filename, strerror(errno));
                }
            }
        }
        if(size) {
            // Allocate rom storage
            Memory *memory = &map->memory[PCE_MEMORY_ROM];
            if (!memory_create(memory, (size + 0x1FFFU) & ~0x1FFFU)) {
                ERROR_MSG("Failed to allocate ROM storage : %s", strerror(errno));
            } else {
                // Fill rom with 0xFF
                (void)memory_fill(memory, 0xFFU);
                // Read ROM data
                size_t count = (size < memory->length) ? size : memory->length;
                size_t nread = fread(memory->data, 1, count, in);
                if (nread != count) {
                    ERROR_MSG("Failed to read ROM data from %s : %s", filename, strerror(errno));
                    memory_destroy(memory);
                } else {
                    ret = true;
                }
            }
        }
        fclose(in);
    }
    return ret;
}

// Load ROM from file and update memory map.
bool rom_load(const char* filename, MemoryMap* map) {
    FILE *in;
    bool ret = false;
    if(rom_load_data(filename, map)) {
        unsigned int i;
        /* Initialize ROM pages. */
        if (map->memory[PCE_MEMORY_ROM].length == 0x60000U) {
            for (i = 0; i < 64; i++) {
                map->page[i].id = PCE_MEMORY_ROM;
                map->page[i].bank = i & 0x1FU;
            }
            for (i = 64; i < 128; i++) {
                map->page[i].id = PCE_MEMORY_ROM;
                map->page[i].bank = (i & 0x0FU) + 32;
            }
        } else if (map->memory[PCE_MEMORY_ROM].length == 0x80000U) {
            for (i = 0; i < 64; i++) {
                map->page[i].id = PCE_MEMORY_ROM;
                map->page[i].bank = i & 0x3FU;
            }
            for (i = 64; i < 128; i++) {
                map->page[i].id = PCE_MEMORY_ROM;
                map->page[i].bank = (i & 0x1FU) + 32;
            }
        } else {
            for (i = 0; i < 128; i++) {
                map->page[i].id = PCE_MEMORY_ROM;
                map->page[i].bank = i % (map->memory[PCE_MEMORY_ROM].length / PCE_BANK_SIZE);
            }
        }
        ret = true;
    }
    return ret;
}
