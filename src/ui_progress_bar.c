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
	int w = scr_w * 75 / 100;
	int h = 10;
	int x = (scr_w - w) / 2;
	int y = 0;

	set_line_color(COLOR_WHITE);
	stroke_rect(x, y, w, h);
	
	// Inset rect
	w -= 2;
	h -= 2;
	x += 1;
	y += 1;
	w = (int)( round((double)w * ui_progress_value) );
	set_fill_color(COLOR_LIGHT_GRAY);
	fill_rect(x, y, w, h);
}

void set_progress_value(double x) {
	ui_progress_value = x;
}

