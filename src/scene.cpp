#include <iostream>
#include <math.h>
#include "poly.hpp"

void Scene::setup() {

    auto torus = std::make_unique<Torus>(20*10, 20*10*2);
    torus->setup(20, 10, 500, 250);

    torus->position.z = 2000;
    torus->position.x = 0;
    torus->position.y = 0;
    torus->position.zoom = 500;
    torus->position.xAngle = 24.79f;
    torus->position.yAngle = 49.99f;
    
    calculatePrecomputedShading(*torus);

    addSolid(std::move(torus));

}

void Scene::calculatePrecomputedShading(Solid& solid) {

    MaterialProperties material = solid.getMaterialProperties(MaterialType::Metal);

    for(int x = 0; x < 1024; x++) {
        for (int y = 0; y < 1024; y++) {

            float f_x = (float)(x - 512) / 512.0f;
            float f_y = (float)(y - 512) / 512.0f;

            float f_xy_sq = f_x * f_x + f_y * f_y;

            if (f_xy_sq <= 1.0f) {
                float f_z = sqrt(1.0f - f_xy_sq);

                Vec3 normal = Vec3(f_x, f_y, f_z).normalize();
                float diff = std::max(0.0f, normal.dot(lux));
            
                Vec3 R = (normal * 2.0f * normal.dot(lux) - lux).normalize();
                float specAngle = std::max(0.0f, R.dot(lux)); // viewer
                float spec = std::pow(specAngle, material.shininess);
            
                float bright = material.k_a+material.k_d * diff+ material.k_s * spec;
                solid.precomputedShading[y*1024+x] = (int32_t) (bright * 65536 * 0.98);
            } else {
                solid.precomputedShading[y*1024+x] = 0;
            }
        }
    }

}
