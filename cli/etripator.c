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
static bool label_output(LabelRepository *repository, CommandLineOptions *options) {
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

static bool load_sections(SectionArray *arr, const char *filename) {
    bool ret = true;
    if (filename) {
        if (!section_load(arr, filename)) {
            ERROR_MSG("Unable to read %s", filename);
            ret = false;
        }
    }
    return ret;
}

static bool load_labels(LabelRepository *repository, const char **filename) {
    bool ret = label_repository_create(repository);
    if (filename != NULL) {
        for(int i=0; ret && filename[i]; i++) {
            ret = label_repository_load(repository, filename[i]);
            if (!ret) {
                ERROR_MSG("An error occured while loading labels from %s", filename[i]);
            }
        }
    }
    return ret;
}

static bool load_comments(CommentRepository *repository, const char **filename) {
    bool ret = comment_repository_create(repository);
    if(filename != NULL) {
        for(int i=0; ret && filename[i]; i++) {
            ret = comment_repository_load(repository, filename[i]);
            if (!ret) {
                ERROR_MSG("An error occured while loading comments from %s", filename[i]);
            }
        }
    }
    return ret;
}

static bool load_rom(MemoryMap *map, const CommandLineOptions *options) {
    bool ret = false;
    if (options->cdrom) {
        ret = cd_memory_map(map);
        /*  Data will be loaded during section disassembly */
    } else {
        ret = rom_load(options->rom_filename, map);
    }
    return ret;
}

static bool load_irq(MemoryMap *map, SectionArray *arr, const CommandLineOptions *options) {
    bool ret = false;
    if(!options->extract_irq) {
        ret = true;
    } else if (options->cdrom) {    
        IPL ipl = {0};
        if (!ipl_read(&ipl, options->rom_filename)) {
            // ...
        } else if (!ipl_sections(&ipl, arr)) {
            ERROR_MSG("An error occured while setting up sections from IPL data.");
        } else {
            ret = true;
        }
    } else if (!irq_read(map, arr)) {
        ERROR_MSG("An error occured while reading irq vector offsets");
    } else {
        ret = true;
    }
    return ret;
}

static bool reset_output(SectionArray *arr) {
    bool ret = true;
    /* For each section reset every existing files */
    for (int i = 0; ret && (i < arr->count); ++i) {
        Section *section = &arr->data[i];
        FILE *out = fopen(section->output, "wb");
        if (out == NULL) {
            ERROR_MSG("Can't open %s : %s", section->output, strerror(errno));
            ret = false;
        } else {
            fclose(out);
        }
    }
    return ret;
}

static bool fill_label_reporitory(LabelRepository *labels, SectionArray *arr) {
    bool ret = true;
    /* Add section name to label repository. */
    for (int i = 0; ret && (i < arr->count); ++i) {
        Section *section = &arr->data[i];
        ret = label_repository_add(labels, section->name, section->logical, section->page, section->description);
        if (!ret) {
            ERROR_MSG("Failed to add section name (%s) to labels", section->name);
        }
    }
    return ret;
}

static bool output_main(MemoryMap *map, LabelRepository *labels, const CommandLineOptions *options) {
    bool ret = false;
    FILE *out = fopen(options->main_filename, "w");
    if (out == NULL) {
        ERROR_MSG("Unable to open %s : %s", options->main_filename, strerror(errno));
    } else {
        label_dump(out, map, labels);
        fclose(out);
        ret = true;
    }
    return ret;
}

static bool code_extract(FILE *out, SectionArray *arr, int index, MemoryMap *map, LabelRepository *labels, CommentRepository *comments, int address) {
    bool ret = false;
    Section *current = &arr->data[index];
    if(current->size <= 0) {
        current->size = compute_size(arr, index, arr->count, map);
    }
    if (!label_extract(current, map, labels)) {
        // ...
    } else {
        /* Process opcodes */
        uint16_t logical = current->logical;
        do {
            (void)decode(out, &logical, current, map, labels, comments, address);
        } while (logical < (current->logical+current->size));
        fputc('\n', out);
        ret = true;
    }
    return true;
}

static void print_header(FILE *out, Section *current, Section *previous) {
    if(previous && ((previous->logical + previous->size) == current->logical)) {
        // ...
    } else if((current->type != SECTION_TYPE_DATA) || (current->data.type != DATA_TYPE_BINARY)) {
        /* Print header */
        fprintf(out, "\t.%s\n"
                    "\t.bank $%03x\n"
                    "\t.org $%04x\n",
                (current->type == SECTION_TYPE_CODE) ? "code" : "data", current->page, current->logical);
    }
}

static bool disassemble(SectionArray *arr, MemoryMap *map, LabelRepository *labels, CommentRepository *comments, CommandLineOptions *options) {
    bool ret = true;
    Section *previous = NULL;
    Section *current = NULL;
    for (int i = 0; ret && (i < arr->count); ++i, previous=current) {
        current = &arr->data[i];
        FILE *out = fopen(current->output, "ab");
        if (out == NULL) {
            ERROR_MSG("Can't open %s : %s", current->output, strerror(errno));
        } else {
            if (options->cdrom && (current->offset != ((current->page << 13) | (current->logical & 0x1fff)))) {
                size_t offset = current->offset;
                /* Copy CDROM data */
                if (!cd_load(options->rom_filename, current->offset, current->size, options->sector_size, current->page, current->logical, map)) {
                    ERROR_MSG("Failed to load CD data (section %d)", i);
                } else {
                    ret = false;
                }
            }

            if(ret) {
                print_header(out, current, previous);
                memory_map_mpr(map, current->mpr);
        
                if (current->type == SECTION_TYPE_CODE) {
                    ret = code_extract(out, arr, i, map, labels, comments, options->address); 
                } else {
                    ret = data_extract(out, current, map, labels, comments, options->address);
                }
            }
            fclose(out);
        }
    }
    return ret;
}

/* ---------------------------------------------------------------- */
int main(int argc, const char **argv) {
    int ret = EXIT_FAILURE;

    CommandLineOptions options = {0};

    MemoryMap map = {0};
    SectionArray section_arr = {0};

    LabelRepository labels = {0};

    CommentRepository comments = {0};

    section_array_reset(&section_arr);

    atexit(exit_callback);

    message_printer_init();

    if (file_message_printer_init() != true) {
        fprintf(stderr, "Failed to setup file printer.\n");
    } else if (console_message_printer_init() != true) {
        fprintf(stderr, "Failed to setup console printer.\n");
    } else if (log_cli(argc, argv) != true) {
        fprintf(stderr, "Failed to log command line.\n");
    } else if (cli_opt_get(&options, argc, argv) != true) {
        // ...
    } else if (memory_map_init(&map) != true) {
        // ...
    } else if (!load_sections(&section_arr, options.cfg_filename)) {
        // ...
    } else if (!load_labels(&labels, options.labels_in)) {
        // ...
    } else if (!load_comments(&comments, options.comments_in)) {
        // ...
    } else if (!load_rom(&map, &options)) {
        // ...
    } else if (!load_irq(&map, &section_arr, &options)) {
        // ...
    } else if (!reset_output(&section_arr)) {
        // ...
    } else if (!fill_label_reporitory(&labels, &section_arr)) {
        // ...
    } else {
        section_array_tidy(&section_arr);
        if(!output_main(&map, &labels, &options)) {
            // ...
        } else {
            ret = EXIT_SUCCESS;
            if(!disassemble(&section_arr, &map, &labels, &comments, &options)) {
                ret = EXIT_FAILURE;
            }
            if (label_output(&labels, &options)) {
                ret = EXIT_FAILURE;
            }
        }
    }

    label_repository_destroy(&labels);
    comment_repository_destroy(&comments);
    memory_map_destroy(&map);
    section_array_delete(&section_arr);
    cli_opt_release(&options);

    return ret;
}
