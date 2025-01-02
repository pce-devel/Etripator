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
#ifndef ETRIPATOR_OUTPUT_H
#define ETRIPATOR_OUTPUT_H

#include "config.h"

#include "label.h"

/// File output
typedef struct {
    uint32_t id;    //< Id in the output registry.
    char *filename; //< Output filename.
    FILE *stream;   //< File stream.
    size_t line;    //< Current line.
    size_t column;  //< Cursor position in the current line.
} Output;

/// Output registry
typedef struct {
    Output *data;       //< Pointer to output array.
    size_t count;       //< Number of output currently in use.
    size_t capacity;    //< Number the output array can hold.
} OutputRegistry;

/// Add a new output file to registry.
/// \param [in out] registry Output registry
/// \param [in] filename Filename to be added
/// \return true if the file was succesfully added or if it is already present.
/// \return false if an error occured.
bool output_registry_add(OutputRegistry *registry, const char *filename);
/// Destroy output registry.
/// \param [in out] registry Output registry
void output_registry_destroy(OutputRegistry *registry);
/// Find the registry entry associated to a given filename.
/// \param [in out] registry Output registry
/// \param [in] filename Filename to be added
/// \param [out] id Index of the entry in the registry (if found)
/// \return true if the entry was found
/// \return false otherwise
bool output_find(const OutputRegistry *registry, const char *filename, uint32_t *id);

/// Open output file for writing data at the end of the file.
/// \param [in out] output File output
/// \return true if the file was succesfully opened
/// \return false if an error occured
bool output_begin(Output *output);
/// Close output file
void output_end(Output *output);

/// Write single char
/// \param [in out] output File output
/// \param [in] c Character to be output
/// \return true if the character was succesfully written
/// \return false if an error occured
bool output_char(Output *output, char c);

/// Write a single hexadecimal byte
/// \param [in] b Byte to be output
/// \return true if the byte was succesfully written
/// \return false if an error occured
bool output_byte(Output *output, uint8_t c);

/// Write null terminated string
/// \param [in out] output File output
/// \param [in] str Null terminated string to be output
/// \return true if the string was succesfully written
/// \return false if an error occured
bool output_string(Output *output, const char *str);

/// Write newline char
/// \param [in out] output File output
/// \return true if the newline was succesfully written
/// \return false if an error occured
bool output_newline(Output *output);

/// Print character up until given column
/// \param [in out] output File output
/// \param [in] c Filling character
/// \param [in] column Target column
/// \return true if the characters were succesfully written
/// \return false if an error occured
bool output_fill_to(Output *output, char c, size_t column);

/// Print n character
/// \param [in out] output File output
/// \param [in] c Filling character
/// \param [in] n character count
/// \return true if the characters were succesfully written
/// \return false if an error occured
bool output_fill_n(Output *output, char c, size_t n);

/// Write inline comment
/// \param [in out] output File output
/// \param [in] str Comment string (can contain newline characters)
/// \return true if the comment was succesfully written
/// \return false if an error occured
bool output_inline_comment(Output *output, const char *str);

/// Write comment
/// \param [in out] output File output
/// \param [in] str Comment string (can contain newline characters)
/// \return true if the comment was succesfully written
/// \return false if an error occured
bool output_comment(Output *output, const char *str);

/// Print address as an inline comment
/// \param [in out] output File output
/// \param [in] page Memory page
/// \param [in] logical Logical address
/// \return true if the comment was succesfully written
/// \return false if an error occured
bool output_address_comment(Output *output, uint16_t page, uint16_t logical);

/// Print label
/// \param [in out] output File output 
/// \param [in] label Lebel to be printed 
/// \return true if the label was succesfully written
/// \return false if an error occured
bool output_label(Output *output, Label *label);

#endif // ETRIPATOR_OUTPUT_H