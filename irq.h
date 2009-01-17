#ifndef _IRQ_H_
#define _IRQ_H_

#include <stdio.h>
#include "section.h"

/* Get irq code offsets from rom */
int getIRQSections(FILE*, Section*);

#endif /* _IRQ_H_ */
