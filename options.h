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
	int useOldCSV;
};
typedef struct CommandLineOptions_ CommandLineOptions;

/* Print usage */
void usage();

/* Extract command line options */
int getCommandLineOptions(int, char**, CommandLineOptions*);

#endif // OPTIONS_H
