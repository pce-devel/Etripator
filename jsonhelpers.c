#include <errno.h>
#include "jsonhelpers.h"

int json_validate_int(const json_t* obj, int* out) {
    if(json_is_string(obj)) {
        errno = 0;
        *out = strtoul(json_string_value(obj), NULL, 16);
        if(errno) {
            return 0;
        }
        return 1;
    }
    else if(json_is_integer(obj)) {
        *out = json_integer_value(obj);
        return 1;
    }
    return 0;
}

