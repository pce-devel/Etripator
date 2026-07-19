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
#include <etripator/data.h>
#include <etripator/utils.h>

#include "data/driver.h"

void data_release(Data *data) {
    data_close(data);
    free(data);
}

bool data_open(Data *data) {
    SANITY_CHECK(data != NULL, false);
    DataDriver *driver = *(DataDriver**)data;
    SANITY_CHECK((driver != NULL) && (driver->open != NULL), false);
    return driver->open(data);
}

void data_close(Data *data) {
    SANITY_CHECK(data != NULL);
    DataDriver *driver = *(DataDriver**)data;
    SANITY_CHECK((driver != NULL) && (driver->close != NULL));
    driver->close(data);    
}

size_t data_tell(Data *data) {
    SANITY_CHECK(data != NULL, 0);
    DataDriver *driver = *(DataDriver**)data;
    SANITY_CHECK((driver != NULL) && (driver->tell != NULL), 0);
    return driver->tell(data);
}

size_t data_size(Data *data) {
    SANITY_CHECK(data != NULL, 0);
    DataDriver *driver = *(DataDriver**)data;
    SANITY_CHECK((driver != NULL) && (driver->size != NULL), 0);
    return driver->size(data);
}

bool data_seek(Data *data, ssize_t delta) {
    SANITY_CHECK(data != NULL, 0);
    DataDriver *driver = *(DataDriver**)data;
    SANITY_CHECK((driver != NULL) && (driver->seek != NULL), false);
    return driver->seek(data, delta);   
}

bool data_jump(Data *data, ssize_t offset) {
    SANITY_CHECK(data != NULL, 0);
    DataDriver *driver = *(DataDriver**)data;
    SANITY_CHECK((driver != NULL) && (driver->jump != NULL), false);
    return driver->jump(data, offset);   
}

bool data_read(Data *data, uint8_t *buffer, size_t size, size_t *nread) {
    SANITY_CHECK(data != NULL, 0);
    DataDriver *driver = *(DataDriver**)data;
    SANITY_CHECK((driver != NULL) && (driver->read != NULL), false);
    bool ret = false;
    if(buffer == NULL) {
        ERROR_MSG("Invalid buffer pointer.");
    } else if(size == 0) {
        ERROR_MSG("Invalid size.");
    } else {
        ret = driver->read(data, buffer, size, nread); 
    }
    return ret;
}

bool data_write(Data *data, const uint8_t *buffer, size_t size, size_t *nwritten){
    SANITY_CHECK(data != NULL, 0);
    DataDriver *driver = *(DataDriver**)data;
    SANITY_CHECK((driver != NULL) && (driver->write != NULL), false);
    bool ret = false;
    if(buffer == NULL) {
        ERROR_MSG("Invalid buffer pointer.");
    } else if(size == 0) {
        ERROR_MSG("Invalid size.");
    } else {
        ret = driver->write(data, buffer, size, nwritten); 
    }
    return ret;
}

bool data_print(Data *data, StringView s) {
    SANITY_CHECK(string_view_empty(s) == false, false);
    bool ret = false;
    size_t nwritten = 0;
    if(!data_write(data, (const uint8_t*)s.data, s.length, &nwritten)) {
        // ...
    } else if(nwritten != s.length) {
        ERROR_MSG("truncated output");
    } else {
        ret = true;
    }
    return ret;
}
