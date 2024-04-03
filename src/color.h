//
//  color.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/14/24.
//

#ifndef color_h
#define color_h

#include <stdint.h>

// NOTE: pixel format is ABGR

#define COLOR_BLACK 	(0xFF000000)
#define COLOR_WHITE 	(0xFFFFFFFF)
#define COLOR_GRAY 		(0xFF888888)
#define COLOR_DARK_GRAY (0xFF444444)
#define COLOR_LIGHT_GRAY (0xFFBBBBBB)
#define COLOR_RED 		(0xff0000FF)
#define COLOR_GREEN 	(0xff00FF00)
#define COLOR_BLUE 		(0xffFF0000)
#define COLOR_PINK 		(0xff9381EA)
#define COLOR_LIME 		(0xff50E599)

uint32_t blend_color(uint32_t x, uint32_t y);

uint32_t color_from_hsv(double h, double s, double v, double a);

#endif /* color_h */
