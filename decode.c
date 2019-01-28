/*
	This file is part of Etripator,
	copyright (c) 2009--2019 Vincent Cruz.

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


int label_extract(section_t *section, memmap_t *map, label_repository_t *repository) {
	int eor, i;
	uint8_t inst;
	uint8_t data[6];
	char buffer[32];

	int offset;
	uint16_t logical;
	uint8_t page;

    const opcode_t *opcode;

	if (section->type != Code) {
		return 1;
	}

	offset = 0;
    logical = section->logical;
    
	INFO_MSG("%s:", section->name);

	/* Walk along section */
	for(eor = 0; !eor; ) {
		/* Read instruction */
		inst = memmap_read(map, logical);
		opcode = opcode_get(inst);
		/* Read data (if any) */
		for (i = 0; i < (opcode->size - 1); i++) {
			data[i] = memmap_read(map, logical + i + 1);
		}

		if (opcode_is_local_jump(inst)) {
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
			delta += opcode->size;
			jump = logical + delta;
			page = memmap_page(map, jump);
			/* Create label name */
			snprintf(buffer, 32, "l%04x_%02d", jump, page);

			/* Insert offset to repository */
			if (!label_repository_add(repository, buffer, jump, page)) {
				return 0;
			}
			INFO_MSG("%04x short jump to %04x (%02x)", logical, jump, page);
		} else if (opcode_is_far_jump(inst)) {
				uint16_t jump = data[0] | (data[1] << 8);
				page = memmap_page(map, jump);
				/* Create label name */
				snprintf(buffer, 32, "l%04x_%02d", jump, page);
				/* Insert offset to repository */
				if (!label_repository_add(repository, buffer, jump, page)) {
					return 0;
				}

				INFO_MSG("%04x long jump to %04x (%02x)", logical, jump, page);
		}

		logical += opcode->size;

		/* Search end of section */
		if (section->size == 0) {
		    /* RTI, RTS, BRK */
			if ((inst == 0x40) || (inst == 0x60) || (inst == 0x00)) {
				section->size = offset;
				eor = 1;
			}
		} else if (offset >= section->size) {
			eor = 1;
		}
	}
	return 1;
}

static int data_extract_binary(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository) {
    uint16_t logical;
    int32_t i;
    for (i = section->size, logical=section->logical; i > 0; i--, logical++) {
        uint8_t data = memmap_read(map, logical++);
        fwrite(&data, 1, 1, out);
    }
    return 1;
}

static int data_extract_hex(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository) {
	int32_t i, j, k;
    uint16_t logical;
    uint8_t data[2];
    int32_t element_size = section->data.element_size;
	int32_t elements_per_line = section->data.elements_per_line;
    const char *data_decl = (element_size > 1) ? ".dw" : ".db";
    char *name = "";
    
    for(i=0, j=0, k=0, logical=section->logical; i<section->size; i++, logical++) {
        uint8_t page = memmap_page(map, logical);
        if(label_repository_find(repository, logical, page, &name)) {
            if(k && (k < element_size)) {
                fprintf(out, "\n          .db");
                for(j=0; j<k; j++) {
                    fprintf(out, "%c$%02x", j ? ',' : ' ', data[j]);
                }
                k = 0;
            }
            if(j) {
                fputc('\n', out);
            }
            fprintf(out, "%s:", name);
            j = 0;
        }
        data[k++] = memmap_read(map, logical);
        if(k >= element_size) {
            char c;
            if(j == 0) {
                fprintf(out, "\n          %s", data_decl);
                c = ' ';
            }
            else {
                c = ',';
            }
            fprintf(out, "%c$", c);
            while(k--) {
                fprintf(out, "%02x", data[k]);
            }
            k = 0;
            j = (j+1) % elements_per_line;
        }
    }
    fputc('\n', out);
    if(k) {
        fprintf(out, "          .db ");
        for(j=0; j<k; j++) {
            fprintf(out, "$%02x%c", data[j], ((j+1)<k) ? ',' : '\n');
        }
    }
    return 1;
}

