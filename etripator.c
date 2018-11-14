/*
    This file is part of Etripator,
    copyright (c) 2009--2018 Vincent Cruz.

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
#include <jansson.h>
#include <time.h>

#include "message.h"
#include "message/console.h"
#include "message/file.h"

#include "cd.h"
#include "decode.h"
#include "irq.h"
#include "labelsloader.h"
#include "labelswriter.h"
#include "memorymap.h"
#include "opcodes.h"
#include "options.h"
#include "rom.h"
#include "ipl.h"
#include "section.h"
#include "sectionsloader.h"

/*
  exit callback
 */
void exit_callback(void) { DestroyMsgPrinters(); }

/*
  output labels
*/
int outputLabels(CommandLineOptions *options, LabelRepository *repository) {
    char buffer[256];

    if(NULL == options->labelsOut) { 
        /* We don't want to destroy the original label file. */
        char *tmp;

        struct timeval tv;
        struct tm now;
        char dateString[128];
            
        gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &now);
        strftime(dateString, 128, "%Y%m%d%H%M%S", &now);
        
        tmp = basename(options->romFileName);
        snprintf(buffer, 256, "%s.%s.lbl", tmp, dateString);     
        options->labelsOut = buffer;
    }
    if (!writeLabels(options->labelsOut, repository)) {
        ERROR_MSG("Failed to write/update label file: %s", options->labelsOut);
        return 0;
    }
    return 1;
}

// [todo] fix
FileMsgPrinter filePrinter;
ConsoleMsgPrinter consolePrinter;

