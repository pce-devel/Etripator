/*
¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

  __/¯\____ ___/\__   _/\__   _/\_   _/\__   _/\___ ___/\__   __/\_   _/\__   
  \_  ____/_> ____ \_/  _  \_ \  <  /_    \_/     _>> ____ \_ >    \_/  _  \_ 
  _> ___/ ¯>__> <<__// __  _/ |>  ></ _/>  </  ¯  \\__> <<__//  /\  // __  _/ 
 _>  \7   <__/:. \__/:. \>  \_/   L/  _____/.  7> .\_/:. \__/  <_/ </:. \>  \_ 
|:::::::::::::::::::::::/:::::::::::::>::::::::/::::::::::::::::::::::::/:::::|
|¯¯\::::/\:/¯\::::/¯¯¯¯<::::/\::/¯¯\:/¯¯¯¯¯¯\::\::/¯¯\::::/¯¯\::::/¯¯¯¯<::::/¯|
|__ |¯¯|  T _ |¯¯¯| ___ |¯¯|  |¯| _ T ______ |¯¯¯¯| _ |¯¯¯| _ |¯¯¯| ___ |¯¯| _|
   \|__|/\|/ \|___|/   \|__|/\|_|/ \|/      \|    |/ \|___|/ \|___|/dNo\|__|/  

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

  This file is part of Etripator,
  copyright (c) 2009--2026 Vincent Cruz.
 
  Etripator is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.Fv
 
  Etripator is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with Etripator.  If not, see <http://www.gnu.org/licenses/>.

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯
*/
#include "decode.h"
#include "message.h"
#include "opcodes.h"

#include <stdarg.h>

#define ETRIPATOR_LAST_COLUMN 80U

static const char g_spacing[] = "          ";

static inline int last_column_spacing(int current_char_count) {
    return (current_char_count < ETRIPATOR_LAST_COLUMN) ? (ETRIPATOR_LAST_COLUMN - current_char_count) : 1;
}

/* Finds any jump address from the current section. */
bool label_extract(LabelRepository *labels, MemoryMap *map, Section *section) {
    assert(labels != NULL);
    assert(map != NULL);
    assert(section != NULL);

    int i;

    char buffer[32];

    uint8_t page;

    const size_t begin = section->logical;
    const size_t end = begin + section->size;
    size_t logical;
    bool ret = true;

    if (section->type != SECTION_TYPE_CODE) {
        ret = false;
    } else for (logical = begin; ret && (logical < end); ) {
        /* Read instruction */
        uint8_t inst = memory_map_read(map, logical);
        const Opcode *opcode = opcode_get(inst);

        uint8_t data[6] = {0};
        uint16_t jump = 0;
        /* Read data (if any) */
        for (i = 0; i < (opcode->size - 1); i++) {
            data[i] = memory_map_read(map, logical + i + 1);
        }
        if (opcode_is_local_jump(inst)) {
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
            page = memory_map_page(map, jump);
            /* Create label name */
            snprintf(buffer, sizeof(buffer), "l%04x_%02d", jump, page);
            /* Insert offset to repository */
            ret = label_repository_add(labels, buffer, jump, page, NULL);
            INFO_MSG("%04x short jump to %04x (%02x)", (uint16_t)logical, jump, page);
        } else if (opcode_is_far_jump(inst)) {
            jump = data[0] | (data[1] << 8);
            page = memory_map_page(map, jump);
            /* Create label name */
            snprintf(buffer, sizeof(buffer), "l%04x_%02d", jump, page);
            /* Insert offset to repository */
            ret = label_repository_add(labels, buffer, jump, page, NULL);
            INFO_MSG("%04x long jump to %04x (%02x)", (uint16_t)logical, jump, page);
        }
        logical += opcode->size;
    }
    return ret;
}

static bool data_extract_binary(Output *output, Section *section, MemoryMap *map, LabelRepository *repository) {
    uint16_t logical;
    int32_t i;
    bool ret = true;
    for (i = 0, logical = section->logical; ret && (i < section->size); i++, logical++) {
        uint8_t data = memory_map_read(map, logical);
        ret = output_raw(output, data);
    }
    return ret;
}

