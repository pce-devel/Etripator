/*
    This file is part of Etripator,
    copyright (c) 2009--2021 Vincent Cruz.

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
#ifndef ETRIPATOR_OPTIONS_H
#define ETRIPATOR_OPTIONS_H

#include <config.h>

/* Command line options */
typedef struct {
    int extract_irq;
    int cdrom;
    const char *cfg_filename;
    const char *rom_filename;
    const char *main_filename;
    const char *labels_out;
    const char **labels_in;
    const char **comments_in;
} cli_opt_t;

/* Extract command line options */
int get_cli_opt(int argc, const char** argv, cli_opt_t* option);

/* Release allocated resources during command line parsing */
void release_cli_opt(cli_opt_t* option);

#endif // ETRIPATOR_OPTIONS_H
