//
//  color.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/14/24.
//

#ifndef color_h
#define color_h

#include <stdint.h>

// ABGR color constants
#define COLOR_ABGR_BLACK 	(0xFF000000)
#define COLOR_ABGR_WHITE 	(0xFFFFFFFF)

// RGB color constants
#define COLOR_RGB_GRAY 		(0x888888)
#define COLOR_RGB_DARK_GRAY (0x444444)
#define COLOR_RGB_LIGHT_GRAY (0xBBBBBB)
#define COLOR_RGB_RED 		(0xFF0000)
#define COLOR_RGB_GREEN 	(0x00FF00)
#define COLOR_RGB_BLUE 		(0x0000FF)
#define COLOR_RGB_PINK 		(0xEA8193)
#define COLOR_RGB_LIME 		(0x99E550)
#define COLOR_RGB_LIGHT_BLUE (0x8888FF)

#define COLOR_RGB_TOMATO_RED (0xF68598)
#define COLOR_RGB_TOMATO_GREEN (0xD7E87F)
#define COLOR_RGB_DARK_GREEN (0x65A97C)
#define COLOR_RGB_DARK_RED (0xff5260)

/* More colors
	Light pink bkgnd:	#FAE4E2
	Dark pink bkgnd:	#FEB7BA
	Lime bkgnd:			#D0E675
	Toma skin light:	#FFF5F3
	Toma skin blush: 	#FFCFD0
	Hair lime light: 	#D5F469
	Hair lime dark: 	#989E5E
	Hair pink flat: 	#FFA6AC
	Hair pink dark: 	#DD656D
	Toemaniac red: 		#FF5260
	Toemaniac stem lite: #BCF845
	Toemaniac stem dark: #65A97C
 
 */


uint32_t blend_color(uint32_t x, uint32_t y);

uint32_t color_from_hsv(double h, double s, double v, double a);

uint32_t color_set_alpha(uint32_t c, uint8_t a);

uint32_t rgb_to_abgr(uint32_t x);

#endif /* color_h */
