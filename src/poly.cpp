#include <iostream>
#include "poly.hpp"

bool Triangle::visible() {

    return (((int32_t) (p3.p_x-p2.p_x)*(p2.p_y-p1.p_y)) - ((int32_t) (p2.p_x-p1.p_x)*(p3.p_y-p2.p_y)) < 0);
};

bool Triangle::behind() {

    return (p1.p_z < 0 && p2.p_z < 0 && p3.p_z < 0);
};

bool Triangle::outside(Scene scene) {

    return ((p1.p_x < 0 && p2.p_x < 0 && p3.p_x < 0) || 
            (p1.p_x >= scene.screen.width && p2.p_x >= scene.screen.width && p3.p_x >= scene.screen.width) ||
            (p1.p_y < 0 && p2.p_y < 0 && p3.p_y < 0) ||
            (p1.p_y >= scene.screen.high && p2.p_y >= scene.screen.high && p3.p_y >= scene.screen.high)
            );
};

/*
     edge13.p_x > edge12.p_x    edge13.p_x < edge12.p_x
              p1                     p1
             /|                      |\
            / |                      | \
   edge12  /  |                      |  \  edge12
          /   |                      |   \
         /    |  edge13     edge13   |    \
        /     |                      |     \
     p2/______|                      |______\ p2
       \      |                      |      /
        \     |                      |     /
         \    |                      |    /
          \   |                      |   /
           \  |                      |  /
   edge23   \ |                      | /  edge23
             \|                      |/
              p3                    p3
*/

