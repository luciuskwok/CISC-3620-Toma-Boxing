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
	r.h = 10;
	r.x = (scr_w - r.w) / 2;
	r.y = 0;

	set_line_color(COLOR_WHITE);
	stroke_rect(r);
	
	// Inset rect
	r.w -= 2;
	r.h -= 2;
	r.x += 1;
	r.y += 1;
	r.w = (int)( round((double)r.w * ui_progress_value) );
	set_fill_color(COLOR_LIGHT_BLUE);
	fill_rect(r);
}

void set_progress_value(double x) {
	ui_progress_value = x;
}

