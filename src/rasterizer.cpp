#include <iostream>
#include <fstream>
#include "rasterizer.hpp"
#include "scene.hpp"
#include "slib.hpp"
#include "smath.hpp"

/*
Check if triangle is visible.
If the triangle is visible, we can proceed with the rasterization process.
The calculation is based on the cross product of the edges of the triangle.
- If the result is positive, the triangle is visible.
- If the result is negative, the triangle is not visible.
- If the result is zero, the triangle is coplanar with the screen.
This is a simplified version of the backface culling algorithm.
The backface culling algorithm is used to determine if a triangle is facing the camera or not.
If the triangle is facing away from the camera, we can skip the rasterization process.
*/

bool Rasterizer::visible(const triangle& triangle) {

    return (((int32_t) (triangle.p3.p_x-triangle.p2.p_x)*(triangle.p2.p_y-triangle.p1.p_y)) - ((int32_t) (triangle.p2.p_x-triangle.p1.p_x)*(triangle.p3.p_y-triangle.p2.p_y)) > 0);
};

bool Rasterizer::behind(const triangle& triangle) {

    return (triangle.p1.p_z < 2 || triangle.p2.p_z < 2 || triangle.p3.p_z < 2);
};

/*
Check if triangle is completely outside the screen.
If all vertices are outside the screen, we can skip the rasterization process.
*/

bool Rasterizer::outside(Scene& scene, const triangle& triangle) {

    return ((triangle.p1.p_x < 0 && triangle.p2.p_x < 0 && triangle.p3.p_x < 0) || 
            (triangle.p1.p_x >= scene.screen.width && triangle.p2.p_x >= scene.screen.width && triangle.p3.p_x >= scene.screen.width) ||
            (triangle.p1.p_y < 0 && triangle.p2.p_y < 0 && triangle.p3.p_y < 0) ||
            (triangle.p1.p_y >= scene.screen.height && triangle.p2.p_y >= scene.screen.height && triangle.p3.p_y >= scene.screen.height)
            );
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

void Rasterizer::draw(triangle& tri, const Solid& solid, Scene& scene) {


    uint32_t flatColor = 0x00000000;
    if (scene.shading == Shading::Flat) {
        slib::vec3 rotatedFacenormal;
        rotatedFacenormal = scene.normalTransformMat * slib::vec4(solid.faceNormals[tri.i], 0);
        float diff = std::max(0.0f, smath::dot(rotatedFacenormal,scene.lux));
        float bright = solid.faces[tri.i].material.properties.k_a+solid.faces[tri.i].material.properties.k_d * diff;
        flatColor = RGBAColor(solid.faces[tri.i].material.Ambient, (int32_t) (bright * 65536 * 4)).bgra_value;
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
                drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge13, tri.edge23, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
            } else {
                //This triangle finishes in the screen (but needs culling top pixels)
                cullTopPixels(tri.p1.p_y, tri.p2.p_y, left, tri.edge13, right, tri.edge12);
                drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge13, tri.edge12, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
                right = tri.p2;
                //Culling the bottom pixels of the bottom triangle
                tri.p3.p_y = std::min(tri.p3.p_y, scene.screen.height);
                drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge13, tri.edge23, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
            }
        } else {
            //Needs to cull the top and the bottom pixels 
            cullTopPixels(tri.p1.p_y, tri.p2.p_y, left, tri.edge13, right, tri.edge12);
            tri.p2.p_y = std::min(tri.p2.p_y, scene.screen.height);
            drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge13, tri.edge12, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
        };
    } else {
        if  (tri.p2.p_y < scene.screen.height) {
            if (tri.p2.p_y < 0) {
                right = right + (tri.edge13 * (tri.p2.p_y - tri.p1.p_y));
                left = tri.p2;
                cullTopPixels(tri.p2.p_y, tri.p3.p_y, left, tri.edge23, right, tri.edge13);
                tri.p3.p_y = std::min(tri.p3.p_y, scene.screen.height);
                drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge23, tri.edge13, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
            } else {
                cullTopPixels(tri.p1.p_y, tri.p2.p_y, left, tri.edge12, right, tri.edge13);
                drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge12, tri.edge13, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
                left = tri.p2;
                cullTopPixels(tri.p2.p_y, tri.p3.p_y, left, tri.edge23, right, tri.edge13);
                tri.p3.p_y = std::min(tri.p3.p_y, scene.screen.height);
                drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge23, tri.edge13, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
            }
        } else {
            cullTopPixels(tri.p1.p_y, tri.p2.p_y, left, tri.edge12, right, tri.edge13);
            tri.p2.p_y = std::min(tri.p2.p_y, scene.screen.height);
            drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge12, tri.edge13, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
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
            return vertex(INT32_MAX, 0, 0, 0, {0,0,0}, {0,0,0}, 0, {0,0,0});
        } else {
            return vertex(INT32_MIN, 0, 0, 0, {0,0,0}, {0,0,0}, 0, {0,0,0});
        }
    }
};

