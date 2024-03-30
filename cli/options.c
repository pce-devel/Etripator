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
  copyright (c) 2009--2023 Vincent Cruz.
 
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

#include <argparse/argparse.h>

#include <message.h>

struct payload_t {
    size_t capacity;
    size_t size;
    const char ***array;
};

static int opt_callback(struct argparse *self, const struct argparse_option *option) {
    struct payload_t *payload = (struct payload_t*)option->data;
    size_t last = payload->size++;
    if(payload->capacity <= payload->size) {
        payload->capacity += 4;
        const char **tmp = (const char**)realloc(*payload->array, payload->capacity*sizeof(const char*));
        if(tmp == NULL) {
            return 0;
        }
        memset(tmp+last, 0, 4*sizeof(const char*));
        *payload->array = tmp;
    }
    (*payload->array)[last] = *(char**)option->value;
    return 1;
}

/* Extract command line options */
int get_cli_opt(int argc, const char** argv, cli_opt_t* option) {
    static const char *const usages[] = {
        "etripator [options] [--] <cfg.json> <in>",
        NULL
    };

    int ret = 0;

    char *dummy;
    struct payload_t labels_payload = { 0, 0, &option->labels_in };
    struct payload_t comments_payload = {0, 0, &option->comments_in};

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('i', "irq-detect", &option->extract_irq, "automatically detect and extract irq vectors when disassembling a ROM, or extract opening code and gfx from CDROM IPL data", NULL, 0, 0),
        OPT_BOOLEAN('c', "cd", &option->cdrom, "cdrom image disassembly. Irq detection and rom. Header jump is not performed", NULL, 0, 0),
        OPT_STRING('o', "out", &option->main_filename, "main asm file containing includes for all sections as long the irq vector table if the irq-detect option is enabled", NULL, 0, 0),
        OPT_STRING('l', "labels", &dummy, "labels definition filename", opt_callback, (intptr_t)&labels_payload, 0),
        OPT_STRING(0, "labels-out", &option->labels_out, "extracted labels output filename. Otherwise the labels will be written to <in>.YYMMDDhhmmss.lbl", NULL, 0, 0),
        OPT_STRING(0, "comments", &dummy, "comments description filename", opt_callback, (intptr_t)&comments_payload, 0),
        OPT_BOOLEAN(0, "address", &option->address, "print statement address as comment", NULL, 0, 0),
        OPT_INTEGER(0, "sector_size", &option->sector_size, "2352 bytes sectors (cd only)", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;

    option->extract_irq = 0;
    option->cdrom = 0;
    option->cfg_filename  = NULL;
    option->rom_filename  = NULL;
    option->main_filename = "main.asm";
    option->labels_in = NULL;
    option->labels_out = NULL;
    option->comments_in = NULL;
    option->address = 0;
    option->sector_size = 2048;

    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nEtripator : a PC Engine disassembler", "  ");
    argc = argparse_parse(&argparse, argc, argv);
    if(!argc) {
        // ...
    } else if((option->sector_size != 2048) && (option->sector_size != 2352)) {
       ERROR_MSG("invalid sector size (must be 2048 or 2352).");
    } else if(argc != 2) {
        if((option->extract_irq) && (argc == 1)) {
            /* Config file is optional with automatic irq vector extraction. */
            option->cfg_filename =  NULL;
            option->rom_filename = argv[0];
            ret = 1;
        }
        else {
            // ...
        }
    }
    else {
        option->cfg_filename = argv[0];
        option->rom_filename = argv[1];
        ret = 1;
    }

    if(ret == 0) {
        argparse_usage(&argparse);
    }

    return ret;
}

/* Release allocated resources during command line parsing */
void release_cli_opt(cli_opt_t* option) {
    if(option->comments_in) {
        free(option->comments_in);
    }
    if(option->labels_in) {
        free(option->labels_in);
    }
    memset(option, 0, sizeof(cli_opt_t));
}
