//
//  file_io.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "file_io.h"

uint32_t read_int(FILE *file) {
	uint32_t x = 0;
	fread(&x, sizeof(x), 1, file);
	return x;
}

bool read_buffer(FILE *file, void *buf, size_t buf_len) {
	return fread(buf, buf_len, 1, file) > 0;
}
