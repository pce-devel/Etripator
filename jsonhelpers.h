#ifndef ETRIPATOR_JSON_HELPERS_H
#define ETRIPATOR_JSON_HELPERS_H

#include <jansson.h>

int json_validate_int(const json_t* obj, int* out);

#endif // ETRIPATOR_JSON_HELPERS_H