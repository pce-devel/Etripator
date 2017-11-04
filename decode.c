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
#include "decode.h"
#include "message.h"
#include "opcodes.h"

/**
 * Initialize section processor
 * \return 1 upon success, 0 otherwise.
 */
int initializeSectionProcessor(SectionProcessor *processor) {
    processor->processed = NULL;
    processor->memmap = NULL;
    processor->out = NULL;

    processor->logical = 0;

    processor->offset = 0;

    processor->buffer = NULL;

    processor->labelRepository = createLabelRepository();
    if (NULL == processor->labelRepository) {
        return 0;
    }
    return 1;
}

/*
 * Reset section processor
 */
void resetSectionProcessor(MemoryMap *memmap, FILE *out, Section *current, SectionProcessor *processor) {
    processor->processed = current;
    processor->memmap = memmap;
    processor->out = out;
    processor->offset = 0;
    processor->logical = current->org;
    current->mpr[(current->org >> 13) & 0x07] = current->bank;
    current->offset = 0;

    updateMPRs(memmap, current->mpr);
}

/**
 * Delete section processor
 */
void deleteSectionProcessor(SectionProcessor *processor) {
    if (NULL != processor->buffer) {
        free(processor->buffer);
        processor->buffer = NULL;
    }
    if (NULL != processor->labelRepository) {
        deleteLabelRepository(processor->labelRepository);
        free(processor->labelRepository);
        processor->labelRepository = NULL;
    }
}

/*
 * Process data section
 */
int processDataSection(SectionProcessor *processor) {
    uint8_t data;
    int i;
    uint16_t addr;
    if (IncData == processor->processed->type) {
        int j;
        fprintf(processor->out, "%s:\n", processor->processed->name);
        for (i = processor->processed->size, addr = processor->logical; i > 0;) {
            fprintf(processor->out, "    .db ");

            for (j = 0; (i > 0) && (j < 8); j++, i--) {
                data = readByte(processor->memmap, addr++);
                fprintf(processor->out, "$%02x%c", data, ((j < 7) && (i > 1)) ? ',' : '\n');
            }
        }
    } else {
        for (i = processor->processed->size, addr = processor->logical; i > 0; i--) {
            data = readByte(processor->memmap, addr++);
            fwrite(&data, 1, 1, processor->out);
        }
    }
    return 1;
}

/**
 * Parse section to identify potential labels
 * \return 1 upon success, 0 if an error occured.
 */
int extractLabels(SectionProcessor *processor) {
    int eor, i;
    uint8_t inst;
    uint8_t data[6];
    char buffer[32];

    int offset;
    uint16_t logical;
    uint8_t page;

    LabelRepository *repository = processor->labelRepository;
    Section *section = processor->processed;

    if (Code != section->type) {
        return 1;
    }

    offset = 0;
    logical = processor->logical;

    INFO_MSG("%s:", section->name);

    /* Push the section org */
    page = getPage(processor->memmap, logical);
    if (0 == addLabel(repository, section->name, logical, page)) {
        return 0;
    }
    /* Walk along section */
    eor = 0;
    while (!eor) {
        /* Read instruction */
        inst = readByte(processor->memmap, logical);
        /* Read data (if any) */
        for (i = 0; i < (pce_opcode[inst].size - 1); i++) {
            data[i] = readByte(processor->memmap, logical + i + 1);
        }

        if (isLocalJump(inst)) {
            uint16_t jump;
            int delta;
            /* For BBR* and BBS* displacement is stored in the 2nd byte */
            i = (((inst)&0x0F) == 0x0F) ? 1 : 0;
            /* Detect negative number */
            if (data[i] & 128) {
                delta = -((data[i] - 1) ^ 0xff);
            } else {
                delta = data[i];
            }
            delta += pce_opcode[inst].size;
            jump = logical + delta;
            page = getPage(processor->memmap, jump);
            /* Create label name */
            snprintf(buffer, 32, "l%04x_%02d", jump, page);

            /* Insert offset to repository */
            if (0 == addLabel(repository, buffer, jump, page)) {
                return 0;
            }

            INFO_MSG("%04x short jump to %04x (%02x)", logical, jump, page);
        } else {
            if (isFarJump(inst)) {
                uint16_t jump = data[0] | (data[1] << 8);
                page = getPage(processor->memmap, jump);
                /* Create label name */
                snprintf(buffer, 32, "l%04x_%02d", jump, page);
                /* Insert offset to repository */
                if (0 == addLabel(repository, buffer, jump, page)) {
                    return 0;
                }

                INFO_MSG("%04x long jump to %04x (%02x)", logical, jump, page);
            }
        }

        logical += pce_opcode[inst].size;
        offset += pce_opcode[inst].size;

        /* Search end of section */
        if (section->size == 0) {
            if ((inst == 0x40) || (inst == 0x60)) {
                section->size = offset;
                eor = 1;
            }
        } else if (offset >= section->size) {
            eor = 1;
        }
    }
    return 1;
}