// [todo] cut this into smaller functions
// [todo]       -> output .db/dw with spacing
// [todo]       -> output comma separated elements
static int data_extract_hex(Output *output, Section *section, MemoryMap *map, LabelRepository *repository,
                            CommentRepository *comments, int extra_infos) {
    const int32_t element_size = section->data.element_size;
    const int32_t elements_per_line = section->data.elements_per_line;

    int32_t i, j;
    uint16_t logical;

    uint8_t line_page = section->page;
    uint16_t line_logical = section->logical;

    bool has_comment = false;
    
    Comment comment = {0};

    uint8_t data[2] = {0};
    int32_t top = 0;

    bool ret = false;

    for (i = 0, j = 0, logical = section->logical; i < section->size; i++, logical++) {
        uint8_t page = memory_map_page(map, logical);
        Label label = {0};
        bool has_label = label_repository_find(repository, logical, page, &label);
        if (has_label) {      
            // flush any bytes left in the buffer.
            if (top && (top < element_size)) {
                ret = output_newline(output);
                ret = output_fill_n(output, ' ', 4U);
                ret = output_string(output, ".db");
                ret = output_char(output, ' ');
                ret = output_8h(output, data[0]);

                for (int32_t l = 1; l < top; l++) { // useless as top is always equal to 1
                    ret = output_char(output, ',');
                    ret = output_8h(output, data[l]);
                }
                top = 0;
            }
            if (i) {
                ret = output_newline(output);
            }
            ret = output_label(output, &label);
            j = 0;
        }

        Comment dummy;
        if (comment_repository_find(comments, logical, page, &dummy)) {
            if (has_comment) {
                if (top && (top < element_size)) {
                    ret = output_newline(output);
                    ret = output_fill_n(output, ' ', 4U);
                    ret = output_string(output, ".db");
                    ret = output_char(output, ' ');
                    ret = output_8h(output, data[0]);

                    for (int32_t l = 1; l < top; l++) { // useless as top is always equal to 1
                        ret = output_char(output, ',');
                        ret = output_8h(output, data[l]);
                    }
                    top = 0;
                }
                ret = output_inline_comment(output, comment.text);
            }
            comment = dummy;
            has_comment = true;
            j = 0;
        }

        data[top++] = memory_map_read(map, logical);

        if (top >= element_size) {
            char sep;
            if (j == 0) {
                ret = output_newline(output);

                line_logical = logical - top + 1;
                line_page = page;

                const char *data_decl = (top > 1) ? ".dw" : ".db";

                ret = output_fill_n(output, ' ', 4U);
                ret = output_string(output, data_decl);
                
                sep = ' ';
            } else {
                sep = ',';
            }
            ret = output_char(output, sep);
            while (top > 1) {
                top--;
                ret = output_8h(output, data[top]);
            }
            j++;

            if (j == elements_per_line) {
                j = 0;
                if (has_comment) {
                    ret = output_inline_comment(output, comment.text);
                    has_comment = false;
                } else if (extra_infos) {
                    ret = output_address_comment(output, line_page, line_logical);
                }
            }
        }
    }
    // flush remaining bytes
    if (top) {
        if (has_comment) {
            ret = output_inline_comment(output, comment.text);
            has_comment = false;
        } else if (extra_infos) {
            ret = output_address_comment(output, line_page, line_logical);
        }
        ret = output_fill_to(output, ' ', 10U);                                     // [todo]
        ret = output_string(output, ".db");
        ret = output_8h(output, data[0]);
        for (int32_t j = 1; j < top; j++) { // useless as top is always equal to 1
            ret = output_char(output, ',');
            ret = output_8h(output, data[j]);
        }
    }
    ret = output_newline(output);
    return ret;
}

