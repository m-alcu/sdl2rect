#include <iostream>
#include <math.h>
#include "rasterizer.hpp"

void Scene::setup() {

    halfwayVector = smath::normalize(lux + eye);


    /*
    auto torus = std::make_unique<Torus>();
    torus->setup(20, 10, 500, 250);

    torus->position.z = -1000;
    torus->position.x = 0;
    torus->position.y = 0;
    torus->position.zoom = 1.0f;
    torus->position.xAngle = 90.0f;
    torus->position.yAngle = 0.0f;
    torus->position.zAngle = 0.0f;
    torus->shading = Shading::Flat;
    
    addSolid(std::move(torus));
    */
        
    /*
    auto cube = std::make_unique<Cube>();
    cube->setup();
    cube->position.z = -500;
    cube->position.x = 0;
    cube->position.y = 0;
    cube->position.zoom = 20;
    cube->position.xAngle = 0.0f;
    cube->position.yAngle = 0.0f;
    cube->position.zAngle = 0.0f;
    cube->shading = Shading::Flat;
    addSolid(std::move(cube));
    */

    auto test = std::make_unique<Test>();
    test->setup();
    test->position.z = -500;
    test->position.x = 0;
    test->position.y = 0;
    test->position.zoom = 20;
    test->position.xAngle = 0.0f;
    test->position.yAngle = 0.0f;
    test->position.zAngle = 0.0f;
    test->shading = Shading::Flat;
    addSolid(std::move(test));


    /*
    auto ascLoader = std::make_unique<AscLoader>();
    ascLoader->setup("resources/knot.asc");

    ascLoader->position.z = -1000;   
    ascLoader->position.x = 0;
    ascLoader->position.y = 0;
    ascLoader->position.zoom = 1;
    ascLoader->position.xAngle = 90.0f;
    ascLoader->position.yAngle = 0.0f;
    ascLoader->position.zAngle = 0.0f;
    
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
    tetrakis->position.zoom = 25;
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
