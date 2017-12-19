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
#include <jansson.h>
#include <time.h>
#include <sys/time.h>

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
    /* We don't want to destroy the original label file. */
    char buffer[256];
    char *tmp;

    struct timeval tv;
    struct tm *now;
    char dateString[128];
        
    gettimeofday(&tv, NULL);
    now = localtime(&tv.tv_sec);
    strftime(dateString, 128, "%Y%m%d%H%M%S", now);
    
    tmp = basename(options->romFileName);
    snprintf(buffer, 256, "%s.%s.lbl", tmp, dateString);     
    if (!writeLabels(&buffer[0], repository)) {
        ERROR_MSG("Failed to write/update label file: %s", buffer);
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

    /* Extract command line options */
    ret = getCommandLineOptions(argc, argv, &cmdOptions);
    if (ret <= 0) {
        usage();
        return (ret < 0);
    }

    failure = 1;

    /* Read configuration file */
    if (cmdOptions.extractIRQ) {
        sectionCount = 5;
        section = (Section *)malloc(sectionCount * sizeof(Section));
        if (NULL == section) {
            ERROR_MSG("Failed to allocate memory: %s", strerror(errno));
            goto error_1;
        }
    } else {
        sectionCount = 0;
        section = NULL;
    }

    if (cmdOptions.cfgFileName) {
        ret = loadSections(cmdOptions.cfgFileName, &section, &sectionCount);
        if (!ret) {
            ERROR_MSG("Unable to read %s", cmdOptions.cfgFileName);
            goto error_1;
        }
    }

    /**/
    sortSections(section, sectionCount);

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
            ret = getIRQSections(&memmap, section);
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
        /*  Data will be loaded during section disassembly */
    }

    /* Initialize decoder */
    createDecoder(&decoder);

    /* Load labels */
    if (NULL != cmdOptions.labelsFileName) {
        ret = loadLabels(cmdOptions.labelsFileName, decoder.labels);
        if (0 == ret) {
            ERROR_MSG("An error occured while loading labels from %s : %s", cmdOptions.labelsFileName, strerror(errno));
            goto error_4;
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

        if (0 != cmdOptions.cdrom) {
            /* Copy CDROM data */
            ret = loadCD(cmdOptions.romFileName, section[i].offset, section[i].size, section[i].bank, section[i].org,
                         &memmap);
            if (0 == ret) {
                ERROR_MSG("Failed to load CD data (section %d)", i);
                goto error_4;
            }
        }

        if (section[i].type != BinData) {
            /* Print header */
            fprintf(out, "\t.%s\n"
                         "\t.bank %x\n"
                         "\t.org $%04x\n",
                    (section[i].type == Code) ? "code" : "data", section[i].bank, section[i].org);
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

    if (cmdOptions.extractIRQ) {
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
    for (i = 0; i < sectionCount; ++i) {
        free(section[i].name);
        free(section[i].filename);
        section[i].name = NULL;
    }
    free(section);

    return failure;
}
