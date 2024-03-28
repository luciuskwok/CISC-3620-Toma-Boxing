//
//  file_io.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#ifndef file_io_h
#define file_io_h

#include <stdio.h>
#include <stdbool.h>

uint32_t read_int(FILE *file);
bool read_buffer(FILE *file, void *buf, size_t buf_len);

#endif /* file_io_h */