static bool data_extract_string(Output *output, Section *section, MemoryMap *map, LabelRepository *repository,
                                CommentRepository *comments, int extra_infos) {
    const int32_t elements_per_line = section->data.elements_per_line;

    int32_t i, j, k;
    uint16_t logical;

    int c = 0;
    bool has_comment = false;

    size_t line_offset = 0;
    uint16_t line_logical = 0;
    uint8_t line_page;

    Comment comment = {0};
    for (i = 0, j = 0, k = 0, logical = section->logical; i < section->size; i++, logical++) {
        uint8_t data = memory_map_read(map, logical);
        uint8_t page = memory_map_page(map, logical);

        Label label = {0};
        bool has_label = label_repository_find(repository, logical, page, &label);
        if (has_label) {
            if (c) { // close string if needed
                output_char(output, '"'); // [todo]
                c = 0;
            }
            if (i) {
                output_newline(output); // [todo]
            }
            j = 0;
            (void)output_label(output, &label); // [todo]
        }

        Comment dummy = {0};
        if (comment_repository_find(comments, logical, page, &dummy)) {
            if (j) {
                if (c) { // close string if needed
                    output_char(output, '"'); // [todo]
                    c = 0;
                }
                j = 0;
            }
            if (has_comment) {
                output_inline_comment(output, comment.text);
            }
            comment = dummy;
            has_comment = true;
        }

        // display directives
        if (j == 0) {
            ///////////////////////////////////////////////////////////////////////
            output_newline(output);
            line_logical = logical;
            line_page = page;
                (void)output_fill_n(output, ' ', 4U);   // [todo]
                (void)output_string(output, ".db");     // [todo]
        }

        // print char
        if ((data >= 0x20) && (data < 0x7f)) {
            // ascii/printable char
            if (!c) {
                if (j) {
                    (void)output_char(output, ',');     // [todo]
                }
                (void)output_char(output, '"');    // [todo]
                c = 1;
            }
            if (data == '"') {
                (void)output_char(output, '\\');    // [todo]
            }
            (void)output_char(output, data);    // [todo]
        } else {
            // otherwise close string and display hex value
            if (c) {
                (void)output_char(output, '"');    // [todo]
                c = 0;
            }
            if (j) {
                (void)output_char(output, ',');    // [todo]
            }
            (void)output_fmt(output, "$%02x", data);    // [todo]
        }

        bool newline = false;
        if(section->data.delimiter_size && (k < section->data.delimiter_size)) {
            if(data == section->data.delimiter[k]) {
                k++;
                if(k >= section->data.delimiter_size) {
                    newline = true;
                    k = 0;
                }
            } else {
                k = 0;
            }
        }
        j++;

        if ((j == elements_per_line) || newline) {
            j = 0;
            if (c) {
                (void)output_char(output, '"');    // [todo]
                c = 0;
            }

            if (has_comment) {
                output_inline_comment(output, comment.text); // [todo]
                has_comment = false;
            } else if (extra_infos) {
                output_address_comment(output, line_logical, line_page); // [todo]
            }
        }
    }
    if (j) {
        if (c) {
            (void)output_char(output, '"');    // [todo]
        }
        if (has_comment) {
            output_inline_comment(output, comment.text); // [todo]
            has_comment = false;
        } else if (extra_infos) {
            output_address_comment(output, line_logical, line_page); // [todo]
        }
    }
    (void)output_newline(output);
    return true;
}

static bool output_flush_comment(Output *output, const Comment *comment, bool *pending, int extra_infos, uint16_t logical, uint8_t page) {
    bool ret;
    if (*pending) {
        // Flush inline comment
        *pending = false;
        ret = output_inline_comment(output, comment->text);
    } else if (extra_infos) {
        // Print page and logical address as inline comment
        ret = output_address_comment(output, logical, page);
    } else {
        ret = true;
    }
    return ret;
}

