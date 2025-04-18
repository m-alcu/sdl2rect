#include <iostream>
#include <fstream>
#include "rasterizer.hpp"
#include "scene.hpp"
#include "slib.hpp"
#include "smath.hpp"
#include "color.hpp"


void Rasterizer::projectRotateAllPoints(const Scene& scene) {
    // Allocate an array of Pixels on the heap
    // Process each vertex and store the result in the allocated array
    for (int i = 0; i < solid->numVertices; i++) {
        vertex screenPoint;
        slib::vec4 point = scene.fullTransformMat * slib::vec4(solid->vertices[i], 1);
        screenPoint.vertexPoint = point * scene.projectionMatrix;
        screenPoint.normal = scene.normalTransformMat * slib::vec4(solid->vertexNormals[i], 0);
        screenPoint.ds = std::max(0.0f, smath::dot(screenPoint.normal, scene.lux)); // Calculate the dot product with the light direction
        screenPoint.p_x = (int32_t) ((screenPoint.vertexPoint.x / screenPoint.vertexPoint.w + 1.0f) * (scene.screen.width / 2.0f)); // Convert from NDC to screen coordinates
        screenPoint.p_y = (int32_t) ((screenPoint.vertexPoint.y / screenPoint.vertexPoint.w + 1.0f) * (scene.screen.height / 2.0f)); // Convert from NDC to screen coordinates
        screenPoint.p_z = screenPoint.vertexPoint.z / screenPoint.vertexPoint.w; // Store the depth value in the z-buffer
        addPoint(std::make_unique<vertex>(screenPoint)); // Add the point to the rasterizer
        }
}

void Rasterizer::addFaces(const Scene& scene) {


    for (int i=0; i<solid->numFaces; i++) {

        Triangle<vertex> tri(
            *projectedPoints[solid->faces[i].vertex1],
            *projectedPoints[solid->faces[i].vertex2],
            *projectedPoints[solid->faces[i].vertex3],
            i
        );

        if (visible(tri)) {
            ClipCullTriangle(std::make_unique<Triangle<vertex>>(tri));
        }
    }
    
    for (auto& trianglePtr : triangles) {
        draw(*trianglePtr, *solid, scene);
    }

}

/*
Check if triangle is visible.
If the triangle is visible, we can proceed with the rasterization process.
The calculation is based on the cross product of the edges of the triangle.
- If the result is positive, the triangle is visible.
- If the result is negative, the triangle is not visible.edeeee
- If the result is zero, the triangle is coplanar with the screen.
This is a simplified version of the backface culling algorithm.
The backface culling algorithm is used to determine if a triangle is facing the camera or not.
If the triangle is facing away from the camera, we can skip the rasterization process.
*/

bool Rasterizer::visible(const Triangle<vertex>& triangle) {

    return (triangle.p3.p_x-triangle.p2.p_x)*(triangle.p2.p_y-triangle.p1.p_y) - (triangle.p2.p_x-triangle.p1.p_x)*(triangle.p3.p_y-triangle.p2.p_y) < 0;
};

