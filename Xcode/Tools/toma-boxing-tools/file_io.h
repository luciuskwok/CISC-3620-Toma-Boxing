//
//  file_io.h
//  toma-boxing-tools
//
//  Created by Lucius Kwok on 3/28/24.
//

#ifndef file_io_h
#define file_io_h

#include <stdio.h>

size_t write_uint32(FILE *file, uint32_t x);
size_t write_uint8(FILE *file, uint8_t x);
size_t write_buffer(FILE *file, void *buf, size_t buf_len);

#endif /* file_io_h */
