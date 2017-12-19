/*
    This file is part of Etripator,
    copyright (c) 2009--2017 Vincent Cruz.

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
#include "section.h"
#include <stdlib.h>

/**
 * Reset a section to its default values.
 **/
void resetSection(Section *out) {
    out->type = UnknownSectionType;
    out->name = NULL;
    out->bank = 0;
    out->org = 0;
    out->offset = 0;
    out->size = 0;
    out->filename = NULL;
    memset(out->mpr, 0, 8);
}

static int compareOutputFilename(const void *a, const void *b) {
    Section* s0 = (Section*)a;
    Section* s1 = (Section*)b;
    return strcmp(s0->filename, s1->filename);
}

static int compareAddresses(const void *a, const void *b) {
    Section* s0 = (Section*)a;
    Section* s1 = (Section*)b;
    if(s0->bank < s1->bank) {
        return -1;
    }
    else if(s0->bank > s1->bank) {
        return 1;
    }
    else if(s0->org < s1->org) {
        return -1;
    }
    else if(s0->org > s1->org) {
        return 1;
    }
    else {
        return 0;
    }
}

void sortSections(Section *sections, int count) {
    int i, j;
    qsort(sections, count, sizeof(Section), &compareOutputFilename);
    for(i=0; i<count; i=j) {
        for(j=i+1; (j<count) && (!strcmp(sections[i].filename, sections[j].filename)); j++) {}
        if((j-i) > 1) {
            qsort(sections+i, j-i, sizeof(Section), &compareAddresses);
        }
    }
}