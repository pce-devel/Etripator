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
#include "options.h"

#include <argparse/argparse.h>

#include <message.h>

struct payload_t {
    size_t capacity;
    size_t size;
    const char ***array;
};

static int opt_callback(struct argparse *self, const struct argparse_option *options) {
    struct payload_t *payload = (struct payload_t*)options->data;
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
    (*payload->array)[last] = *(char**)options->value;
    return 1;
}

// Extract command line options
bool cli_opt_get(CommandLineOptions *options, int argc, const char** argv) {
    static const char *const usages[] = {
        "etripator [options] [--] <cfg.json> <in>",
        NULL
    };

    bool ret = false;

    char *dummy;
    struct payload_t labels_payload = { 0, 0, &options->labels_in };
    struct payload_t comments_payload = {0, 0, &options->comments_in};

    struct argparse_option arg_opt[] = {
        OPT_HELP(),
        OPT_BOOLEAN('i', "irq-detect", &options->extract_irq, "automatically detect and extract irq vectors when disassembling a ROM, or extract opening code and gfx from CDROM IPL data", NULL, 0, 0),
        OPT_BOOLEAN('c', "cd", &options->cdrom, "cdrom image disassembly. Irq detection and rom. Header jump is not performed", NULL, 0, 0),
        OPT_STRING('o', "out", &options->main_filename, "main asm file containing includes for all sections as long the irq vector table if the irq-detect option is enabled", NULL, 0, 0),
        OPT_STRING('l', "labels", &dummy, "labels definition filename", opt_callback, (intptr_t)&labels_payload, 0),
        OPT_STRING(0, "labels-out", &options->labels_out, "extracted labels output filename. Otherwise the labels will be written to <in>.YYMMDDhhmmss.lbl", NULL, 0, 0),
        OPT_STRING(0, "comments", &dummy, "comments description filename", opt_callback, (intptr_t)&comments_payload, 0),
        OPT_BOOLEAN(0, "address", &options->address, "print statement address as comment", NULL, 0, 0),
        OPT_INTEGER(0, "sector_size", &options->sector_size, "2352 bytes sectors (cd only)", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;

    options->extract_irq = 0;
    options->cdrom = 0;
    options->cfg_filename  = NULL;
    options->rom_filename  = NULL;
    options->main_filename = "main.asm";
    options->labels_in = NULL;
    options->labels_out = NULL;
    options->comments_in = NULL;
    options->address = 0;
    options->sector_size = 2048;

    argparse_init(&argparse, arg_opt, usages, 0);
    argparse_describe(&argparse, "\nEtripator : a PC Engine disassembler", "  ");
    argc = argparse_parse(&argparse, argc, argv);
    if(!argc) {
        // ...
    } else if((options->sector_size != 2048) && (options->sector_size != 2352)) {
       ERROR_MSG("invalid sector size (must be 2048 or 2352).");
    } else if(argc != 2) {
        if((options->extract_irq) && (argc == 1)) {
            /* Config file is optional with automatic irq vector extraction. */
            options->cfg_filename =  NULL;
            options->rom_filename = argv[0];
            ret = true;
        }
        else {
            // ...
        }
    }
    else {
        options->cfg_filename = argv[0];
        options->rom_filename = argv[1];
        ret = true;
    }

    if(!ret) {
        argparse_usage(&argparse);
    }

    return ret;
}

// Release allocated resources during command line parsing
void cli_opt_release(CommandLineOptions *options) {
    if(options->comments_in) {
        free(options->comments_in);
    }
    if(options->labels_in) {
        free(options->labels_in);
    }
    memset(options, 0, sizeof(CommandLineOptions));
}
