#include <iostream>
#include "poly.h"

void Rectangle::draw(uint32_t *pixels, Screen screen, int y_start, int y_end, int x_start, int x_end, uint32_t color) {
    for (int hy=y_start;hy<y_end;hy++) {
        for(int hx=x_start;hx<x_end;hx++) {
            pixels[hy * screen.width + hx] = color;
        }
    }
}

void Rectangle::randomDraw(uint32_t *pixels, Screen screen) {

    RGBValue color;
    color.rgba.red = rand() % 255;
    color.rgba.green = rand() % 255;
    color.rgba.blue = rand() % 255;
    color.rgba.alpha = 0x00;

    int x_start = rand() % screen.width;
    int x_end = rand() % screen.width;
    int y_start = rand() % screen.high;
    int y_end = rand() % screen.high;
    if (x_start > x_end) {
        x_start = x_start + x_end;
        x_end = x_start - x_end;
        x_start = x_start - x_end;
    };
    if (y_start > y_end) {
        y_start = y_start + y_end;
        y_end = y_start - y_end;
        y_start = y_start - y_end;
    };

    draw(pixels, screen, y_start, y_end, x_start, x_end, color.long_value);
};

void Triangle::draw(uint32_t *pixels, Screen screen, Vertex p1, Vertex p2, Vertex p3, uint32_t color) {

}