static bool output_jump_table(Output *output, Section *section, MemoryMap *map, LabelRepository *repository,
                              CommentRepository *comments, int extra_infos) {
    const int32_t elements_per_line = section->data.elements_per_line;

    bool ret = true;

    int32_t i;

    Label label = {0};
    Comment comment = {0};

    uint8_t line_page = section->page;
    uint16_t line_logical = section->logical;
    uint16_t logical = line_logical;
    int32_t line_elmnt_index = 0;

    bool has_comment = false;

    for (i = 0; i < section->size; i += 2, logical += 2) {
        const uint8_t page = memory_map_page(map, logical);
        const uint8_t data[2] = {
            [0] = memory_map_read(map, logical),
            [1] = memory_map_read(map, logical + 1),
        };

        // Check if we need to emit a label for the current address
        if (label_repository_find(repository, logical, page, &label)) {
            if (i) {
                ret = output_newline(output);
            }
            ret = ret && output_label(output, &label);
            if(!ret) {
                ERROR_MSG("failed to outout label: %s (%02x:%04x)", label.name, label.page, label.logical);
                return false;
            }
            line_elmnt_index = 0;  // we will jump to a new line
        }

        // Check for inline comment
        Comment dummy = {0};
        if (comment_repository_find(comments, logical, page, &dummy)) {
            // Flush any pending comment
            if (has_comment) {
                if(!output_inline_comment(output, comment.text)) {
                    ERROR_MSG("failed to output inline comment (%02x:%04x)", comment.page, comment.logical);
                    return false;
                }
            }
            // Save the new comment.
            // It'll be printed at the end of line.
            // Note that the line will end if:
            //   - we printed the number `elements_per_line` items
            //   - there's comment for an element on the line.
            comment = dummy;
            has_comment = true; 
            line_elmnt_index = 0; // we will jump to a new line
        }

        // New line
        if (line_elmnt_index == 0) {
            // Save the logical address and page of the first line element.
            line_logical = logical;
            line_page = page;

            // Jump table elements are 2 bytes words
            ret = false;
            if(!output_newline(output)) {
                // ...
            } else if(!output_fill_n(output, ' ', 4U)) {
                // ...
            } else if(!output_string(output, ".dw")) {
                // ...
            } else {
                ret = true;
            }
            if(!ret) {
                ERROR_MSG("failed to output new line (%02x:%04x)", page, logical);
                return false;
            }
        } else if(!output_char(output, ',')) { // Print element separator
            ERROR_MSG("failed to output element separator (%02x:%04x)", page, logical);
            return false;
        }
        // Output jump address
        uint16_t jump_logical = data[0] | (data[1] << 8);
        uint8_t jump_page = memory_map_page(map, jump_logical);
        if (label_repository_find(repository, jump_logical, jump_page, &label)) {
            ret = output_string(output, label.name);
        } else {
            ret = output_16h(output, jump_logical);
        }
        if(!ret) {
            ERROR_MSG("failed to output jump table lement (%02x:%04x)", jump_page, jump_logical);
            return false;
        }
        // Next item
        line_elmnt_index++;
        // Check if we need to jump to a new line
        if (line_elmnt_index == elements_per_line) {
            line_elmnt_index = 0;
            if(!output_flush_comment(output, &comment, &has_comment, extra_infos, line_logical, line_page)) {
                ERROR_MSG("failed to flush comment");
                return false;
            }          
        }
    }
    // Flush any pending comment
    if (line_elmnt_index) {
        if(!output_flush_comment(output, &comment, &has_comment, extra_infos, line_logical, line_page)) {
            ERROR_MSG("failed to flush comment");
            return false;
        }         
    }

    return true;
}

/* Process data section. The result will be output has a binary file or an asm file containing hex values or strings. */
bool data_extract(Output *output, Section *section, MemoryMap *map, LabelRepository *repository,
                 CommentRepository *comments, int extra_infos) {
    switch (section->data.type) {
    case DATA_TYPE_BINARY:
        INFO_MSG("DATA_TYPE_BINARY");
        return data_extract_binary(output, section, map, repository);
    case DATA_TYPE_HEX:
        return data_extract_hex(output, section, map, repository, comments, extra_infos);
    case DATA_TYPE_STRING:
        INFO_MSG("DATA_TYPE_STRING");
        return data_extract_string(output, section, map, repository, comments, extra_infos);
    case DATA_TYPE_JUMP_TABLE:
        INFO_MSG("DATA_TYPE_JUMP_TABLE");
        return output_jump_table(output, section, map, repository, comments, extra_infos);
    default:
        ERROR_MSG("unhandled data type %d", section->data.type);
        return false;
    }
}

