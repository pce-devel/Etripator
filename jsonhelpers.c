#include "jsonhelpers.h"

#include <errno.h>
#include <string.h>

int json_validate_int(const json_t* obj, int* out) {
    int ret = 0;
    if(json_is_string(obj)) {
        errno = 0;
        *out = strtoul(json_string_value(obj), NULL, 16);
        if(errno == 0) {
            ret = 1;
        }
    } else if(json_is_integer(obj)) {
        *out = (int)json_integer_value(obj);
        ret = 1;
    }
    return ret;
}

char* json_load_description(const json_t* obj) {
    json_t *tmp = json_object_get(obj, "description");
    char *out = NULL;
    if(json_is_string(tmp)) {
        out = strdup(json_string_value(tmp));
    } else if (json_is_array(tmp)) {
        int index;
        json_t* value;

        size_t len = 0;
        json_array_foreach(tmp, index, value) {
            if(json_is_string(value)) {
                const char *str = json_string_value(value);
                if(out) {
                    out[len-1] = '\n';
                }
                size_t n = len + strlen(str) + 1;
                char *ptr = realloc(out, n);
                memcpy(ptr+len, str, strlen(str));
                ptr[n-1] = '\0';
                out = ptr;
                len = n;
            }
        }
    }
    return out;
}

void json_print_description(FILE *out, const char *key, const char *str) {
    fprintf(out, "\"%s\":[", key);
    while(*str) {
        fprintf(out, "\n\t\t\t\"");
        for(;*str && (*str != '\n'); str++) {
            fputc(*str, out);
        }
        fprintf(out, "\"%c", *str ? ',' : ' ');
        if(*str) str++;
    }
    fprintf(out, "\n\t]");
}
