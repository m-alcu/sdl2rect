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

    orderVertices(&tri.p1, &tri.p2, &tri.p3);
    tri.edge12 = gradientDy(tri.p1, tri.p2, scene.lux, solid.faces[tri.i]);
    tri.edge23 = gradientDy(tri.p2, tri.p3, scene.lux, solid.faces[tri.i]);
    tri.edge13 = gradientDy(tri.p1, tri.p3, scene.lux, solid.faces[tri.i]);

    uint32_t flatColor = 0x00000000;
    if (scene.shading == Shading::Flat) {
        slib::vec3 rotatedFacenormal;
        rotatedFacenormal = scene.normalTransformMat * slib::vec4(solid.faceNormals[tri.i], 0);
        float diff = std::max(0.0f, smath::dot(rotatedFacenormal,scene.lux));
        float bright = solid.faces[tri.i].material.properties.k_a+solid.faces[tri.i].material.properties.k_d * diff;
        flatColor = RGBAColor(solid.faces[tri.i].material.Ambient, (int32_t) (bright * 65536 * 4)).bgra_value;
    }

    vertex left = vertex(tri.p1, scene.lux, solid.faces[tri.i]);
    vertex right = left;
    if(tri.edge13.p_x < tri.edge12.p_x) {
        if (tri.p2.p_y < scene.screen.height) {

            /*
            This case happens when both top and bottom half of the triangle are inside (complete or not) the screen.
            - draw the top half of the triangle (p1 -> p2)
            - update the 2nd vertex (p2)
            - draw the bottom half of the triangle (p2, p3) culling the bottom pixels greater than the screen height.
            */
            if (tri.p2.p_y < 0) {
                left = left + (tri.edge13 * (tri.p2.p_y - tri.p1.p_y));
            } else {
                drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge13, tri.edge12, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
            }
            update2ndVertex(right, tri.p2, scene.lux, solid.faces[tri.i]);
            tri.p3.p_y = std::min(tri.p3.p_y, scene.screen.height);
            drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge13, tri.edge23, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
        } else {

            /*
            This case happens when only top half of the triangle is inside (complete or not) the screen.
            - draw the top half of the triangle (p1, p2) culling the bottom pixels greater than the screen height.
            */

            tri.p2.p_y = std::min(tri.p2.p_y, scene.screen.height);
            drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge13, tri.edge12, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
        };
    } else {
        if  (tri.p2.p_y < scene.screen.height) {
            if (tri.p2.p_y < 0) {
                right = right + (tri.edge13 * (tri.p2.p_y - tri.p1.p_y));
            } else {
                drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge12, tri.edge13, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
            }
            update2ndVertex(left, tri.p2, scene.lux, solid.faces[tri.i]);
            tri.p3.p_y = std::min(tri.p3.p_y, scene.screen.height);
            drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge23, tri.edge13, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
        } else {
            tri.p2.p_y = std::min(tri.p2.p_y, scene.screen.height);
            drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge12, tri.edge13, scene, solid.faces[tri.i], flatColor, solid.precomputedShading);
        };
    }
};

void Rasterizer::orderVertices(vertex *p1, vertex *p2, vertex *p3) {

    if (p1->p_y > p2->p_y) std::swap(*p1,*p2);
    if (p2->p_y > p3->p_y) std::swap(*p2,*p3);
    if (p1->p_y > p2->p_y) std::swap(*p1,*p2);
}

// Computes the pixel step gradient from left and right gradients.
// left: starting gradient
// right: ending gradient
vertex Rasterizer::gradientDx(const vertex &left, const vertex &right) {

    // Calculate the change in x, then shift right by 16 bits to get pixel steps.
    int16_t dx = (right.p_x - left.p_x) >> 16;

    if (dx == 0) return vertex(0, 0, 0, 0, {0,0,0}, {0,0,0}, 0, {0,0,0}); // Avoid division by zero
    slib::vec3 v = (right.vertexPoint - left.vertexPoint) / dx;
    slib::vec3 n = (right.normal - left.normal) / dx;
    float dz = (right.p_z - left.p_z) / dx;
    int32_t ds = (right.ds - left.ds) / dx;
    slib::zvec2 tex = (right.tex - left.tex) / dx;
    return vertex(dx, 0, dz, 0, n, v, ds, tex);
}

