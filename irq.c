#include "irq.h"

static char* IRQNames[5] = {
	"irq_2.asm",
	"irq_1.asm",
	"irq_timer.asm",
	"irq_nmi.asm",
	"irq_reset.asm"
};

/* Get irq code offsets from rom */
int getIRQSections(FILE* iRomFile, Section* iSectionArray)
{
	int err, i;
	size_t nRead, size;
	uint8_t offset[2];
	unsigned int headerJump = 0;
	
	/* Header ? */
	fseek(iRomFile, 0, SEEK_END);
	size  = ftell(iRomFile);
	fseek(iRomFile, 0, SEEK_SET);
	size -= ftell(iRomFile);
	
	headerJump = size & 0x1fff;
	
	/* Jump to irq offsets */
	err = fseek(iRomFile, headerJump + 0x1ff6, SEEK_SET);
	if(err < 0)
		return 0;

	for(i=0; i<5; ++i) {
		/* Read offset */
		nRead = fread(offset, 1, 2, iRomFile);
		if(nRead != 2)
			return 0;

		/* Initialize section */
		iSectionArray[i].name  = IRQNames[i];
		iSectionArray[i].type  = CODE;
		iSectionArray[i].bank  = 0;
		iSectionArray[i].org   = (offset[1] << 8) | offset[0];
		iSectionArray[i].type  = CODE;
		iSectionArray[i].start = ((offset[1] & 0x1f) << 8) | offset[0];
		iSectionArray[i].size  = -1;
		iSectionArray[i].id    = i;

		printf("%s found at %04x (%lx)\n", iSectionArray[i].name, iSectionArray[i].org, iSectionArray[i].start);
	}
	
	return 1;
}
