/*
 * This file is part of Etripator,
 * copyright (c) 2009--2023 Vincent Cruz.
 *
 * Etripator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Etripator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Etripator.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "decode.h"
#include "message.h"
#include "opcodes.h"

#include <stdarg.h>

#define ETRIPATOR_LAST_COLUMN 40

static const char *spacing = "          ";

static int last_column_spacing(int current_char_count) {
	return (current_char_count < ETRIPATOR_LAST_COLUMN) ? (ETRIPATOR_LAST_COLUMN - current_char_count) : 1;
} 

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

static void print_inline_comment(FILE *out, int n, const char *str) {
	while(*str) {
		for(; n<ETRIPATOR_LAST_COLUMN; n++) {
			fputc(' ', out);
		}
		n = 0;
		fputc(';', out);
		fputc(' ', out);
		for(;*str && (*str != '\n'); ++str) {
            fputc(*str, out);
        }
		if(*str =='\n') {
			++str;
			if(*str) {
				fputc('\n', out);
			}
		}
	}
}

static void print_statement_address(FILE *out, int n, uint16_t logical, uint8_t page) {
	fprintf(out, "%*c; bank: $%03x logical: $%04x", last_column_spacing(n), ' ', page, logical);
}

static void print_label(FILE *out, label_t *label) {
	int n = last_column_spacing((int)strlen(label->name) + 1);
	/* Print description */
	print_comment(out, label->description);
	/* Print label with bank and logical address as comments*/
	fprintf(out, "%s:%*c; bank: $%03x logical: $%04x", label->name, n, ' ', label->page, label->logical);
}

/* Finds any jump address from the current section. */
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

static int data_extract_hex(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository, comment_repository_t *comments, int extra_infos) {
    const int32_t element_size = section->data.element_size;
	const int32_t elements_per_line = section->data.elements_per_line;

	int32_t i, j;
    uint16_t logical;

    label_t label;
	comment_t comment;

	uint8_t buffer[2] = {0};
	int32_t top = 0;

	size_t start = ftell(out);

	uint8_t page = memmap_page(map, logical);
	uint16_t start_logical = logical;

    for(i=0, j=0, logical=section->logical; i<section->size; i++, logical++) {
		page = memmap_page(map, logical);
		
		int has_comment = comment_repository_find(comments, logical, page, &comment);
		int has_label   = label_repository_find(repository, logical, page, &label);

		if(has_label) {
			// flush any bytes left in the buffer.
			if(top && (top < element_size)) {
				fprintf(out, "\n%s.db $%02x", spacing, buffer[0]);
				for(int32_t l=1; l<top; l++) {		// useless as top is always equal to 1
					fprintf(out, ",$%02x", buffer[l]);
				}
				top = 0;
			}
			if(i) {
				fputc('\n', out);
			}
			print_label(out, &label);
			start = ftell(out);
			start_logical = logical;
			j = 0;
		}

		buffer[top++] = memmap_read(map, logical);

		if((top >= element_size) || has_comment) {
			char sep;
			if((j == 0) || has_comment) {
				const char *data_decl = (top > 1) ? ".dw" : ".db";
				fputc('\n', out);

				start = ftell(out);				
				start_logical = logical-(top-1);

				fprintf(out, "%s%s", spacing, data_decl);
				sep = ' ';
			} else {
				sep = ',';
			}
			fputc(sep, out);
			fputc('$', out);
			if(top > 1) {
				while(top--) {
					fprintf(out, "%02x", buffer[top]);
				}
			} else {
				fprintf(out, "%02x", buffer[0]);
			}
			top = 0;
			j = (j+1) % elements_per_line;

			if(has_comment) {
				print_inline_comment(out, (int)(ftell(out) - start), comment.text);
				j = 0;
			} else if((j == 0) && extra_infos) {
				print_statement_address(out, (int)(ftell(out)-start), start_logical, memmap_page(map, start_logical));
			}
		}
	}
	// flush remaining bytes
    if(top) {
		if(extra_infos) {
			print_statement_address(out, (int)(ftell(out)-start), start_logical, memmap_page(map, start_logical));
		}
        fprintf(out, "\n%s.db $%02x", spacing, buffer[0]);
		for(int32_t j=1; j<top; j++) {		// useless as top is always equal to 1
			fprintf(out, ",$%02x", buffer[j]);
		}
    }
    fputc('\n', out);
    return 1;
}

