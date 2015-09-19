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

static char* IRQNames[5] = {
    "irq_2.asm",
    "irq_1.asm",
    "irq_timer.asm",
    "irq_nmi.asm",
    "irq_reset.asm"
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
    off_t   offset = 0x1ff6;
    
    for(i=0; i<5; ++i)
    {
        /* Read offset */
        addr[0] = readByte(memmap, offset++);
        addr[1] = readByte(memmap, offset++);
        
        /* Initialize section */
        section[i].name  = IRQNames[i];
        section[i].type  = CODE;
        section[i].bank  = 0;
        section[i].org   = (addr[1] << 8) | addr[0];
        section[i].type  = CODE;
        section[i].start = ((addr[1] & 0x1f) << 8) | addr[0];
        section[i].size  = 0;
        section[i].id    = i;

        printf("%s found at %04x (%lx)\n", section[i].name, section[i].org, section[i].start);
    }
    
    return 1;
}
