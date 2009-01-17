#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "options.h"

/* Print usage */
void usage()
{
	fprintf(stderr, "Usage : etripator [options] <csv> <in>\n"
			"--irq-detect or -i	: automatically detect and extract irq vectors\n"
			"--cd or -c		: cdrom image disassembly, irq detection is not performed\n"
			"--help or -h		: displays this message\n"
			"--out or -o <file>     : main asm file containing includes for all sections \n"
			"                         as long the irq vector table if the irq-detect option\n"
			"                         is enable\n");
}

/* Extract command line options */
int getCommandLineOptions(int argc, char** argv, CommandLineOptions* iOptions)
{
	char *shortOptions = "icho:";
	struct option longOptions[5] = {
		{"irq-detect", 0, 0, 'i'},
		{"cd",	       0, 0, 'c'},
		{"help",       0, 0, 'h'},
		{"out",        1, 0, 'o'},
		{ 0,           0, 0,  0 }
	};
	int idx, opt;

    /* Reset options */
	iOptions->extractIRQ   = 0;
	iOptions->cdrom        = 0;
	iOptions->csvFileName  = NULL;
	iOptions->romFileName  = NULL;
	iOptions->mainFileName = "main.asm";

	/* Note : IRQ detection is disabled with the cdrom option */
	while ((opt = getopt_long (argc, argv, shortOptions, longOptions, &idx)) > 0)
	{
		switch(opt)
		{
			case 'i':
				if(!iOptions->cdrom)
					iOptions->extractIRQ = 1;
				break;

			case 'c':
				iOptions->cdrom      = 1;
				iOptions->extractIRQ = 0;
				break;

			case 'o':
				iOptions->mainFileName = optarg;
				break;

			case 'h':
				return 0;

			default:
				return -1;
		}		
	}

	if((argc - optind) != 2)
	{
		iOptions->csvFileName  = iOptions->romFileName = NULL;
		return -1;
	}

	iOptions->csvFileName = argv[optind];
	iOptions->romFileName = argv[optind+1];

	return 1;
}