static int data_extract_string(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository, comment_repository_t *comments, int extra_infos) {
    const int32_t elements_per_line = section->data.elements_per_line;

	int32_t i, j;
	uint16_t logical;

	int c = 0;
	int has_label = 0;
	int has_comment = 0;

	size_t line_offset = 0;
	uint16_t line_logical = 0;
	uint8_t line_page;

	comment_t comment = {0};

	for(i=0, j=0, logical=section->logical; i<section->size; i++, logical++) {
		uint8_t data = memmap_read(map, logical);
		uint8_t page = memmap_page(map, logical);

		label_t label = {0};
		
		has_label = label_repository_find(repository, logical, page, &label);
		
		if(has_label) {
			if(c) {			// close string if neededs
				fputc('"', out);
				c = 0;
			}
			if(i) {
				fputc('\n', out);
			}
			j = 0;
			print_label(out, &label);
		}

		comment_t dummy;
		if(comment_repository_find(comments, logical, page, &dummy)) {
			if(j) {
				if(c) {			// close string if neededs
					fputc('"', out);
					c = 0;
				}
				j = 0;
			}
			if(has_comment) {
				print_inline_comment(out, (int)(ftell(out) - line_offset), comment.text);
			}
			memcpy(&comment, &dummy, sizeof(comment_t));
			has_comment = 1;
		}

		// display directives
		if(j==0) {
			fputc('\n', out);
			line_offset = ftell(out);		// record star of line
			line_logical = logical;
			line_page = page;
			fprintf(out, "%s.db ", spacing);
		}
	
		// print char
		if((data >= 0x20) && (data < 0x7f)) {
			// ascii/printable char
			if(!c) {
				if(j) {
					fputc(',', out);
				}
				fputc('"', out);
				c = 1;
			}
			if(data == '"') {
				fputc('\\', out);
			}
			fputc(data, out);
		} else {
			// otherwise close string and display hex value
			if(c) {
				fputc('"', out);
				c = 0;
			}
			if(j) {
				fputc(',', out);
			}
			fprintf(out, "$%02x", data);
		}
		j++;

		if(j == elements_per_line) {
			j = 0;
			if(c) {
				fputc('"', out);
				c = 0;
			}
	
			int n = (int)(ftell(out) - line_offset);
			if(has_comment) {
				print_inline_comment(out, n, comment.text);
				has_comment = 0;
			} else if(extra_infos) {
				print_statement_address(out, n, line_logical, line_page);
			}
		}
	}
	fputc('\n', out);
	return 1;
}

/* Process data section. The result will be output has a binary file or an asm file containing hex values or strings. */
int data_extract(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository, comment_repository_t *comments, int extra_infos) {
    switch(section->data.type) {
        case Binary:
            return data_extract_binary(out, section, map, repository);
        case Hex:
            return data_extract_hex(out, section, map, repository, comments, extra_infos);
        case String:
            return data_extract_string(out, section, map, repository, comments, extra_infos);
		default:
			return 0;
    }
}

/* Process code section. */
int decode(FILE *out, uint16_t *logical, section_t *section, memmap_t *map, label_repository_t *repository, comment_repository_t *comments, int extra_infos) {
	int i, delta;
	uint8_t inst, data[6], is_jump;
	char eor;
	uint8_t page;
	uint32_t offset;
	uint8_t current_page;
	uint16_t current_logical;
	uint16_t next_logical;
    label_t label;

    const opcode_t *opcode;

	eor = 0;

	memset(data, 0, 6);
	page = memmap_page(map, *logical); 

	/* Opcode */
	inst = memmap_read(map, *logical);
    opcode = opcode_get(inst);
    
	current_page = page;
	current_logical = *logical;
	next_logical = *logical + opcode->size;

	/* Is there a label ? */
	if (label_repository_find(repository, current_logical, page, &label)) {
		print_label(out, &label);
		fputc('\n', out);
	}

	/* Offset of the line start */
	size_t start = ftell(out);

	/* Front spacing */
	fwrite(spacing, 1, 10, out);

	/* Print opcode string */
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
			uint16_t zp_offset = 0x2000 + data[0];                                            // [todo] RAM may not be in mpr1 ...
			page = memmap_page(map, zp_offset);
			if (label_repository_find(repository, zp_offset, page, &label)) {
				fprintf(out, "<%s, ", label.name);
			} else {
				fprintf(out, "<$%02x, ", data[0]);
			}
		}
		page = memmap_page(map, offset);
		// Label name should have been set by the label extraction pass.
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
				if (label_repository_find(repository, offset, page, &label)) {
					fprintf(out, "<%s, ", label.name);
				} else {
					fprintf(out, "<%02x, ", data[0]);
				}
				offset = (data[1] << 8) | data[2];
				page = memmap_page(map, offset);
				if (label_repository_find(repository, offset, page, &label)) {
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
					if (label_repository_find(repository, offset, page, &label)) {
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

	/* display inline comments if any */
	comment_t comment = {0};
	int n = (int)(ftell(out) - start);
	if(comment_repository_find(comments, current_logical, current_page, &comment)) {
		print_inline_comment(out, n, comment.text);
	} else if(extra_infos) {
		print_statement_address(out, n, current_logical, current_page);
	}
	fputc('\n', out);
	return eor;
}

/* Computes section size. */
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

/* Output hardware IO port and RAM labels. */
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
