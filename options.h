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
#ifndef OPTIONS_H
#define OPTIONS_H

#include "config.h"

/* Command line options */
struct CommandLineOptions_
{
    uint8_t extractIRQ;
    uint8_t cdrom;
    char *cfgFileName;
    char *romFileName;
    char *mainFileName;
};
typedef struct CommandLineOptions_ CommandLineOptions;

/* Print usage */
void usage();

/* Extract command line options */
int getCommandLineOptions(int, char**, CommandLineOptions*);

#endif // OPTIONS_H
