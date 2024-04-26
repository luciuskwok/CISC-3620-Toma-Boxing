//
//  ui_progress_bar.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/29/24.
//

#include "ui_progress_bar.h"
#include "drawing.h"
#include "color.h"
#include <math.h>


// Globals
double ui_progress_value = 0.0f;

void progress_bar_init(void) {
	
}

void draw_progress_bar(void) {
	// Draws progress bar at top of screen
	int scr_w = get_screen_width();
	//int scr_h = get_screen_height();
	rectangle_t r;
	r.w = scr_w * 75 / 100;
	r.h = 6;
	r.x = (scr_w - r.w) / 2;
	r.y = 4;

	// Border
	set_fill_color_rgba(COLOR_RGB_BLACK, 127);
	fill_rect(r);
	
	// Empty area
	r = inset_rect(r, 2, 2);
	set_fill_color_rgba(COLOR_RGB_WHITE, 127);
	fill_rect(r);
	
	// Filled area
	r.w = (int)( round((double)r.w * ui_progress_value) );
	set_fill_color_rgba(COLOR_RGB_RED, 255);
	fill_rect(r);
}

void set_progress_value(double x) {
	ui_progress_value = x;
}

