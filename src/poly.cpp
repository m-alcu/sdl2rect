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

    Gradient left = Gradient(p1, solid, lux);
    Gradient right = left;
    if(Triangle::edge13.p_x < Triangle::edge12.p_x) {
        drawTriSector(p1, p2, left, right, Triangle::pixels, Triangle::screen, Triangle::edge13, Triangle::edge12);
        right.update(p2, solid, lux);
        drawTriSector(p2, p3, left, right, Triangle::pixels, Triangle::screen, Triangle::edge13, Triangle::edge23);
    } else {
        drawTriSector(p1, p2, left, right, Triangle::pixels, Triangle::screen, Triangle::edge12, Triangle::edge13);
        left.update(p2, solid, lux);
        drawTriSector(p2, p3, left, right, Triangle::pixels, Triangle::screen, Triangle::edge23, Triangle::edge13);
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

    int32_t dy = (int32_t) (p2.p_y - p1.p_y);
    int32_t dx = ((int32_t) (p2.p_x - p1.p_x)) << 16;
    int64_t dz = ((int64_t) (p2.p_z - p1.p_z)) << 32;

    Vertex p1Normal = solid.vertexNormals[p1.vtx];
    float s1 = std::max(0.0f,(lux.x * p1Normal.x + lux.y * p1Normal.y + lux.z * p1Normal.z));
    Vertex p2Normal = solid.vertexNormals[p2.vtx];
    float s2 = std::max(0.0f,(lux.x * p2Normal.x + lux.y * p2Normal.y + lux.z * p2Normal.z));

    int32_t ds = (int32_t) ((s2 - s1) * 65536); 
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

void Gradient::update(const Pixel &p, const Solid& solid, Vertex lux) {

    p_x = ( p.p_x << 16 ) + 0x8000;
    v_z = p.p_z;
    Vertex pNormal = solid.vertexNormals[p.vtx];
    float s = std::max(0.0f,(lux.x * pNormal.x + lux.y * pNormal.y + lux.z * pNormal.z));
    ds = (int32_t) (s * 65536); //is float
}

void Triangle::drawTriSector(Pixel top, Pixel bottom, Gradient& left, Gradient& right, uint32_t *pixels, Screen screen, Gradient leftDy, Gradient rightDy) {

    for(int16_t hy=top.p_y; hy<bottom.p_y; hy++) {
        if (hy >= 0 && hy < screen.high) { //vertical clipping
            Gradient gradientDx = Gradient::gradientDx(left, right);
            Gradient initGradient = left;
            for(int hx=(left.p_x >> 16); hx<(right.p_x >> 16); hx++) {
                if (hx >= 0 && hx < screen.width) { //horizontal clipping
                    if (zBuffer[hy * screen.width + hx] > initGradient.v_z) {
                        if (shading == Shading::Flat) {
                            pixels[hy * screen.width + hx] = Triangle::color;
                        } else {
                            pixels[hy * screen.width + hx] = RGBValue(Triangle::color, initGradient.ds).bgra_value;
                        }
                        zBuffer[hy * screen.width + hx] = initGradient.v_z;
                    }
                }
                initGradient = initGradient + gradientDx;
            }
        }
        left = left + leftDy;
        right = right + rightDy;
    }
};

// Computes the pixel step gradient from left and right gradients.
// left: starting gradient
// right: ending gradient
Gradient Gradient::gradientDx(const Gradient &left, const Gradient &right) {

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


