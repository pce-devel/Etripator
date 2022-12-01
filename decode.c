/*
	This file is part of Etripator,
	copyright (c) 2009--2022 Vincent Cruz.

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

#include <stdarg.h>

static void print_comment(FILE *out, const char *str) {
    if(!str) {
        return;
    }
    while(*str) {
        fputc(';', out);
        fputc(' ', out);
        for(;*str && (*str != '\n'); str++) {
            fputc(*str, out);
        }
        fputc('\n', out);
        if(*str) {
            str++;
        }
    }
}

static void print_label(FILE *out, label_t *label) {
	int n = (int)(strlen(label->name) + 1);
	n = (n < 40) ? (40 - n) : 1; 
	/* Print description */
	print_comment(out, label->description);
	/* Print label with bank and logical address as comments*/
	fprintf(out, "%s:%*c; bank: $%03x logical: $%04x", label->name, n, ' ', label->page, label->logical);
}

/**
 * Finds any jump address from the current section.
 * @param [in] section Current section.
 * @param [in] map Memory map.
 * @param [in out] repository Label repository.
 * @return 1 upon success, 0 otherwise.
 */
int label_extract(section_t *section, memmap_t *map, label_repository_t *repository) {
	int i;
	uint8_t inst;
	uint8_t data[6];
	char buffer[32];

	uint16_t logical;
	uint8_t page;

    const opcode_t *opcode;

	if (section->type != Code) {
		return 1;
	}

	/* Walk along section */
    for(logical = section->logical; logical < (section->logical + section->size); logical += opcode->size) {
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
			if (!label_repository_add(repository, buffer, jump, page, NULL)) {
				return 0;
			}
			INFO_MSG("%04x short jump to %04x (%02x)", logical, jump, page);
		} else if (opcode_is_far_jump(inst)) {
				uint16_t jump = data[0] | (data[1] << 8);
				page = memmap_page(map, jump);
				/* Create label name */
				snprintf(buffer, 32, "l%04x_%02d", jump, page);
				/* Insert offset to repository */
				if (!label_repository_add(repository, buffer, jump, page, NULL)) {
					return 0;
				}

				INFO_MSG("%04x long jump to %04x (%02x)", logical, jump, page);
		}
	}
	return 1;
}

