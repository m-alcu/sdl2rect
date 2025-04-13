#include <iostream>
#include <math.h>
#include "rasterizer.hpp"

void Scene::setup() {


    
    
    auto torus = std::make_unique<Torus>();
    torus->setup(20, 10, 500, 250);

    torus->position.z = -1000;
    torus->position.x = 0;
    torus->position.y = 0;
    torus->position.zoom = 1.0f;
    torus->position.xAngle = 90.0f;
    torus->position.yAngle = 0.0f;
    torus->position.zAngle = 0.0f;
    
    addSolid(std::move(torus));
    

    /*
    auto ascLoader = std::make_unique<AscLoader>();
    ascLoader->setup("resources/knot.asc");

    ascLoader->position.z = -5000;   
    ascLoader->position.x = 0;
    ascLoader->position.y = 0;
    ascLoader->position.zoom = 1;
    ascLoader->position.xAngle = 90.0f;
    ascLoader->position.yAngle = 0.0f;
    ascLoader->position.zAngle = 0.0f;
    
    calculatePrecomputedShading(*ascLoader);
    addSolid(std::move(ascLoader));
    */

    /*
    auto obj = std::make_unique<ObjLoader>();
    obj->setup("resources/axis.obj");

    obj->position.z = -5000;   
    obj->position.x = 0;
    obj->position.y = 0;
    obj->position.zoom = 1;
    obj->position.xAngle = 0.0f;
    obj->position.yAngle = 0.0f;
    obj->position.zAngle = 0.0f;
    
    calculatePrecomputedShading(*obj);
    addSolid(std::move(obj));
    */

    /*
    auto tetrakis = std::make_unique<Tetrakis>();
    tetrakis->setup();

    tetrakis->position.z = -5000;   
    tetrakis->position.x = 0;
    tetrakis->position.y = 0;
    tetrakis->position.zoom = 1;
    tetrakis->position.xAngle = 90.0f;
    tetrakis->position.yAngle = 0.0f;
    tetrakis->position.zAngle = 0.0f;
    
    addSolid(std::move(tetrakis));
    */

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
