/*
    This file is part of Etripator,
    copyright (c) 2009--2019 Vincent Cruz.

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
#include <config.h>

#include <jansson.h>
#include <time.h>

#include <message.h>
#include <message/console.h>
#include <message/file.h>

#include <cd.h>
#include <decode.h>
#include <irq.h>
#include <label.h>
#include <label/load.h>
#include <label/save.h>
#include <memorymap.h>
#include <opcodes.h>
#include <rom.h>
#include <ipl.h>
#include <section.h>
#include <section/load.h>

#include "options.h"

/*
  exit callback
 */
void exit_callback(void) { msg_printer_destroy(); }

/*
  output labels
*/
int label_output(cli_opt_t *option, label_repository_t *repository) {
    char buffer[256];

    if(NULL == option->labels_out) { 
        /* We don't want to destroy the original label file. */
        char *tmp;

        struct timeval tv;
        struct tm *now;
        char dateString[128];
            
        gettimeofday(&tv, NULL);
        now = localtime(&tv.tv_sec);
        strftime(dateString, 128, "%Y%m%d%H%M%S", now);
        
        tmp = basename(option->rom_filename);
        snprintf(buffer, 256, "%s.%s.lbl", tmp, dateString);     
        option->labels_out = buffer;
    }
    if (!label_repository_save(option->labels_out, repository)) {
        ERROR_MSG("Failed to write/update label file: %s", option->labels_out);
        return 0;
    }
    return 1;
}