void Triangle::draw(const Solid& solid, Scene scene, const Face& face, Vec3 faceNormal) {

    orderPixels(&p1, &p2, &p3);
    Triangle::edge12 = gradientDy(p1, p2, solid, scene.luxInversePrecomputed);
    Triangle::edge23 = gradientDy(p2, p3, solid, scene.luxInversePrecomputed);
    Triangle::edge13 = gradientDy(p1, p3, solid, scene.luxInversePrecomputed);

    uint32_t flatColor = 0x00000000;
    if (scene.shading == Shading::Flat) {
        float diff = std::max(0.0f, faceNormal.dot(scene.luxInversePrecomputed));
        float bright = face.material.properties.k_a+face.material.properties.k_d * diff;
        flatColor = RGBValue(face.material.Ambient, (int32_t) (bright * 65536 * 4)).bgra_value;
    }

    Gradient left = Gradient(p1, solid, scene), right = left;
    if(Triangle::edge13.p_x < Triangle::edge12.p_x) {
        drawTriSector(p1.p_y, p2.p_y, left, right, Triangle::pixels, Triangle::edge13, Triangle::edge12, scene, face, flatColor);
        right.updateFromPixel(p2, solid, scene);
        drawTriSector(p2.p_y, p3.p_y, left, right, Triangle::pixels, Triangle::edge13, Triangle::edge23, scene, face, flatColor);
    } else {
        drawTriSector(p1.p_y, p2.p_y, left, right, Triangle::pixels, Triangle::edge12, Triangle::edge13, scene, face, flatColor);
        left.updateFromPixel(p2, solid, scene);
        drawTriSector(p2.p_y, p3.p_y, left, right, Triangle::pixels, Triangle::edge23, Triangle::edge13, scene, face, flatColor);
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

Gradient Triangle::gradientDy(Pixel p1, Pixel p2, const Solid& solid, Vec3 lux) {

    int16_t dy = p2.p_y - p1.p_y;
    int32_t dx = ((int32_t) (p2.p_x - p1.p_x)) << 16;
    int64_t dz = ((int64_t) (p2.p_z - p1.p_z)) << 32;

    float s1 = std::max(0.0f,(lux.dot(solid.vertexNormals[p1.vtx])));
    float s2 = std::max(0.0f,(lux.dot(solid.vertexNormals[p2.vtx])));
    int32_t ds = (int32_t) ((s2 - s1) * 65536); 
    if (dy > 0) {
        Vec3 v = (solid.vertices[p2.vtx] - solid.vertices[p1.vtx]) / dy;
        Vec3 n = (solid.vertexNormals[p2.vtx] - solid.vertexNormals[p1.vtx]) / dy;
        return  { dx / dy , dz / dy, v, n, ds / dy };
    } else {
        if (dx > 0) {
            return { INT32_MAX , 0, {0,0,0}, {0,0,0}, 0};
        } else {
            return { INT32_MIN , 0, {0,0,0}, {0,0,0}, 0};
        }
    }
};

void Gradient::updateFromPixel(const Pixel &p, const Solid& solid, Scene scene) {
    p_x = ( p.p_x << 16 ) + 0x8000;
    p_z = p.p_z;
    vertexPoint = solid.vertices[p.vtx];
    vertexNormal = solid.vertexNormals[p.vtx];
    ds = (int32_t) (std::max(0.0f,scene.luxInversePrecomputed.dot(vertexNormal)) * 65536);
}

void Triangle::drawTriSector(int16_t top, int16_t bottom, Gradient& left, Gradient& right, uint32_t *pixels, Gradient leftDy, Gradient rightDy, Scene scene, const Face& face, uint32_t flatColor) {

    for(int hy=(top * scene.screen.width); hy<(bottom * scene.screen.width); hy+=scene.screen.width) {
        if (hy >= 0 && hy < (scene.screen.width * scene.screen.high)) { //vertical clipping
            Gradient gDx = Gradient::gradientDx(left, right);
            Gradient gRaster = left;
            for(int hx=(left.p_x >> 16); hx<(right.p_x >> 16); hx++) {
                if (hx >= 0 && hx < scene.screen.width) { //horizontal clipping
                    if (zBuffer[hy + hx] > gRaster.p_z) {
                        switch (scene.shading) {
                            case Shading::Flat: 
                                pixels[hy + hx] = flatColor;
                                break;      
                            case Shading::Gouraud: 
                                pixels[hy + hx] = RGBValue(face.material.Ambient, gRaster.ds).bgra_value;
                                break;
                            case Shading::BlinnPhong:
                                pixels[hy + hx] = blinnPhongShading(gRaster, scene, face);
                                break;                                
                            case Shading::Phong:
                                pixels[hy + hx] = phongShading(gRaster, scene, face);
                                break;
                            default: pixels[hy + hx] = flatColor;
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


uint32_t Triangle::phongShading(Gradient gRaster, Scene scene, Face face) {

    Vec3 normal = gRaster.vertexNormal.normalize();
    float diff = std::max(0.0f, normal.dot(scene.luxInversePrecomputed));

    Vec3 R = (gRaster.vertexNormal * 2.0f * normal.dot(scene.luxInversePrecomputed) - scene.luxInversePrecomputed).normalize();
    float specAngle = std::max(0.0f, R.dot(scene.eyeInversePrecomputed)); // viewer
    float spec = std::pow(specAngle, face.material.properties.shininess);

    float bright = face.material.properties.k_a+face.material.properties.k_d * diff+ face.material.properties.k_s * spec;
    return RGBValue(face.material.Ambient, (int32_t) (bright * 65536 * 0.98)).bgra_value;

}

uint32_t Triangle::blinnPhongShading(Gradient gRaster, Scene scene, Face face) {

    // Normalize vectors
    Vec3 N = gRaster.vertexNormal.normalize(); // Normal at the fragment
    Vec3 L = scene.luxInversePrecomputed; // Light direction
    Vec3 V = scene.eyeInversePrecomputed; // Viewer direction (you may want to define this differently later)

    // Diffuse component
    float diff = std::max(0.0f, N.dot(L));

    // Halfway vector H = normalize(L + V)
    Vec3 H = (L + V).normalize();

    // Specular component: spec = (N · H)^shininess
    float specAngle = std::max(0.0f, N.dot(H));
    float spec = std::pow(specAngle, face.material.properties.shininess * 4); // Blinn Phong shininess needs *4 to be like Phong

    // Calculate brightness
    float bright = face.material.properties.k_a + 
                   face.material.properties.k_d * diff + 
                   face.material.properties.k_s * spec;

    // Final color composition (ambient color scaled by total brightness)
    return RGBValue(face.material.Ambient, (int32_t)(bright * 65536 * 0.98)).bgra_value;
}


// Computes the pixel step gradient from left and right gradients.
// left: starting gradient
// right: ending gradient
Gradient Gradient::gradientDx(const Gradient &left, const Gradient &right) {

    // Calculate the change in x, then shift right by 16 bits to get pixel steps.
    int16_t dx = (right.p_x - left.p_x) >> 16;

    if (dx == 0) return {0, 0, {0, 0, 0}, {0, 0, 0}, 0};
    Vec3 v = (right.vertexPoint - left.vertexPoint) / dx;
    Vec3 n = (right.vertexNormal - left.vertexNormal) / dx;
    int64_t dz = (right.p_z - left.p_z) / dx;
    int32_t ds = (right.ds - left.ds) / dx;
    return { dx, dz, v, n, ds };
}



