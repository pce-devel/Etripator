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

// Get irq code offsets from rom.
bool irq_read(MemoryMap* map, SectionArray *out) {
    assert(map != NULL);
    assert(out != NULL);

    bool ret = false;
    if(map->memory[PCE_MEMORY_ROM].data == NULL) {
        ERROR_MSG("No ROM in memory map.");
    } else if(map->memory[PCE_MEMORY_ROM].length < (PCE_IRQ_TABLE + PCE_IRQ_COUNT)) {
        ERROR_MSG("ROM is abnormally small.");
    } else {
        uint16_t offset = PCE_IRQ_TABLE;
        map->mpr[7] = 0;

        ret = true;

        for(size_t i=0; ret && (i<PCE_IRQ_COUNT); i++) {
            // IRQ name.
            const char *name = g_irq_names[i];
            // Read offset.
            uint8_t lo = memory_map_read(map, offset++);
            uint8_t hi = memory_map_read(map, offset++);

            // Initialize section
            Section tmp = {
                .name     = strdup(name),
                .type     = SECTION_TYPE_CODE,
                .page     = 0,
                .logical  = (hi << 8) | lo,
                .size     = 0,
                .mpr      = {
                    [0] = 0xFFU, // I/O
                    [1] = 0xF8U, // RAM
                    // [2..7] : 0 // ROM                    
                },
                .description = NULL,
            };

            size_t filename_len = strlen(name) + 5U; // .asm\0
            tmp.output = (char*)malloc(filename_len);
            snprintf(tmp.output, filename_len, "%s.asm", name);

            INFO_MSG("%s found at %04x", tmp.name, tmp.logical);

            if(section_array_add(out, &tmp) < 0) {
                ERROR_MSG("failed to add section");
                ret = false;
            }
        }

        const Section irq_table = {
            .name = strdup("irq_table"),
            .page = 0x00,
            .type = SECTION_TYPE_DATA,
            .logical = PCE_IRQ_TABLE,
            .size = PCE_IRQ_COUNT * 2U,
            .mpr = { [0] = 0xFU, [1] = 0xF8 },
            .data = { 
                .type = DATA_TYPE_JUMP_TABLE,
                .elements_per_line = 1U
            },
            .output = strdup("main.asm")
        };

        if(section_array_add(out, &irq_table) < 0) {
            ERROR_MSG("failed to add IRQ table section");
            ret = false;
        }
    }
    return ret;
}
