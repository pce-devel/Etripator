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
  copyright (c) 2009--2024 Vincent Cruz.
 
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

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯
*/
#include "output.h"

#include "message.h"

#include "memory_map.h"
#include "label.h"
#include "section.h"
#include "comment.h"

#define OUTPUT_REGISTRY_INC 8U

#define ETRIPATOR_LAST_COLUMN 80U

#define ETRIPATOR_SPACING 10U

// Add a new output file to registry.
bool output_registry_add(OutputRegistry *registry, const char *filename) {
    assert(registry != NULL);
    assert(filename != NULL);

    bool ret = true;
    size_t i;
    for(i=0; (i<registry->count) && strcmp(filename, registry->data[i].filename); i++) {
    }

    if(i < registry->count) {
        // ...
    } else {
        if(registry->count >= registry->capacity) {
            size_t n = registry->capacity + OUTPUT_REGISTRY_INC;
            Output *data = realloc(registry->data, n * sizeof(Output));
            if(data == NULL) {
                ERROR_MSG("failed to resize buffer %s", strerror(errno));
                ret = false;
            } else {
                registry->data = data;
                registry->capacity = n;
            }
        }

        if(ret) {
            Output *current = &registry->data[registry->count];
            current->id = 0;
            current->column = current->line = 0;
            current->stream = NULL;
            current->filename = strdup(filename);
            if(current->filename == NULL) {
                ERROR_MSG("failed to add string %s", strerror(errno));
                ret = false;
            } else {
                current->id = registry->count++;
            }
        }

    }
    return ret;
}

// Destroy output registry.
void output_registry_destroy(OutputRegistry *registry) {
    assert(registry != NULL);
    for(size_t i=0; i<registry->count; i++) {
        free(registry->data[i].filename);
        if(registry->data[i].stream) {
            fclose(registry->data[i].stream);
        }
    }
    free(registry->data);
    registry->data = NULL;
    registry->count = registry->capacity = 0;
}

// Find the registry entry associated to a given filename.
bool output_find(const OutputRegistry *registry, const char *filename, uint32_t *id) {
    assert(registry != NULL);
    assert(filename != NULL);
    assert(id != NULL);

    bool ret = false;
    size_t i;
    for(i=0; (i<registry->count) && (ret == false); i++) {
        ret = (strcmp(filename, registry->data[i].filename) == 0);
    }
    *id = i;
    return ret;
}

// Open output file for writing data at the end of the file.
bool output_begin(Output *output) {
    assert(output != NULL);
    assert(output->filename != NULL);
    bool ret = false;
    if(output->stream != NULL) {
        fclose(output->stream);
    }
    output->stream = fopen(output->filename, "ab");
    if(output->stream == NULL) {
        ERROR_MSG("failed to open file %s: %s", output->filename, strerror(errno));
    } else {
        ret = true;
    }
    return ret;
}

// Close output file
void output_end(Output *output) {
    assert(output != NULL);
    fclose(output->stream);
    output->stream = NULL;
}

// Write single char
bool output_char(Output *output, char c) {
    bool ret = false;
    if(fputc(c, output->stream) == EOF) {
        ERROR_MSG("failed to print '%c' to %s: %s", c, output->filename, strerror(errno));
    } else {
        output->column++;
        ret = true;
    }
    return ret;
}

static char hex(uint8_t b) {
    char c;
    b &= 0x0FU;
    if(b >= 10U) {
        c = (b - 10U) + 'a';
    } else {
        c = b + '0';
    }
    return c;
}

// Write a single hexadecimal byte
bool output_byte(Output *output, uint8_t c) {
    bool ret = false;
    if(output_char(output, hex(c>>4)) != true) {
        // ...
    } else if(output_char(output, hex(c)) != true) {
        // ...
    } else {
        ret = true;
    }
    return ret;
}

// Write a single hexadecimal 16 bits word
bool output_word(Output *output, uint16_t w) {
    bool ret = false;
    if(output_byte(output, w >> 8) != true) {
        // ...
    } else if(output_byte(output, w & 0xFFU) != true) {
        // ...
    } else {
        ret = true;
    }
    return ret;
}

// Write null terminated string
bool output_string(Output *output, const char *str) {
    bool ret;
    for(ret=true; ret && (*str!='\0'); str++) {
        ret = output_char(output, *str);
    }
    return ret;
}

// Write newline
bool output_newline(Output *output) {
    bool ret = false;
    if(fputc('\n', output->stream) == EOF) {
        ERROR_MSG("failed to print newline to %s: %s", output->filename, strerror(errno));
    } else {
        output->line++;
        output->column = 0;
        ret = true;
    }
    return ret;
}

// Print character up until given column
bool output_fill_to(Output *output, char c, size_t column) {
    bool ret;
    for(ret=true; ret && (output->column < column); ) {
        ret = output_char(output, c);
    }
    return ret;
}

// Print n character
bool output_fill_n(Output *output, char c, size_t n) {
    bool ret;
    for(ret=true; ret && (n > 0); --n) {
        ret = output_char(output, c);
    }
    return ret;
}

// Write inline comment 
bool output_inline_comment(Output *output, const char *str) {
    bool ret = true;
    if(str == NULL) {
        // ...
    } else {
        while (ret && (*str != '\0')) {
            ret = false;
            if(output_fill_to(output, ' ', ETRIPATOR_LAST_COLUMN) != true) {
                // ...
            } else if(output_char(output, ';') != true) {
                // ...
            } else if(output_char(output, ' ') != true) {
                // ...
            } else {
                for (ret = true; ret && (*str != '\0') && (*str != '\n'); ++str) {
                    ret = output_char(output, *str);
                }

                if (ret && (*str == '\n')) {
                    ++str;
                    if (*str) {
                        ret = output_newline(output);
                    }
                }
            }
        }
    }
    return ret;
}

// Write comment
bool output_comment(Output *output, const char *str) {
    bool ret = true;
    if(str == NULL) {
        // ...
    } else {
        while (ret && (*str != '\0')) {
            if((ret = output_char(output, ';')) == false) {
                // ...
            } else if((ret = output_char(output, ' ')) == false) {
                // ...     
            } else {
                for (; ret && (*str != '\0') && (*str != '\n'); str++) {
                    ret = output_char(output, *str);
                }

                if (ret) {
                    ret = output_newline(output);
                    if (*str != '\0') {
                        ++str;
                    }
                }
            }
        }
    }
    return ret;
}

// Print address as an inline comment
bool output_address_comment(Output *output, uint16_t page, uint16_t logical) {
    assert(output != NULL);

    bool ret = output_fill_to(output, ' ', ETRIPATOR_LAST_COLUMN);
    if(ret) {
        int n = fprintf(output->stream, "; page: $%03x logical: $%04x", page, logical);
        if(n != 27) {
            ERROR_MSG("failed to write comment to %s: %s", output->filename, strerror(errno));
            ret = false;
        }
        if(n > 0) {
            output->column += n;
        }
    }

    return ret;
}

// Print label
bool output_label(Output *output, Label *label) {
    assert(output != NULL);
    assert(label != NULL);
    assert(label->name != NULL);

    bool ret = true;

    if(label->description != NULL) {
        ret = output_comment(output, label->description);
    }

    if(ret) {
        label->line = output->line;
        label->column = output->column;
        label->file_id = output->id;
        
        ret = false;
        if(output_string(output, label->name) != true) {
            // ...
        } else if(output_char(output, ':') != true) {
            // ...
        } else {
            ret = output_address_comment(output, label->page, label->logical);
        }
    }

    return ret;
}