inline void Rasterizer::drawTriHalf(int32_t top, int32_t bottom, vertex& left, vertex& right, vertex leftDy, vertex rightDy, Scene& scene, const Face& face, uint32_t flatColor, uint32_t* precomputedShading) {

    for(int hy=(top * scene.screen.width); hy<(bottom * scene.screen.width); hy+=scene.screen.width) {
        vertex vDx = vertex(0, 0, 0, 0, {0,0,0}, {0,0,0}, 0, {0,0,0});
        int16_t dx = (right.p_x - left.p_x) >> 16;
        if (dx != 0) vDx = (right - left) / dx;
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
            if (zBuffer[hy + hx] > vRaster.p_z) {
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
                    case Shading::Precomputed:
                        pixels[hy + hx] = precomputedPhongShadingFragment(vRaster, scene, face, precomputedShading);
                        break;                                
                    default: pixels[hy + hx] = flatColor;
                }
                zBuffer[hy + hx] = vRaster.p_z;
            }
            vRaster += vDx;
        }
        left += leftDy;
        right += rightDy;
    }
};

inline uint32_t Rasterizer::gouraudShadingFragment(vertex vRaster, Scene& scene, Face face) {
    return RGBAColor(face.material.Ambient, (int32_t) ((face.material.properties.k_a+face.material.properties.k_d * vRaster.ds) * 65536 * 4)).bgra_value;
}

inline uint32_t Rasterizer::phongShadingFragment(vertex gRaster, Scene& scene, Face face) {

    slib::vec3 normal = smath::normalize(gRaster.normal);
    float diff = std::max(0.0f, smath::dot(normal,scene.lux));

    slib::vec3 R = smath::normalize(normal * 2.0f * smath::dot(normal,scene.lux) - scene.lux);
    float specAngle = std::max(0.0f, smath::dot(R,scene.eye)); // viewer
    float spec = std::pow(specAngle, face.material.properties.shininess);

    float bright = face.material.properties.k_a+face.material.properties.k_d * diff+ face.material.properties.k_s * spec;
    return RGBAColor(face.material.Ambient, (int32_t) (bright * 65536 * 0.98)).bgra_value;

}

inline uint32_t Rasterizer::blinnPhongShadingFragment(vertex gRaster, Scene& scene, Face face) {

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
    float spec = std::pow(specAngle, face.material.properties.shininess * 4); // Blinn Phong shininess needs *4 to be like Phong

    // Calculate brightness
    float bright = face.material.properties.k_a + 
                   face.material.properties.k_d * diff + 
                   face.material.properties.k_s * spec;

    // Final color composition (ambient color scaled by total brightness)
    return RGBAColor(face.material.Ambient, (int32_t)(bright * 65536 * 0.98)).bgra_value;
}

inline uint32_t Rasterizer::precomputedPhongShadingFragment(vertex gRaster, Scene& scene, Face face, uint32_t* precomputedShading) {

    slib::vec3 normal = smath::normalize(gRaster.normal);

    int16_t normal_x = std::max((int16_t) 0,std::min( (int16_t) (PRECOMPUTE_SIZE-1),(int16_t) (normal.x * PRECOMPUTE_SIZE/2 + PRECOMPUTE_SIZE/2)));
    int16_t normal_y = std::max((int16_t) 0,std::min( (int16_t) (PRECOMPUTE_SIZE-1),(int16_t) (normal.y * PRECOMPUTE_SIZE/2 + PRECOMPUTE_SIZE/2)));
    return RGBAColor(face.material.Ambient, precomputedShading[normal_y*PRECOMPUTE_SIZE+normal_x]).bgra_value;

}






