//
//  color.c
//  SDL_Xcode
//
//  Created by Lucius Kwok on 3/14/24.
//

#include "color.h"
#include <math.h>


uint32_t color_from_rgba_int(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (a << 24) | (b << 16) | (g << 8) | r;
}

uint32_t color_from_rgba_float(float r, float g, float b, float a) {
    return ((int)(a * 255.0) << 24) | ((int)(b * 255.0) << 16) |
        ((int)(g * 255.0) << 8) | (int)(r * 255.0);
}

uint32_t blend_color(uint32_t x, uint32_t y) {
    // This function blends y on top of x, ignoring the alpha of x
    
    // Decompose into channels
    uint32_t ya = (y & 0xFF000000) >> 24;
    uint32_t yb = (y & 0x00FF0000) >> 16;
    uint32_t yg = (y & 0x0000FF00) >> 8;
    uint32_t yr = (y & 0x000000FF) >> 0;

    uint32_t xa = 255 - ya;
    uint32_t xb = (x & 0x00FF0000) >> 16;
    uint32_t xg = (x & 0x0000FF00) >> 8;
    uint32_t xr = (x & 0x000000FF) >> 0;
        
    // Apply alpha
    uint32_t zb = (xb * xa + yb * ya) / 255;
    uint32_t zg = (xg * xa + yg * ya) / 255;
    uint32_t zr = (xr * xa + yr * ya) / 255;
   
    // Clamp to max
    zb = (zb < 255)? zb : 255;
    zg = (zg < 255)? zg : 255;
    zr = (zr < 255)? zr : 255;
    
    return color_from_rgba_int(zr, zg, zb, 255);
}

uint32_t color_from_hsv(float h, float s, float v, float a) {
    // Adapted from: https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
    
    double hh, p, q, t, ff;
    long i;
    float r, g, b;

    if (s <= 0.0) {       // < is bogus, just shuts up warnings
        return color_from_rgba_float(v, v, v, a);
    }
    hh = fmodf(h, 360.0);
    hh = hh / 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch(i) {
    case 0:
        r = v;
        g = t;
        b = p;
        break;
    case 1:
        r = q;
        g = v;
        b = p;
        break;
    case 2:
        r = p;
        g = v;
        b = t;
        break;

    case 3:
        r = p;
        g = q;
        b = v;
        break;
    case 4:
        r = t;
        g = p;
        b = v;
        break;
    case 5:
    default:
        r = v;
        g = p;
        b = q;
        break;
    }
    return color_from_rgba_float(r, g, b, a);
}
