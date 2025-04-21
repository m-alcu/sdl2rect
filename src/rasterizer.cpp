#include <iostream>
#include <fstream>
#include <omp.h>
#include "rasterizer.hpp"
#include "scene.hpp"
#include "slib.hpp"
#include "smath.hpp"
#include "color.hpp"


void Rasterizer::ProcessVertex(const Scene& scene) {
    // Allocate an array of Pixels on the heap
    // Process each vertex and store the result in the allocated array
    for (int i = 0; i < solid->numVertices; i++) {
        vertex screenPoint;
        screenPoint.point = scene.fullTransformMat * slib::vec4(solid->vertices[i], 1);
        screenPoint.ndc = slib::vec4(screenPoint.point, 1) * scene.projectionMatrix;
        screenPoint.normal = scene.normalTransformMat * slib::vec4(solid->vertexNormals[i], 0);
        screenPoint.p_x = (int32_t) ((screenPoint.ndc.x / screenPoint.ndc.w + 1.0f) * (scene.screen.width / 2.0f)); // Convert from NDC to screen coordinates
        screenPoint.p_y = (int32_t) ((screenPoint.ndc.y / screenPoint.ndc.w + 1.0f) * (scene.screen.height / 2.0f)); // Convert from NDC to screen coordinates
        screenPoint.p_z = screenPoint.ndc.z / screenPoint.ndc.w; // Store the depth value in the z-buffer
        addPoint(std::make_unique<vertex>(screenPoint)); // Add the point to the rasterizer
        }
}

