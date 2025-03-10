#include <iostream>
#include "poly.hpp"


void Triangle::swapPixel(Pixel *p1, Pixel *p2) {
    std::swap(p1->p_x, p2->p_x);
    std::swap(p1->p_y, p2->p_y);
    std::swap(p1->v_z, p2->v_z);
    std::swap(p1->s, p2->s);
}

void Triangle::draw() {

    orderPixels(&p1, &p2, &p3);
    calculateEdges(p1,p2,p3);

    Gradient left = Gradient(p1);
    Gradient right = left;
    if(Triangle::edge13.p_x < Triangle::edge12.p_x) {
        drawTriSector(p1, p2, left, right, Triangle::pixels, Triangle::screen, Triangle::edge13, Triangle::edge12);
        right.update(p2);
        drawTriSector(p2, p3, left, right, Triangle::pixels, Triangle::screen, Triangle::edge13, Triangle::edge23);
    } else {
        drawTriSector(p1, p2, left, right, Triangle::pixels, Triangle::screen, Triangle::edge12, Triangle::edge13);
        left.update(p2);
        drawTriSector(p2, p3, left, right, Triangle::pixels, Triangle::screen, Triangle::edge23, Triangle::edge13);
    }
};

void Triangle::orderPixels(Pixel *p1, Pixel *p2, Pixel *p3) {
    if (p1->p_y > p2->p_y) swapPixel(p1,p2);
    if (p2->p_y > p3->p_y) swapPixel(p2,p3);
    if (p1->p_y > p2->p_y) swapPixel(p1,p2);
}

void Triangle::calculateEdges(Pixel p1, Pixel p2, Pixel p3) {
    Triangle::edge12 = calculateEdge(p1,p2);
    Triangle::edge23 = calculateEdge(p2,p3);
    Triangle::edge13 = calculateEdge(p1,p3);
}

void Triangle::drawTriSector(Pixel top, Pixel bottom, Gradient& left, Gradient& right, uint32_t *pixels, Screen screen, Gradient leftEdge, Gradient rightEdge) {

    for(int16_t hy=top.p_y; hy<bottom.p_y; hy++) {
        if (hy >= 0 && hy < screen.high) { //vertical clipping
            Gradient pixelStep = Gradient::computePixelStep(left, right);
            Gradient pixelGradient = left;
            for(int hx=(left.p_x >> 16); hx<(right.p_x >> 16); hx++) {
                if (hx >= 0 && hx < screen.width) { //horizontal clipping
                    if (zBuffer[hy * screen.width + hx] > pixelGradient.v_z) {
                        if (shading == Shading::Flat) {
                            pixels[hy * screen.width + hx] = Triangle::color;
                        } else {
                            pixels[hy * screen.width + hx] = RGBValue(Triangle::color, pixelGradient.ds).bgra_value;
                        }
                        zBuffer[hy * screen.width + hx] = pixelGradient.v_z;
                    }
                }
                pixelGradient = pixelGradient + pixelStep;
            }
        }
        left = left + leftEdge;
        right = right + rightEdge;
    }
};

Gradient Triangle::calculateEdge(Pixel p1, Pixel p2) {
    int32_t dy = (int32_t) (p2.p_y - p1.p_y);
    int32_t dx = ((int32_t) (p2.p_x - p1.p_x)) << 16;
    int64_t dz = ((int64_t) (p2.v_z - p1.v_z)) << 32;
    int32_t ds = (int32_t) ((p2.s - p1.s) * 65536); 
    if (dy > 0) {
        return  { dx / dy , dx / dy, dy / dy, dz / dy, ds / dy };
    } else {
        if (dx > 0) {
            return { INT32_MAX , INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX};
        } else {
            return { INT32_MIN , INT32_MAX, INT32_MAX, INT32_MIN, INT32_MIN};
        }
    }
};

bool Triangle::visible() {

    return (((int32_t) (p3.p_x-p2.p_x)*(p2.p_y-p1.p_y)) - ((int32_t) (p2.p_x-p1.p_x)*(p3.p_y-p2.p_y)) < 0);
};

bool Triangle::outside() {

    return ((p1.p_x < 0 && p2.p_x < 0 && p3.p_x < 0) || 
            (p1.p_x >= screen.width && p2.p_x >= screen.width && p3.p_x >= screen.width) ||
            (p1.p_y < 0 && p2.p_y < 0 && p3.p_y < 0) ||
            (p1.p_y >= screen.high && p2.p_y >= screen.high && p3.p_y >= screen.high)
            );
};

bool Triangle::behind() {

    return (p1.v_z < 0 && p2.v_z < 0 && p3.v_z < 0);
};

// Computes the pixel step gradient from left and right gradients.
// left: starting gradient
// right: ending gradient
Gradient Gradient::computePixelStep(const Gradient &left, const Gradient &right) {
    // Calculate the change in x, then shift right by 16 bits to get pixel steps.
    int16_t stepDx = (right.p_x - left.p_x) >> 16;
    // Avoid division by zero
    int64_t stepVx = (stepDx == 0) ? 0 : (right.v_x - left.v_x) / stepDx;
    int64_t stepVy = (stepDx == 0) ? 0 : (right.v_y - left.v_y) / stepDx;
    int64_t stepVz = (stepDx == 0) ? 0 : (right.v_z - left.v_z) / stepDx;
    int32_t stepDs = (stepDx == 0) ? 0 : (right.ds - left.ds) / stepDx;
    // For phong normals, you might calculate du and dv similarly if needed.
    return { stepDx, stepVz, stepVy, stepVz, stepDs };
}

void Gradient::update(const Pixel &p) {
    p_x = ( p.p_x << 16 ) + 0x8000;
    v_z = ( p.v_z << 32 ) + 0x80000000;
    ds = (int32_t) (p.s * 65536); //is float
}