static int data_extract_binary(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository) {
    uint16_t logical;
    int32_t i;
    for (i=0, logical=section->logical; i < section->size; i++, logical++) {
        uint8_t data = memmap_read(map, logical);
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
    label_t label;
    
    for(i=0, j=0, k=0, logical=section->logical; i<section->size; i++, logical++) {
        uint8_t page = memmap_page(map, logical);
        if(label_repository_find(repository, logical, page, &label)) {
            if(k && (k < element_size)) {
                fprintf(out, "\n          .db");
                for(j=0; j<k; j++) {
                    fprintf(out, "%c$%02x", j ? ',' : ' ', data[j]);
                }
                k = 0;
            }
            if(i) {
                fputc('\n', out);
            }
            print_label(out, &label);
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
    if(k) {
        fprintf(out, "\n          .db ");
        for(j=0; j<k; j++) {
            fprintf(out, "$%02x%c", data[j], ((j+1)<k) ? ',' : '\n');
        }
    }
    fputc('\n', out);
    return 1;
}

static int data_extract_string(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository) {
	int32_t i, j, k;
    uint16_t logical;
	int32_t elements_per_line = section->data.elements_per_line;
    label_t label;
    char c;
    
    for(i=0, j=0, k=0, c=0, logical=section->logical; i<section->size; i++, logical++) {
        uint8_t data;
        uint8_t page = memmap_page(map, logical);
        if(label_repository_find(repository, logical, page, &label)) {
            if(j && c) {
            	putc('"', out);
            }
			if(i) {
				fputc('\n', out);
			}
            print_label(out, &label);
            j = 0;
            c = 0;
        }
        data = memmap_read(map, logical);
        if(j == 0) {
            if(c) {
                fputc('"', out);
            }
            fprintf(out, "\n          db ");
            c = 0;
        }
        
        j = (j+1) % elements_per_line;
        
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
			if(j && ((i+1) < section->size)) {
				fputc(',', out);
			}
        }
    }
    if((j || (i >= section->size)) && c) {
        fputc('"', out);
        fputc('\n', out);
    }
    fputc('\n', out);
    return 1;
}

/**
 * Process data section. The result will be output has a binary file or an asm file containing hex values or strings.
 * @param [out] out File output.
 * @param [in] section Current section.
 * @param [in] map Memory map.
 * @param [in] repository Label repository.
 * @return 1 upon success, 0 otherwise.
 */
int data_extract(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository) {
    switch(section->data.type) {
        case Binary:
            return data_extract_binary(out, section, map, repository);
        case Hex:
            return data_extract_hex(out, section, map, repository);
        case String:
            return data_extract_string(out, section, map, repository);
		default:
			return 0;
    }
}

static const char *spacing = "          ";

/**
 * Process code section.
 * @param [out] out File output.
 * @param [in out] logical Current logical address.
 * @param [in] section Current section.
 * @param [in] map Memory map.
 * @param [in] repository Label repository.
 * @return 1 if rts, rti or brk instruction was decoded, 0 otherwise.
 */
int decode(FILE *out, uint16_t *logical, section_t *section, memmap_t *map, label_repository_t *repository) {
	int i, delta;
	uint8_t inst, data[6], is_jump;
	char eor;
	uint8_t page;
	uint32_t offset;
	uint16_t next_logical;
    label_t label;

    const opcode_t *opcode;

	eor = 0;

	memset(data, 0, 6);
	page = memmap_page(map, *logical); 

	/* Opcode */
	inst = memmap_read(map, *logical);
    opcode = opcode_get(inst);
    
	next_logical = *logical + opcode->size;

	/* Is there a label ? */
	if (label_repository_find(repository, *logical, page, &label)) {
		print_label(out, &label);
		fputc('\n', out);
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
			uint16_t zp_offset = 0x2000 + data[0];                                              // [todo] RAM may not be in mpr1 ...
			page = memmap_page(map, zp_offset);
			if (label_repository_find(repository, zp_offset, page, &label)) {
				fprintf(out, "<%s, ", label.name);
			} else {
				fprintf(out, "<$%02x, ", data[0]);
			}
		}
		page = memmap_page(map, offset);
		label_repository_find(repository, offset, page, &label);
		fwrite(label.name, 1, strlen(label.name), out);
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
			case PCE_OP_nn_ZZ_X:                                /* #$aa, <$zp, X */
				extra = ", X";
			case PCE_OP_nn_ZZ:                                  /* #$aa, <$zp */
				offset = 0x2000 + data[1];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
					fprintf(out, "#$%02x, <%s%s", data[0], label.name, extra);
				}
				break;
			case PCE_OP_nn_hhll_X:                              /* #$aa, $hhll, X */
				extra = ", X";
			case PCE_OP_nn_hhll:                                /* #$aa, $hhll */
				offset = (data[1] << 8) + data[2];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
					fprintf(out, "#$%02x, %s%s", data[0], label.name, extra);
				}
				break;

			case PCE_OP_ZZ_Y:                                   /* <zp, Y */
				extra = ", Y";
			case PCE_OP_ZZ_X:                                   /* <zp, X */
				if (none == extra) {
					extra = ", X";
				}
			case PCE_OP_ZZ:                                     /* <zp    */
				offset = 0x2000 + data[0];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
					fprintf(out, "<%s%s", label.name, extra);
				}
				break;

			case PCE_OP__ZZ__Y_:                                /* [zp], Y */
				extra = "], Y";
			case PCE_OP__ZZ_X__:                                /* [zp, X] */
				if (none == extra) {
					extra = ", X";
				}
			case PCE_OP__ZZ__:                                  /* [zp] */
				if (none == extra) {
					extra = "]";
				}
				offset = 0x2000 + data[0];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
					fprintf(out, "[%s%s", label.name, extra);
				}
				break;

			case PCE_OP__hhll_X__:                              /* [hhll, X] */
				extra = ", X]";
			case PCE_OP__hhll__:                                /* [hhll] */
				if (none == extra) {
					extra = "]";
				}
				offset = (data[0] << 8) | data[1];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
					fprintf(out, "[%s%s", label.name, extra);
				}
				break;

			case PCE_OP_hhll_X:                                 /* hhll, X */
				extra = ", X";
			case PCE_OP_hhll_Y:                                 /* hhll, Y */
				if (none == extra) {
					extra = ", Y";
				}
			case PCE_OP_hhll:                                   /* hhll */
				offset = (data[0] << 8) | data[1];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
					fprintf(out, "%s%s", label.name, extra);
				}
				break;

			case PCE_OP_ZZ_hhll:                                /* <zp, $hhll */
				offset = 0x2000 + data[0];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
					fprintf(out, "<%s, ", label.name);
				} else {
					fprintf(out, "<%02x, ", data[0]);
				}
				offset = (data[1] << 8) | data[2];
				page = memmap_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
					fprintf(out, "%s", label.name);
				} else {
					fprintf(out, "$%04x", offset);
				}
				has_label = 1;
				break;

			case PCE_OP_shsl_dhdl_hhll:                         /* shsl, dhdl, sz */
				/* Source and destination */
				for (i = 0; i < 4; i += 2) {
					offset = (data[i] << 8) | data[i + 1];
					page = memmap_page(map, offset);
					has_label = label_repository_find(repository, offset, page, &label);
					if (has_label) {
						fprintf(out, "%s, ", label.name);
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

			if (!has_label) {
			    const char *format;
			    i = 0;
				while((format = opcode_format(opcode, i))) {
					fprintf(out, format, data[i]);
					i++;
				}
			}
		}
	}
	fputc('\n', out);
	return eor;
}

