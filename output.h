/*
    This file is part of Etripator,
    copyright (c) 2009--2018 Vincent Cruz.

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
#ifndef OUTPUT_H
#define OUTPUT_H

#include "config.h"

typedef int (*OutputInstruction)(void* impl, const uint8_t *data, size_t size); // instruction name, addressing mode
typedef int (*OutputLabelDecl)(void* impl, uint16_t page, uint32_t address, const char *name);
typedef int (*OutputVariableDecl)(void* impl, uint16_t page, uint32_t address, const char *name, size_t element_count, size_t element_size);
typedef int (*OutputData)(void* impl, uint16_t page, uint32_t address, const uint8_t *elements, size_t element_count, size_t element_size);
typedef int (*OutputComment)(void* impl, uint16_t page, uint32_t address, const char *comment);
#endif // OUTPUT_H
