/*
¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

  __/¯\____ ___/\__   _/\__   _/\_   _/\__   _/\___ ___/\__   __/\_   _/\__   
  \_  ____/_> ____ \_/  _  \_ \  <  /_    \_/     _>> ____ \_ >    \_/  _  \_ 
  _> ___/ ¯>__> <<__// __  _/ |>  ></ _/>  </  ¯  \\__> <<__//  /\  // __  _/ 
 _>  \7   <__/:. \__/:. \>  \_/   L/  _____/.  7> .\_/:. \__/  <_/ </:. \>  \_ 
|:::::::::::::::::::::::/:::::::::::::>::::::::/::::::::::::::::::::::::/:::::|
|¯¯\::::/\:/¯\::::/¯¯¯¯<::::/\::/¯¯\:/¯¯¯¯¯¯\::\::/¯¯\::::/¯¯\::::/¯¯¯¯<::::/¯|
|__ |¯¯|  T _ |¯¯¯| ___ |¯¯|  |¯| _ T ______ |¯¯¯¯| _ |¯¯¯| _ |¯¯¯| ___ |¯¯| _|
   \|__|/\|/ \|___|/   \|__|/\|_|/ \|/      \|    |/ \|___|/ \|___|/dNo\|__|/  

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

  This file is part of Etripator,
  copyright (c) 2009--2024 Vincent Cruz.
 
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

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯
*/
#include <config.h>

#include <jansson.h>
#include <cwalk.h>

#include <time.h>

#include <message.h>
#include <message/console.h>
#include <message/file.h>

#include <cd.h>
#include <decode.h>
#include <irq.h>
#include <label.h>
#include <comment.h>
#include <memory_map.h>
#include <opcodes.h>
#include <rom.h>
#include <ipl.h>
#include <section.h>
#include <comment.h>

#include "options.h"

// exit callback
void exit_callback() { 
    message_printer_destroy();
}

// save found labels to a file named <rom name>.YYmmddHHMMSS.lbl
static bool label_output(CommandLineOptions *options, LabelRepository *repository) {
    bool ret = false;
    char buffer[256U] = {0};
    if (options->labels_out == NULL) {
        // Create a new filename as we don't want to destroy the original label file.
        const char *filename;
        size_t length;

        cwk_path_get_basename(options->rom_filename, &filename, &length);
        if(filename == NULL) {
            filename = options->rom_filename;
        }

        time_t t = time(NULL);
        struct tm *now = localtime(&t);

        char date_string[128U] = {0};
        strftime(date_string, sizeof(date_string), "%Y%m%d%H%M%S", now);

        snprintf(buffer, 256, "%s.%s.lbl", filename, date_string);     
        options->labels_out = buffer;
    }
    if (!label_repository_save(repository, options->labels_out)) {
        ERROR_MSG("Failed to write/update label file: %s", options->labels_out);
    } else {
        ret = true;
    }
    if(options->labels_out == buffer) {
        options->labels_out = NULL;
    }
    return ret;
}

// log command line
static bool log_cli(int argc, const char* argv[]) {
    bool ret = false;

    size_t len = 0;
    for(int i=0; i<argc; i++) {
        len += strlen(argv[i]) + 1;
    }
    char *buffer = (char*)malloc(len);
    if(buffer != NULL) {
        char *ptr = buffer;
        for(int i=0; i<argc; i++) {
            size_t n = strlen(argv[i]);
            strcpy(ptr, argv[i]);
            ptr += n;
            *ptr++ = ' ';
        }
        buffer[len-1] = '\0';
        INFO_MSG("Command line: %s", buffer);
        free(buffer);
        ret = true;
    }
    return ret;
}

