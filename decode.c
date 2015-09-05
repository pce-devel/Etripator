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
#include "message.h"
#include "decode.h"

/*
 * Initialize section processor
 */
int initializeSectionProcessor(SectionProcessor* iProcessor)
{
    iProcessor->processed = NULL;
    iProcessor->rom       = NULL;
    iProcessor->out       = NULL;

    iProcessor->physicalAddr = 0;
    iProcessor->logicalAddr = 0;

    iProcessor-> offset = 0;

    iProcessor->buffer      = NULL;
    iProcessor->labelIndex  = 0;
    return initializeLabelRepository(&iProcessor->labelRepository);
}

/*
 * Reset section processor
 */
void resetSectionProcessor(ROM* iRom, FILE* iOut, Section* iProcessed, SectionProcessor* iProcessor)
{
    iProcessor->processed = iProcessed;
    iProcessor->rom       = iRom;
    iProcessor->out       = iOut;

    iProcessor->physicalAddr = (iProcessed->bank << 13) | (iProcessed->org & 0x1fff);
    iProcessor->logicalAddr  = iProcessed->org;

    iProcessor->offset = 0;

    iProcessor->labelIndex = 0;
    resetLabelRepository(&iProcessor->labelRepository);
}
  
/*
 * Delete section processor
 */
void deleteSectionProcessor(SectionProcessor* iProcessor)
{
    if(iProcessor->buffer != NULL)
    {
        free(iProcessor->buffer);
        iProcessor->buffer = NULL;
    }

    deleteLabelRepository(&iProcessor->labelRepository);
}

/*
 * Process data section
 */
int processDataSection(SectionProcessor* iProcessor)
{
    uint8_t data;
    int     i;
    off_t   addr;
    if(iProcessor->processed->type == INC_DATA)
    {
        int j;
        for(i=iProcessor->processed->size, addr=iProcessor->physicalAddr; i>0; )
        {
            fprintf(iProcessor->out, "  .db ");

            for(j=0; (i>0) && (j<8); j++, i--)
            {
                data = readROM(iProcessor->rom, addr++);
                fprintf(iProcessor->out, "$%02x%c", data, (i>1) ? ',' : '\n');
            }
        }
    }
    else
    {
        for(i=iProcessor->processed->size, addr=iProcessor->physicalAddr; i>0; i--)
        {
            data = readROM(iProcessor->rom, addr++);
            fwrite(&data, 1, 1, iProcessor->out);
        }
    }
    return 1;
}

/* Parse section to identify potential labels */
int getLabels(SectionProcessor* iProcessor)
{
    int eor, i;
    uint8_t inst;
    uint8_t data[6];
    
    size_t offset;
    uint16_t logical;

    LabelRepository *repository = &(iProcessor->labelRepository);
    Section *section = iProcessor->processed;

    if(CODE != section->type)
        return 1;

    offset  = 0;
    logical = iProcessor->logicalAddr;

    printf("\n%s:\n", section->name);

    /* Push the section org */
    if(pushLabel(repository, logical) == 0)
        return 0;

    /* Walk along section */
    eor = 0;
    while(!eor)
    {
        /* Read instruction */
        inst = readROM(iProcessor->rom, iProcessor->physicalAddr+offset);
        offset++;
        /* Read data (if any) */
        for(i=0; i<(pce_opcode[inst].size-1); i++, offset++)
        {
            data[i] = readROM(iProcessor->rom, iProcessor->physicalAddr+offset);
        }

        if(isLocalJump(inst))
        {
            int delta;
            /* For BBR* and BBS* displacement is stored in the 2nd byte */
            i = (((inst) & 0x0F) == 0x0F) ? 1 : 0;
            /* Detect negative number */
            if(data[i] & 128)
                delta = - ((data[i] - 1) ^ 0xff);
            else
                delta = data[i];
            delta += pce_opcode[inst].size;

            /* Insert offset to repository */
            if(pushLabel(repository, logical+delta) == 0)
                return 0;

            printf("%04x short jump to %04x\n", logical, logical+delta);
        }
        else 
        {
            if(isFarJump(inst))
            {
                uint16_t jump = data[0] | (data[1] << 8);
                /* Insert offset to repository */
                if(pushLabel(repository, jump) == 0)
                    return 0;

                printf("%04x long jump to %04x\n", logical, jump);
            }
        }

        logical += pce_opcode[inst].size;

        /* Search end of section */
        if(section->size == 0)
        {
            if((inst == 0x40) || (inst == 0x60))
            {
                section->size = offset;
                eor = 1;
            }
        }
        else if(offset >= section->size)
        {
            eor = 1;
        }       
    }

    finalizeLabelRepositoty(repository);

    return 1;
}

/* Initialize label index so that it points to the label close to current org offset */
void getLabelIndex(SectionProcessor* iProcessor)
{
    int i;
    
    /* Room for huge improvments */
    for(i=iProcessor->labelIndex; i<iProcessor->labelRepository.last; ++i)
    {
        if(iProcessor->labelRepository.labels[i].offset >= (iProcessor->logicalAddr + iProcessor->offset))
        {
            break;
        }
    }
    
    iProcessor->labelIndex = i;
}

/* Maximum number of characters per line */
#define MAX_CHAR_PER_LINE 80

/*
 * Process opcode
 */