/* Process code section. */
// [todo] 1 function per addressing mode
// [todo] make a single instruction decode function
bool decode(Output *output, uint16_t *logical, Section *section, MemoryMap *map, LabelRepository *repository,
            CommentRepository *comments, int extra_infos) {
    int i, delta;
    uint8_t inst, data[6], is_jump;
    char eor;
    uint8_t page;
    uint32_t offset;
    uint8_t current_page;
    uint16_t current_logical;
    uint16_t next_logical;
    Label label;

    const Opcode *opcode;

    eor = 0;

    memset(data, 0, 6);
    page = memory_map_page(map, *logical);

    /* Opcode */
    inst = memory_map_read(map, *logical);
    opcode = opcode_get(inst);
    
	current_page = page;
	current_logical = *logical;
	next_logical = *logical + opcode->size;

	/* Is there a label ? */
	if (label_repository_find(repository, current_logical, page, &label)) {
        (void)output_label(output, &label); // [todo]
        (void)output_newline(output);       // [todo]
	}

	/* Front spacing */
   (void)output_fill_n(output, ' ', 10U); // [todo] ret + value

	/* Print opcode string */
    (void)output_string(output, opcode->name);

	/* Add spacing */
    (void)output_fill_to(output, ' ', 20U); // [todo] ret + value

	/* End Of Routine (eor) is set to 1 if the instruction is RTI, RTS or BRK */
	eor = ((inst == 0x40) || (inst == 0x60) || (inst == 0x00));
	
	/* Data */
	if (opcode->size > 1) {
		for (i = 0; i < (opcode->size - 1); i++) {
			data[i] = memory_map_read(map, *logical + i + 1);
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
        (void)output_char(output, 'A'); // [todo]
	} else if (is_jump) {
		/* BBR* and BBS* */
		if ((inst & 0x0F) == 0x0F) {
			uint16_t zp_offset = 0x2000 + data[0];                                            // [todo] RAM may not be in mpr1 ...
			page = memory_map_page(map, zp_offset);
			if (label_repository_find(repository, zp_offset, page, &label)) {
                (void)output_fmt(output, "<%s, ", label.name); // [todo]
			} else {
                (void)output_fmt(output, "<$%02x, ", data[0]); // [todo]
			}
		}
		page = memory_map_page(map, offset);
		// Label name should have been set by the label extraction pass.
		label_repository_find(repository, offset, page, &label);
        (void)output_string(output, label.name);
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
				/* fall through */
			case PCE_OP_nn_ZZ:                                  /* #$aa, <$zp */
				offset = 0x2000 + data[1];
				page = memory_map_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
                    (void)output_fmt(output, "#$%02x, <%s%s", data[0], label.name, extra); // [todo]
				}
				break;
			case PCE_OP_nn_hhll_X:                              /* #$aa, $hhll, X */
				extra = ", X";
				/* fall through */
			case PCE_OP_nn_hhll:                                /* #$aa, $hhll */
				offset = (data[1] << 8) + data[2];
				page = memory_map_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
                    (void)output_fmt(output, "#$%02x, %s%s", data[0], label.name, extra); // [todo]
				}
				break;

			case PCE_OP_ZZ_Y:                                   /* <zp, Y */
				extra = ", Y";
				/* fall through */
			case PCE_OP_ZZ_X:                                   /* <zp, X */
				if (none == extra) {
					extra = ", X";
				}
				/* fall through */
			case PCE_OP_ZZ:                                     /* <zp    */
				offset = 0x2000 + data[0];
				page = memory_map_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
                    (void)output_fmt(output, "<%s%s", label.name, extra); // [todo]
				}
				break;

			case PCE_OP__ZZ__Y_:                                /* [zp], Y */
				extra = "], Y";
				/* fall through */
			case PCE_OP__ZZ_X__:                                /* [zp, X] */
				if (none == extra) {
					extra = ", X";
				}
				/* fall through */
			case PCE_OP__ZZ__:                                  /* [zp] */
				if (none == extra) {
					extra = "]";
				}
				offset = 0x2000 + data[0];
				page = memory_map_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
                    (void)output_fmt(output, "[%s%s", label.name, extra); // [todo]
				}
				break;

			case PCE_OP__hhll_X__:                              /* [hhll, X] */
				extra = ", X]";
				/* fall through */
			case PCE_OP__hhll__:                                /* [hhll] */
				if (none == extra) {
					extra = "]";
				}
				offset = (data[0] << 8) | data[1];
				page = memory_map_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
                    (void)output_fmt(output, "[%s%s", label.name, extra); // [todo]
				}
				break;

			case PCE_OP_hhll_X:                                 /* hhll, X */
				extra = ", X";
				/* fall through */
			case PCE_OP_hhll_Y:                                 /* hhll, Y */
				if (none == extra) {
					extra = ", Y";
				}
				/* fall through */
			case PCE_OP_hhll:                                   /* hhll */
				offset = (data[0] << 8) | data[1];
				page = memory_map_page(map, offset);
				has_label = label_repository_find(repository, offset, page, &label);
				if (has_label) {
                    (void)output_fmt(output, "%s%s", label.name, extra); // [todo]
				}
				break;

			case PCE_OP_ZZ_hhll:                                /* <zp, $hhll */
				offset = 0x2000 + data[0];
				page = memory_map_page(map, offset);
				if (label_repository_find(repository, offset, page, &label)) {
                    (void)output_fmt(output, "<%s, ", label.name); // [todo]
				} else {
                    (void)output_fmt(output, "<%02x, ", data[0]); // [todo]
				}
				offset = (data[1] << 8) | data[2];
				page = memory_map_page(map, offset);
				if (label_repository_find(repository, offset, page, &label)) {
                    (void)output_fmt(output, "%s", label.name); // [todo]
				} else {
                    (void)output_fmt(output, "$%04x", offset); // [todo]
				}
				has_label = 1;
				break;

			case PCE_OP_shsl_dhdl_hhll:                         /* shsl, dhdl, sz */
				/* Source and destination */
				for (i = 0; i < 4; i += 2) {
					offset = (data[i] << 8) | data[i + 1];
					page = memory_map_page(map, offset);
					if (label_repository_find(repository, offset, page, &label)) {
                        (void)output_fmt(output, "%s, ", label.name); // [todo]
					} else {
                        (void)output_fmt(output, "$%04x, ", offset); // [todo]
					}
				}
				/* Size */
                (void)output_fmt(output, "$%02x%02x", data[4], data[5]); // [todo]
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
                    (void)output_fmt(output, format, data[i]); // [todo]
					i++;
				}
			}
		}
	}

	/* display inline comments if any */
	Comment comment = {0};
	if(comment_repository_find(comments, current_logical, current_page, &comment)) {
        (void)output_inline_comment(output, comment.text);
	} else if(extra_infos) {
        (void)output_address_comment(output, current_page, current_logical);
	}
    (void)output_newline(output);
    return eor ? false : true;
}

