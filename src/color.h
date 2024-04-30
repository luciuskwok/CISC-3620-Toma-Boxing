//
//  color.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/14/24.
//

#ifndef color_h
#define color_h

#include <stdint.h>

// ABGR color
#define COLOR_ABGR_BLACK 	(0xFF000000)
#define COLOR_ABGR_WHITE 	(0xFFFFFFFF)

// RGB color
#define COLOR_RGB_BLACK 		(0x000000)
#define COLOR_RGB_WHITE 		(0xFFFFFF)
#define COLOR_RGB_GRAY 			(0x888888)
#define COLOR_RGB_DARK_GRAY 	(0x444444)
#define COLOR_RGB_LIGHT_GRAY 	(0xBBBBBB)
#define COLOR_RGB_RED 			(0xFF0000)
#define COLOR_RGB_GREEN 		(0x00FF00)
#define COLOR_RGB_BLUE 			(0x0000FF)

#define COLOR_RGB_GRAY_90		(0xE5E5E5)
#define COLOR_RGB_GRAY_80		(0xCCCCCC)
#define COLOR_RGB_GRAY_70		(0xB2B2B2)
#define COLOR_RGB_GRAY_60		(0x999999)
#define COLOR_RGB_GRAY_50		(0x7F7F7F)
#define COLOR_RGB_GRAY_40		(0x666666)
#define COLOR_RGB_GRAY_30		(0x4C4C4C)
#define COLOR_RGB_GRAY_20		(0x333333)
#define COLOR_RGB_GRAY_10		(0x191919)

// Toma color palette from title background
#define COLOR_RGB_OUTLINE 		(0x68283C)
#define COLOR_RGB_SKIN_1 		(0xFAE6DC)
#define COLOR_RGB_SKIN_2 		(0xF5A9A5)
#define COLOR_RGB_PINK_1 		(0xFFCFD0)
#define COLOR_RGB_PINK_2 		(0xFEB7BA)
#define COLOR_RGB_PINK_3 		(0xF27F84)
#define COLOR_RGB_RED_1 		(0xFF5260)
#define COLOR_RGB_RED_2 		(0xFE7D92)
#define COLOR_RGB_RED_3 		(0xCB5D73)
#define COLOR_RGB_GREEN_1 		(0xDDFB8A)
#define COLOR_RGB_GREEN_2		(0x8FE2B7)
#define COLOR_RGB_GREEN_3 		(0x809B88)
#define COLOR_RGB_GREEN_4 		(0x65A97C)
#define COLOR_RGB_WHITE_3 		(0xBBC5E1)
#define COLOR_RGB_WHITE_2 		(0xF7F0F4)
#define COLOR_RGB_YELLOW_1		(0xFBF573)
#define COLOR_RGB_YELLOW_2		(0xFAE571)

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
uint32_t rgba_to_abgr(uint32_t hex, uint32_t alpha);

#endif /* color_h */