void Rasterizer::DrawFaces(const Scene& scene) {

    //#pragma omp parallel for
    for (int i=0; i<solid->numFaces; i++) {

        Triangle<vertex> tri(
            *projectedPoints[solid->faces[i].vertex1],
            *projectedPoints[solid->faces[i].vertex2],
            *projectedPoints[solid->faces[i].vertex3],
            i
        );

        if (Visible(tri)) {
            ClipCullDrawTriangle(tri, scene);
        }
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

bool Rasterizer::Visible(const Triangle<vertex>& triangle) {

    return (triangle.p3.p_x-triangle.p2.p_x)*(triangle.p2.p_y-triangle.p1.p_y) - (triangle.p2.p_x-triangle.p1.p_x)*(triangle.p3.p_y-triangle.p2.p_y) < 0;
    //return (triangle.p3.point.x-triangle.p2.point.x)*(triangle.p2.point.y-triangle.p1.point.y) - (triangle.p2.point.x-triangle.p1.point.x)*(triangle.p3.point.y-triangle.p2.point.y) < 0;

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
    float r, g, b, ds;
    if (scene.shading == Shading::Flat) {
        slib::vec3 rotatedFacenormal;
        rotatedFacenormal = scene.normalTransformMat * slib::vec4(solid.faceNormals[tri.i], 0);
        float diff = std::max(0.0f, smath::dot(rotatedFacenormal,scene.lux));
        r = std::min(solid.faces[tri.i].material.Ka[0] + solid.faces[tri.i].material.Kd[0] * diff, 255.0f);
        g = std::min(solid.faces[tri.i].material.Ka[1] + solid.faces[tri.i].material.Kd[1] * diff, 255.0f);
        b = std::min(solid.faces[tri.i].material.Ka[2] + solid.faces[tri.i].material.Kd[2] * diff, 255.0f);
        flatColor = Color(b, g, r).toBgra();
    } 

    if (scene.shading == Shading::Gouraud) {
        ds = std::max(0.0f, smath::dot(tri.p1.normal, scene.lux));
        r = std::min(solid.faces[tri.i].material.Ka[0] + solid.faces[tri.i].material.Kd[0] * ds, 255.0f);
        g = std::min(solid.faces[tri.i].material.Ka[1] + solid.faces[tri.i].material.Kd[1] * ds, 255.0f);
        b = std::min(solid.faces[tri.i].material.Ka[2] + solid.faces[tri.i].material.Kd[2] * ds, 255.0f);
        tri.p1.color = Color(b, g, r);
        ds = std::max(0.0f, smath::dot(tri.p2.normal, scene.lux));
        r = std::min(solid.faces[tri.i].material.Ka[0] + solid.faces[tri.i].material.Kd[0] * ds, 255.0f);
        g = std::min(solid.faces[tri.i].material.Ka[1] + solid.faces[tri.i].material.Kd[1] * ds, 255.0f);
        b = std::min(solid.faces[tri.i].material.Ka[2] + solid.faces[tri.i].material.Kd[2] * ds, 255.0f);
        tri.p2.color = Color(b, g, r);
        ds = std::max(0.0f, smath::dot(tri.p3.normal, scene.lux));
        r = std::min(solid.faces[tri.i].material.Ka[0] + solid.faces[tri.i].material.Kd[0] * ds, 255.0f);
        g = std::min(solid.faces[tri.i].material.Ka[1] + solid.faces[tri.i].material.Kd[1] * ds, 255.0f);
        b = std::min(solid.faces[tri.i].material.Ka[2] + solid.faces[tri.i].material.Kd[2] * ds, 255.0f);
        tri.p3.color = Color(b, g, r);
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
            return vertex(INT32_MAX, 0, 0, {0,0,0}, {0,0,0,0}, {0,0,0}, {0,0,0});
        } else {
            return vertex(INT32_MIN, 0, 0, {0,0,0}, {0,0,0,0}, {0,0,0}, {0,0,0});
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
                        pixels[hy + hx] = vRaster.color.toBgra();
                        break;
                    case Shading::BlinnPhong:
                        pixels[hy + hx] = BlinnPhongPixelShading(vRaster, scene, face);
                        break;                                
                    case Shading::Phong:
                        pixels[hy + hx] = PhongPixelShading(vRaster, scene, face);
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

inline uint32_t Rasterizer::PhongPixelShading(const vertex& gRaster, const Scene& scene, const Face& face) {

    slib::vec3 normal = smath::normalize(gRaster.normal);
    float diff = std::max(0.0f, smath::dot(normal,scene.lux));

    slib::vec3 R = smath::normalize(normal * 2.0f * smath::dot(normal,scene.lux) - scene.lux);
    float specAngle = std::max(0.0f, smath::dot(R,scene.eye)); // viewer
    float spec = std::pow(specAngle, face.material.Ns);

    if (spec > 0.95) { 
        return 0xffffffff; // White point if the light is too close to the normal
    }

    float r, g, b;
    // Calculate brightness
    r = std::min(face.material.Ka[0] + face.material.Kd[0] * diff + face.material.Ks[0] * spec, 255.0f);
    g = std::min(face.material.Ka[1] + face.material.Kd[1] * diff + face.material.Ks[1] * spec, 255.0f);
    b = std::min(face.material.Ka[2] + face.material.Kd[2] * diff + face.material.Ks[2] * spec, 255.0f);

    return Color(b, g, r).toBgra();  // Create a color object with the calculated RGB values and full alpha (255)
}

inline uint32_t Rasterizer::BlinnPhongPixelShading(const vertex& gRaster, const Scene& scene, const Face& face) {

    // Normalize vectors
    slib::vec3 N = smath::normalize(gRaster.normal); // Normal at the fragment
    slib::vec3 L = scene.lux; // Light direction
    slib::vec3 V = scene.eye; // Viewer direction (you may want to define this differently later)

    // Diffuse component
    float diff = std::max(0.0f, smath::dot(N,L));

    // Halfway vector H = normalize(L + V)
    //slib::vec3 H = smath::normalize(L + V);

    // Specular component: spec = (N · H)^shininess
    float specAngle = std::max(0.0f, smath::dot(N,scene.halfwayVector)); // viewer
    float spec = std::pow(specAngle, face.material.Ns); // Blinn Phong shininess needs *4 to be like Phong

    if (spec > 0.95) { 
        return 0xffffffff; // White point if the light is too close to the normal
    }

    float r, g, b;
    // Calculate brightness
    r = std::min(face.material.Ka[0] + face.material.Kd[0] * diff + face.material.Ks[0] * spec, 255.0f);
    g = std::min(face.material.Ka[1] + face.material.Kd[1] * diff + face.material.Ks[1] * spec, 255.0f);
    b = std::min(face.material.Ka[2] + face.material.Kd[2] * diff + face.material.Ks[2] * spec, 255.0f);

    return Color(b, g, r).toBgra();  // Create a color object with the calculated RGB values and full alpha (255)
}

void Rasterizer::ClipCullDrawTriangle(Triangle<vertex>& t, const Scene& scene)
{
    // Cull tests (unchanged)
    if (t.p1.ndc.x > t.p1.ndc.w &&
        t.p2.ndc.x > t.p2.ndc.w &&
        t.p3.ndc.x > t.p3.ndc.w)
        return;

    if (t.p1.ndc.x < -t.p1.ndc.w &&
        t.p2.ndc.x < -t.p2.ndc.w &&
        t.p3.ndc.x < -t.p3.ndc.w)
        return;

    if (t.p1.ndc.y > t.p1.ndc.w &&
        t.p2.ndc.y > t.p2.ndc.w &&
        t.p3.ndc.y > t.p3.ndc.w)
        return;

    if (t.p1.ndc.y < -t.p1.ndc.w &&
        t.p2.ndc.y < -t.p2.ndc.w &&
        t.p3.ndc.y < -t.p3.ndc.w)
        return;

    if (t.p1.ndc.z > t.p1.ndc.w &&
        t.p2.ndc.z > t.p2.ndc.w &&
        t.p3.ndc.z > t.p3.ndc.w)
        return;

    if (t.p1.ndc.z < -t.p1.ndc.w &&
        t.p2.ndc.z < -t.p2.ndc.w &&
        t.p3.ndc.z < -t.p3.ndc.w)
        return;

    // Check most common case first: No near clipping necessary
    if (t.p1.ndc.z >= -t.p1.ndc.w &&
        t.p2.ndc.z >= -t.p2.ndc.w &&
        t.p3.ndc.z >= -t.p3.ndc.w)
    {
        draw(t, *solid, scene);
        return;
    }

    // If near clipping might be necessary, proceed with clipping logic:
    // p1 out, p2 & p3 in
    const auto Clip1Out2In = [this](vertex &p1, vertex &p2, vertex &p3, int16_t i, const Scene& scene)
    {
        const float alphaA = (p1.ndc.z + p1.ndc.w) /
                             ((p1.ndc.z + p1.ndc.w) - (p2.ndc.z + p2.ndc.w));
        const float alphaB = (p1.ndc.z + p1.ndc.w) /
                             ((p1.ndc.z + p1.ndc.w) - (p3.ndc.z + p3.ndc.w));
        
        const auto p1a = p1 + (p2 - p1) * alphaA;
        const auto p1b = p1 + (p3 - p1) * alphaB;

        Triangle<vertex> tri(p1a, p2, p3, i);
        draw(tri, *solid, scene);
        Triangle<vertex> tri2(p1b, p1a, p3, i);
        draw(tri2, *solid, scene);
    };

    // p1 & p2 out, p3 in
    const auto Clip2Out1In = [this](vertex &p1, vertex &p2, vertex &p3, int16_t i, const Scene& scene)
    {
        const float alphaA = (p1.ndc.z + p1.ndc.w) /
                             ((p1.ndc.z + p1.ndc.w) - (p3.ndc.z + p3.ndc.w));
        const float alphaB = (p2.ndc.z + p2.ndc.w) /
                             ((p2.ndc.z + p2.ndc.w) - (p3.ndc.z + p3.ndc.w));

        p1 = p1 + (p3 - p1) * alphaA;
        p2 = p2 + (p3 - p2) * alphaB;

        Triangle<vertex> tri(p1, p2, p3, i);
        draw(tri, *solid, scene);
    };

    // Now proceed with detailed near-clipping logic:
    if (t.p1.ndc.z < -t.p1.ndc.w)
    {
        if (t.p2.ndc.z < -t.p2.ndc.w)
            Clip2Out1In(t.p1, t.p2, t.p3, t.i, scene);
        else if (t.p3.ndc.z < -t.p3.ndc.w)
            Clip2Out1In(t.p1, t.p3, t.p2, t.i, scene);
        else
            Clip1Out2In(t.p1, t.p2, t.p3, t.i, scene);
    }
    else if (t.p2.ndc.z < -t.p2.ndc.w)
    {
        if (t.p3.ndc.z < -t.p3.ndc.w)
            Clip2Out1In(t.p2, t.p3, t.p1, t.i, scene);
        else
            Clip1Out2In(t.p2, t.p1, t.p3, t.i, scene);
    }
    else if (t.p3.ndc.z < -t.p3.ndc.w)
    {
        Clip1Out2In(t.p3, t.p1, t.p2, t.i, scene);
    }
}







