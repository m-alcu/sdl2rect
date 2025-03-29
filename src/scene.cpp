#include <iostream>
#include <math.h>
#include "rasterizer.hpp"

void Scene::setup() {


    
    auto torus = std::make_unique<Torus>(20*10, 20*10*2);
    torus->setup(20, 10, 500, 250);

    torus->position.z = -5000;
    torus->position.x = 0;
    torus->position.y = 0;
    torus->position.zoom = 1;
    torus->position.xAngle = 90.0f;
    torus->position.yAngle = 0.0f;
    torus->position.zAngle = 0.0f;
    
    calculatePrecomputedShading(*torus);

    addSolid(std::move(torus));
    

    /*
    auto torus = std::make_unique<Test>(8, 4);
    torus->setup();
    torus->position.z = 1000000;
    torus->position.x = 0;
    torus->position.y = 0;
    torus->position.zoom = 1620;
    torus->position.xAngle = 0.0f;
    torus->position.yAngle = 0.0f;
    torus->position.zAngle = 0.0f;

    calculatePrecomputedShading(*torus);

    addSolid(std::move(torus));
    */

    /*
    auto torus2 = std::make_unique<Torus>(20*10, 20*10*2);
    torus2->setup(20, 10, 500, 250);

    torus2->position.z = 2000;
    torus2->position.x = 500;
    torus2->position.y = 0;
    torus2->position.zoom = 500;
    torus2->position.xAngle = 90f;
    torus2->position.yAngle = 49.99f;
    
    calculatePrecomputedShading(*torus2);

    addSolid(std::move(torus2));
    */

}

void Scene::calculatePrecomputedShading(Solid& solid) {

    MaterialProperties material = solid.getMaterialProperties(MaterialType::Metal);

    for(int x = 0; x < PRECOMPUTE_SIZE; x++) {
        for (int y = 0; y < PRECOMPUTE_SIZE; y++) {

            float f_x = (float)(x - PRECOMPUTE_SIZE/2) / (PRECOMPUTE_SIZE/2);
            float f_y = (float)(y - PRECOMPUTE_SIZE/2) / (PRECOMPUTE_SIZE/2);

            float f_xy_sq = f_x * f_x + f_y * f_y;

            if (f_xy_sq <= 1.0f) {
                float f_z = sqrt(1.0f - f_xy_sq);

                slib::vec3 normal = smath::normalize(slib::vec3({f_x, f_y, f_z}));
                float diff = std::max(0.0f, smath::dot(normal,lux));
            
                slib::vec3 R = smath::normalize(normal * 2.0f * smath::dot(normal,lux) - lux);
                float specAngle = std::max(0.0f, smath::dot(R,lux)); // viewer
                float spec = std::pow(specAngle, material.shininess);
            
                float bright = material.k_a+material.k_d * diff+ material.k_s * spec;
                solid.precomputedShading[y*PRECOMPUTE_SIZE+x] = (int32_t) (bright * 65536 * 0.98);
            } else {
                solid.precomputedShading[y*PRECOMPUTE_SIZE+x] = 0;
            }
        }
    }

}
