#ifndef IRQ_H
#define IRQ_H

#include "config.h"
#include "section.h"

/* Get irq code offsets from rom */
int getIRQSections(FILE*, Section*);

#endif /* IRQ_H */
