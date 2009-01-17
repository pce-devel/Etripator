#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdint.h>

/* Command line options */
struct CommandLineOptions_
{
	uint8_t extractIRQ;
	uint8_t cdrom;
	char *csvFileName;
	char *romFileName;
	char *mainFileName;
};
typedef struct CommandLineOptions_ CommandLineOptions;

/* Print usage */
void usage();

/* Extract command line options */
int getCommandLineOptions(int, char**, CommandLineOptions*);

#endif /* _OPTIONS_H_ */
