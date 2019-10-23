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
#include "options.h"

#include <argparse/argparse.h>

struct labels_in_payload {
    size_t capacity;
    size_t size;
    const char ***array;
};

static int labels_opt_callback(struct argparse *self, const struct argparse_option *option) {
    struct labels_in_payload *payload = (struct labels_in_payload*)option->data;
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

    char *dummy;
    struct labels_in_payload payload = { 0, 0, &option->labels_in };

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('i', "irq-detect", &option->extract_irq, "automatically detect and extract irq vectors when disassembling a ROM, or extract opening code and gfx from CDROM IPL data", NULL, 0, 0),
        OPT_BOOLEAN('c', "cd", &option->cdrom, "cdrom image disassembly. Irq detection and rom. Header jump is not performed", NULL, 0, 0),
        OPT_STRING('o', "out", &option->main_filename, "main asm file containing includes for all sections as long the irq vector table if the irq-detect option is enabled", NULL, 0, 0),
        OPT_STRING('l', "labels", &dummy, "labels definition filename", labels_opt_callback, (intptr_t)&payload, 0),
        OPT_STRING(0, "labels-out", &option->labels_out, "extracted labels output filename. Otherwise the labels will be written to <in>.YYMMDDhhmmss.lbl", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;

    option->extract_irq = 0;
    option->cdrom = 0;
    option->cfg_filename  = NULL;
    option->rom_filename  = NULL;
    option->main_filename = "main.asm";
    option->labels_in = NULL;

    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nEtripator : a PC Engine disassembler", "  ");
    argc = argparse_parse(&argparse, argc, argv);
    if(!argc) {
        argparse_usage(&argparse);
        return 0;
    }
    if(argc != 2) {
        if((option->extract_irq) && (argc == 1)) {
            /* Config file is optional with automatic irq vector extraction. */
            option->cfg_filename =  NULL;
            option->rom_filename = argv[0];
        }
        else {
            argparse_usage(&argparse);
            return 0;
        }
    }
    else {
        option->cfg_filename = argv[0];
        option->rom_filename = argv[1];
    }
    return 1;
}
