//
//  file_io.c
//  toma-boxing-tools
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "file_io.h"


size_t write_uint32(FILE *file, uint32_t x) {
	size_t num_members_written = fwrite(&x, sizeof(x), 1, file);
	return sizeof(x) * num_members_written;
}

size_t write_uint8(FILE *file, uint8_t x) {
	size_t num_members_written = fwrite(&x, sizeof(x), 1, file);
	return sizeof(x) * num_members_written;
}

size_t write_buffer(FILE *file, void *buf, size_t buf_len) {
	size_t num_members_written = fwrite(buf, buf_len, 1, file);
	return buf_len * num_members_written;
}