// Inline function for culling top pixels.
inline void cullTopPixels(int32_t& top, int32_t& bottom, vertex& left, vertex& leftDy, vertex& right, vertex& rightDy) {
// Culling the top pixels greater than the screen height.
    if (top < 0) {
        int32_t final = std::min(bottom, 0);
        left += leftDy * (final - top);
        right += rightDy * (final - top);
        top = final;
    }
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
     p2/______|                      |______\ p2 <--- here we update middle vertex
       \      |                      |      /
        \     |                      |     /
         \    |                      |    /
          \   |                      |   /
           \  |                      |  /
   edge23   \ |                      | /  edge23
             \|                      |/
              p3                    p3
*/

void Rasterizer::draw(Triangle<vertex>& tri, const Solid& solid, const Scene& scene) {

    uint32_t flatColor = 0x00000000;
    uint16_t r, g, b;
    if (scene.shading == Shading::Flat) {
        slib::vec3 rotatedFacenormal;
        rotatedFacenormal = scene.normalTransformMat * slib::vec4(solid.faceNormals[tri.i], 0);
        float diff = std::max(0.0f, smath::dot(rotatedFacenormal,scene.lux));
        r = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[0] + solid.faces[tri.i].material.Kd[0] * diff), 255);
        g = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[1] + solid.faces[tri.i].material.Kd[1] * diff), 255);
        b = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[2] + solid.faces[tri.i].material.Kd[2] * diff), 255);
        flatColor = Color(b, g, r).toScreen();
    } 

    if (scene.shading == Shading::Gouraud) {
        r = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[0] + solid.faces[tri.i].material.Kd[0] * tri.p1.ds), 255);
        g = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[1] + solid.faces[tri.i].material.Kd[1] * tri.p1.ds), 255);
        b = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[2] + solid.faces[tri.i].material.Kd[2] * tri.p1.ds), 255);
        tri.p1.color = Color(b << 8, g << 8, r << 8);
        r = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[0] + solid.faces[tri.i].material.Kd[0] * tri.p2.ds), 255);
        g = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[1] + solid.faces[tri.i].material.Kd[1] * tri.p2.ds), 255);
        b = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[2] + solid.faces[tri.i].material.Kd[2] * tri.p2.ds), 255);
        tri.p2.color = Color(b << 8, g << 8, r << 8);
        r = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[0] + solid.faces[tri.i].material.Kd[0] * tri.p3.ds), 255);
        g = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[1] + solid.faces[tri.i].material.Kd[1] * tri.p3.ds), 255);
        b = std::min(static_cast<int>(solid.faces[tri.i].material.Ka[2] + solid.faces[tri.i].material.Kd[2] * tri.p3.ds), 255);
        tri.p3.color = Color(b << 8, g << 8, r << 8);
    }

    orderVertices(&tri.p1, &tri.p2, &tri.p3);
    tri.p1.p_x = tri.p1.p_x << 16; // shift to 16.16 space
    tri.p2.p_x = tri.p2.p_x << 16; // shift to 16.16 space
    tri.p3.p_x = tri.p3.p_x << 16; // shift to 16.16 space
    tri.edge12 = gradientDy(tri.p1, tri.p2);
    tri.edge23 = gradientDy(tri.p2, tri.p3);
    tri.edge13 = gradientDy(tri.p1, tri.p3);

    vertex left = tri.p1, right = tri.p1;
    if(tri.edge13.p_x < tri.edge12.p_x) {
        if (tri.p2.p_y < scene.screen.height) {
            if (tri.p2.p_y < 0) {
                //Culling the entire top triangle
                left += tri.edge13 * (tri.p2.p_y - tri.p1.p_y);
                right = tri.p2;
                //Culling the bottom pixels of the bottom triangle
                cullTopPixels(tri.p2.p_y, tri.p3.p_y, left, tri.edge13, right, tri.edge23);
                tri.p3.p_y = std::min(tri.p3.p_y, scene.screen.height);
                drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge13, tri.edge23, scene, solid.faces[tri.i], flatColor);
            } else {
                //This triangle finishes in the screen (but needs culling top pixels)
                cullTopPixels(tri.p1.p_y, tri.p2.p_y, left, tri.edge13, right, tri.edge12);
                drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge13, tri.edge12, scene, solid.faces[tri.i], flatColor);
                right = tri.p2;
                //Culling the bottom pixels of the bottom triangle
                tri.p3.p_y = std::min(tri.p3.p_y, scene.screen.height);
                drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge13, tri.edge23, scene, solid.faces[tri.i], flatColor);
            }
        } else {
            //Needs to cull the top and the bottom pixels 
            cullTopPixels(tri.p1.p_y, tri.p2.p_y, left, tri.edge13, right, tri.edge12);
            tri.p2.p_y = std::min(tri.p2.p_y, scene.screen.height);
            drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge13, tri.edge12, scene, solid.faces[tri.i], flatColor);
        };
    } else {
        if  (tri.p2.p_y < scene.screen.height) {
            if (tri.p2.p_y < 0) {
                right = right + (tri.edge13 * (tri.p2.p_y - tri.p1.p_y));
                left = tri.p2;
                cullTopPixels(tri.p2.p_y, tri.p3.p_y, left, tri.edge23, right, tri.edge13);
                tri.p3.p_y = std::min(tri.p3.p_y, scene.screen.height);
                drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge23, tri.edge13, scene, solid.faces[tri.i], flatColor);
            } else {
                cullTopPixels(tri.p1.p_y, tri.p2.p_y, left, tri.edge12, right, tri.edge13);
                drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge12, tri.edge13, scene, solid.faces[tri.i], flatColor);
                left = tri.p2;
                cullTopPixels(tri.p2.p_y, tri.p3.p_y, left, tri.edge23, right, tri.edge13);
                tri.p3.p_y = std::min(tri.p3.p_y, scene.screen.height);
                drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge23, tri.edge13, scene, solid.faces[tri.i], flatColor);
            }
        } else {
            cullTopPixels(tri.p1.p_y, tri.p2.p_y, left, tri.edge12, right, tri.edge13);
            tri.p2.p_y = std::min(tri.p2.p_y, scene.screen.height);
            drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge12, tri.edge13, scene, solid.faces[tri.i], flatColor);
        };
    }
};


