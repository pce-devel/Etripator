/*
  Etripator: PCE disassembler
  Author   : Vincent Cruz
  Date     : 2004-End of the world
  [include licence here]
  i'm not responsible of any damage this program wil cause.
  use it on your own risk. blablabla...
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv.h"
#include "section.h"
#include "opcodes.h"
#include "irq.h"
#include "options.h"

/*
  exit callback
 */
void exit_callback()
{}

/* Main */
int main(int argc, char** argv)
{
  FILE* out;
  FILE* in;
  FILE* mainFile;
  
  char eor;
  unsigned int size;
  unsigned int i;
  int err, failure;

  CommandLineOptions cmdOptions;

  Section* section;
  size_t sectionCount;

  SectionProcessor processor;
 
  /* Extract command line options */
  err = getCommandLineOptions(argc, argv, &cmdOptions);
  if(err <= 0)
  {
	usage();
	return (err < 0) ? 1 : 0;
  }

  failure = 1;

  /* Read cfg file */
  sectionCount = cmdOptions.extractIRQ ? 5 : 0;
  section      = NULL;
  
  if(cmdOptions.useOldCSV)
  {
	  err = readSectionsFromCSV(cmdOptions.cfgFileName, ';', &section, &sectionCount);
  }
  else
  {
	  err = readSectionsFromCFG(cmdOptions.cfgFileName, &section, &sectionCount);
  }
  
  if(!err)
  {
  	fprintf(stderr,"Unable to read %s.\n", cmdOptions.cfgFileName);
  	goto error_1;
  }

  /* Open rom */
  in = fopen(cmdOptions.romFileName, "rb");
  if(in == NULL)
  {
	fprintf(stderr,"Can't open %s\n", cmdOptions.romFileName);
	goto error_1;
  }  

  fseek(in, 0, SEEK_END);

  /* Get file size */
  size  = ftell(in);
  fseek(in, 0, SEEK_SET);
  size -= ftell(in);

  /* Get irq offsets */
  if(cmdOptions.extractIRQ)
  {
  	if(getIRQSections(in, section) == 0)
  	{
		fprintf(stderr, "An error occured while reading irq vector offsets\n");
		goto error_2;
  	}
  }

  if(!cmdOptions.cdrom)
  {
  	/* Adjust file start to jump header (on hucard only) */
  	off_t headerJump = size & 0x1fff;
  	size &= ~0x1fff;
  	
  	for(i=0; i<sectionCount; ++i)
  	{
  		section[i].start += headerJump;
  	}
  }
  
  /* Initialize section processor */
  initializeSectionProcessor(&processor);

  /* Disassemble and output */
  for(i=0; i<sectionCount; ++i)
  {
	out = fopen(section[i].name, "wb");
	if(out == NULL)
	{
		fprintf(stderr,"Can't open %s\n", section[i].name);
		goto error_4;
	}

	if(section[i].type != BIN_DATA)
	{
		/* Print header */
		fprintf(out, "\t.%s\n"
					 "\t.bank %x\n"
					 "\t.org $%04x\n",
					 (section[i].type == CODE) ? "code" : "data", section[i].bank,
					 section[i].org);
	}

	fseek(in, section[i].start, SEEK_SET);
	eor = 0;

    /* Reset section processor */
    resetSectionProcessor(in, out, i, section+i, &processor);
	
	if(section[i].type == CODE)
	{
		/* Fetch labels */
		if(!getLabels(&processor))
		{
			goto error_4;
		}

		fseek(in, section[i].start, SEEK_SET);

		/* Process opcodes */
		while(!eor) {
			eor = processOpcode(&processor);

			if(!cmdOptions.extractIRQ)
			{
				if((processor.fileOffset - processor.processed->start) >= processor.processed->size)
					eor = 1;
				else
					eor = 0;
			}
		}
	}
	else {
		err = processDataSection(&processor);
	}
	
	fclose(out);
	out = NULL;
  }

  fclose(in);
  in = NULL;
  
  /* Open main asm file */
  mainFile = fopen(cmdOptions.mainFileName, "w");
  if(mainFile == NULL)
  {
  	fprintf(stderr, "Unable to open %s\n", cmdOptions.mainFileName);
  	goto error_4;
  }
  
  for(i=0; i<sectionCount; ++i)
  {
	  switch(section[i].type)
	  {
		case CODE:
		case INC_DATA:
			fprintf(mainFile, "\n\t.include \"%s\"\n", section[i].name);
			break;
			
		case BIN_DATA:
			fprintf(mainFile, "\n\t.data\n\t.bank %x\n\t.org $%04x\n\t.incbin \"%s\"\n",
					section[i].bank,
					section[i].org,
					section[i].name);
			break;
	  };
  }
  	
  if(cmdOptions.extractIRQ)
  {
		fprintf(mainFile, "\n\t.data\n\t.bank 0\n\t.org $FFF6\n");
		for(i=0; i<5; ++i)
		{
			fprintf(mainFile, "\t.dw $%04x\n", section[i].org);
		}
  }
  	
  fclose(mainFile);
  failure = 0;
  
error_4:
  deleteSectionProcessor(&processor);

error_2:
  if(in != NULL)
	fclose(in);

error_1:
  i = cmdOptions.extractIRQ ? 5 : 0;
  for(; i<sectionCount; ++i)
  {
  	free(section[i].name);
  	section[i].name = NULL;
  }
  free(section);

  return failure;
}
