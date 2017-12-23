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

static char* IRQNames[5] = {
    "irq_2",
    "irq_1",
    "irq_timer",
    "irq_nmi",
    "irq_reset"
};

/**
 * Get irq code offsets from rom.
 * \param [in]  memmcp  Memory map.
 * \param [out] section Sections.
 * \param [out] count Section count;
 * \return 0 on error, 1 otherwise.
 */
int getIRQSections(MemoryMap* memmap, Section **section, int *count)
{
    int i, j;
    uint8_t addr[2];
    uint16_t offset = 0xfff6;
    size_t  filenameLen;
    Section *tmp;
    
    j = *count;
    tmp = (Section*)realloc(*section, (j+5) * sizeof(Section));
    if(NULL == tmp) {
        ERROR_MSG("Failed to allocate extra IRQ sections.");
        return 0;
    }
    *section = tmp;
    *count += 5;
    
    for(i=0; i<5; ++i)
    {
        /* Read offset */
        addr[0] = readByte(memmap, offset++);
        addr[1] = readByte(memmap, offset++);
        
        /* Initialize section */
        tmp[j+i].name     = strdup(IRQNames[i]);
        tmp[j+i].type     = Code;
        tmp[j+i].bank     = 0;
        tmp[j+i].org      = (addr[1] << 8) | addr[0];
        tmp[j+i].offset   = 0;
        tmp[j+i].size     = 0;
        memset(tmp[j+i].mpr, 0, 8);
        tmp[j+i].mpr[0] = 0xff;
        tmp[j+i].mpr[1] = 0xf8;
        
	    filenameLen = strlen(IRQNames[i]) + 5;
        tmp[j+i].filename = (char*)malloc(filenameLen);
        snprintf(tmp[j+i].filename, filenameLen, "%s.asm", IRQNames[i]);

        INFO_MSG("%s found at %04x", tmp[j+i].name, tmp[j+i].org);
    }
    
    return 1;
}
