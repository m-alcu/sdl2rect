#include <iostream>
#include "poly.hpp"


void Triangle::swapPixel(Pixel *p1, Pixel *p2) {
    std::swap(p1->x, p2->x);
    std::swap(p1->y, p2->y);
    std::swap(p1->z, p2->z);
}

void Triangle::draw() {

    orderPixels(&p1, &p2, &p3);
    calculateEdges(p1,p2,p3);

    int32_t leftSide = ( p1.x << 16 ) + 0x8000;
    int32_t rightSide = leftSide;
    if(Triangle::edge13.dx < Triangle::edge12.dx) {
        drawTriSector(p1.y, p2.y, &leftSide, &rightSide, Triangle::pixels, Triangle::screen, Triangle::edge13, Triangle::edge12);
        rightSide = (p2.x << 16) + 0x8000;
        drawTriSector(p2.y, p3.y, &leftSide, &rightSide, Triangle::pixels, Triangle::screen, Triangle::edge13, Triangle::edge23);
    } else {
        drawTriSector(p1.y, p2.y, &leftSide, &rightSide, Triangle::pixels, Triangle::screen, Triangle::edge12, Triangle::edge13);
        leftSide = (p2.x << 16) + 0x8000;
        drawTriSector(p2.y, p3.y, &leftSide, &rightSide, Triangle::pixels, Triangle::screen, Triangle::edge23, Triangle::edge13);
    }
};

void Triangle::orderPixels(Pixel *p1, Pixel *p2, Pixel *p3) {
    if (p1->y > p2->y) swapPixel(p1,p2);
    if (p2->y > p3->y) swapPixel(p2,p3);
    if (p1->y > p2->y) swapPixel(p1,p2);
}

void Triangle::calculateEdges(Pixel p1, Pixel p2, Pixel p3) {
    Triangle::edge12 = calculateEdge(p1,p2);
    Triangle::edge23 = calculateEdge(p2,p3);
    Triangle::edge13 = calculateEdge(p1,p3);
}

void Triangle::drawTriSector(uint16_t top, uint16_t bottom, int32_t *leftSide, int32_t *rightSide, uint32_t *pixels, Screen screen, Gradient leftEdge, Gradient rightEdge) {
    for(uint16_t hy=top; hy<bottom; hy++) {
        for(int hx=(*leftSide >> 16); hx<(*rightSide >> 16); hx++) {
            pixels[hy * screen.width + hx] = Triangle::color;
        }
        *leftSide += leftEdge.dx;
        *rightSide += rightEdge.dx;
    }
};

Gradient Triangle::calculateEdge(Pixel p1, Pixel p2) {
    int32_t dy = (int32_t) (p2.y - p1.y);
    int32_t dx = ((int32_t) (p2.x - p1.x)) << 16;
    int32_t dz = ((int32_t) (p2.z - p1.z)) << 16;
    Gradient grad;
    if (dy > 0) {
        return { dx / dy , dz / dy};
    } else {
        if (dx > 0) {
            return { INT32_MAX , 0};
        } else {
            return { INT32_MIN , 0};
        }
    }
};

bool Triangle::visible() {

    return (((int32_t) (p3.x-p2.x)*(p2.y-p1.y)) - ((int32_t) (p2.x-p1.x)*(p3.y-p2.y)) < 0);
};