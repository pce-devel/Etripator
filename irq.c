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
 * \param [in]  rom     ROM.
 * \param [out] section IRQ vector sections.
 * \return 0 on error, 1 otherwise.
 */
int getIRQSections(ROM *rom, Section* iSectionArray)
{
    int i;
    uint8_t addr[2];
    off_t   offset = 0x1ff6;
    
    for(i=0; i<5; ++i)
    {
        /* Read offset */
        addr[0] = readROM(rom, offset++);
        addr[1] = readROM(rom, offset++);
        
        /* Initialize section */
        iSectionArray[i].name  = IRQNames[i];
        iSectionArray[i].type  = CODE;
        iSectionArray[i].bank  = 0;
        iSectionArray[i].org   = (addr[1] << 8) | addr[0];
        iSectionArray[i].type  = CODE;
        iSectionArray[i].start = ((addr[1] & 0x1f) << 8) | addr[0];
        iSectionArray[i].size  = 0;
        iSectionArray[i].id    = i;

        printf("%s found at %04x (%lx)\n", iSectionArray[i].name, iSectionArray[i].org, iSectionArray[i].start);
    }
    
    return 1;
}
