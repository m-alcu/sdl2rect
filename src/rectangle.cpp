#include <iostream>
#include "poly.h"

void Rectangle::draw(uint32_t *pixels, Screen screen, int y_start, int y_end, int x_start, int x_end, uint32_t color) {
    for (int hy=y_start;hy<y_end;hy++) {
        for(int hx=x_start;hx<x_end;hx++) {
            pixels[hy * screen.width + hx] = color;
        }
    }
}