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
#include "irq.h"
#include "message.h"

#define PCE_IRQ_TABLE 0xFFF6U
#define PCE_IRQ_COUNT 5

static char* g_irq_names[PCE_IRQ_COUNT] = {
    "irq_2",
    "irq_1",
    "irq_timer",
    "irq_nmi",
    "irq_reset"
};

// [todo] add an extra section for the address table
// Get irq code offsets from rom.
bool irq_read(MemoryMap* map, Section **section, int *count) {
    assert(map != NULL);
    assert(section != NULL);
    assert(count != NULL);

    bool ret = false;
    if(map->memory[PCE_MEMORY_ROM].data == NULL) {
        ERROR_MSG("No ROM in memory map.");
    } else if(map->memory[PCE_MEMORY_ROM].length < (PCE_IRQ_TABLE + PCE_IRQ_COUNT)) {
        ERROR_MSG("ROM is abnormally small.");
    } else {
        Section *tmp = (Section*)realloc(*section, (*count+PCE_IRQ_COUNT) * sizeof(Section));
        if(tmp == NULL) {
            ERROR_MSG("Failed to allocate extra IRQ sections.");
        } else {
            int j = *count;
            *section = tmp;
            *count += PCE_IRQ_COUNT;

            for(size_t i=0; i<PCE_IRQ_COUNT; i++, j++) {
                // IRQ name.
                const char *name = g_irq_names[i];
                // Read offset.
                uint8_t lo = memmap_read(map, offset++);
                uint8_t hi = memmap_read(map, offset++);

// [todo] code_add....
                // Initialize section
                tmp[j].name     = strdup(name);
                tmp[j].type     = Code;
                tmp[j].page     = 0;
                tmp[j].logical  = (hi << 8) | lo;
                tmp[j].size     = 0;

                tmp[j].mpr[0] = 0xFFU;      // I/O
                tmp[j].mpr[1] = 0xF8U;      // RAM
                for(int k=2; k<PCE_MPR_COUNT; k++) {
                    tmp[j].mpr[k] = 0x00;   // ROM
                }

                filename_len = strlen(name) + 5U; // .asm\0
                tmp[j].output = (char*)malloc(filename_len);
                snprintf(tmp[j].output, filename_len, "%s.asm", name);

                tmp[j].description = NULL;

                INFO_MSG("%s found at %04x", tmp[j].name, tmp[j].logical);
            }
        }
    }

    return ret;
}
