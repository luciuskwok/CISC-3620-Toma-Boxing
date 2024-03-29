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

uint32_t read_uint32(FILE *file);
uint16_t read_uint16(FILE *file);
uint8_t read_uint8(FILE *file);

int32_t read_int32(FILE *file);
int16_t read_int16(FILE *file);
int8_t read_int8(FILE *file);

bool read_buffer(FILE *file, void *buf, size_t buf_len);

#endif /* file_io_h */
