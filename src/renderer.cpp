#include <iostream>
#include <math.h>
#include "rasterizer.hpp"
#include "renderer.hpp"
#include "scene.hpp"

/*
Local/Model Space (vertex data)
↓ (model matrix)
World Space
↓ (view matrix) <----- your view / fpsview functions do this
Camera (View) Space
↓ (perspective projection matrix) <---- your perspective(...) does this
Clip Space (4D coordinates)
↓ (perspective divide by w coordinate, GPU automatic)
Normalized Device Coordinates (NDC, range [-1, 1])
↓ (viewport transform, GPU automatic)
Screen Space (pixels)
*/


void Renderer::drawScene(Scene& scene) {

    prepareScene(scene);
    for (auto& solidPtr : scene.solids) {
        drawRenderable(*solidPtr, scene);
    }
}

void Renderer::prepareScene(Scene& scene) {

    std::fill_n(scene.pixels, scene.screen.width * scene.screen.height, 0);
    std::fill_n(
        scene.zBuffer,
        scene.screen.width * scene.screen.height,
        std::numeric_limits<float>::max() // Initialize zBuffer to the maximum float value
    );

    float zNear = 0.1f; // Near plane distance
    float zFar  = 1000.0f; // Far plane distance
    float aspectRatio = scene.screen.width / scene.screen.height; // Width / Height ratio
    float fovRadians = 60.0f * (3.1415926f / 180.0f);

    scene.projectionMatrix = smath::perspective(zFar, zNear, aspectRatio, fovRadians);
}

void Renderer::drawRenderable(Solid& solid, Scene& scene) {

    prepareRenderable(solid, scene);
    vertex * projectedPoints = projectRotateAllPoints(solid, scene);
    addFaces(projectedPoints, solid, scene);
    delete[] projectedPoints;
}

void Renderer::prepareRenderable(const Solid& solid, Scene& scene) {

    slib::mat4 rotate = smath::rotation(slib::vec3({solid.position.xAngle, solid.position.yAngle, solid.position.zAngle}));
    slib::mat4 translate = smath::translation(slib::vec3({solid.position.x, solid.position.y, solid.position.z}));
    slib::mat4 scale = smath::scale(slib::vec3({solid.position.zoom, solid.position.zoom, solid.position.zoom}));
    scene.fullTransformMat = translate * rotate * scale;
    scene.normalTransformMat = rotate;
}

slib::vec4 Renderer::projectedPoint(slib::vec3 point, int16_t i, const Scene& scene) {

    slib::vec4 projectedPoint =  scene.projectionMatrix * slib::vec4(point, 1.0f);
    if (projectedPoint.w != 0) {
        projectedPoint.x /= projectedPoint.w;
        projectedPoint.y /= projectedPoint.w;
        projectedPoint.z /= projectedPoint.w;
    }
    return projectedPoint;
}

vertex Renderer::screenPoint(slib::vec3 point, slib::vec3 normal, slib::vec4 projectedPoint, int16_t i, const Scene& scene) {

    vertex vertex;
    // Apply the viewport transformation to convert from NDC to screen coordinates
    vertex.p_x = (int32_t) ((projectedPoint.x + 1.0f) * (scene.screen.width / 2.0f)); // Convert from NDC to screen coordinates
    vertex.p_y = (int32_t) ((projectedPoint.y + 1.0f) * (scene.screen.height / 2.0f)); // Convert from NDC to screen coordinates
    vertex.p_z = projectedPoint.z;
    vertex.vtx = i;
    vertex.normal = normal;
    vertex.vertexPoint = point;
    return vertex;
}

vertex* Renderer::projectRotateAllPoints(Solid& solid, const Scene& scene) {
    // Allocate an array of Pixels on the heap
    slib::vec3 point;
    slib::vec3 normal;
    vertex* screenPoints = new vertex[solid.numVertices];
    // Process each vertex and store the result in the allocated array
    for (int i = 0; i < solid.numVertices; i++) {

        point = scene.fullTransformMat * slib::vec4(solid.vertices[i], 1);
        normal = scene.normalTransformMat * slib::vec4(solid.vertexNormals[i], 0);
        slib::vec4 proyectedPoint = projectedPoint(point, i, scene);
        screenPoints[i] = screenPoint(point, normal, proyectedPoint, i, scene);
        }
    // Return the pointer to the array
    return screenPoints;
}

void Renderer::addFaces(vertex *projectedPoints, const Solid& solid, Scene& scene) {

    slib::vec3 rotatedFacenormal;

    Rasterizer rasterizer(&solid, scene.pixels, scene.zBuffer);

    for (int i=0; i<solid.numFaces; i++) {
        // Pass the address of 'solid' since it is a reference to an abstract Solid.

        triangle tri(projectedPoints[solid.faces[i].vertex1], projectedPoints[solid.faces[i].vertex2], projectedPoints[solid.faces[i].vertex3], i);

        if (rasterizer.visible(tri) && !rasterizer.outside(scene, tri) && !rasterizer.behind(tri)) {
            divideTriangle(tri, rasterizer, scene, solid.faces[i]);
        }
    }
    
    for (auto& trianglePtr : rasterizer.triangles) {
        rasterizer.draw(*trianglePtr, solid, scene);
    }

}

