#include <iostream>
#include "poly.hpp"


void Triangle::swapPixel(Pixel *p1, Pixel *p2) {
    std::swap(p1->x, p2->x);
    std::swap(p1->y, p2->y);
    std::swap(p1->z, p2->z);
    std::swap(p1->s, p2->s);
    std::swap(p1->u, p2->u);
    std::swap(p1->v, p2->v);
}

void Triangle::draw() {

    orderPixels(&p1, &p2, &p3);
    calculateEdges(p1,p2,p3);

    Gradient left;
    left.dx = ( p1.x << 16 ) + 0x8000;
    left.dz = ( p1.z << 32 ) + 0x80000000;
    left.ds = (int32_t) (p1.s * 65536); //is float
    Gradient right = left;
    if(Triangle::edge13.dx < Triangle::edge12.dx) {
        drawTriSector(p1, p2, left, right, Triangle::pixels, Triangle::screen, Triangle::edge13, Triangle::edge12);
        right.dx = (p2.x << 16) + 0x8000;
        right.dz = (p2.z << 32) + 0x80000000;
        right.ds = (int32_t) (p2.s * 65536);
        drawTriSector(p2, p3, left, right, Triangle::pixels, Triangle::screen, Triangle::edge13, Triangle::edge23);
    } else {
        drawTriSector(p1, p2, left, right, Triangle::pixels, Triangle::screen, Triangle::edge12, Triangle::edge13);
        left.dx = (p2.x << 16) + 0x8000;
        left.dz = (p2.z << 32) + 0x80000000;
        left.ds = (int32_t) (p2.s * 65536);
        drawTriSector(p2, p3, left, right, Triangle::pixels, Triangle::screen, Triangle::edge23, Triangle::edge13);
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

void Triangle::drawTriSector(Pixel top, Pixel bottom, Gradient& left, Gradient& right, uint32_t *pixels, Screen screen, Gradient leftEdge, Gradient rightEdge) {

    for(int16_t hy=top.y; hy<bottom.y; hy++) {
        if (hy >= 0 && hy < screen.high) { //vertical clipping

            int16_t dx = (right.dx - left.dx) >> 16; //lengh of x in pixels (steps to plot)
            int64_t dz = dx == 0 ? 0 : (right.dz - left.dz) / dx;
            int32_t ds = dx == 0 ? 0 : (right.ds - left.ds) / dx;

            int64_t z = left.dz;
            int32_t s = left.ds;
            RGBValue color;
            for(int hx=(left.dx >> 16); hx<(right.dx >> 16); hx++) {
                if (hx >= 0 && hx < screen.width) { //horizontal clipping
                    if (zBuffer[hy * screen.width + hx] > z) {

                        if (shading == Shading::Flat) {
                            pixels[hy * screen.width + hx] = Triangle::color;
                        } else {
                            color.long_value = Triangle::color;
                            color.rgba.red = (uint8_t) ((color.rgba.red * s) >> 16);
                            color.rgba.green = (uint8_t) ((color.rgba.green * s) >> 16);
                            color.rgba.blue = (uint8_t) ((color.rgba.blue * s) >> 16); 
                            pixels[hy * screen.width + hx] = color.long_value;
                        }
                        zBuffer[hy * screen.width + hx] = z;
                    }
                }
                z += dz;
                s += ds;
            }
        }
        left.dx += leftEdge.dx;
        right.dx += rightEdge.dx;
        left.dz += leftEdge.dz;
        right.dz += rightEdge.dz;
        left.ds += leftEdge.ds;
        right.ds += rightEdge.ds;

    }
};

Gradient Triangle::calculateEdge(Pixel p1, Pixel p2) {
    int32_t dy = (int32_t) (p2.y - p1.y);
    int32_t dx = ((int32_t) (p2.x - p1.x)) << 16;
    int32_t dz = ((int32_t) (p2.z - p1.z)) << 16;
    int32_t ds = (int32_t) ((p2.s - p1.s) * 65536); 
    Gradient grad;
    if (dy > 0) {
        return { dx / dy , dz / dy, ds / dy, 0, 0};
    } else {
        if (dx > 0) {
            return { INT32_MAX , INT32_MAX, INT32_MAX, 0, 0};
        } else {
            return { INT32_MIN , INT32_MIN, INT32_MIN, 0, 0};
        }
    }
};

bool Triangle::visible() {

    return (((int32_t) (p3.x-p2.x)*(p2.y-p1.y)) - ((int32_t) (p2.x-p1.x)*(p3.y-p2.y)) < 0);
};

bool Triangle::outside() {

    return ((p1.x < 0 && p2.x < 0 && p3.x < 0) || 
            (p1.x >= screen.width && p2.x >= screen.width && p3.x >= screen.width) ||
            (p1.y < 0 && p2.y < 0 && p3.y < 0) ||
            (p1.y >= screen.high && p2.y >= screen.high && p3.y >= screen.high)
            );
};

bool Triangle::behind() {

    return (p1.z < 0 && p2.z < 0 && p3.z < 0);
};