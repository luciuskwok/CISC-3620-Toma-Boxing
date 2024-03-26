//
//  image.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/26/24.
//

#ifndef image_h
#define image_h

#include <stdbool.h>
#include <stdint.h>

// Image type
typedef struct {
	uint32_t *pixels;
	uint32_t w;
	uint32_t h;
} image_t;

image_t *load_image(const char *file);
void free_image(image_t *image);
void draw_image(image_t *image);

#endif /* image_h */