/* Computes section size. */
int32_t compute_size(MemoryMap *map, SectionArray *sections, int index) {
    uint8_t i;
    uint8_t data[7];
    Section *current = &sections->data[index];
    uint32_t start = current->logical;
    uint32_t logical = start;

    // Search for the closest section past the current one.
    // This also ensures that we don't cross the current page.
    uint32_t max_offset = 0xffffffff;
    for (i = 0; i < sections->count; i++) {
        if (i != index) {
            if (current->page == sections->data[i].page) {
                uint32_t offset_current = current->offset & 0x1fff;
                uint32_t offset_it = sections->data[i].offset & 0x1fff;
                if ((offset_current < offset_it) && (max_offset > offset_it)) {
                    max_offset = offset_it;
                }
            }
        }
    }
    for (int eor = 0; !eor;) {
        if ((logical & 0x1fff) >= max_offset) {
            break;
        }
        uint8_t page = memory_map_page(map, logical);
        data[0] = memory_map_read(map, logical);
        const Opcode *opcode = opcode_get(data[0]);
        for (i = 1; i < opcode->size; i++) {
            data[i] = memory_map_read(map, logical + i);
        }
        logical += opcode->size;
        if (opcode_is_far_jump(data[0])) {
            uint32_t jump = data[1] | (data[2] << 8);
            if (data[0] == 0x4c) { // jmp hhll
                uint8_t jump_page = memory_map_page(map, jump);
                if (page == jump_page) {
                    if (jump < logical) {
                        eor = 1;
                    } else {
                        logical = jump;
                    }
                }
            }
        } else if (opcode_is_local_jump(data[0])) {
            uint32_t jump = 0;
            int32_t delta = 0;
            /* For BBR* and BBS* displacement is stored in the 2nd byte */
            i = ((data[0] & 0x0F) == 0x0F) ? 2 : 1;
            if (data[i] & 128)
                delta = -((data[i] - 1) ^ 0xff);
            else
                delta = data[i];
            jump = logical + delta;
            if (data[0] == 0x80) { // bra offset
                if (jump < logical) {
                    eor = 1;
                }
            } else if (jump > logical) {
                logical = jump;
            }
        } else if ((data[0] == 0x40) || (data[0] == 0x60) || (data[0] == 0x00)) { // rts, rti or brk
            eor = 1;
        }
    }
    return (logical - start);
}

/* Output hardware IO port and RAM labels. */
void label_dump(Output *output, MemoryMap *map, LabelRepository *labels) {
    bool ret = true;
    const int count = label_repository_size(labels);
    for (int i = 0; ret && (i < count); i++) {
        Label label = {0};
        if (label_repository_get(labels, i, &label)) {
            // IO port and RAM
            if ((label.page == 0xFFU) || (label.page == 0xF8U)) {
                ret = false;
                /*
                ; comment if any
                label .equ $logical
                */
                if(output_comment(output, label.description) != true) {
                    // ..(
                } else if(output_string(output, label.name) != true) {
                    // ..
                } else if(output_string(output, " .equ $") != true) {
                    // ..
                } else if(output_byte(output, label.logical >> 8) != true) {
                    // ..
                } else if(output_byte(output, label.logical & 0xFFU) != true) {
                    // ..
                } else if(output_newline(output) != true) {
                    // ..
                } else {
                    ret = true;
                }
            }
        }
    }
}

