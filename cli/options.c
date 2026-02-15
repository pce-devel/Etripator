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
  copyright (c) 2009--2026 Vincent Cruz.
 
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
#include "options.h"

#include <errno.h>

#include <cargs.h>

#include <message.h>

enum {
    CLI_OPTION_ID_IRQ_DETECT = 0,
    CLI_OPTION_ID_CD,
    CLI_OPTION_ID_MAIN_OUT,
    CLI_OPTION_ID_LABELS_IN,
    CLI_OPTION_ID_HELP,
    CLI_OPTION_ID_LABELS_OUT,
    CLI_OPTION_ID_COMMENTS_IN,
    CLI_OPTION_ID_ADDR_PRINT,
    CLI_OPTION_ID_SECTOR_SIZE,
    CLI_OPTION_ID_COUNT,
    CLI_OPTION_ID_ERROR = '?'
} CLIOptionID;

static struct cag_option g_options[] = {
    {.identifier = CLI_OPTION_ID_IRQ_DETECT,
     .access_letters = "i",
     .access_name = "irq-detect",
     .value_name = NULL,
     .description = "automatically detect and extract irq vectors when disassembling a ROM, or extract opening code and gfx from CDROM IPL data"},
    {.identifier = CLI_OPTION_ID_CD,
     .access_letters = "c",
     .access_name = "cd",
     .value_name = NULL,
     .description = "cdrom image disassembly. Irq detection and rom. Header jump is not performed"},
    {.identifier = CLI_OPTION_ID_MAIN_OUT,
     .access_letters = "o",
     .access_name = "out",
     .value_name = "FILENAME",
     .description = "main asm file containing includes for all sections as long the irq vector table if the irq-detect option is enabled"},
    {.identifier = CLI_OPTION_ID_LABELS_IN,
     .access_letters = "l",
     .access_name = "labels",
     .value_name = "FILENAME",
     .description = "labels definition filename"},
    {.identifier = CLI_OPTION_ID_HELP,
     .access_letters = "h",
     .access_name = "help",
     .value_name = NULL,
     .description = "Shows the command help"},
    {.identifier = CLI_OPTION_ID_LABELS_OUT,
     .access_letters = NULL,
     .access_name = "labels-out",
     .value_name = "FILENAME",
     .description = "Shows the command help"},
    {.identifier = CLI_OPTION_ID_COMMENTS_IN,
     .access_letters = NULL,
     .access_name = "comments",
     .value_name = "FILENAME",
     .description = "comments description filename"},
    {.identifier = CLI_OPTION_ID_ADDR_PRINT,
     .access_letters = NULL,
     .access_name = "address",
     .value_name = NULL,
     .description = "print statement address as comment"},
    {.identifier = CLI_OPTION_ID_SECTOR_SIZE,
     .access_letters = NULL,
     .access_name = "sector_size",
     .value_name = "SIZE",
     .description = "Sector size in bytes (2048 or 2352) (cd only)"},
};

static void cliopt_reset(CommandLineOptions *options) {
    options->extract_irq = false;
    options->cdrom = false;
    options->cfg_filename  = NULL;
    options->rom_filename  = NULL;
    options->main_filename = "main.asm";
    options->labels_in = NULL;
    options->labels_out = NULL;
    options->comments_in = NULL;
    options->address = false;
    options->sector_size = 2048;
}

static bool cliopt_validate_input(const char *filename) {
    bool ret = false;
    struct stat infos = {0};

    if(filename == NULL) {
        ERROR_MSG("missing argument");
    } else if(strlen(filename) == 0) {
        ERROR_MSG("empty argument");
    } else if(lstat(filename, &infos) < 0) {
        ERROR_MSG("invalid filename: %s", strerror(errno));
    } else if(!S_ISREG(infos.st_mode)) {
        ERROR_MSG("%s is not a regular file", filename);
    } else {
        ret = true;
    }
    return ret;
}

#define S_WRITE (S_IWUSR | S_IWGRP | S_IWOTH)

