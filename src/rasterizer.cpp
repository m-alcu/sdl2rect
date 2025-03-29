#include <iostream>
#include <fstream>
#include "rasterizer.hpp"
#include "scene.hpp"
#include "slib.hpp"
#include "smath.hpp"

bool Rasterizer::visible() {

    return (((int32_t) (p3.p_x-p2.p_x)*(p2.p_y-p1.p_y)) - ((int32_t) (p2.p_x-p1.p_x)*(p3.p_y-p2.p_y)) < 0);
};

bool Rasterizer::behind() {

    return (p1.p_z < 0 && p2.p_z < 0 && p3.p_z < 0);
};

bool Rasterizer::outside(Scene& scene) {

    return ((p1.p_x < 0 && p2.p_x < 0 && p3.p_x < 0) || 
            (p1.p_x >= scene.screen.width && p2.p_x >= scene.screen.width && p3.p_x >= scene.screen.width) ||
            (p1.p_y < 0 && p2.p_y < 0 && p3.p_y < 0) ||
            (p1.p_y >= scene.screen.height && p2.p_y >= scene.screen.height && p3.p_y >= scene.screen.height)
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

void Rasterizer::draw(const Solid& solid, Scene& scene, const Face& face, slib::vec3 faceNormal) {

    orderVertices(&p1, &p2, &p3);
    edge12 = gradientDy(p1, p2, solid.vertices, scene, face);
    edge23 = gradientDy(p2, p3, solid.vertices, scene, face);
    edge13 = gradientDy(p1, p3, solid.vertices, scene, face);

    uint32_t flatColor = 0x00000000;
    if (scene.shading == Shading::Flat) {
        float diff = std::max(0.0f, smath::dot(faceNormal,scene.lux));
        float bright = face.material.properties.k_a+face.material.properties.k_d * diff;
        flatColor = RGBAColor(face.material.Ambient, (int32_t) (bright * 65536 * 4)).bgra_value;
    }

    Gradient left = Gradient(p1, solid.vertices, scene, face);
    Gradient right = left;
    if(edge13.p_x < edge12.p_x) {
        drawTriSector(p1.p_y, p2.p_y, left, right, edge13, edge12, scene, face, flatColor, solid.precomputedShading);
        updateFromPixel(right, p2, solid.vertices, scene, face);
        drawTriSector(p2.p_y, p3.p_y, left, right, edge13, edge23, scene, face, flatColor, solid.precomputedShading);
    } else {
        drawTriSector(p1.p_y, p2.p_y, left, right, edge12, edge13, scene, face, flatColor, solid.precomputedShading);
        updateFromPixel(left, p2, solid.vertices, scene, face);
        drawTriSector(p2.p_y, p3.p_y, left, right, edge23, edge13, scene, face, flatColor, solid.precomputedShading);
    }
};

void Rasterizer::orderVertices(vertex *p1, vertex *p2, vertex *p3) {

    if (p1->p_y > p2->p_y) std::swap(*p1,*p2);
    if (p2->p_y > p3->p_y) std::swap(*p2,*p3);
    if (p1->p_y > p2->p_y) std::swap(*p1,*p2);
}

Gradient Rasterizer::gradientDy(vertex p1, vertex p2, slib::vec3* rotatedVertices, Scene& scene, Face face) {

    int dy = p2.p_y - p1.p_y;
    int32_t dx = ((int32_t) (p2.p_x - p1.p_x)) << 16;
    float dz = p2.p_z - p1.p_z;

    float diff1 = std::max(0.0f, smath::dot(scene.lux,p1.normal));
    float bright1 = face.material.properties.k_a+face.material.properties.k_d * diff1;
    float diff2 = std::max(0.0f, smath::dot(scene.lux,p2.normal));
    float bright2 = face.material.properties.k_a+face.material.properties.k_d * diff2;
    int32_t ds = (int32_t) ((bright2 - bright1) * 65536 * 4); 
    if (dy > 0) {
        slib::vec3 v = (rotatedVertices[p2.vtx] - rotatedVertices[p1.vtx]) / dy;
        slib::vec3 n = (p2.normal - p1.normal) / dy;
        int32_t result = ds / dy;
        return  { dx / dy , dz / dy, v, n, result };
    } else {
        if (dx > 0) {
            return { INT32_MAX , 0, {0,0,0}, {0,0,0}, 0};
        } else {
            return { INT32_MIN , 0, {0,0,0}, {0,0,0}, 0};
        }
    }
};

void Rasterizer::updateFromPixel(Gradient& updated, const vertex &p, slib::vec3* rotatedVertices, Scene& scene, Face face) {
    updated.p_x = ( p.p_x << 16 ) + 0x8000;
    updated.p_z = p.p_z;
    updated.vertexPoint = rotatedVertices[p.vtx];
    updated.vertexNormal = p.normal;
    float diff = std::max(0.0f, smath::dot(scene.lux, updated.vertexNormal));
    float bright = face.material.properties.k_a+face.material.properties.k_d * diff;
    updated.ds = (int32_t) (bright * 65536 * 4);
}

void Rasterizer::drawTriSector(int16_t top, int16_t bottom, Gradient& left, Gradient& right, Gradient leftDy, Gradient rightDy, Scene& scene, const Face& face, uint32_t flatColor, uint32_t* precomputedShading) {

    for(int hy=(top * scene.screen.width); hy<(bottom * scene.screen.width); hy+=scene.screen.width) {
        if (hy >= 0 && hy < (scene.screen.width * scene.screen.height)) { //vertical clipping
            Gradient gDx = Rasterizer::gradientDx(left, right);
            Gradient gRaster = left;
            for(int hx=(left.p_x >> 16); hx<(right.p_x >> 16); hx++) {
                if (hx >= 0 && hx < scene.screen.width) { //horizontal clipping
                    if (zBuffer[hy + hx] > gRaster.p_z) {
                        switch (scene.shading) {
                            case Shading::Flat: 
                                pixels[hy + hx] = flatColor;
                                break;      
                            case Shading::Gouraud: 
                                pixels[hy + hx] = RGBAColor(face.material.Ambient, gRaster.ds).bgra_value;
                                break;
                            case Shading::BlinnPhong:
                                pixels[hy + hx] = blinnPhongShading(gRaster, scene, face);
                                break;                                
                            case Shading::Phong:
                                pixels[hy + hx] = phongShading(gRaster, scene, face);
                                break;
                            case Shading::Precomputed:
                                pixels[hy + hx] = precomputedPhongShading(gRaster, scene, face, precomputedShading);
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


uint32_t Rasterizer::phongShading(Gradient gRaster, Scene& scene, Face face) {

    slib::vec3 normal = smath::normalize(gRaster.vertexNormal);
    float diff = std::max(0.0f, smath::dot(normal,scene.lux));

    slib::vec3 R = smath::normalize(normal * 2.0f * smath::dot(normal,scene.lux) - scene.lux);
    float specAngle = std::max(0.0f, smath::dot(R,scene.eye)); // viewer
    float spec = std::pow(specAngle, face.material.properties.shininess);

    float bright = face.material.properties.k_a+face.material.properties.k_d * diff+ face.material.properties.k_s * spec;
    return RGBAColor(face.material.Ambient, (int32_t) (bright * 65536 * 0.98)).bgra_value;

}

uint32_t Rasterizer::blinnPhongShading(Gradient gRaster, Scene& scene, Face face) {

    // Normalize vectors
    slib::vec3 N = smath::normalize(gRaster.vertexNormal); // Normal at the fragment
    slib::vec3 L = scene.lux; // Light direction
    slib::vec3 V = scene.eye; // Viewer direction (you may want to define this differently later)

    // Diffuse component
    float diff = std::max(0.0f, smath::dot(N,L));

    // Halfway vector H = normalize(L + V)
    slib::vec3 H = smath::normalize(L + V);

    // Specular component: spec = (N · H)^shininess
    float specAngle = std::max(0.0f, smath::dot(N,H));
    float spec = std::pow(specAngle, face.material.properties.shininess * 4); // Blinn Phong shininess needs *4 to be like Phong

    // Calculate brightness
    float bright = face.material.properties.k_a + 
                   face.material.properties.k_d * diff + 
                   face.material.properties.k_s * spec;

    // Final color composition (ambient color scaled by total brightness)
    return RGBAColor(face.material.Ambient, (int32_t)(bright * 65536 * 0.98)).bgra_value;
}

uint32_t Rasterizer::precomputedPhongShading(Gradient gRaster, Scene& scene, Face face, uint32_t* precomputedShading) {

    slib::vec3 normal = smath::normalize(gRaster.vertexNormal);

    int16_t normal_x = std::max((int16_t) 0,std::min( (int16_t) (PRECOMPUTE_SIZE-1),(int16_t) (normal.x * PRECOMPUTE_SIZE/2 + PRECOMPUTE_SIZE/2)));
    int16_t normal_y = std::max((int16_t) 0,std::min( (int16_t) (PRECOMPUTE_SIZE-1),(int16_t) (normal.y * PRECOMPUTE_SIZE/2 + PRECOMPUTE_SIZE/2)));
    return RGBAColor(face.material.Ambient, precomputedShading[normal_y*PRECOMPUTE_SIZE+normal_x]).bgra_value;

}


// Computes the pixel step gradient from left and right gradients.
// left: starting gradient
// right: ending gradient
Gradient Rasterizer::gradientDx(const Gradient &left, const Gradient &right) {

    // Calculate the change in x, then shift right by 16 bits to get pixel steps.
    int16_t dx = (right.p_x - left.p_x) >> 16;

    if (dx == 0) return {0, 0, {0, 0, 0}, {0, 0, 0}, 0};
    slib::vec3 v = (right.vertexPoint - left.vertexPoint) / dx;
    slib::vec3 n = (right.vertexNormal - left.vertexNormal) / dx;
    float dz = (right.p_z - left.p_z) / dx;
    int32_t ds = (right.ds - left.ds) / dx;
    return { dx, dz, v, n, ds };
}