/* ---------------------------------------------------------------- */
int main(int argc, const char **argv) {
    int ret = EXIT_FAILURE;

    atexit(exit_callback);

    message_printer_init();

    if (file_message_printer_init() != true) {
        fprintf(stderr, "Failed to setup file printer.\n");
    } else if (console_message_printer_init() != true) {
        fprintf(stderr, "Failed to setup console printer.\n");
    } else if (log_cli(argc, argv) != true) {
        fprintf(stderr, "Failed to log command line.\n");
    } else {
        // [todo]
    }

    CommandLineOptions options;

    FILE *out;
    FILE *main_file;
    int failure;

    MemoryMap map = {0};

    SectionArray section_arr = {0};

    failure = 1;

    section_array_reset(&section_arr);

    /* Extract command line options */
    ret = cli_opt_get(&options, argc, argv);
    if (ret <= 0) {
        goto error_1;
    }

    /* Read configuration file */
    if (options.cfg_filename) {
        ret = section_load(&section_arr, options.cfg_filename);
        if (!ret) {
            ERROR_MSG("Unable to read %s", options.cfg_filename);
            goto error_1;
        }
    }

    /* Initialize memory map */
    ret = memory_map_init(&map);
    if (!ret) {
        goto error_1;
    }

    /* Read ROM */
    if (!options.cdrom) {
        ret = rom_load(options.rom_filename, &map);
        if (!ret) {
            goto error_2;
        }

        /* Get irq offsets */
        if (options.extract_irq) {
            ret = irq_read(&map, &section_arr);
            if (!ret) {
                ERROR_MSG("An error occured while reading irq vector offsets");
                goto error_2;
            }
        }
    } else {
        ret = cd_memory_map(&map);
        if (!ret) {
            goto error_2;
        }

        if (options.extract_irq) {
            IPL ipl;
            ret = ipl_read(&ipl, options.rom_filename);
            ret = ret && ipl_sections(&ipl, &section_arr);
            if (!ret) {
                ERROR_MSG("An error occured while setting up sections from IPL data.");
                goto error_2;
            }
        }
        /*  Data will be loaded during section disassembly */
    }

// [todo]    section_sort(section, section_count);

    CommentRepository *comments_repository = comment_repository_create();
    /* Load comments */
    if(NULL != options.comments_in) {
        for(int i=0; options.comments_in[i]; i++) {
            ret = comment_repository_load(comments_repository, options.comments_in[i]);
            if (!ret) {
                ERROR_MSG("An error occured while loading comments from %s : %s", options.comments_in[i], strerror(errno));
                goto error_3;
            }
        }
    }

    LabelRepository *repository = label_repository_create();
    /* Load labels */
    if (NULL != options.labels_in) {
        for(int i=0; options.labels_in[i]; i++) {
            ret = label_repository_load(repository, options.labels_in[i]);
            if (!ret) {
                ERROR_MSG("An error occured while loading labels from %s : %s", options.labels_in[i], strerror(errno));
                goto error_4;
            }
        }
    }

    /* For each section reset every existing files */
    for (int i = 0; i < section_arr.count; ++i) {
        Section *section = &section_arr.data[i];
        out = fopen(section->output, "wb");
        if (NULL == out) {
            ERROR_MSG("Can't open %s : %s", section->output, strerror(errno));
            goto error_4;
        }
        fclose(out);
    }

    /* Add section name to label repository. */
    for (int i = 0; i < section_arr.count; ++i) {
        Section *section = &section_arr.data[i];
        ret = label_repository_add(repository, section->name, section->logical, section->page, section->description);
        if (!ret) {
            ERROR_MSG("Failed to add section name (%s) to labels", section->name);
            goto error_4;
        }
    }

    /* Disassemble and output */
    Section *previous = NULL;
    Section *current = NULL;
    for (int i = 0; i < section_arr.count; ++i, previous=current) {
        current = &section_arr.data[i];
        out = fopen(current->output, "ab");
        if (!out) {
            ERROR_MSG("Can't open %s : %s", current->output, strerror(errno));
            goto error_4;
        }

        if (options.cdrom || (current->offset != ((current->page << 13) | (current->logical & 0x1fff)))) {
            size_t offset = current->offset;
            /* Copy CDROM data */
            ret = cd_load(options.rom_filename, current->offset, current->size, options.sector_size, current->page, current->logical, &map);
            if (0 == ret) {
                ERROR_MSG("Failed to load CD data (section %d)", i);
                goto error_4;
            }
        }

        if((previous != NULL) && (current->logical < (previous->logical + previous->size))
                              && (current->page == previous->page) 
                              && (current->type != previous->type)) {
            WARNING_MSG("Section %s and %s overlaps! %x %x.%x", current->name, previous->name);
        }

        if((previous != NULL) && (0 == strcmp(current->output, previous->output))
                              && (current->page == previous->page)
                              && (current->logical <= (previous->logical + previous->size))) {
            // "Merge" sections and adjust size if necessary.
            if(current->size > 0) {
                uint32_t end0 = previous->logical + previous->size;
                uint32_t end1 = current->logical + current->size;
                if(end1 > end0) {
                    current->size = end1 - end0;
                    current->logical = end0;
                    INFO_MSG("Section %s has been merged with %s!", current->name, previous->name);
                }
                else {
                    // The previous section overlaps the current one.
                    // We skip it as it has already been processed.
                    fclose(out);
                    continue;
                }
            } else {
                current->logical = previous->logical + previous->size;
                INFO_MSG("Section %s has been merged with %s!", current->name, previous->name);
            }
        } else if((current->type != SECTION_TYPE_DATA) || (current->data.type != DATA_TYPE_BINARY)) {
            /* Print header */
            fprintf(out, "\t.%s\n"
                         "\t.bank $%03x\n"
                         "\t.org $%04x\n",
                    (current->type == SECTION_TYPE_CODE) ? "code" : "data", current->page, current->logical);
        }

        memory_map_mpr(&map, current->mpr);
       
        if (current->type == SECTION_TYPE_CODE) {
            if(current->size <= 0) {
                current->size = compute_size(current, i, section_arr.count, &map);
            }

            /* Extract labels */
            ret = label_extract(current, &map, repository);
            if (!ret) {
                goto error_4;
            }
            /* Process opcodes */
            uint16_t logical = current->logical;
            do {
                (void)decode(out, &logical, current, &map, repository, comments_repository, options.address);
            } while (logical < (current->logical+current->size));
            fputc('\n', out);
        } else {
            ret = data_extract(out, current, &map, repository, comments_repository, options.address);
            if (!ret) {
                // [todo]
            }
        }

        fclose(out);
    }

    /* Open main asm file */
    main_file = fopen(options.main_filename, "w");
    if (!main_file) {
        ERROR_MSG("Unable to open %s : %s", options.main_filename, strerror(errno));
        goto error_4;
    }

    label_dump(main_file, &map, repository);

    if (!options.cdrom && options.extract_irq) {
        fprintf(main_file, "\n\t.data\n\t.bank 0\n\t.org $FFF6\n");
        for (int i = 0; i < 5; ++i) {
            fprintf(main_file, "\t.dw $%04x\n", current->logical);
        }
    }

    fclose(main_file);

    /* Output labels  */
    if (!label_output(&options, repository)) {
        goto error_4;
    }
    failure = 0;

error_4:
    label_repository_destroy(repository);
error_3:
    comment_repository_destroy(comments_repository);
error_2:
    memory_map_destroy(&map);
error_1:
    cli_opt_release(&options);

    section_array_delete(&section_arr);

    return failure;
}