inline void Rasterizer::orderVertices(vertex *p1, vertex *p2, vertex *p3) {

    if (p1->p_y > p2->p_y) std::swap(*p1,*p2);
    if (p2->p_y > p3->p_y) std::swap(*p2,*p3);
    if (p1->p_y > p2->p_y) std::swap(*p1,*p2);
};

inline vertex Rasterizer::gradientDy(vertex p1, vertex p2) {

    int dy = p2.p_y - p1.p_y;
    if (dy > 0) {
        return (p2 - p1) / dy;
    } else {
        if (p2.p_x - p1.p_x > 0) {
            return vertex(INT32_MAX, 0, 0, {0,0,0}, {0,0,0,0}, 0, {0,0,0}, {0,0,0});
        } else {
            return vertex(INT32_MIN, 0, 0, {0,0,0}, {0,0,0,0}, 0, {0,0,0}, {0,0,0});
        }
    }
};

inline void Rasterizer::drawTriHalf(int32_t top, int32_t bottom, vertex& left, vertex& right, vertex leftDy, vertex rightDy, const Scene& scene, const Face& face, uint32_t flatColor) {

    auto* pixels = static_cast<uint32_t*>(scene.sdlSurface->pixels);

    for(int hy=(top * scene.screen.width); hy<(bottom * scene.screen.width); hy+=scene.screen.width) {
        int16_t dx = (right.p_x - left.p_x) >> 16;
        vertex vDx = dx != 0 ? vDx = (right - left) / dx : vertex();
        vertex vRaster = left;
        int32_t xInitial = left.p_x >> 16;
        
        // Culling the left pixels less than 0
        if (xInitial < 0) {
            int32_t xFinal = std::min(right.p_x >> 16, 0);
            vRaster = vRaster + vDx * (xFinal - xInitial);
            xInitial = xFinal;
        }
        // Culling the right pixels greater than the screen width
        int32_t xFinal = std::min(right.p_x >> 16, scene.screen.width);
        
        for(int hx = xInitial; hx < xFinal; hx++) {
            if (scene.zBuffer->TestAndSet(hy + hx, vRaster.p_z)) {
                switch (scene.shading) {
                    case Shading::Flat: 
                        pixels[hy + hx] = flatColor;
                        break;      
                    case Shading::Gouraud: 
                        pixels[hy + hx] = gouraudShadingFragment(vRaster, scene, face);
                        break;
                    case Shading::BlinnPhong:
                        pixels[hy + hx] = blinnPhongShadingFragment(vRaster, scene, face);
                        break;                                
                    case Shading::Phong:
                        pixels[hy + hx] = phongShadingFragment(vRaster, scene, face);
                        break;                         
                    default: pixels[hy + hx] = flatColor;
                }
            }
            vRaster += vDx;
        }
        left += leftDy;
        right += rightDy;
    }
};

