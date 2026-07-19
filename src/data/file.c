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

#ifndef PATH_MAX
#   define PATH_MAX (1024U)
#endif

#define MODE_MAX (8U)

typedef struct {
    const DataDriver *driver;
    char path[PATH_MAX];
    char mode[MODE_MAX];
    FILE *stream;
    enum {
        DATA_FILE_STATE_NONE,
        DATA_FILE_STATE_OPENED,
    } state;
} DataFile;

static bool data_file_open(Data *data) {
    bool ret = false;
    DataFile *impl = (DataFile*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data file.");
    } else if(impl->state != DATA_FILE_STATE_NONE) {
        ERROR_MSG("invalid state.");
    } else {
        impl->stream = fopen(impl->path, impl->mode);
        if(impl->stream == NULL) {
            ERROR_MSG("failed to open %s: %s", impl->path, strerror(errno));
        } else {
            ret = true;
        }
    }
    return ret;
}

static void data_file_close(Data *data) {
    DataFile *impl = (DataFile*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data file.");
    } else if(impl->state != DATA_FILE_STATE_OPENED) {
        WARNING_MSG("invalid state.");
    } else {
        fclose(impl->stream);
        impl->stream = NULL;
        impl->state = DATA_FILE_STATE_NONE;
    }
}

size_t data_file_tell(Data *data) {
    size_t ret = 0;
    DataFile *impl = (DataFile*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data file.");
    } else if(impl->state != DATA_FILE_STATE_OPENED) {
        ERROR_MSG("invalid state.");
    } else {
        ssize_t offset = ftell(impl->stream);
        if(offset < 0) {
            ERROR_MSG("failed to get file position indicator on %s: %s", impl->path, strerror(errno));
        } else {
            ret = (size_t)offset;
        }
    }
    return ret;
}

static size_t data_file_size(Data *data) {
    DataFile *impl = (DataFile*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data file.");
        return 0;
    }
    size_t ret = 0;

    // Compute file size.
    struct stat infos = {0};
    int fd = fileno(impl->stream);
    if (fd < 0) {
        ERROR_MSG("Failed to retrieve file descriptior for %s : %s", impl->path, strerror(errno));
    } else if (fstat(fd, &infos) < 0) {
        ERROR_MSG("Failed to retrieve file informations of %s : %s", impl->path, strerror(errno));
    } else {
        ret = infos.st_size;
    }
    return ret;
}

static bool data_file_seek(Data *data, ssize_t delta) {
    bool ret = false;
    DataFile *impl = (DataFile*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data file.");
    } else if(impl->state != DATA_FILE_STATE_OPENED) {
        ERROR_MSG("invalid state.");
    } else if(fseek(impl->stream, delta, SEEK_CUR) != 0) {
        ERROR_MSG("failed to seek to requested position in %s: %s", impl->path, strerror(errno));
    } else {
        ret = true;
    }
    return ret;
}

bool data_file_jump(Data *data, ssize_t offset) {
    bool ret = false;
    DataFile *impl = (DataFile*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data file.");
    } else if(impl->state != DATA_FILE_STATE_OPENED) {
        ERROR_MSG("invalid state.");
    } else if(fseek(impl->stream, offset, (offset<0) ? SEEK_END : SEEK_SET) != 0) {
        ERROR_MSG("failed to jump to requested position in %s: %s", impl->path, strerror(errno));
    } else {
        ret = true;
    }
    return ret;
}

static bool data_file_read(Data *data, uint8_t *buffer, size_t size, size_t *nread) {
    SANITY_CHECK((buffer != NULL) && (size != 0), false);
    bool ret = false;
    DataFile *impl = (DataFile*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data file.");
    } else if(impl->stream == NULL) {
        ERROR_MSG("invalid data file stream.");
    } else if(impl->state != DATA_FILE_STATE_OPENED) {
        ERROR_MSG("invalid state.");
    } else {
        size_t n = fread(buffer, 1, size, impl->stream);
        int error = ferror(impl->stream);
        if(error) {
            ERROR_MSG("failed to read data from %s: %s", impl->path, strerror(error));
        } else {
            if(*nread) {
                if(feof(impl->stream)) {
                    *nread = 0;
                } else {
                    *nread = n;
                }
            }
            ret = true;
        }
    }
    return ret;
}

static bool data_file_write(Data *data, const uint8_t *buffer, size_t size, size_t *nwritten) {
    SANITY_CHECK((buffer != NULL) && (size != 0), false);
    bool ret = false;
    DataFile *impl = (DataFile*)data;
    if(impl == NULL) {
        ERROR_MSG("invalid data file.");
    } else if(impl->stream == NULL) {
        ERROR_MSG("invalid data file stream.");
    } else if(impl->state != DATA_FILE_STATE_OPENED) {
        ERROR_MSG("invalid state.");
    } else {
        size_t n = fwrite(buffer, 1, size, impl->stream);
        int error = ferror(impl->stream);
        if(error) {
            ERROR_MSG("failed to write data to %s: %s", impl->path, strerror(error));
        } else {
            if(*nwritten) {
                if(feof(impl->stream)) {
                    *nwritten = 0;
                } else {
                    *nwritten = n;
                }
            }
            ret = true;
        }
    }
    return ret;
}

static const DataDriver g_data_file_driver = {
    .open = data_file_open,
    .close = data_file_close,
    .tell = data_file_tell,
    .size = data_file_size,
    .seek = data_file_seek,
    .jump = data_file_jump,
    .read = data_file_read,
    .write = data_file_write,
};

Data* data_file_create(const char *filepath, const char *mode) {
    DataFile* out = malloc(sizeof(DataFile));
    *out = (DataFile) {
        .driver = &g_data_file_driver,
    };
    size_t i;
    for(i=0; (i<PATH_MAX) && (filepath[i] != '\0'); i++) {
        out->path[i] = filepath[i];
    }
    if(i == PATH_MAX) {
        ERROR_MSG("invalid file path");
        free(out);
        return NULL;
    }
    for(i=0; i<PATH_MAX; i++) {
        out->path[i] = '\0';
    }
    for(i=0; (i<MODE_MAX) && (mode[i] != '\0'); i++) {
        out->mode[i] = mode[i];
    }
    if(i == MODE_MAX) {
        ERROR_MSG("invalid mode");
        free(out);
        return NULL;
    }
    for(i=0; i<MODE_MAX; i++) {
        out->mode[i] = '\0';
    }
    return (Data*)out;
}
