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
#ifndef ETRIPATOR_JSON_HELPERS_H
#define ETRIPATOR_JSON_HELPERS_H

#include "config.h"

#include <jansson.h>

int json_validate_int(const json_t* obj, int* out);
char* json_load_description(const json_t* obj);
void json_print_description(FILE *out, const char *key, const char *str);

#endif // ETRIPATOR_JSON_HELPERS_H