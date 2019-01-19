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
#include "irq.h"
#include "message.h"

#define PCE_IRQ_TABLE 0xfff6
#define PCE_IRQ_COUNT 5

static char* g_irq_names[PCE_IRQ_COUNT] = {
    "irq_2",
    "irq_1",
    "irq_timer",
    "irq_nmi",
    "irq_reset"
};

/**
 * Get irq code offsets from rom.
 * \param [in]  map Memory map.
 * \param [out] section Sections.
 * \param [out] count Section count;
 * \return 0 on error, 1 otherwise.
 */
int irq_read(memmap_t* map, section_t **section, int *count) {
    int i;
    uint8_t addr[2];
    size_t  filename_len;
    
    uint16_t offset = PCE_IRQ_TABLE;
    int j = *count;
    section_t *tmp = (section_t*)realloc(*section, (j+PCE_IRQ_COUNT) * sizeof(section_t));
    if(NULL == tmp) {
        ERROR_MSG("Failed to allocate extra IRQ sections.");
        return 0;
    }
    *section = tmp;
    *count += PCE_IRQ_COUNT;
    
    for(i=0; i<PCE_IRQ_COUNT; ++i) {
        /* Read offset */
        addr[0] = memmap_read(map, offset++);
        addr[1] = memmap_read(map, offset++);
        
        /* Initialize section */
        tmp[j+i].name     = strdup(g_irq_names[i]);
        tmp[j+i].type     = Code;
        tmp[j+i].page     = 0;
        tmp[j+i].logical  = (addr[1] << 8) | addr[0];
        tmp[j+i].offset   = 0;
        tmp[j+i].size     = 0;
        memset(tmp[j+i].mpr, 0, 8);
        tmp[j+i].mpr[0] = 0xff;
        tmp[j+i].mpr[1] = 0xf8;
        
	    filename_len = strlen(g_irq_names[i]) + 5;
        tmp[j+i].output = (char*)malloc(filename_len);
        snprintf(tmp[j+i].output, filename_len, "%s.asm", g_irq_names[i]);

        INFO_MSG("%s found at %04x", tmp[j+i].name, tmp[j+i].logical);
    }
    
    return 1;
}
