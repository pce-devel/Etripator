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
#include <etripator/message.h>
#include "driver.h"

// [todo] allow size expansion?
typedef struct {
    const DataDriver *driver;
    uint8_t* buffer;
    size_t size;
    size_t offset;
    enum {
        DATA_BUFFER_STATE_NONE,
        DATA_BUFFER_STATE_OPENED,
    } state;
} DataBuffer;

static bool data_buffer_open(Data *data) {
    bool ret = false;
    DataBuffer *impl = (DataBuffer*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data buffer.");
    } else if(impl->buffer == NULL) {
        ERROR_MSG("invalid data buffer storage.");
    } else if(impl->size == 0) {
        ERROR_MSG("invalid data buffer size.");
    } else if(impl->state != DATA_BUFFER_STATE_NONE) {
        ERROR_MSG("invalid state.");
    } else {
        impl->state = DATA_BUFFER_STATE_OPENED;
        impl->offset = 0;
        ret = true;
    }
    return ret;
}

static void data_buffer_close(Data *data) {
    DataBuffer *impl = (DataBuffer*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data buffer.");
    } else if(impl->state != DATA_BUFFER_STATE_OPENED) {
        WARNING_MSG("invalid state.");
    } else {
        impl->state = DATA_BUFFER_STATE_NONE;
    }
}

size_t data_buffer_tell(Data *data) {
    size_t ret = 0;
    DataBuffer *impl = (DataBuffer*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data buffer.");
    } else if(impl->state != DATA_BUFFER_STATE_OPENED) {
        ERROR_MSG("invalid state.");
    } else {
        ret = impl->offset;
    }
    return ret;
}

static size_t data_buffer_size(Data *data) {
    size_t ret = 0;
    DataBuffer *impl = (DataBuffer*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data buffer.");
    } else {
        ret = impl->size;
    }
    return ret;
}

static bool data_buffer_seek(Data *data, ssize_t delta) {
    bool ret = false;
    DataBuffer *impl = (DataBuffer*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data buffer.");
    } else if(impl->state != DATA_BUFFER_STATE_OPENED) {
        ERROR_MSG("invalid state.");
    } else {
        ssize_t offset = impl->offset + delta;
        if((offset < 0) || (((size_t)offset) >= impl->size)) {
            ERROR_MSG("offset jumps out of data buffer bounds.");
        } else {
            impl->offset = offset;
            ret = true;
        }
    }
    return ret;
}

bool data_buffer_jump(Data *data, ssize_t offset) {
    bool ret = false;
    DataBuffer *impl = (DataBuffer*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data buffer.");
    } else if(impl->state != DATA_BUFFER_STATE_OPENED) {
        ERROR_MSG("invalid state.");
    } else {
        ssize_t jump;
        if(offset >= 0) {
            jump = offset;
        } else {
            jump = impl->size + offset;
        }
        if((jump < 0) || (((size_t)jump) >= impl->size)) {
            ERROR_MSG("offset jumps out of data buffer bounds.");
        } else {
            impl->offset = jump;
            ret = true;
        }
    }
    return ret;
}

static bool data_buffer_read(Data *data, uint8_t *buffer, size_t size, size_t *nread) {
    SANITY_CHECK((buffer != NULL) && (size != 0), false);
    bool ret = false;
    DataBuffer *impl = (DataBuffer*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data buffer.");
    } else if(impl->buffer == NULL) {
        ERROR_MSG("invalid data buffer storage.");
    } else if(impl->state != DATA_BUFFER_STATE_OPENED) {
        ERROR_MSG("invalid state.");
    } else {
        size_t count = impl->size - impl->offset;
        if(count > size) {
            count = size;
        }
        if(count) {
            memcpy(buffer, impl->buffer + impl->offset, count);
            impl->offset += count;
        }
        if(nread != NULL) {
            *nread = count;
        }
        ret = true;
    }
    return ret;
}

static bool data_buffer_write(Data *data, const uint8_t *buffer, size_t size, size_t *nwritten) {
    SANITY_CHECK((buffer != NULL) && (size != 0), false);

    bool ret = false;
    DataBuffer *impl = (DataBuffer*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data buffer.");
    } else if(impl->buffer == NULL) {
        ERROR_MSG("invalid data buffer storage.");
    } else if(impl->state != DATA_BUFFER_STATE_OPENED) {
        ERROR_MSG("invalid state.");
    } else {
        size_t count = impl->size - impl->offset;
        if(count > size) {
            count = size;
        }
        if(count) {
            memcpy(impl->buffer + impl->offset, buffer, count);
            impl->offset += count;
        }
        if(nwritten != NULL) {
            *nwritten = count;
        }
        ret = true;
    }
    return ret;
}

static const DataDriver g_data_buffer_driver = {
    .open = data_buffer_open,
    .close = data_buffer_close,
    .tell = data_buffer_tell,
    .size = data_buffer_size,
    .seek = data_buffer_seek,
    .jump = data_buffer_jump,
    .read = data_buffer_read,
    .write = data_buffer_write,
};

Data* data_buffer_create(uint8_t *buffer, size_t size) {
    if((buffer == NULL) || (size == 0)) {
        ERROR_MSG("Invalid parameters.");
        return NULL;
    }
    DataBuffer* out = malloc(sizeof(DataBuffer));
    *out = (DataBuffer) {
        .driver = &g_data_buffer_driver,
        .buffer = buffer,
        .size = size,
        .offset = 0,
    };
    return (Data*)out;
}
