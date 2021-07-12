#include <iostream>
#include "poly.h"

void Rectangle::draw(uint32_t *pixels, Screen screen) {
    for (int hy=Rectangle::start.y;hy<Rectangle::end.y;hy++) {
        for(int hx=Rectangle::start.x;hx<Rectangle::end.x;hx++) {
            pixels[hy * screen.width + hx] = Rectangle::color;
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

    Rectangle::start.x = x_start;
    Rectangle::start.y = y_start;
    Rectangle::end.x = x_end;
    Rectangle::end.y = y_end;
    Rectangle::color = color.long_value;

    draw(pixels, screen);
};

void Triangle::draw(uint32_t *pixels, Screen screen) {

}