/* ---------------------------------------------------------------- */
int main(int argc, const char **argv) {
    cli_opt_t option;

    FILE *out;
    FILE *main_file;
    int i;
    int ret, failure;

    console_msg_printer_t console_printer;
    file_msg_printer_t file_printer;

    memmap_t map;

    section_t *section;
    int section_count;

    atexit(exit_callback);

    msg_printer_init();

    file_msg_printer_init(&file_printer);
    console_msg_printer_init(&console_printer);

    if (msg_printer_add(&file_printer.super)) {
        fprintf(stderr, "Failed to setup file printer.\n");
        return EXIT_FAILURE;
    }
    if (msg_printer_add(&console_printer.super)) {
        fprintf(stderr, "Failed to setup console printer.\n");
        return EXIT_FAILURE;
    }

    failure = 1;
    section_count = 0;
    section = NULL;

    /* Extract command line options */
    ret = get_cli_opt(argc, argv, &option);
    if (ret <= 0) {
        goto error_1;
    }

    /* Read configuration file */
    if (option.cfg_filename) {
        ret = section_load(option.cfg_filename, &section, &section_count);
        if (!ret) {
            ERROR_MSG("Unable to read %s", option.cfg_filename);
            goto error_1;
        }
    }

    /* Initialize memory map */
    ret = memmap_init(&map);
    if (!ret) {
        goto error_1;
    }

    /* Read ROM */
    if (!option.cdrom) {
        ret = rom_load(option.rom_filename, &map);
        if (!ret) {
            goto error_2;
        }

        /* Get irq offsets */
        if (option.extract_irq) {
            ret = irq_read(&map, &section, &section_count);
            if (!ret) {
                ERROR_MSG("An error occured while reading irq vector offsets");
                goto error_2;
            }
        }
    } else {
        ret = cd_memmap(&map);
        if (!ret) {
            goto error_2;
        }

        if (option.extract_irq) {
            ipl_t ipl;
            ret = ipl_read(&ipl, option.rom_filename);
            ret = ret && ipl_sections(&ipl, &section, &section_count);
            if (!ret) {
                ERROR_MSG("An error occured while setting up sections from IPL data.");
                goto error_2;
            }
        }
        /*  Data will be loaded during section disassembly */
    }

    section_sort(section, section_count);

    label_repository_t *repository = label_repository_create();
    
    /* Load labels */
    if (NULL != option.labels_in) {
        for(i=0; option.labels_in[i]; i++) {
            ret = label_repository_load(option.labels_in[i], repository);
            if (!ret) {
                ERROR_MSG("An error occured while loading labels from %s : %s", option.labels_in[i], strerror(errno));
                goto error_4;
            }
        }
    }

    /* For each section reset every existing files */
    for (i = 0; i < section_count; ++i) {
        out = fopen(section[i].output, "wb");
        if (NULL == out) {
            ERROR_MSG("Can't open %s : %s", section[i].output, strerror(errno));
            goto error_4;
        }
        fclose(out);
    }

    /* Add section name to label repository. */
    for (i = 0; i < section_count; ++i) {
        ret = label_repository_add(repository, section[i].name, section[i].logical, section[i].page);
        if (!ret) {
            ERROR_MSG("Failed to add section name (%s) to labels", section[i].name);
            goto error_4;
        }
    }

    /* Disassemble and output */
    for (i = 0; i < section_count; ++i) {
        out = fopen(section[i].output, "ab");
        if (!out) {
            ERROR_MSG("Can't open %s : %s", section[i].output, strerror(errno));
            goto error_4;
        }

        if ((0 != option.cdrom) || (section[i].offset != ((section[i].page << 13) | (section[i].logical & 0x1fff)))) {
            /* Copy CDROM data */
            ret = cd_load(option.rom_filename, section[i].offset, section[i].size, section[i].page, section[i].logical, &map);
            if (0 == ret) {
                ERROR_MSG("Failed to load CD data (section %d)", i);
                goto error_4;
            }
        }

        if((i > 0) && (section[i].logical < (section[i-1].logical + section[i-1].size))
                   && (section[i].page == section[i-1].page) 
                   && (section[i].type != section[i-1].type)) {
            WARNING_MSG("Section %s and %s overlaps!", section[i].name, section[i-1].name);
        }

        if((i > 0) && (0 == strcmp(section[i].output, section[i-1].output))
                   && (section[i].page == section[i-1].page)
                   && (section[i].logical <= (section[i-1].logical + section[i-1].size))) {
            // "Merge" sections and adjust size if necessary.
            if(section[i].size > 0) {
                uint32_t end0 = section[i-1].logical + section[i-1].size;
                uint32_t end1 = section[i].logical + section[i].size;
                if(end1 > end0) {
                    section[i].size = end1 - end0;
                    section[i].logical = end0;
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
                section[i].logical = section[i-1].logical + section[i-1].size;
                INFO_MSG("Section %s has been merged with %s!", section[i].name, section[i-1].name);
            }
        }
        else {
            /* Print header */
            fprintf(out, "\t.%s\n"
                         "\t.bank $%03x\n"
                         "\t.org $%04x\n",
                    (section[i].type == Code) ? "code" : "data", section[i].page, section[i].logical);
        }

        memmap_mpr(&map, section[i].mpr);
       
        if (section[i].type == Code) {
            if(section[i].size <= 0) {
                section[i].size = compute_size(&section[i], &map);
            }

            /* Extract labels */
            ret = label_extract(&section[i], &map, repository);
            if (!ret) {
                goto error_4;
            }
            /* Process opcodes */
            uint16_t logical = section[i].logical;
            do {
                (void)decode(out, &logical, &section[i], &map, repository);
            } while (logical < (section[i].logical+section[i].size));
        } else {
            ret = data_extract(out, &section[i], &map, repository);
            if (!ret) {
            }
        }
        fputc('\n', out);

        fclose(out);
        out = NULL;
    }

    /* Open main asm file */
    main_file = fopen(option.main_filename, "w");
    if (!main_file) {
        ERROR_MSG("Unable to open %s : %s", option.main_filename, strerror(errno));
        goto error_4;
    }

    if (!option.cdrom && option.extract_irq) {
        fprintf(main_file, "\n\t.data\n\t.bank 0\n\t.org $FFF6\n");
        for (i = 0; i < 5; ++i) {
            fprintf(main_file, "\t.dw $%04x\n", section[i].logical);
        }
    }

    fclose(main_file);

    /* Output labels  */
    if (!label_output(&option, repository)) {
        goto error_4;
    }
    failure = 0;

error_4:
    label_repository_destroy(repository);
error_2:
    memmap_destroy(&map);
error_1:
    if(option.labels_in) {
        free(option.labels_in);
        option.labels_in = NULL;
    }

    for (i = 0; i < section_count; ++i) {
        free(section[i].name);
        free(section[i].output);
        section[i].name = NULL;
    }
    free(section);

    return failure;
}