vertex Rasterizer::gradientDy(vertex p1, vertex p2, slib::vec3& lux, Face face) {

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

void Rasterizer::update2ndVertex(vertex& updated, const vertex &p, slib::vec3& lux, Face face) {
    updated.p_x = ( p.p_x << 16 ) + 0x8000; // shift to pixel space
    updated.p_z = p.p_z;
    updated.vertexPoint = p.vertexPoint;
    updated.normal = p.normal;
    float bright = face.material.properties.k_a+face.material.properties.k_d * std::max(0.0f, smath::dot(lux, updated.normal));
    updated.ds = (int32_t) (bright * 65536 * 4);
    updated.tex = p.tex;
}

void Rasterizer::drawTriHalf(int32_t top, int32_t bottom, vertex& left, vertex& right, vertex leftDy, vertex rightDy, Scene& scene, const Face& face, uint32_t flatColor, uint32_t* precomputedShading) {

    // Clip the triangle to the screen bounds
    if (top < 0) {
        int32_t final = std::min(bottom, 0);
        left = left + (leftDy * (final - top));
        right = right + (rightDy * (final - top));
        top = final;
    }

    for(int hy=(top * scene.screen.width); hy<(bottom * scene.screen.width); hy+=scene.screen.width) {
        vertex vDx = Rasterizer::gradientDx(left, right);
        vertex vRaster = left;

        int32_t xInitial = left.p_x >> 16;
        
        if (xInitial < 0) {
            int32_t xFinal = std::min(right.p_x >> 16, 0);
            vRaster = vRaster + vDx * (xFinal - xInitial);
            xInitial = xFinal;
        }
        int32_t xFinal = std::min(right.p_x >> 16, scene.screen.width);
        for(int hx = xInitial; hx < xFinal; hx++) {
            if (zBuffer[hy + hx] > vRaster.p_z) {
                switch (scene.shading) {
                    case Shading::Flat: 
                        pixels[hy + hx] = flatColor;
                        break;      
                    case Shading::Gouraud: 
                        pixels[hy + hx] = RGBAColor(face.material.Ambient, vRaster.ds).bgra_value;
                        break;
                    case Shading::BlinnPhong:
                        pixels[hy + hx] = blinnPhongShadingShader(vRaster, scene, face);
                        break;                                
                    case Shading::Phong:
                        pixels[hy + hx] = phongShadingShader(vRaster, scene, face);
                        break;
                    case Shading::Precomputed:
                        pixels[hy + hx] = precomputedPhongShadingShader(vRaster, scene, face, precomputedShading);
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


uint32_t Rasterizer::phongShadingShader(vertex gRaster, Scene& scene, Face face) {

    slib::vec3 normal = smath::normalize(gRaster.normal);
    float diff = std::max(0.0f, smath::dot(normal,scene.lux));

    slib::vec3 R = smath::normalize(normal * 2.0f * smath::dot(normal,scene.lux) - scene.lux);
    float specAngle = std::max(0.0f, smath::dot(R,scene.eye)); // viewer
    float spec = std::pow(specAngle, face.material.properties.shininess);

    float bright = face.material.properties.k_a+face.material.properties.k_d * diff+ face.material.properties.k_s * spec;
    return RGBAColor(face.material.Ambient, (int32_t) (bright * 65536 * 0.98)).bgra_value;

}

uint32_t Rasterizer::blinnPhongShadingShader(vertex gRaster, Scene& scene, Face face) {

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

uint32_t Rasterizer::precomputedPhongShadingShader(vertex gRaster, Scene& scene, Face face, uint32_t* precomputedShading) {

    slib::vec3 normal = smath::normalize(gRaster.normal);

    int16_t normal_x = std::max((int16_t) 0,std::min( (int16_t) (PRECOMPUTE_SIZE-1),(int16_t) (normal.x * PRECOMPUTE_SIZE/2 + PRECOMPUTE_SIZE/2)));
    int16_t normal_y = std::max((int16_t) 0,std::min( (int16_t) (PRECOMPUTE_SIZE-1),(int16_t) (normal.y * PRECOMPUTE_SIZE/2 + PRECOMPUTE_SIZE/2)));
    return RGBAColor(face.material.Ambient, precomputedShading[normal_y*PRECOMPUTE_SIZE+normal_x]).bgra_value;

}






