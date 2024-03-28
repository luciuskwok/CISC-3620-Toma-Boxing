//
//  main.c
//  toma-boxing-tools
//
//  Created by Lucius Kwok on 3/28/24.
//

#include <stdio.h>
#include "image_convert.h"
#include "font_convert.h"


void convert_png_to_indexed(void) {
	const char input[] = "/Users/lucius/_allSync/Academics/2024.1 Spring Courses/CISC 3620 Graphics/Toma-Boxing/(Artwork)/riverside_1x.png";
	const char output[] = "/Users/lucius/_allSync/Academics/2024.1 Spring Courses/CISC 3620 Graphics/Toma-Boxing/(Artwork)/riverside_1x.indexed_image";
	convert_png(input, output);

}

int main(int argc, const char * argv[]) {
	
	const char *png = "/Users/lucius/_allSync/Academics/2024.1 Spring Courses/CISC 3620 Graphics/Toma-Boxing/(Artwork)/atari_font.png";
	const char *data = "/Users/lucius/_allSync/Academics/2024.1 Spring Courses/CISC 3620 Graphics/Toma-Boxing/assets/font.data";
	
	//write_atari_font_to_png(png);
	convert_png_to_font_data(png, data);
	
	//printf("Size of char: %ld\n", sizeof(char));
	
	return 0;
}