static int data_extract_string(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository) {
	int32_t i, j, k;
    uint16_t logical;
	int32_t elements_per_line = section->data.elements_per_line;
    char *name = "";
    char c;
    
    for(i=0, j=0, k=0, c=0, logical=section->logical; i<section->size; i++, logical++) {
        uint8_t data;
        char c;
        uint8_t page = memmap_page(map, logical);
        if(label_repository_find(repository, logical, page, &name)) {
            if(j) {
                fputc('\n', out);
            }
            fprintf(out, "%s:", name);
            j = 0;
        }
        data = memmap_read(map, logical);
        if(j == 0) {
            fprintf(out, "\n          db");
            c = ' ';
        }
        else {
            c = ',';
        }
        if((data >= 0x20) && (data < 0x7f)) {
            if(c == 0) {
                fputc('"', out);
                c = 1;
            }
            if(data == '"') {
                fputc('\\', out);
            }
            fputc(data, out);
        }
        else {
            if(c) {
               fprintf(out, "\",");
               c = 0;
            }
            fprintf(out, "$%02x", data);
        }
        fputc(c, out);
        j = (j+1) % elements_per_line;
    }
    return 1;
}

int data_extract(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository) {
    switch(section->data.type) {
        case Binary:
            return data_extract_binary(out, section, map, repository);
        case Hex:
            return data_extract_hex(out, section, map, repository);
        case String:
            return data_extract_string(out, section, map, repository);
    }
    return 0;
}

static const char *spacing = "          ";

/**
 *
 */
