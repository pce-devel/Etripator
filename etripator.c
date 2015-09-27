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
#include "config.h"
#include "message.h"
#include "cfg.h"

#include "memorymap.h"
#include "section.h"
#include "decode.h"
#include "opcodes.h"
#include "irq.h"
#include "rom.h"
#include "cd.h"
#include "options.h"
#include "labelsloader.h"

/*
  exit callback
 */
void exit_callback(void)
{
    PrintMsgClose();
}

/* Main */
int main(int argc, char** argv)
{
    FILE* out;
    FILE* mainFile;

    unsigned int i;
    int err, failure;
    SECTION_ERR sectErr;

    CommandLineOptions cmdOptions;

    Section* section;
    size_t sectionCount;

    SectionProcessor processor;

    MemoryMap memmap;

    atexit(exit_callback);

    PrintMsgOpenFile(NULL);

    /* Extract command line options */
    err = getCommandLineOptions(argc, argv, &cmdOptions);
    if(err <= 0)
    {
        usage();
        return (err < 0);
    }

    failure = 1;

    /* Read cfg file */
    if(cmdOptions.extractIRQ)
    {
        sectionCount = 5;
        section = (Section*)malloc(sectionCount * sizeof(Section));
        if(section == NULL)
        {
            ERROR_MSG("Failed to allocate memory: %s", strerror(errno));
            goto error_1;
        }
    }
    else
    {
        sectionCount = 0;
        section      = NULL;
    }

    if(cmdOptions.cfgFileName)
    {
        sectErr = readSectionsFromCFG(cmdOptions.cfgFileName, &section, &sectionCount);
        if(sectErr != SECTION_OK)
        {
            ERROR_MSG("Unable to read %s", cmdOptions.cfgFileName);
            goto error_1;
        }
    }

    /* Initialize memory map. */
    err = initializeMemoryMap(&memmap);
    if(err)
    {
        goto error_1;
    }

    /* Read ROM */
    if(0 == cmdOptions.cdrom)
    {
        err = loadROM(cmdOptions.romFileName, &memmap);
        if(err)
        {
            goto error_2;
        }

        /* Get irq offsets */
        if(cmdOptions.extractIRQ)
        {
            if(getIRQSections(&memmap, section) == 0)
            {
                ERROR_MSG("An error occured while reading irq vector offsets");
                goto error_2;
            }
        }
    }
    else
    {
        err = addCDRAMMemoryMap(&memmap);
        if(err)
        {
            goto error_2;
        }
        /*  Data will be loaded during section disassembly */
    }

    /* Initialize section processor */
    initializeSectionProcessor(&processor);

    /* Load labels */
    if(NULL != cmdOptions.labelsFileName)
    {
        err = loadLabels(cmdOptions.labelsFileName, processor.labelRepository);
        if(err == 0)
        {
            ERROR_MSG("An error occured while loading labels from %s : %s", cmdOptions.labelsFileName, strerror(errno));
            goto error_4;
        }
    }

    /* For each section reset every existing files */
    for(i=0; i<sectionCount; ++i)
    {
        out = fopen(section[i].filename, "wb");
        if(out == NULL)
        {
            ERROR_MSG("Can't open %s : %s", section[i].filename, strerror(errno));
            goto error_4;
        }
        fclose(out);
    }

    /* Disassemble and output */
    for(i=0; i<sectionCount; ++i)
    {
        out = fopen(section[i].filename, "ab");
        if(out == NULL)
        {
            ERROR_MSG("Can't open %s : %s", section[i].filename, strerror(errno));
            goto error_4;
        }

        if(0 != cmdOptions.cdrom)
        {
            /* Copy CDROM data */
            err = loadCD(cmdOptions.romFileName, section[i].start, section[i].size, section[i].bank, section[i].org, &memmap);
            if(0 == err)
            {
                ERROR_MSG("Failed to load CD data (section %d)", i);
                goto error_4;
            }
        }

        if(section[i].type != BIN_DATA)
        {
            /* Print header */
            fprintf(out, "\t.%s\n"
                         "\t.bank %x\n"
                         "\t.org $%04x\n",
                         (section[i].type == CODE) ? "code" : "data", section[i].bank,
                          section[i].org);
        }

        /* Reset section processor */
        resetSectionProcessor(&memmap, out, &section[i], &processor);

        if(CODE == section[i].type)
        {
            char eor;
            
            /* Extract labels */
            if(!extractLabels(&processor))
            {
                goto error_4;
            }

            /* Process opcodes */
            do 
            {
                eor = processOpcode(&processor);

                if(!cmdOptions.extractIRQ)
                {
                    if(processor.offset >= processor.processed->size)
                        eor = 1;
                    else
                        eor = 0;
                }
            } while(!eor);
        }
        else
        {
            err = processDataSection(&processor);
        }

        fclose(out);
        out = NULL;
    }

    /* Open main asm file */
    mainFile = fopen(cmdOptions.mainFileName, "w");
    if(mainFile == NULL)
    {
        ERROR_MSG("Unable to open %s : %s", cmdOptions.mainFileName, strerror(errno));
        goto error_4;
    }
  
    if(cmdOptions.extractIRQ)
    {
        fprintf(mainFile, "\n\t.data\n\t.bank 0\n\t.org $FFF6\n");
        for(i=0; i<5; ++i)
        {
            fprintf(mainFile, "\t.dw $%04x\n", section[i].org);
        }
    }

    fclose(mainFile);
    failure = 0;

error_4:
    deleteSectionProcessor(&processor);
error_2:
    destroyMemoryMap(&memmap);
error_1:
    for(i=0; i<sectionCount; ++i)
    {
        free(section[i].name);
        free(section[i].filename);
        section[i].name = NULL;
    }
    free(section);

    return failure;
}
