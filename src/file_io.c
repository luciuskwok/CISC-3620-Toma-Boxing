//
//  file_io.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "file_io.h"

uint32_t read_uint32(FILE *file) {
	uint32_t x = 0;
	fread(&x, sizeof(x), 1, file);
	return x;
}

uint16_t read_uint16(FILE *file) {
	uint16_t x = 0;
	fread(&x, sizeof(x), 1, file);
	return x;
}

uint8_t read_uint8(FILE *file) {
	uint8_t x = 0;
	fread(&x, sizeof(x), 1, file);
	return x;
}

int32_t read_int32(FILE *file) {
	int32_t x = 0;
	fread(&x, sizeof(x), 1, file);
	return x;
}

int16_t read_int16(FILE *file) {
	int32_t x = 0;
	fread(&x, sizeof(x), 1, file);
	return x;
}

int8_t read_int8(FILE *file) {
	int32_t x = 0;
	fread(&x, sizeof(x), 1, file);
	return x;
}

bool read_buffer(FILE *file, void *buf, size_t buf_len) {
	return fread(buf, buf_len, 1, file) > 0;
}