static const char *spacing = "          ";

/*
 * Process opcode
 */
char processOpcode(SectionProcessor *processor) {
    int i, delta;
    uint8_t inst, data[6], isJump;
    char eor, *name;
    uint8_t page;
    uint32_t offset;
    uint16_t logical, nextLogical;

    eor = 0;

    memset(data, 0, 6);
    logical = processor->logical + processor->offset;
    page = getPage(processor->memmap, logical); 

    /* Opcode */
    inst = readByte(processor->memmap, logical);

    nextLogical = logical + pce_opcode[inst].size;

    /* Is there a label ? */
    if (findLabel(processor->labelRepository, logical, page, &name)) {
        /* Print label*/
        fprintf(processor->out, "%s:\n", name);
    }

    /* Front spacing */
    fwrite(spacing, 1, 10, processor->out);

    /* Print opcode sting */
    fwrite(pce_opcode[inst].name, 1, 4, processor->out);

    /* Add spacing */
    fwrite(spacing, 1, 4, processor->out);

    /* End Of Routine (eor) is set to 1 if the instruction is RTI or RTS */
    eor = ((inst == 64) || (inst == 96));

    /* Data */
    if (pce_opcode[inst].size > 1) {
        for (i = 0; i < (pce_opcode[inst].size - 1); i++) {
            data[i] = readByte(processor->memmap, logical + i + 1);
        }
    }

    processor->offset += pce_opcode[inst].size;
    logical = nextLogical;

    /* Swap LSB and MSB for words */
    if (pce_opcode[inst].size > 2) {
        uint8_t swap;
        /* Special case : TST, BBR*, BBS*. The first byte is zp var */
        i = ((pce_opcode[inst].type == 3) || (pce_opcode[inst].type == 4) || (pce_opcode[inst].type == 5) ||
             (pce_opcode[inst].type == 6) || (pce_opcode[inst].type == 13) || (pce_opcode[inst].type == 20))
                ? 1
                : 0;

        for (; i < (pce_opcode[inst].size - 2); i += 2) {
            swap = data[i];
            data[i] = data[i + 1];
            data[i + 1] = swap;
        }
    }

    /* Handle special cases (jumps, tam/tma and unsupported opcodes ) */
    isJump = 0;
    if (isLocalJump(inst)) {
        isJump = 1;

        i = (((inst)&0x0F) == 0x0F) ? 1 : 0;

        /* Detect negative number */
        if (data[i] & 128)
            delta = -((data[i] - 1) ^ 0xff);
        else
            delta = data[i];

        offset = logical + delta;
        data[i] = offset >> 8;
        data[i + 1] = offset & 0xff;
    } else if (isFarJump(inst)) {
        isJump = 1;
        /* Build offset */
        offset = (data[0] << 8) | data[1];
    } else {
        offset = 0;
        /* Unknown instructions are output as raw data
         * (ie) .db XX
         */
        if (pce_opcode[inst].type == 22) {
            data[0] = inst;
        }
    }

    if (pce_opcode[inst].type == 1) {
        fputc('A', processor->out);
    } else if (isJump) {
        /* BBR* and BBS* */
        if ((inst & 0x0F) == 0x0F) {
            uint16_t zp_offset = 0x2000 + data[0];
            page = getPage(processor->memmap, zp_offset);
            if (findLabel(processor->labelRepository, zp_offset, page, &name)) {
                fprintf(processor->out, "<%s, ", name);
            } else {
                fprintf(processor->out, "<$%02x, ", data[0]);
            }
        }
        page = getPage(processor->memmap, offset);
        findLabel(processor->labelRepository, offset, page, &name);
        fwrite(name, 1, strlen(name), processor->out);
    } else {
        int hasLabel = 0;
        if ((inst == 0x43) || (inst == 0x53)) {
            /* tam and tma */
            /* Compute log base 2 of data */
            for (i = 0; (i < 8) && ((data[0] & 1) == 0); ++i, data[0] >>= 1) {
            }
            data[0] = i;
        }

        /* Print data */
        if (pce_opcode[inst].type) {
            /* [todo] this is ugly... */
            const char *none = "";
            const char *extra = none;
            switch (pce_opcode[inst].type) {
            case 4: /* #$aa, <$zp, X */
                extra = ", X";
            case 3: /* #$aa, <$zp */
                offset = 0x2000 + data[1];
                page = getPage(processor->memmap, offset);
                hasLabel = findLabel(processor->labelRepository, offset, page, &name);
                if (hasLabel) {
                    fprintf(processor->out, "#$%02x, <%s%s", data[0], name, extra);
                }
                break;
            case 6: /* #$aa, $hhll, X */
                extra = ", X";
            case 5: /* #$aa, $hhll */
                offset = (data[0] << 8) + data[1];
                page = getPage(processor->memmap, offset);
                hasLabel = findLabel(processor->labelRepository, offset, page, &name);
                if (hasLabel) {
                    fprintf(processor->out, "#$%02x, %s%s", data[0], name, extra);
                }
                break;

            case 9: /* <zp, Y */
                extra = ", Y";
            case 8: /* <zp, X */
                if (none == extra) {
                    extra = ", X";
                }
            case 7: /* <zp    */
                offset = 0x2000 + data[0];
                page = getPage(processor->memmap, offset);
                hasLabel = findLabel(processor->labelRepository, offset, page, &name);
                if (hasLabel) {
                    fprintf(processor->out, "<%s%s", name, extra);
                }
                break;

            case 12: /* [zp], Y */
                extra = "], Y";
            case 11: /* [zp, X] */
                if (none == extra) {
                    extra = ", X]";
                }
            case 10: /* [zp] */
                if (none == extra) {
                    extra = "]";
                }
                offset = 0x2000 + data[0];
                page = getPage(processor->memmap, offset);
                hasLabel = findLabel(processor->labelRepository, offset, page, &name);
                if (hasLabel) {
                    fprintf(processor->out, "[%s%s", name, extra);
                }
                break;

            case 21: /* [hhll, X] */
                extra = ", X]";
            case 15: /* [hhll] */
                if (none == extra) {
                    extra = "]";
                }
                offset = (data[0] << 8) | data[1];
                page = getPage(processor->memmap, offset);
                hasLabel = findLabel(processor->labelRepository, offset, page, &name);
                if (hasLabel) {
                    fprintf(processor->out, "[%s%s", name, extra);
                }
                break;

            case 16: /* hhll, X */
                extra = ", X";
            case 17: /* hhll, Y */
                if (none == extra) {
                    extra = ", Y";
                }
            case 14: /* hhll */
                offset = (data[0] << 8) | data[1];
                page = getPage(processor->memmap, offset);
                hasLabel = findLabel(processor->labelRepository, offset, page, &name);
                if (hasLabel) {
                    fprintf(processor->out, "%s%s", name, extra);
                }
                break;

            case 13: /* <zp, $hhll */
                offset = 0x2000 + data[0];
                page = getPage(processor->memmap, offset);
                hasLabel = findLabel(processor->labelRepository, offset, page, &name);
                if (hasLabel) {
                    fprintf(processor->out, "<%s, ", name);
                } else {
                    fprintf(processor->out, "<%02x, ", data[0]);
                }
                offset = (data[1] << 8) | data[2];
                page = getPage(processor->memmap, offset);
                hasLabel = findLabel(processor->labelRepository, offset, page, &name);
                if (hasLabel) {
                    fprintf(processor->out, "%s", name);
                } else {
                    fprintf(processor->out, "$%04x", offset);
                }
                hasLabel = 1;
                break;

            case 18: /* shsl, dhdl, sz */
                /* Source and destination */
                for (i = 0; i < 4; i += 2) {
                    offset = (data[i] << 8) | data[i + 1];
                    page = getPage(processor->memmap, offset);
                    hasLabel = findLabel(processor->labelRepository, offset, page, &name);
                    if (hasLabel) {
                        fprintf(processor->out, "%s, ", name);
                    } else {
                        fprintf(processor->out, "$%04x, ", offset);
                    }
                }
                /* Size */
                fprintf(processor->out, "$%02x%02x", data[4], data[5]);
                hasLabel = 1;
                break;

            default:
                hasLabel = 0;
                break;
            }

            if (!hasLabel) {
                for (i = 0; pce_opstring[pce_opcode[inst].type][i] != NULL; ++i) {
                    fprintf(processor->out, pce_opstring[pce_opcode[inst].type][i], data[i]);
                }
            }
        }
    }
    fputc('\n', processor->out);
    return eor;
}
