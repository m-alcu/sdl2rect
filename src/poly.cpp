#include <iostream>
#include "poly.h"


void swapNum( uint16_t *x, uint16_t *y) {
    *x = *x + *y;
    *y = *x - *y;
    *x = *x - *y;
}

void swapVertex(Vertex *p1, Vertex *p2) {
    swapNum(&(p1->x), &(p2->x));
    swapNum(&(p1->y), &(p2->y));
}


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

    uint16_t x_start = rand() % screen.width;
    uint16_t x_end = rand() % screen.width;
    uint16_t y_start = rand() % screen.high;
    uint16_t y_end = rand() % screen.high;
    if (x_start > x_end) {
        swapNum(&x_start, &x_end);
    };
    if (y_start > y_end) {
        swapNum(&y_start, &y_end);
    };

    Rectangle::start.x = x_start;
    Rectangle::start.y = y_start;
    Rectangle::end.x = x_end;
    Rectangle::end.y = y_end;
    Rectangle::color = color.long_value;

    draw(pixels, screen);
};

void Triangle::draw(uint32_t *pixels, Screen screen) {

    RGBValue color;
    color.rgba.red = rand() % 255;
    color.rgba.green = rand() % 255;
    color.rgba.blue = rand() % 255;
    color.rgba.alpha = 0x00;

    Triangle::p1.x = rand() % screen.width;
    Triangle::p2.x = rand() % screen.width;
    Triangle::p3.x = rand() % screen.width;

    Triangle::p1.y = rand() % screen.high;
    Triangle::p2.y = rand() % screen.high;
    Triangle::p3.y = rand() % screen.high;

    Triangle::color = color.long_value;

    if (p1.y > p2.y) swapVertex(&p1,&p2);
    if (p2.y > p3.y) swapVertex(&p2,&p3);
    if (p1.y > p2.y) swapVertex(&p1,&p2);

    Triangle::edge12 = getEdge(p1,p2);
    Triangle::edge23 = getEdge(p2,p3);
    Triangle::edge13 = getEdge(p1,p3);

    int32_t leftSide = ( p1.x << 16 ) + 0x8000;
    int32_t rightSide = leftSide;
    if(Triangle::edge13 < Triangle::edge12) {
        drawSector(p1.y, p2.y, &leftSide, &rightSide, pixels, screen, Triangle::edge13, Triangle::edge12);
        rightSide = (p2.x << 16) + 0x8000;
        drawSector(p2.y, p3.y, &leftSide, &rightSide, pixels, screen, Triangle::edge13, Triangle::edge23);
    } else {
        drawSector(p1.y, p2.y, &leftSide, &rightSide, pixels, screen, Triangle::edge12, Triangle::edge13);
        leftSide = (p2.x << 16) + 0x8000;
        drawSector(p2.y, p3.y, &leftSide, &rightSide, pixels, screen, Triangle::edge23, Triangle::edge13);
    }

}

void Triangle::drawSector(uint16_t top, uint16_t bottom, int32_t *leftSide, int32_t *rightSide, uint32_t *pixels, Screen screen, int32_t leftEdge, int32_t rightEdge) {
    for(uint16_t hy=top;hy<bottom; hy++) {
        for(int hx=(*leftSide >> 16); hx<(*rightSide >> 16); hx++) {
            pixels[hy * screen.width + hx] = Triangle::color;
        }
        *leftSide = *leftSide + leftEdge;
        *rightSide = *rightSide + rightEdge;
    }
}

int32_t Triangle::getEdge(Vertex p1, Vertex p2) {
    int32_t long_y = (int32_t) (p2.y - p1.y);
    if (long_y > 0) {
        int32_t long_x = (int32_t) (p2.x - p1.x);
        long_x = long_x << 16;
        return long_x / long_y;
    } else {
        return 0;    
    }
}