#include <iostream>
#include <math.h>
#include "rasterizer.hpp"
#include "renderer.hpp"
#include "scene.hpp"

void Renderer::drawScene(Scene& scene) {

    prepareScene(scene);
    for (auto& solidPtr : scene.solids) {
        drawRenderable(*solidPtr, scene);
    }
}

void Renderer::prepareScene(Scene& scene) {

    std::fill_n(scene.pixels, scene.screen.width * scene.screen.high, 0);
    std::fill_n(
        scene.zBuffer,
        scene.screen.width * scene.screen.high,
        std::numeric_limits<float>::max()
    );
}

void Renderer::drawRenderable(Solid& solid, Scene& scene) {

    prepareRenderable(solid, scene);
    vertex * projectedPoints = projectRotateAllPoints(solid, scene);
    slib::vec3 * rotatedVertexNormals = rotateVertexNormals(solid, scene);
    drawFaces(projectedPoints, solid, scene, rotatedVertexNormals);
    delete[] projectedPoints;
    delete[] rotatedVertexNormals;
}

void Renderer::prepareRenderable(const Solid& solid, Scene& scene) {

    slib::mat4 rotate = smath::rotation(slib::vec3({solid.position.xAngle, solid.position.yAngle, solid.position.zAngle}));
    slib::mat4 translate = smath::translation(slib::vec3({solid.position.x, solid.position.y, solid.position.z}));
    scene.rotate = translate * rotate;
}

vertex Renderer::proj3to2D(slib::vec3 point, Screen screen, Position position, int16_t i) {

    vertex pixel;
    pixel.p_x = (int16_t) ((position.zoom * (point.x)) / (point.z)) + screen.width / 2;
    pixel.p_y = (int16_t) ((position.zoom * (point.y)) / (point.z)) + screen.high / 2;
    pixel.p_z = point.z;
    pixel.vtx = i;
    return pixel;
}

slib::vec3* Renderer::rotateVertexNormals(const Solid& solid, const Scene& scene) {

    slib::vec3* rNormals = new slib::vec3[solid.numVertices];
    for (int i = 0; i < solid.numVertices; i++) {

        slib::vec4 normal = scene.rotate * slib::vec4(solid.vertexNormals[i], 0);
        rNormals[i] = {normal.x, normal.y, normal.z};
    }
    return rNormals;
}


vertex* Renderer::projectRotateAllPoints(Solid& solid, const Scene& scene) {
    // Allocate an array of Pixels on the heap
    vertex* projectedPoints = new vertex[solid.numVertices];
    // Process each vertex and store the result in the allocated array
    for (int i = 0; i < solid.numVertices; i++) {

        slib::vec4 point = scene.rotate * slib::vec4(solid.vertices[i], 1);
        projectedPoints[i] = proj3to2D({point.x, point.y, point.z}, scene.screen, solid.position, i);
    }
    // Return the pointer to the array
    return projectedPoints;
}

void Renderer::drawFaces(vertex *projectedPoints, const Solid& solid, Scene& scene, slib::vec3 *rotatedVertexNormals) {

    for (int i=0; i<solid.numFaces; i++) {
        // Pass the address of 'solid' since it is a reference to an abstract Solid.
        Rasterizer triangle(&solid, scene.pixels, scene.zBuffer);
        triangle.p1 = projectedPoints[solid.faces[i].vertex1];
        triangle.p2 = projectedPoints[solid.faces[i].vertex2];
        triangle.p3 = projectedPoints[solid.faces[i].vertex3];

        if (triangle.visible() && !triangle.outside(scene) && !triangle.behind()) {

            slib::mat4 rotate = smath::rotation(slib::vec3({solid.position.xAngle, solid.position.yAngle, solid.position.zAngle}));
            slib::vec4 rotatedFacenormal = rotate * slib::vec4(solid.faceNormals[i], 0);

            triangle.draw(solid, scene, solid.faces[i], {rotatedFacenormal.x, rotatedFacenormal.y, rotatedFacenormal.z}, rotatedVertexNormals);
        }
    }                        
}

bool Renderer::triangleNearCenter(vertex p1, vertex p2, vertex p3, Scene& scene) {

    return pointNearCenter(p1.p_x, p1.p_y, scene) || pointNearCenter(p2.p_x, p2.p_y, scene) || pointNearCenter(p3.p_x, p3.p_y, scene);
    }

bool Renderer::pointNearCenter(int16_t x, int16_t y,  Scene& scene) {

    return (x > (-10 +(scene.screen.width / 2)) && x < (10 +(scene.screen.width / 2)) && y > (-10 +(scene.screen.high / 2)) && y < (10 +(scene.screen.high / 2)));

}
