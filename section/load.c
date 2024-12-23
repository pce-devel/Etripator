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
#include "../section.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <jansson.h>

#include "../jsonhelpers.h"
#include "../message.h"

static inline SectionType json_validate_section_type(const char *str) {
    for (int i = 0; i < SECTION_TYPE_COUNT; i++) {
        if (strncmp(section_type_name(i), str, strlen(section_type_name(i))) == 0) {
            return (SectionType)i;
        }
    }
    return SECTION_TYPE_UNKNOWN;
}

static inline DataType json_validate_data_type(const char *str) {
    for (int i = 0; i < DATA_TYPE_COUNT; i++) {
        if (strncmp(data_type_name(i), str, strlen(data_type_name(i))) == 0) {
            return (DataType)i;
        }
    }
    return DATA_TYPE_UNKNOWN;
}

static bool json_parse_section_type(Section *out, const json_t *obj) {
    bool ret = false;
    const json_t *tmp = json_object_get(obj, "type");
    if (tmp == NULL) {
        ERROR_MSG("Missing section type.");
    } else if (!json_is_string(tmp)) {
        ERROR_MSG("Invalid section type.");
    } else {
        out->type = json_validate_section_type(json_string_value(tmp));     
        if (out->type == SECTION_TYPE_UNKNOWN) {
            ERROR_MSG("Invalid section type value.");
        } else {
            ret =  true;
        }
    }
    return ret;
}

static bool json_parse_section_page(Section *out, const json_t *obj) {
    bool ret = false;
    int num = -1;
    const json_t *tmp = json_object_get(obj, "page");
    if (tmp == NULL) {
        ERROR_MSG("Missing section page");
    } else if (!json_validate_int(tmp, &num)) {
        ERROR_MSG("Invalid section memory page.");
    } else if ((num < 0) || (num > 255)) {
        ERROR_MSG("Section memory page out of range.");
    } else {
        out->page = (uint8_t)num;
        ret = true;
    }
    return ret;
}

static bool json_parse_section_logical(Section *out, const json_t *obj) {
    bool ret = false;
    int num = -1;
    const json_t *tmp = json_object_get(obj, "logical");
    if (tmp == NULL) {
        ERROR_MSG("Missing section logical address");
    } else if (!json_validate_int(tmp, &num)) {
        ERROR_MSG("Invalid or missing section logical address.");
    } else if ((num < 0) || (num > 0xFFFF)) {
        ERROR_MSG("Section logical address out of range.");
    } else {
        out->logical = (uint16_t)num;
        ret = true;
    }
    return ret;
}

static bool json_parse_section_offset(Section *out, const json_t *obj) {
    bool ret = true;
    int num = 1;
    json_t *tmp = json_object_get(obj, "offset");
    if(tmp == NULL) {
        out->offset = (out->page << 13) | (out->logical & 0x1FFF);
    } else if (json_validate_int(tmp, &num)) {
        out->offset = (uint32_t)num;
    } else {
        ERROR_MSG("Invalid or missing offset value.");
        ret = false;
    }
    return ret;
}

static bool json_parse_section_size(Section *out, const json_t *obj) {
    bool ret = true;
    int num = -1;
    const json_t *tmp = json_object_get(obj, "size");
    if (tmp == NULL) {
        // use default value
    } else if (json_validate_int(tmp, &num)) {
        out->size = (int32_t)num;
    } else {
        ERROR_MSG("Invalid section size.");
        ret = false;
    }
    return ret;
}

static bool json_parse_section_mpr(Section *out, const json_t *obj) {
    bool ret = false;
    const json_t *tmp = json_object_get(obj, "mpr");
    if(tmp == NULL) {
        ERROR_MSG("Missing section memory map");
    } else if(!json_is_array(tmp)) {
        ERROR_MSG("Invalid section memory map");
    } else {
        int num;
        size_t index;
        json_t* value;
        json_array_foreach(tmp, index, value) {
            ret = false;
            if(index > 7U) {
                ERROR_MSG("Too many mpr values");
            } else if(!json_validate_int(value, &num)) {
                ERROR_MSG("Invalid type for mpr value");
            } else if((num < 0) || (num > 0xFF)) {
                ERROR_MSG("Invalid mpr %d value", index);    
            } else {
                out->mpr[index] = (uint8_t)num;
                ret = true;
            }
            if(!ret) {
                break;
            }
        }
    }
    return ret;
}

static bool json_parse_section_output(Section *out, const json_t *obj) {
    bool ret = false;
    const json_t *tmp = json_object_get(obj, "filename");
    if (tmp == NULL) {
        ERROR_MSG("Misssing section output filename");
    } else if (!json_is_string(tmp)) {
        ERROR_MSG("Invalid output filename.");
    } else {
        out->output = strdup(json_string_value(tmp));
        if(out->output == NULL) {
            ERROR_MSG("Failed to parse section output filename: %s", strerror(errno));
        } else {
            ret = true;
        }
    }
    return ret;
}

static bool json_parse_data_config_type(DataConfig *out, const json_t *obj) {
    bool ret = false;
    const json_t *tmp = json_object_get(obj, "type");
    if (tmp == NULL) {
        ERROR_MSG("Missing section data type.");
    } else if (!json_is_string(tmp)) {
        ERROR_MSG("Invalid section data type.");
    } else {
        out->type = json_validate_data_type(json_string_value(tmp));     
        if (out->type == DATA_TYPE_UNKNOWN) {
            ERROR_MSG("Invalid section data type value.");
        } else {
            ret =  true;
        }
    }
    return ret;
}