/**
 * Computes section size.
 * @param [in] sections Section array.
 * @param [in] index Index of the current section.
 * @param [in] count Number of sections.
 * @param [in] map Memory map.
 * @return Section size. 
 */
int32_t compute_size(section_t *sections, int index, int count, memmap_t *map) {
    uint8_t i;
    uint8_t data[7];
    section_t *current = &sections[index];
    uint32_t start = current->logical;
    uint32_t logical = start;

    // Search for the closest section past the current one.
    // This also ensures that we don't cross the current page.
    uint32_t max_offset = 0xffffffff;
    for(i=0; i<count; i++) {
        if(i != index) {
            if(current->page == sections[i].page) {
                uint32_t offset_current = current->offset & 0x1fff;
                uint32_t offset_it = sections[i].offset & 0x1fff;
                if((offset_current < offset_it) && (max_offset > offset_it)){
                    max_offset = offset_it;
                }
            }
        }
    }
    for(int eor=0; !eor; ) {
        if((logical & 0x1fff) >= max_offset) {
            printf("%x %x\n", logical&0x1fff, max_offset);
            break;
        }
        uint8_t page = memmap_page(map, logical);
    	data[0] = memmap_read(map, logical);
        const opcode_t *opcode = opcode_get(data[0]);
        for (i = 1; i < opcode->size; i++) {
    		data[i] = memmap_read(map, logical + i);
    	}
        logical += opcode->size;
        if(opcode_is_far_jump(data[0])) {
            uint32_t jump = data[1] | (data[2] << 8);
            if(data[0] == 0x4c) {   // jmp hhll
                uint8_t jump_page = memmap_page(map, jump);
                if(page == jump_page) {
                    if(jump < logical) {
                        eor = 1;
                    }
                    else {
                        logical = jump; 
                    }
                }
            }
        }
        else if(opcode_is_local_jump(data[0])) {
            uint32_t jump = 0;
            int32_t delta = 0;
			/* For BBR* and BBS* displacement is stored in the 2nd byte */
    		i = ((data[0]&0x0F) == 0x0F) ? 2 : 1;
    		if (data[i] & 128)
    			delta = -((data[i] - 1) ^ 0xff);
    		else
    			delta = data[i];
    		jump = logical + delta;
            if(data[0] == 0x80) {      // bra offset
                if(jump < logical) {
                    eor = 1;
                }
            }
            else if(jump > logical) {
                logical = jump;
            }
        }
        else if((data[0] == 0x40) || (data[0] == 0x60) || (data[0] == 0x00)) { // rts, rti or brk
            eor = 1;
        }
    }
    return (logical - start);
}

/**
 * Output hardware IO port and RAM labels.
 * @param [out] out File output.
 * @param [in] map Memory map.
 * @param [in] repository Label repository.
 */
void label_dump(FILE *out, memmap_t *map, label_repository_t *repository) {
    int count = label_repository_size(repository);
    for(int i=0; i<count; i++) {
        label_t label;
        if(label_repository_get(repository, i, &label)) {
            // IO port and RAM
            if((label.page == 0xff) || (label.page == 0xf8)) {
                print_comment(out, label.description);
                fprintf(out, "%s .equ $%04x\n", label.name, label.logical);
            }
        }
    }
}