inline uint32_t Rasterizer::gouraudShadingFragment(vertex vRaster, const Scene& scene, Face face) {

    unsigned char r = std::min(static_cast<int>(face.material.Ka[0] + face.material.Kd[0] * vRaster.ds), 255);
    unsigned char g = std::min(static_cast<int>(face.material.Ka[1] + face.material.Kd[1] * vRaster.ds), 255);
    unsigned char b = std::min(static_cast<int>(face.material.Ka[2] + face.material.Kd[2] * vRaster.ds), 255);
    return Color(b, g, r).toScreen();
}

inline uint32_t Rasterizer::phongShadingFragment(vertex gRaster, const Scene& scene, Face face) {

    slib::vec3 normal = smath::normalize(gRaster.normal);
    float diff = std::max(0.0f, smath::dot(normal,scene.lux));

    slib::vec3 R = smath::normalize(normal * 2.0f * smath::dot(normal,scene.lux) - scene.lux);
    float specAngle = std::max(0.0f, smath::dot(R,scene.eye)); // viewer
    float spec = std::pow(specAngle, face.material.Ns);

    uint16_t r = std::min(static_cast<int>(face.material.Ka[0] + face.material.Kd[0] * diff + face.material.Ks[0] * spec), 255);
    uint16_t g = std::min(static_cast<int>(face.material.Ka[1] + face.material.Kd[1] * diff + face.material.Ks[1] * spec), 255);
    uint16_t b = std::min(static_cast<int>(face.material.Ka[2] + face.material.Kd[2] * diff + face.material.Ks[2] * spec), 255);

    /*
    if (diff > 0.995) { 
        return 0xffffffff; // White point if the light is too close to the normal
    }*/

    return Color(b, g, r).toScreen();  // Create a color object with the calculated RGB values and full alpha (255)
}

inline uint32_t Rasterizer::blinnPhongShadingFragment(vertex gRaster, const Scene& scene, Face face) {

    // Normalize vectors
    slib::vec3 N = smath::normalize(gRaster.normal); // Normal at the fragment
    slib::vec3 L = scene.lux; // Light direction
    slib::vec3 V = scene.eye; // Viewer direction (you may want to define this differently later)

    // Diffuse component
    float diff = std::max(0.0f, smath::dot(N,L));

    // Halfway vector H = normalize(L + V)
    slib::vec3 H = smath::normalize(L + V);

    // Specular component: spec = (N · H)^shininess
    float specAngle = std::max(0.0f, smath::dot(N,H));
    float spec = std::pow(specAngle, face.material.Ns * 4); // Blinn Phong shininess needs *4 to be like Phong

    // Calculate brightness
    uint16_t r = std::max(0, std::min(static_cast<int>(face.material.Ka[0] + face.material.Kd[0] * diff + face.material.Ks[0] * spec), 255));
    uint16_t g = std::max(0, std::min(static_cast<int>(face.material.Ka[1] + face.material.Kd[1] * diff + face.material.Ks[1] * spec), 255));
    uint16_t b = std::max(0, std::min(static_cast<int>(face.material.Ka[2] + face.material.Kd[2] * diff + face.material.Ks[2] * spec), 255));

    /*
    if (diff > 0.99) { 
        return 0xffffffff; // White point if the light is too close to the normal
    }*/

    return Color(b, g, r).toScreen();  // Create a color object with the calculated RGB values and full alpha (255)
}