int code_extract_single(FILE *out, uint16_t *logical, section_t *section, memmap_t *map, label_repository_t *repository) {
	int i, delta;
	uint8_t inst, data[6], is_jump;
	char eor, *name;
	uint8_t page;
	uint32_t offset;
	uint16_t next_logical;

    const opcode_t *opcode;

	eor = 0;

	memset(data, 0, 6);
	page = memmap_page(map, *logical); 

	/* Opcode */
	inst = memmap_read(map, *logical);
    opcode = opcode_get(inst);
    
	next_logical = *logical + opcode->size;

	/* Is there a label ? */
	if (label_repository_find(repository, *logical, page, &name)) {
		/* Print label*/
		fprintf(out, "%s:\n", name);
	}

	/* Front spacing */
	fwrite(spacing, 1, 10, out);

	/* Print opcode sting */
	fwrite(opcode->name, 1, 4, out);

	/* Add spacing */
	fwrite(spacing, 1, 4, out);

	/* End Of Routine (eor) is set to 1 if the instruction is RTI, RTS or BRK */
	eor = ((inst == 0x40) || (inst == 0x60) || (inst == 0x00));
	
	/* Data */
	if (opcode->size > 1) {
		for (i = 0; i < (opcode->size - 1); i++) {
			data[i] = memmap_read(map, *logical + i + 1);
		}
	}

	*logical = next_logical;

	/* Swap LSB and MSB for words */
	if (opcode->size > 2) {
		uint8_t swap;
		/* Special case : TST, BBR*, BBS*. The first byte is zp var */
		i = ((opcode->type == 3) || (opcode->type ==  4) || (opcode->type ==  5) ||
			 (opcode->type == 6) || (opcode->type == 13) || (opcode->type == 20))
				? 1
				: 0;

		for (; i < (opcode->size - 2); i += 2) {
			swap = data[i];
			data[i] = data[i + 1];
			data[i + 1] = swap;
		}
	}

	/* Handle special cases (jumps, tam/tma and unsupported opcodes ) */
	is_jump = 0;
	if (opcode_is_local_jump(inst)) {
		is_jump = 1;

		i = (((inst)&0x0F) == 0x0F) ? 1 : 0;

		/* Detect negative number */
		if (data[i] & 128)
			delta = -((data[i] - 1) ^ 0xff);
		else
			delta = data[i];

		offset = *logical + delta;
		data[i] = offset >> 8;
		data[i + 1] = offset & 0xff;
	} else if (opcode_is_far_jump(inst)) {
		is_jump = 1;
		/* Build offset */
		offset = (data[0] << 8) | data[1];
	} else {
		offset = 0;
		/* Unknown instructions are output as raw data
		 * (ie) .db XX
		 */
		if (opcode->type == 22) {
			data[0] = inst;
		}
	}

	if (opcode->type == 1) {
		fputc('A', out);
	} else if (is_jump) {
		/* BBR* and BBS* */
		if ((inst & 0x0F) == 0x0F) {
			uint16_t zp_offset = 0x2000 + data[0];
			page = memmap_page(map, zp_offset);
			if (label_repository_find(repository, zp_offset, page, &name)) {
				fprintf(out, "<%s, ", name);
			} else {
				fprintf(out, "<$%02x, ", data[0]);
			}
		}
		page = memmap_page(map, offset);
		label_repository_find(repository, offset, page, &name);
		fwrite(name, 1, strlen(name), out);
	} else {
		int has_label = 0;
		if ((inst == 0x43) || (inst == 0x53)) {
			/* tam and tma */
			/* Compute log base 2 of data */
			for (i = 0; (i < 8) && ((data[0] & 1) == 0); ++i, data[0] >>= 1) {
			}
			data[0] = i;
		}

		/* Print data */
		if (opcode->type) {
			/* [todo] this is ugly... */
			const char *none = "";
			const char *extra = none;
			switch (opcode->type) {
			case 4: /* #$aa, <$zp, X */
				extra = ", X";
			case 3: /* #$aa, <$zp */
				offset = 0x2000 + data[1];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &name);
				if (has_label) {
					fprintf(out, "#$%02x, <%s%s", data[0], name, extra);
				}
				break;
			case 6: /* #$aa, $hhll, X */
				extra = ", X";
			case 5: /* #$aa, $hhll */
				offset = (data[0] << 8) + data[1];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &name);
				if (has_label) {
					fprintf(out, "#$%02x, %s%s", data[0], name, extra);
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
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &name);
				if (has_label) {
					fprintf(out, "<%s%s", name, extra);
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
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &name);
				if (has_label) {
					fprintf(out, "[%s%s", name, extra);
				}
				break;

			case 21: /* [hhll, X] */
				extra = ", X]";
			case 15: /* [hhll] */
				if (none == extra) {
					extra = "]";
				}
				offset = (data[0] << 8) | data[1];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &name);
				if (has_label) {
					fprintf(out, "[%s%s", name, extra);
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
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &name);
				if (has_label) {
					fprintf(out, "%s%s", name, extra);
				}
				break;

			case 13: /* <zp, $hhll */
				offset = 0x2000 + data[0];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &name);
				if (has_label) {
					fprintf(out, "<%s, ", name);
				} else {
					fprintf(out, "<%02x, ", data[0]);
				}
				offset = (data[1] << 8) | data[2];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &name);
				if (has_label) {
					fprintf(out, "%s", name);
				} else {
					fprintf(out, "$%04x", offset);
				}
				has_label = 1;
				break;

			case 18: /* shsl, dhdl, sz */
				/* Source and destination */
				for (i = 0; i < 4; i += 2) {
					offset = (data[i] << 8) | data[i + 1];
					page = memmap_page(map, offset);
					has_label = label_repository_find(repository, offset, page, &name);
					if (has_label) {
						fprintf(out, "%s, ", name);
					} else {
						fprintf(out, "$%04x, ", offset);
					}
				}
				/* Size */
				fprintf(out, "$%02x%02x", data[4], data[5]);
				has_label = 1;
				break;

			default:
				has_label = 0;
				break;
			}
/* [todo]
			if (!has_label) {
				for (i = 0; pce_opstring[pce_opcode[inst].type][i] != NULL; ++i) {
					fprintf(out, pce_opstring[pce_opcode[inst].type][i], data[i]);
				}
			}
*/
		}
	}
	fputc('\n', out);
	return eor;
}