/* Main */
int main(int argc, char **argv) {
    FILE *out;
    FILE *mainFile;

    int i;
    int ret, failure;

    CommandLineOptions cmdOptions;

    Section *section;
    int sectionCount;

    Decoder decoder;

    MemoryMap memmap;

    atexit(exit_callback);

    SetupMsgPrinters();

    SetupFileMsgPrinter(&filePrinter);
    SetupConsoleMsgPrinter(&consolePrinter);

    if (AddMsgPrinter(&filePrinter.super)) {
        fprintf(stderr, "Failed to setup file printer.\n");
        return EXIT_FAILURE;
    }
    if (AddMsgPrinter(&consolePrinter.super)) {
        fprintf(stderr, "Failed to setup console printer.\n");
        return EXIT_FAILURE;
    }

    failure = 1;
    sectionCount = 0;
    section = NULL;

    /* Extract command line options */
    ret = getCommandLineOptions(argc, argv, &cmdOptions);
    if (ret <= 0) {
        usage();
        goto error_1;
    }

    /* Read configuration file */
    if (cmdOptions.cfgFileName) {
        ret = loadSections(cmdOptions.cfgFileName, &section, &sectionCount);
        if (!ret) {
            ERROR_MSG("Unable to read %s", cmdOptions.cfgFileName);
            goto error_1;
        }
    }

    /* Initialize memory map */
    ret = initializeMemoryMap(&memmap);
    if (0 == ret) {
        goto error_1;
    }

    /* Read ROM */
    if (0 == cmdOptions.cdrom) {
        ret = loadROM(cmdOptions.romFileName, &memmap);
        if (0 == ret) {
            goto error_2;
        }

        /* Get irq offsets */
        if (cmdOptions.extractIRQ) {
            ret = getIRQSections(&memmap, &section, &sectionCount);
            if (0 == ret) {
                ERROR_MSG("An error occured while reading irq vector offsets");
                goto error_2;
            }
        }
    } else {
        ret = addCDRAMMemoryMap(&memmap);
        if (0 == ret) {
            goto error_2;
        }

        if (cmdOptions.extractIRQ) {
            IPL ipl;
            ret = readIPL(&ipl, cmdOptions.romFileName);
            ret = ret && getIPLSections(&ipl, &section, &sectionCount);
            if (0 == ret) {
                ERROR_MSG("An error occured while setting up sections from IPL data.");
                goto error_2;
            }
        }
        /*  Data will be loaded during section disassembly */
    }

    /**/
    sortSections(section, sectionCount);

    /* Initialize decoder */
    createDecoder(&decoder);

    /* Load labels */
    if (NULL != cmdOptions.labelsIn) {
        for(i=0; cmdOptions.labelsIn[i]; i++) {
            ret = loadLabels(cmdOptions.labelsIn[i], decoder.labels);
            if (0 == ret) {
                ERROR_MSG("An error occured while loading labels from %s : %s", cmdOptions.labelsIn[i], strerror(errno));
                goto error_4;
            }
        }
    }

    /* For each section reset every existing files */
    for (i = 0; i < sectionCount; ++i) {
        out = fopen(section[i].filename, "wb");
        if (NULL == out) {
            ERROR_MSG("Can't open %s : %s", section[i].filename, strerror(errno));
            goto error_4;
        }
        fclose(out);
    }

    /* Add section name to label repository. */
    for (i = 0; i < sectionCount; ++i) {
        ret = addLabel(decoder.labels, section[i].name, section[i].org, section[i].bank);
        if (0 == ret) {
            ERROR_MSG("Failed to add section name (%s) to labels", section[i].name);
            goto error_4;
        }
    }

    /* Disassemble and output */
    for (i = 0; i < sectionCount; ++i) {
        out = fopen(section[i].filename, "ab");
        if (NULL == out) {
            ERROR_MSG("Can't open %s : %s", section[i].filename, strerror(errno));
            goto error_4;
        }

        if ((0 != cmdOptions.cdrom) || (section[i].offset != ((section[i].bank << 13) | (section[i].org & 0x1fff)))) {
            /* Copy CDROM data */
            ret = loadCD(cmdOptions.romFileName, section[i].offset, section[i].size, section[i].bank, section[i].org,
                         &memmap);
            if (0 == ret) {
                ERROR_MSG("Failed to load CD data (section %d)", i);
                goto error_4;
            }
        }

        if (section[i].type != BinData) {
            if((i > 0) && (0 == strcmp(section[i].filename, section[i-1].filename))
                       && (section[i].type == section[i-1].type)
                       && (section[i].bank == section[i-1].bank)
                       && (section[i].org <= (section[i-1].org + section[i-1].size))) {
                // "Merge" sections and adjust size if necessary.
                if(section[i].size > 0) {
                    uint32_t end0 = section[i-1].org + section[i-1].size;
                    uint32_t end1 = section[i].org + section[i].size;
                    if(end1 > end0) {
                        section[i].size = end1 - end0;
                        section[i].org = end0;
                        INFO_MSG("Section %s has been merged with %s!", section[i].name, section[i-1].name);
                    }
                    else {
                        // The previous section overlaps the current one.
                        // We skip it as it has already been processed.
                        fclose(out);
                        out = NULL;
                        continue;
                    }
                }
                else {
                    section[i].org = section[i-1].org + section[i-1].size;
                    INFO_MSG("Section %s has been merged with %s!", section[i].name, section[i-1].name);
                }
            }
            else {
                /* Print header */
                fprintf(out, "\t.%s\n"
                             "\t.bank $%03x\n"
                             "\t.org $%04x\n",
                        (section[i].type == Code) ? "code" : "data", section[i].bank, section[i].org);
            }
        }

        /* Reset decoder */
        resetDecoder(&memmap, out, &section[i], &decoder);

        if (Code == section[i].type) {
            char eor;

            /* Extract labels */
            ret = extractLabels(&decoder);
            if (0 == ret) {
                goto error_4;
            }
            /* Process opcodes */
            do {
                eor = processOpcode(&decoder);
                if (!cmdOptions.extractIRQ) {
                    if (decoder.offset >= decoder.current->size)
                        eor = 1;
                    else
                        eor = 0;
                }
            } while (!eor);
        } else {
            ret = processDataSection(&decoder);
            if (0 == ret) {
            }
        }
        fputc('\n', decoder.out);

        fclose(out);
        out = NULL;
    }

    /* Open main asm file */
    mainFile = fopen(cmdOptions.mainFileName, "w");
    if (NULL == mainFile) {
        ERROR_MSG("Unable to open %s : %s", cmdOptions.mainFileName, strerror(errno));
        goto error_4;
    }

    if (!cmdOptions.cdrom && cmdOptions.extractIRQ) {
        fprintf(mainFile, "\n\t.data\n\t.bank 0\n\t.org $FFF6\n");
        for (i = 0; i < 5; ++i) {
            fprintf(mainFile, "\t.dw $%04x\n", section[i].org);
        }
    }

    fclose(mainFile);

    /* Output labels  */
    if (!outputLabels(&cmdOptions, decoder.labels)) {
        goto error_4;
    }
    failure = 0;

error_4:
    deleteDecoder(&decoder);
error_2:
    destroyMemoryMap(&memmap);
error_1:
    if(cmdOptions.labelsIn) {
        free(cmdOptions.labelsIn);
        cmdOptions.labelsIn = NULL;
    }

    for (i = 0; i < sectionCount; ++i) {
        free(section[i].name);
        free(section[i].filename);
        section[i].name = NULL;
    }
    free(section);

    return failure;
}
