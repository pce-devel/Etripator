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

/* Print usage */
void usage()
{
    fprintf(stderr, "Usage : etripator [options] <cfg> <in>\n"
            "--irq-detect or -i    : automatically detect and extract irq vectors when disassembling a ROM, or extract opening code and gfx from CDROM IPL data.\n"
            "--cd or -c            : cdrom image disassembly. Irq detection and rom\n"
            "                        header jump are not performed.\n"
            "--help or -h          : displays this message.\n"
            "--out or -o <file>    : main asm file containing includes for all sections \n"
            "                        as long the irq vector table if the irq-detect\n"
            "                        option is enabled.\n"
            "--labels or -l <file> : labels definition filename.\n"
            "--labels-out <file>   : extracted labels output filename. Otherwise the labels will be written to <in>.YYMMDDhhmmss.lbl.\n"
            "<cfg>                 : configuration file (optional with -i option).\n"
            "<in>                  : binary.\n");
}

/* Extract command line options */
int get_cli_opt(int argc, char** argv, cli_opt_t* option)
{
    char *short_opt = "icho:l:";
    struct option long_opt[] = {
        {"irq-detect", 0, 0, 'i'},
        {"cd",         0, 0, 'c'},
        {"help",       0, 0, 'h'},
        {"out",        1, 0, 'o'},
        {"labels",     1, 0, 'l'},
        {"labels-out", 1, 0,  1 },
        { 0,           0, 0,  0 }
    };
    int idx, opt;
    size_t top = 0;
    size_t count = 2;
    /* Reset options */
    option->extract_irq   = 0;
    option->cdrom         = 0;
    option->cfg_filename  = NULL;
    option->rom_filename  = NULL;
    option->main_filename = "main.asm";
    option->labels_out    = NULL;
    option->labels_in     = NULL;
    /* Note : IRQ detection is disabled with the cdrom option */
    while ((opt = getopt_long (argc, argv, short_opt, long_opt, &idx)) > 0) {
        switch(opt) {
            case 1:
                option->labels_out = optarg;
                break;
            case 'i':
                option->extract_irq = 1;
                break;
            case 'c':
                option->cdrom = 1;
                break;
            case 'o':
                option->main_filename = optarg;
                break;
            case 'l':
                if((top >= count) || (NULL == option->labels_in)) {
                    size_t capacity = count * 2;
                    char **tmp = (char**)realloc(option->labels_in, capacity * sizeof(char*));
                    if(NULL == tmp) {
                        return -1;
                    }
                    memset(tmp+top, 0, (capacity-count) * sizeof(char*));
                    option->labels_in = tmp;
                    count = capacity;
                }            
                option->labels_in[top++] = optarg;
                break;
            
            case 'h':
                return 0;

            default:
                return -1;
        }       
    }

    /* Retrieve config file and rom file. */
    if((argc - optind) != 2)
    {
        if((option->extract_irq) && ((argc - optind) == 1))
        {
            /* Config file is optional with automatic irq vector extraction. */
            option->cfg_filename =  NULL;
            option->rom_filename = argv[optind];
        }
        else
        {
            /* Not enough parameters!. */
            option->cfg_filename  = option->rom_filename = NULL;
            return -1;
        }
    }
    else
    {
        option->cfg_filename = argv[optind];
        option->rom_filename = argv[optind+1];
    }
    return 1;
}
