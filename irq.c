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
 * \param [out] section IRQ vector sections.
 * \return 0 on error, 1 otherwise.
 */
int getIRQSections(MemoryMap* memmap, Section *section)
{
    int i;
    uint8_t addr[2];
    uint16_t offset = 0xfff6;
    size_t  filenameLen;
    for(i=0; i<5; ++i)
    {
        /* Read offset */
        addr[0] = readByte(memmap, offset++);
        addr[1] = readByte(memmap, offset++);
        
        /* Initialize section */
        section[i].name     = strdup(IRQNames[i]);
        section[i].type     = Code;
        section[i].bank     = 0;
        section[i].org      = (addr[1] << 8) | addr[0];
        section[i].type     = Code;
        section[i].offset   = 0;
        section[i].size     = 0;
        memset(section[i].mpr, 0, 8);
        section[i].mpr[0] = 0xff;
        section[i].mpr[1] = 0xf8;
        
	filenameLen = strlen(IRQNames[i]) + 5;
        section[i].filename = (char*)malloc(filenameLen);
        snprintf(section[i].filename, filenameLen, "%s.asm", IRQNames[i]);

        INFO_MSG("%s found at %04x", section[i].name, section[i].org);
    }
    
    return 1;
}