static bool cliopt_validate_output(const char *filename) {
    bool ret = false;
    struct stat infos = {0};

    if(filename == NULL) {
        ERROR_MSG("missing argument");
    } else if(strlen(filename) == 0) {
        ERROR_MSG("empty argument");
    } else if(lstat(filename, &infos) < 0) {
        if(errno != ENOENT) {
            ERROR_MSG("invalid filename: %s", strerror(errno));
        } else {
            ret = true;
        }
    } else if(!S_ISREG(infos.st_mode)) {
        ERROR_MSG("%s is not a regular file", filename);
    } else if((infos.st_mode & S_WRITE) == 0) { // this is a crude access test
        ERROR_MSG("%s is not writeable", filename);
    } else {
        ret = true;
    }
    return ret;
}

typedef const char* String;

typedef struct {
    size_t capacity;
    size_t count;
    String *data;
} FilenameList;

static bool cliopt_add_input(FilenameList *list, const char *filename) {
    bool ret = cliopt_validate_input(filename);
    if(ret) {
        if(list->count >= list->capacity) {
            size_t n = list->capacity + 4U;
            String *tmp = (String*)realloc(list->data, n * sizeof(String));
            if(tmp == NULL) {
                ERROR_MSG("failed to expand filename list: %s", strerror(errno));
                ret = false;
            } else {
                for(size_t i=list->capacity; i<n; i++) {
                    tmp[i] = NULL;
                }
                list->data = tmp;
                list->capacity = n;
            }
        }
        if(ret) {
            list->data[list->count] = filename;
            list->count++;
        }
    }
    return ret;
}

bool cli_opt_get(CommandLineOptions *out, int argc, char** argv) {
    bool ret = true;

    cag_option_context context = {0};
    cag_option_init(&context, g_options, CAG_ARRAY_SIZE(g_options), argc, argv);

    cliopt_reset(out);

    FilenameList labels = {0};
    FilenameList comments = {0};

    char *end = NULL;

    while (ret && cag_option_fetch(&context)) {
        switch (cag_option_get_identifier(&context)) {
            case CLI_OPTION_ID_IRQ_DETECT:
                out->extract_irq = true;
                break;          
            case CLI_OPTION_ID_CD:
                out->cdrom = true;
                break;
            case CLI_OPTION_ID_MAIN_OUT:
                out->main_filename = cag_option_get_value(&context);
                ret = cliopt_validate_output(out->main_filename);
                break;
            case CLI_OPTION_ID_LABELS_IN:
                ret = cliopt_add_input(&labels, cag_option_get_value(&context));
                break;
            case CLI_OPTION_ID_LABELS_OUT:
                out->labels_out = cag_option_get_value(&context);
                ret = cliopt_validate_output(out->labels_out);
                break;
            case CLI_OPTION_ID_COMMENTS_IN:
                ret = cliopt_add_input(&comments, cag_option_get_value(&context));
                break;
            case CLI_OPTION_ID_ADDR_PRINT:
                out->address = true;
                break;
            case CLI_OPTION_ID_SECTOR_SIZE:
                errno = 0;
                out->sector_size = (int)strtoul(cag_option_get_value(&context), &end, 10);
                if(errno || (*end != '\0')) {
                    ERROR_MSG("invalid sector size");
                    ret = false;
                } else if((out->sector_size != 2048) && (out->sector_size != 2352)) {
                    ERROR_MSG("invalid sector size (must be 2048 or 2352).");
                    ret = false;
                }
                break;
            case CLI_OPTION_ID_HELP:
                printf("Usage: etripator [options] [--] <cfg.json> <in>\n");
                cag_option_print(g_options, CAG_ARRAY_SIZE(g_options), stdout);
                ret = false;
                break;
            case CLI_OPTION_ID_ERROR:
            default:
                cag_option_print_error(&context, stderr);
                ret = false;
                break;
        }
    }

    int index = cag_option_get_index(&context);
    int count = argc - index;

    if(count == 2) {
        out->cfg_filename = argv[index++];
        out->rom_filename = argv[index++];
    } else if((count == 1) && out->extract_irq) {        
        out->cfg_filename = NULL;
        out->rom_filename = argv[index++];
    } else {
        ret = false;
    }
    out->comments_in = comments.data;
    out->labels_in = labels.data;
    return ret;
}

void cli_opt_release(CommandLineOptions *options) {
    free(options->comments_in);
    free(options->labels_in);
    cliopt_reset(options);
}