void Rasterizer::ClipCullTriangle(std::unique_ptr<Triangle<vertex>> t)
{
    // Cull tests (unchanged)
    if (t->p1.vertexPoint.x > t->p1.vertexPoint.w &&
        t->p2.vertexPoint.x > t->p2.vertexPoint.w &&
        t->p3.vertexPoint.x > t->p3.vertexPoint.w)
        return;

    if (t->p1.vertexPoint.x < -t->p1.vertexPoint.w &&
        t->p2.vertexPoint.x < -t->p2.vertexPoint.w &&
        t->p3.vertexPoint.x < -t->p3.vertexPoint.w)
        return;

    if (t->p1.vertexPoint.y > t->p1.vertexPoint.w &&
        t->p2.vertexPoint.y > t->p2.vertexPoint.w &&
        t->p3.vertexPoint.y > t->p3.vertexPoint.w)
        return;

    if (t->p1.vertexPoint.y < -t->p1.vertexPoint.w &&
        t->p2.vertexPoint.y < -t->p2.vertexPoint.w &&
        t->p3.vertexPoint.y < -t->p3.vertexPoint.w)
        return;

    if (t->p1.vertexPoint.z > t->p1.vertexPoint.w &&
        t->p2.vertexPoint.z > t->p2.vertexPoint.w &&
        t->p3.vertexPoint.z > t->p3.vertexPoint.w)
        return;

    if (t->p1.vertexPoint.z < -t->p1.vertexPoint.w &&
        t->p2.vertexPoint.z < -t->p2.vertexPoint.w &&
        t->p3.vertexPoint.z < -t->p3.vertexPoint.w)
        return;

    // Check most common case first: No near clipping necessary
    if (t->p1.vertexPoint.z >= -t->p1.vertexPoint.w &&
        t->p2.vertexPoint.z >= -t->p2.vertexPoint.w &&
        t->p3.vertexPoint.z >= -t->p3.vertexPoint.w)
    {
        // Directly draw the triangle without further clipping
        Triangle<vertex> tri(t->p1, t->p2, t->p3, t->i);
        addTriangle(std::make_unique<Triangle<vertex>>(tri));
        return;
    }

    // If near clipping might be necessary, proceed with clipping logic:
    // p1 out, p2 & p3 in
    const auto Clip1Out2In = [this](vertex &p1, vertex &p2, vertex &p3, int16_t i)
    {
        const float alphaA = (p1.vertexPoint.z + p1.vertexPoint.w) /
                             ((p1.vertexPoint.z + p1.vertexPoint.w) - (p2.vertexPoint.z + p2.vertexPoint.w));
        const float alphaB = (p1.vertexPoint.z + p1.vertexPoint.w) /
                             ((p1.vertexPoint.z + p1.vertexPoint.w) - (p3.vertexPoint.z + p3.vertexPoint.w));
        
        const auto p1a = p1 + (p2 - p1) * alphaA;
        const auto p1b = p1 + (p3 - p1) * alphaB;

        addTriangle(std::make_unique<Triangle<vertex>>(Triangle<vertex>(p1a, p2, p3, i)));
        addTriangle(std::make_unique<Triangle<vertex>>(Triangle<vertex>(p1b, p1a, p3, i)));
    };

    // p1 & p2 out, p3 in
    const auto Clip2Out1In = [this](vertex &p1, vertex &p2, vertex &p3, int16_t i)
    {
        const float alphaA = (p1.vertexPoint.z + p1.vertexPoint.w) /
                             ((p1.vertexPoint.z + p1.vertexPoint.w) - (p3.vertexPoint.z + p3.vertexPoint.w));
        const float alphaB = (p2.vertexPoint.z + p2.vertexPoint.w) /
                             ((p2.vertexPoint.z + p2.vertexPoint.w) - (p3.vertexPoint.z + p3.vertexPoint.w));

        p1 = p1 + (p3 - p1) * alphaA;
        p2 = p2 + (p3 - p2) * alphaB;

        addTriangle(std::make_unique<Triangle<vertex>>(Triangle<vertex>(p1, p2, p3, i)));
    };

    // Now proceed with detailed near-clipping logic:
    if (t->p1.vertexPoint.z < -t->p1.vertexPoint.w)
    {
        if (t->p2.vertexPoint.z < -t->p2.vertexPoint.w)
            Clip2Out1In(t->p1, t->p2, t->p3, t->i);
        else if (t->p3.vertexPoint.z < -t->p3.vertexPoint.w)
            Clip2Out1In(t->p1, t->p3, t->p2, t->i);
        else
            Clip1Out2In(t->p1, t->p2, t->p3, t->i);
    }
    else if (t->p2.vertexPoint.z < -t->p2.vertexPoint.w)
    {
        if (t->p3.vertexPoint.z < -t->p3.vertexPoint.w)
            Clip2Out1In(t->p2, t->p3, t->p1, t->i);
        else
            Clip1Out2In(t->p2, t->p1, t->p3, t->i);
    }
    else if (t->p3.vertexPoint.z < -t->p3.vertexPoint.w)
    {
        Clip1Out2In(t->p3, t->p1, t->p2, t->i);
    }
}