void Renderer::orderVerticesX(vertex *p1, vertex *p2, vertex *p3) {
    if (p1->p_x > p2->p_x) std::swap(*p1,*p2);
    if (p2->p_x > p3->p_x) std::swap(*p2,*p3);
    if (p1->p_x > p2->p_x) std::swap(*p1,*p2);
}

vertex Renderer::gradientDy(vertex p1, vertex p2, slib::vec3& lux, const Face& face) {

    int dy = p2.p_y - p1.p_y;
    int32_t dx = ((int32_t) (p2.p_x - p1.p_x)) << 16;
    float dz = p2.p_z - p1.p_z;
    float bright1 = face.material.properties.k_a+face.material.properties.k_d * std::max(0.0f, smath::dot(lux,p1.normal));
    float bright2 = face.material.properties.k_a+face.material.properties.k_d * std::max(0.0f, smath::dot(lux,p2.normal));
    int32_t ds = (int32_t) ((bright2 - bright1) * 65536 * 4); 
    if (dy > 0) {
        vertex vertex;
        vertex.p_x = dx / dy;
        vertex.p_z = dz / dy;
        vertex.vertexPoint = (p2.vertexPoint - p1.vertexPoint) / dy;
        vertex.normal = (p2.normal - p1.normal) / dy;
        vertex.ds = ds / dy;
        vertex.tex = (p2.tex - p1.tex) / dy;
        return vertex;
    } else {
        if (dx > 0) {
            return vertex(INT32_MAX, 0, 0, 0, {0,0,0}, {0,0,0}, 0, {0,0,0});
        } else {
            return vertex(INT32_MIN, 0, 0, 0, {0,0,0}, {0,0,0}, 0, {0,0,0});
        }
    }
};

void Renderer::divideTriangle(triangle& tri, Rasterizer& rasterizer, Scene& scene, const Face& face) {

    orderVerticesX(&tri.p1, &tri.p2, &tri.p3);
    if (tri.p1.p_x > 0) {
        if (tri.p3.p_x < scene.screen.width) {
            rasterizer.addTriangle(std::make_unique<triangle>(tri));
        } else {
            //p3 > screen width -> al menos se sale por la derecha
            if (tri.p2.p_x < scene.screen.width) {
                //Pico sale por la derecha
                if (tri.p2.p_y < tri.p1.p_y) {
                    //2 - 3 - 1
                    vertex p2 = vertex(tri.p2, scene.lux, face);
                    vertex p3 = vertex(tri.p3, scene.lux, face);
                    vertex Dx = gradientDy(p2, p3, scene.lux, face);
                    
                    int32_t corte = scene.screen.width - tri.p2.p_x;
                    vertex p3 = tri.p2 + Dx * ( corte >> 16 );
                    triangle addTri1(tri.p1, tri.p2, p3, tri.i);
                    divideTriangle(addTri1, rasterizer, scene);
                    triangle addTri2(tri.p1, p3, tri.p3, tri.i);
                    divideTriangle(addTri2, rasterizer, scene);
                } else {
                    
                }
            } else {
                //Hay 1 vertice dentro de la pantalla y 2 fuera => 1 triangulo
                vertex Dx2 = gradientDy(tri.p1, tri.p2, scene.lux, face);
                vertex Dx3 = gradientDy(tri.p1, tri.p3, scene.lux, face);
                int32_t corte = scene.screen.width - tri.p1.p_x;
                vertex p2 = tri.p1 + Dx2 * ( corte >> 16 );
                vertex p3 = tri.p1 + Dx3 * ( corte >> 16 );
                triangle addTri(tri.p1, p2, p3, tri.i);
                rasterizer.addTriangle(std::make_unique<triangle>(addTri));
            }
        }
    } else {
        //p1 < 0 -> al menos se sale por la izquierda 
        if (tri.p2.p_x > 0) {
            //Solo hay 1 vertice fuera => 2 triangulos o mas
            vertex Dx2 = gradientDy(tri.p2, tri.p1, scene.lux, face);
            vertex Dx3 = gradientDy(tri.p3, tri.p1, scene.lux, face);
            int32_t corte = scene.screen.width - tri.p1.p_x;
            vertex p2 = tri.p1 + Dx2 * ( p2.p_x >> 16 );
            vertex p3 = tri.p1 + Dx3 * ( p3.p_x >> 16 );
            triangle addTri1(p2, tri.p2, p3, tri.i);
            divideTriangle(addTri1, rasterizer, scene);
            triangle addTri2(p2, p3, tri.p3, tri.i);
            divideTriangle(addTri2, rasterizer, scene);
        } else {
            //Hay 1 vertice dentro de la pantalla y 2 fuera => 1 triangulo
            vertex Dx1 = gradientDy(tri.p3, tri.p1, scene.lux, face);
            vertex Dx2 = gradientDy(tri.p3, tri.p2, scene.lux, face);
            int32_t corte = scene.screen.width - tri.p1.p_x;
            vertex p1 = tri.p1 + Dx1 * ( tri.p3.p_x >> 16 );
            vertex p2 = tri.p1 + Dx2 * ( tri.p3.p_x >> 16 );
            triangle addTri(p1, p2, tri.p3, tri.i);
            rasterizer.addTriangle(std::make_unique<triangle>(addTri));
        }
    }
}


