//
//  main.c
//  toma-boxing-tools
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "image_convert.h"
#include "font_convert.h"

#include <stdio.h>
#include <SDL2/SDL.h>


void generate_default_atari_font_png(void) {
	const char *png = "/Users/lucius/_allSync/Academics/2024.1 Spring Courses/CISC 3620 Graphics/Toma-Boxing/(Artwork)/atari_font.png";
	write_atari_font_to_png(png);
}

void create_custom_font_data(void) {
	const char *png = "/Users/lucius/_allSync/Academics/2024.1 Spring Courses/CISC 3620 Graphics/Toma-Boxing/(Artwork)/custom_font.png";
	const char *data = "/Users/lucius/_allSync/Academics/2024.1 Spring Courses/CISC 3620 Graphics/Toma-Boxing/assets/custom_font";
	convert_png_to_font_data(png, data);
}

void convert_png_to_indexed_image(void) {
	const char input[] = "/Users/lucius/_allSync/Academics/2024.1 Spring Courses/CISC 3620 Graphics/Toma-Boxing/(Artwork)/title.png";
	const char output[] = "/Users/lucius/_allSync/Academics/2024.1 Spring Courses/CISC 3620 Graphics/Toma-Boxing/(Artwork)/title.idximg";
	convert_png(input, output);
}

int main(int argc, const char * argv[]) {
	//convert_png_to_indexed();
	create_custom_font_data();
	
	return 0;
}