char processOpcode(SectionProcessor* iProcessor) {
    int i, delta;
    uint8_t inst, data[6], isJump;
    char line[MAX_CHAR_PER_LINE], eor, *ptr;
    uint16_t offset;
    uint16_t logical, nextLogical;

    eor = 0;
    ptr = line;

    /* Opcode */
    inst = readROM(iProcessor->rom, iProcessor->physicalAddr + iProcessor->offset);

    /* Get label index */
    getLabelIndex(iProcessor);

    logical     = iProcessor->logicalAddr + iProcessor->offset;
    nextLogical = logical + pce_opcode[inst].size;
    
    /* Is there a label ? */
    if( (iProcessor->labelIndex < iProcessor->labelRepository.last) &&
        (iProcessor->labelRepository.labels[iProcessor->labelIndex].offset >= logical) &&
        (iProcessor->labelRepository.labels[iProcessor->labelIndex].offset < nextLogical) )
    {
        /* Print label*/
        sprintf(line, "l%04x_%02x: ", iProcessor->labelRepository.labels[iProcessor->labelIndex].offset,  iProcessor->processed->id);

        /* Add displacement */
        if(iProcessor->labelRepository.labels[iProcessor->labelIndex].offset != logical)
        {
            iProcessor->labelRepository.labels[iProcessor->labelIndex].displacement = iProcessor->labelRepository.labels[iProcessor->labelIndex].offset - logical;
        }
    }
    else
    {
        memset(line, ' ', 10 * sizeof(char));
    }

    ptr += 10;

    /* Print opcode sting */
    memcpy(ptr, pce_opcode[inst].name, 4 * sizeof(char));
    ptr += 4;

    /* Add spacing */  
    memset(ptr, ' ', 4 * sizeof(char));
    ptr += 4;

    /* End Of Routine (eor) is set to 1 if the instruction is RTI or RTS */
    if((inst == 64) || (inst == 96)) eor = 1;

    /* Data */
    if(pce_opcode[inst].size > 1)
    {
        for(i=0; i<(pce_opcode[inst].size-1); i++)
        {
            data[i] = readROM(iProcessor->rom, iProcessor->physicalAddr + iProcessor->offset + i + 1);
        }
    }

    iProcessor->offset += pce_opcode[inst].size;
    logical = nextLogical;

    /* Swap LSB and MSB for words */
    if(pce_opcode[inst].size > 2)
    {
        uint8_t swap;
        /* Special case : TST, BBR*, BBS*. The first byte is zp var */
        i = ((pce_opcode[inst].type ==  3) ||
             (pce_opcode[inst].type ==  4) ||
             (pce_opcode[inst].type ==  5) ||
             (pce_opcode[inst].type ==  6) ||
             (pce_opcode[inst].type == 13) ||
             (pce_opcode[inst].type == 20)) ? 1 : 0;

        for(; i<(pce_opcode[inst].size-2); i+=2)
        {
            swap      = data[i];
            data[i]   = data[i+1];
            data[i+1] = swap;
        }
    }

    /* Handle special cases (jumps, tam/tma and unsupported opcodes ) */
    isJump = 0;
    if(isLocalJump(inst)) {
        isJump = 1;

        i = (((inst) & 0x0F) == 0x0F) ? 1 : 0;

        /* Detect negative number */
        if(data[i] & 128)
            delta = - ((data[i] - 1) ^ 0xff);
        else
            delta = data[i];

        offset = logical + delta;
        data[i  ] = offset >> 8;
        data[i+1] = offset & 0xff;
    }
    else 
    {
        if(isFarJump(inst))
        {
            isJump = 1;
            /* Build offset */
            offset = (data[1] << 8) | data[0];
        }
        else 
        {
            offset = 0;
            
            /* Unknown instructions are output as raw data 
             * (ie) .db XX
             */
            if(pce_opcode[inst].type == 22)
            {
                data[0] = inst;
            }
        }
    }

    if(pce_opcode[inst].type == 1) {
        *(ptr++) = 'A';
    }
    else {
        if((inst == 0x43) || (inst == 0x53))
        {
            /* tam and tma */
            /* Compute log base 2 of data */
            for(i=0; (i<8) && ((data[0] & 1) == 0); ++i, data[0] >>= 1);
            data[0] = i;        
        }

        /* Add section to jump label */
        if(pce_opcode[inst].type == 20)
        {
            data[3] = iProcessor->processed->id & 0xff;
        }
        else if(pce_opcode[inst].type == 19)
        {
            data[2] = iProcessor->processed->id & 0xff;
        }

        /* Print data */
        if(pce_opcode[inst].type)
        {
            for(i=0; pce_opstring[pce_opcode[inst].type][i] != NULL; ++i)
            {
                sprintf(ptr, pce_opstring[pce_opcode[inst].type][i], data[i]);
                ptr += strlen(ptr);
            }
        }
    }

    /* Add displacement to jump offset ? */
    if(isJump)
    {
        /* Search in label database ( todo : dicotomic search ) */
        for(i=0; i < iProcessor->labelRepository.last; ++i)
        {
            if(iProcessor->labelRepository.labels[i].offset == offset)
            {
                /* Displacement may not exceed 7 (maximum opcode data size) */
                if(iProcessor->labelRepository.labels[i].displacement != 0)
                {
                    ptr[0] = '+';
                    ptr[1] = '0' + iProcessor->labelRepository.labels[i].displacement;
                    ptr += 2;
                }
                break;
            }
        }
    }

    *(ptr++) = '\n';
    *ptr = '\0';

    /* Output line */
    fwrite(line, sizeof(char), ptr-line, iProcessor->out);
    return eor;
}
