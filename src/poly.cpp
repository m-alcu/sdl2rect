#include <iostream>
#include "poly.hpp"

bool Triangle::visible() {

    return (((int32_t) (p3.p_x-p2.p_x)*(p2.p_y-p1.p_y)) - ((int32_t) (p2.p_x-p1.p_x)*(p3.p_y-p2.p_y)) < 0);
};

bool Triangle::behind() {

    return (p1.p_z < 0 && p2.p_z < 0 && p3.p_z < 0);
};

bool Triangle::outside() {

    return ((p1.p_x < 0 && p2.p_x < 0 && p3.p_x < 0) || 
            (p1.p_x >= screen.width && p2.p_x >= screen.width && p3.p_x >= screen.width) ||
            (p1.p_y < 0 && p2.p_y < 0 && p3.p_y < 0) ||
            (p1.p_y >= screen.high && p2.p_y >= screen.high && p3.p_y >= screen.high)
            );
};

void Triangle::draw(const Solid& solid, Vertex lux) {

    orderPixels(&p1, &p2, &p3);
    Triangle::edge12 = gradientDy(p1, p2, solid, lux);
    Triangle::edge23 = gradientDy(p2, p3, solid, lux);
    Triangle::edge13 = gradientDy(p1, p3, solid, lux);

    Gradient left = Gradient(p1, solid, lux), right = left;
    if(Triangle::edge13.p_x < Triangle::edge12.p_x) {
        drawTriSector(p1.p_y, p2.p_y, left, right, Triangle::pixels, Triangle::screen, Triangle::edge13, Triangle::edge12);
        right.updateFromPixel(p2, solid, lux);
        drawTriSector(p2.p_y, p3.p_y, left, right, Triangle::pixels, Triangle::screen, Triangle::edge13, Triangle::edge23);
    } else {
        drawTriSector(p1.p_y, p2.p_y, left, right, Triangle::pixels, Triangle::screen, Triangle::edge12, Triangle::edge13);
        left.updateFromPixel(p2, solid, lux);
        drawTriSector(p2.p_y, p3.p_y, left, right, Triangle::pixels, Triangle::screen, Triangle::edge23, Triangle::edge13);
    }
};

void Triangle::orderPixels(Pixel *p1, Pixel *p2, Pixel *p3) {

    if (p1->p_y > p2->p_y) swapPixel(p1,p2);
    if (p2->p_y > p3->p_y) swapPixel(p2,p3);
    if (p1->p_y > p2->p_y) swapPixel(p1,p2);
}

void Triangle::swapPixel(Pixel *p1, Pixel *p2) {

    std::swap(p1->p_x, p2->p_x);
    std::swap(p1->p_y, p2->p_y);
    std::swap(p1->p_z, p2->p_z);
    std::swap(p1->vtx, p2->vtx);
}

Gradient Triangle::gradientDy(Pixel p1, Pixel p2, const Solid& solid, Vertex lux) {

    int16_t dy = p2.p_y - p1.p_y;
    int32_t dx = ((int32_t) (p2.p_x - p1.p_x)) << 16;
    int64_t dz = ((int64_t) (p2.p_z - p1.p_z)) << 32;

    float s1 = std::max(0.0f,(lux.dot(solid.vertexNormals[p1.vtx])));
    float s2 = std::max(0.0f,(lux.dot(solid.vertexNormals[p2.vtx])));
    int32_t ds = (int32_t) ((s2 - s1) * 65536); 
    if (dy > 0) {
        Vertex v = (solid.vertices[p2.vtx] - solid.vertices[p1.vtx]) / dy;
        Vertex n = (solid.vertexNormals[p2.vtx] - solid.vertexNormals[p1.vtx]) / dy;
        return  { dx / dy , dz / dy, v, n, ds / dy };
    } else {
        if (dx > 0) {
            return { INT32_MAX , INT64_MAX, {0,0,0}, {0,0,0}, INT32_MAX};
        } else {
            return { INT32_MIN , INT64_MIN, {0,0,0}, {0,0,0}, INT32_MIN};
        }
    }
};

void Gradient::updateFromPixel(const Pixel &p, const Solid& solid, Vertex lux) {
    p_x = ( p.p_x << 16 ) + 0x8000;
    p_z = p.p_z;
    vertexPoint = solid.vertices[p.vtx];
    vertexNormal = solid.vertexNormals[p.vtx];
    ds = (int32_t) (std::max(0.0f,lux.dot(vertexNormal)) * 65536);
}

void Triangle::drawTriSector(int16_t top, int16_t bottom, Gradient& left, Gradient& right, uint32_t *pixels, Screen screen, Gradient leftDy, Gradient rightDy) {

    for(int hy=(top * screen.width); hy<(bottom * screen.width); hy+=screen.width) {
        if (hy >= 0 && hy < (screen.width * screen.high)) { //vertical clipping
            Gradient gDx = Gradient::gradientDx(left, right);
            Gradient gRaster = left;
            for(int hx=(left.p_x >> 16); hx<(right.p_x >> 16); hx++) {
                if (hx >= 0 && hx < screen.width) { //horizontal clipping
                    if (zBuffer[hy + hx] > gRaster.p_z) {
                        if (shading == Shading::Flat) {
                            pixels[hy + hx] = Triangle::color;
                        } else {
                            pixels[hy + hx] = RGBValue(Triangle::color, gRaster.ds).bgra_value;
                        }
                        zBuffer[hy + hx] = gRaster.p_z;
                    }
                }
                gRaster += gDx;
            }
        }
        left += leftDy;
        right += rightDy;
    }
};

// Computes the pixel step gradient from left and right gradients.
// left: starting gradient
// right: ending gradient
Gradient Gradient::gradientDx(const Gradient &left, const Gradient &right) {

    // Calculate the change in x, then shift right by 16 bits to get pixel steps.
    int16_t dx = (right.p_x - left.p_x) >> 16;

    if (dx == 0) return {0, 0, {0, 0, 0}, {0, 0, 0}, 0};
    Vertex v = (right.vertexPoint - left.vertexPoint) / dx;
    Vertex n = (right.vertexNormal - left.vertexNormal) / dx;
    int64_t dz = (right.p_z - left.p_z) / dx;
    int32_t ds = (right.ds - left.ds) / dx;
    return { dx, dz, v, n, ds };
}