static bool json_parse_data_config_element_size(DataConfig *out, const json_t *obj) {
    bool ret = false;
    const json_t *value = json_object_get(obj, "element_size");
    if(value == NULL) {
        ret = true; // use default value
    } else if(out->type != DATA_TYPE_HEX) {
        ERROR_MSG("Element size is only valid for hex data type");
    } else if(!json_validate_int(value, &out->element_size)) {
        ERROR_MSG("Invalid element size");
    } else if((out->element_size < 0) || (out->element_size > 2)) {
        ERROR_MSG("Invalid element size value: %d", out->element_size);
    } else {
        ret = true;
    }
    return ret;
}

static bool json_parse_data_config_element_per_line(DataConfig *out, const json_t *obj) {
    bool ret = false;
    const json_t *value = json_object_get(obj, "elements_per_line");
    if(value == NULL) {
        ret = true; // use default value
    } else if(out->type != DATA_TYPE_HEX) {
        ERROR_MSG("Number of elements per line is only valid for hex data section");
    } else if (!json_validate_int(value, &out->elements_per_line)) {
        ERROR_MSG("Invalid number of elements per line.");
    } else {
        ret = true;
    }
    return ret;
}

static bool json_parse_section_data_string_delimiter(DataConfig *out, const json_t *obj) {
    bool ret = false;
    const json_t *tmp = json_object_get(obj, "delimiter");
    if(tmp == NULL) {
        ret = true; // use default value
    } else if(out->type != DATA_TYPE_STRING) {
        ERROR_MSG("string delimiter is only valid for string data section");
    } else if(!json_is_array(tmp)) {
        ERROR_MSG("Invalid string delimiter");
    } else {
        int num;
        size_t index;
        json_t* value;
        json_array_foreach(tmp, index, value) {
            ret = false;
            if(index > 7U) {
                ERROR_MSG("too many char in string delimiter");
            } else if(!json_validate_int(value, &num)) {
                ERROR_MSG("Invalid type for string delimiter char");
            } else if((num < 0) || (num > 0xFF)) {
                ERROR_MSG("Invalid char %d value", index);    
            } else {
                out->delimiter[index] = (uint8_t)num;
                ret = true;
            }
            if(!ret) {
                break;
            }
        }
        out->delimiter_size = index;
    }
    return ret;
}

static bool json_parse_section_data_config(Section *out, const json_t *obj) {
    bool ret = false;
    const json_t *tmp = json_object_get(obj, "data"); 
    if (tmp == NULL) {
        ret = true;
    } else if(out->type != SECTION_TYPE_DATA) {
        ERROR_MSG("Data configuration only valid for data sections");
    } else if(!json_parse_data_config_type(&out->data, tmp)) {
        // ...
    } else if(!json_parse_data_config_element_size(&out->data, tmp)) {
        // ...
    } else if(json_parse_data_config_element_per_line(&out->data, tmp) == false) {
        // ...
    } else if(json_parse_section_data_string_delimiter(&out->data, tmp) == false) {
        // ...
    } else {
        ret = true;
    }
    return ret;
}

/// Initializes section from JSON object.
/// \param [in] obj JSON object.
/// \param [out] out Section.
/// \return 1 upon success
/// \return 0 if an error occured.
static bool section_parse(Section *out, const json_t *obj) {
    assert(obj != NULL);
    assert(out != NULL);
    
    bool ret = false;
    
    section_reset(out);
    if (!json_is_object(obj)) {
        ERROR_MSG("Invalid json element.");
    } else if(!json_parse_section_type(out, obj)) {
        // ...
    } else if(!json_parse_section_page(out, obj)) {
        // ...
    } else if(!json_parse_section_logical(out, obj)) {
        // ...
    } else if(!json_parse_section_offset(out, obj)) {
        // ...
    } else if(!json_parse_section_size(out, obj)) {
        // ...
    } else if(!json_parse_section_mpr(out, obj)) {

    } else if(!json_parse_section_output(out, obj)) {
        // ...
    } else if(!json_parse_section_data_config(out, obj)) {
        // ...
    } else {
        // description (optional)
        ret = json_load_description(obj, "description", &out->description);
    }
    return ret;
}

// Load sections from a JSON file.
bool section_load(SectionArray *arr, const char *filename) {
    bool ret = false;
    json_error_t err;
    Section *ptr;
    const char* key;
    
    section_array_reset(arr);

    json_t* root = json_load_file(filename, 0, &err);
    if(root == NULL) {
        ERROR_MSG("Failed to parse %s:%d:%d: %s", filename, err.line, err.column, err.text);
    } else {
        if(!json_is_object(root)) {
            ERROR_MSG("Invalid root element");
        } else {
            json_t* obj = NULL;
            size_t size = json_object_size(root);
            
            ret = true;
            json_object_foreach(root, key, obj) {
                Section tmp = {0};
                section_reset(&tmp);

                if(!section_parse(&tmp, obj)) {
                    ret = false;
                } else {
                    tmp.name = strdup(key);
                    if(section_array_add(arr, &tmp) <= 0) {
                        section_delete(&tmp, 1);
                    }                        
                }
            }
        }
        json_decref(root);
    }
    return ret;
}
