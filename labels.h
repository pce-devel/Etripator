#ifndef _LABELS_H_
#define _LABELS_H_

#include <stdio.h>

#include "section.h"
#include "opcodes.h"
#include "rbtree.h"

/* Is the instruction a local jump ? */
#define isLocalJump(i) \
( \
	(((i) & 0x0F) == 0x0F) || /* BBR* and BBS* */ \
	((i)          == 0x90) || /* BCC           */ \
	((i)          == 0xB0) || /* BCS           */ \
	((i)          == 0x80) || /* BRA           */ \
	((i)          == 0xF0) || /* BEQ           */ \
	((i)          == 0x30) || /* BMI           */ \
	((i)          == 0xD0) || /* BNE           */ \
	((i)          == 0x10) || /* BPL           */ \
	((i)          == 0x44) || /* BSR           */ \
	((i)          == 0x50) || /* BVC           */ \
	((i)          == 0x70)    /* BVS           */ \
)

/* Is the instruction a "far" jump ? */
#define isFarJump(i) \
( \
	((i) == 0x4C) || /* JMP */ \
	((i) == 0x20)    /* JSR */ \
)

/* Label */
struct Label_
{
	uint16_t	offset;	      /* Label offset */
	uint8_t		displacement; /* Displacement to label offset if it's 
				       * centered on an instruciton */
};
typedef struct Label_ Label;

/* Label repository structure 
 * 		1 per bank
 * 		There's at max 8k labels per bank
 */
struct LabelRepository_
{
	uint16_t	size;	      /* Size of label repository */
	uint16_t	last;	      /* Last element in the repository */
	Label		*labels;      /* Sorted labels */
};
typedef struct LabelRepository_ LabelRepository;

/* Initialize label repository */
int initializeLabelRepository(LabelRepository*);

/* Reset label repsitory */
void resetLabelRepository(LabelRepository*);

/* Delete label repository */
void deleteLabelRepository(LabelRepository*);

/* Push label to repository */
int pushLabel(LabelRepository*, uint16_t);

/* Finalize label repository */
void finalizeLabelRepositoty(LabelRepository*);

/* Parse section to identify potential labels */
int getLabels(FILE*, Section*, LabelRepository*);

/* Initialize label index so that it points to the label close to current org offset */
void getLabelIndex(LabelRepository*, SectionProcessor*);

#endif /* _LABELS_H_ */
