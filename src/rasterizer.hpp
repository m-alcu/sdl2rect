#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <cstdint>
#include "objects/tetrakis.hpp"
#include "objects/torus.hpp"
#include "objects/test.hpp"
#include "objects/ascLoader.hpp"
#include "objects/objLoader.hpp"
#include "scene.hpp"
#include "slib.hpp"
#include "smath.hpp"
#include "tri.hpp"

enum class ClipPlane {
    Left, Right, Bottom, Top, Near, Far
};

template<class Effect>
class Rasterizer {
    public:
        Rasterizer() :  fullTransformMat(smath::identity()), 
                        normalTransformMat(smath::identity()),
                        viewMatrix(smath::identity())
          {}

        void drawRenderable(Solid& solid, Scene& scn) {
            setRenderable(&solid);
            scene = &scn;
            prepareRenderable();
            ProcessVertex();
            DrawFaces();
        }

    private:
        typedef typename Effect::Vertex vertex;
        std::vector<std::unique_ptr<vertex>> projectedPoints;
        std::vector<std::unique_ptr<Triangle<vertex>>> triangles;
        Solid* solid;  // Pointer to the abstract Solid
        Scene* scene; // Pointer to the Scene
        slib::mat4 fullTransformMat;
        slib::mat4 normalTransformMat;
        slib::mat4 viewMatrix;
        Effect effect;    
        
        void setRenderable(Solid* solidPtr) {
            projectedPoints.clear();
            projectedPoints.resize(solidPtr->numVertices);
            solid = solidPtr;
        }

        void prepareRenderable() {
            slib::mat4 rotate = smath::rotation(slib::vec3({solid->position.xAngle, solid->position.yAngle, solid->position.zAngle}));
            slib::mat4 translate = smath::translation(slib::vec3({solid->position.x, solid->position.y, solid->position.z}));
            slib::mat4 scale = smath::scale(slib::vec3({solid->position.zoom, solid->position.zoom, solid->position.zoom}));
            //slib::mat4 viewMatrix = smath::view(scene->camera.eye, scene->camera.target, scene->camera.up);
            viewMatrix = smath::fpsview(scene->camera.pos, scene->camera.pitch, scene->camera.yaw);

            fullTransformMat = translate * rotate * scale;
            normalTransformMat = rotate;
        }

        void ProcessVertex()
        {
            projectedPoints.resize(solid->numVertices);
        
            std::transform(
                solid->vertexData.begin(),
                solid->vertexData.end(),
                projectedPoints.begin(),
                [&](const auto& vData) {
                    return effect.vs(vData, fullTransformMat, viewMatrix, normalTransformMat, *scene);
                }
            );
        }

        void DrawFaces() {

            #pragma omp parallel for
            for (int i=0; i<solid->numFaces; i++) {
        
                Triangle<vertex> tri(
                    *projectedPoints[solid->faceData[i].face.vertex1],
                    *projectedPoints[solid->faceData[i].face.vertex2],
                    *projectedPoints[solid->faceData[i].face.vertex3],
                    i
                );
        
                if (Visible(tri)) {
                    ClipCullDrawTriangleSutherlandHodgman(tri);
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

        bool Visible(const Triangle<vertex>& triangle) {

            return (triangle.p3.p_x-triangle.p2.p_x)*(triangle.p2.p_y-triangle.p1.p_y) - (triangle.p2.p_x-triangle.p1.p_x)*(triangle.p3.p_y-triangle.p2.p_y) < 0;
        };

        /*
        Clipping is done using the Sutherland-Hodgman algorithm (1974) in the ndc space.
        The Sutherland-Hodgman algorithm is a polygon clipping algorithm that clips a polygon against a convex clipping region.
        The algorithm works by iterating through each edge of the polygon and checking if the vertices are inside or outside the clipping plane.
        If a vertex is inside, it is added to the output polygon. If a vertex is outside, the algorithm checks if the previous vertex was inside. If it was, the edge between the two vertices is clipped and the intersection point is added to the output polygon.
        The algorithm continues until all edges have been processed.
        https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
        */

        void ClipCullDrawTriangleSutherlandHodgman(const Triangle<vertex>& t) {
            std::vector<vertex> polygon = { t.p1, t.p2, t.p3 };

            for (ClipPlane plane : {ClipPlane::Left, ClipPlane::Right, ClipPlane::Bottom, 
                                    ClipPlane::Top, ClipPlane::Near, ClipPlane::Far}) {
                polygon = ClipAgainstPlane(polygon, plane);
                if (polygon.empty()) return; // Completely outside
            }

            // Triangulate fan-style and draw
            for (size_t i = 1; i + 1 < polygon.size(); ++i) {
                Triangle<vertex> tri(polygon[0], polygon[i], polygon[i + 1], t.i);
                draw(tri);
            }
        }

        std::vector<vertex> ClipAgainstPlane(const std::vector<vertex>& poly, ClipPlane plane) {
            std::vector<vertex> output;
            if (poly.empty()) return output;
        
            vertex prev = poly.back();
            bool prevInside = IsInside(prev, plane);
        
            for (const auto& curr : poly) {
                bool currInside = IsInside(curr, plane);
        
                if (currInside != prevInside) {
                    float alpha = ComputeAlpha(prev, curr, plane);
                    vertex interpolated = prev + (curr - prev) * alpha;
                    float oneOverW = 1.0f / interpolated.ndc.w;
                    interpolated.p_x = (int32_t) ((interpolated.ndc.x * oneOverW + 1.0f) * (scene->screen.width / 2.0f)); // Convert from NDC to screen coordinates
                    interpolated.p_y = (int32_t) ((interpolated.ndc.y * oneOverW + 1.0f) * (scene->screen.height / 2.0f)); // Convert from NDC to screen coordinates
                    interpolated.p_z = interpolated.ndc.z * oneOverW; // Store the depth value in the z-buffer
                    output.push_back(interpolated);
                }
        
                if (currInside)
                    output.push_back(curr);
        
                prev = curr;
                prevInside = currInside;
            }
        
            return output;
        } 
        
        bool IsInside(const vertex& v, ClipPlane plane) {
            const auto& p = v.ndc;
            switch (plane) {
                case ClipPlane::Left:   return p.x >= -p.w;
                case ClipPlane::Right:  return p.x <=  p.w;
                case ClipPlane::Bottom: return p.y >= -p.w;
                case ClipPlane::Top:    return p.y <=  p.w;
                case ClipPlane::Near:   return p.z >= -p.w;
                case ClipPlane::Far:    return p.z <=  p.w;
            }
            return false;
        }
        
        float ComputeAlpha(const vertex& a, const vertex& b, ClipPlane plane) {
            const auto& pa = a.ndc;
            const auto& pb = b.ndc;
            float num, denom;
        
            switch (plane) {
                case ClipPlane::Left:
                    num = pa.x + pa.w; denom = (pa.x + pa.w) - (pb.x + pb.w); break;
                case ClipPlane::Right:
                    num = pa.x - pa.w; denom = (pa.x - pa.w) - (pb.x - pb.w); break;
                case ClipPlane::Bottom:
                    num = pa.y + pa.w; denom = (pa.y + pa.w) - (pb.y + pb.w); break;
                case ClipPlane::Top:
                    num = pa.y - pa.w; denom = (pa.y - pa.w) - (pb.y - pb.w); break;
                case ClipPlane::Near:
                    num = pa.z + pa.w; denom = (pa.z + pa.w) - (pb.z + pb.w); break;
                case ClipPlane::Far:
                    num = pa.z - pa.w; denom = (pa.z - pa.w) - (pb.z - pb.w); break;
            }
        
            return denom != 0.0f ? num / denom : 0.0f;
        }
        
        /*
        Drawing a triangle with edges 12, 13 and 23.
        The triangle is divided into two halves, one for each edge.
        The left edge is the one with the smallest x coordinate, and the right edge is the one with the largest x coordinate.
        The left edge is drawn first, and then the right edge is drawn.

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

        void draw(Triangle<vertex>& tri) {

            effect.gs(tri, *solid, *scene, normalTransformMat);

            orderVertices(&tri.p1, &tri.p2, &tri.p3);
            tri.p1.p_x = tri.p1.p_x << 16; // shift to 16.16 space
            tri.p2.p_x = tri.p2.p_x << 16; // shift to 16.16 space
            tri.p3.p_x = tri.p3.p_x << 16; // shift to 16.16 space
            tri.edge12 = gradientDy(tri.p1, tri.p2);
            tri.edge23 = gradientDy(tri.p2, tri.p3);
            tri.edge13 = gradientDy(tri.p1, tri.p3);

            vertex left = tri.p1, right = tri.p1;
            if(tri.edge13.p_x < tri.edge12.p_x) {
                drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge13, tri.edge12, solid->faceData[tri.i].face, tri.flatColor);
                right = tri.p2;
                drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge13, tri.edge23, solid->faceData[tri.i].face, tri.flatColor);
            } else {
                drawTriHalf(tri.p1.p_y, tri.p2.p_y, left, right, tri.edge12, tri.edge13, solid->faceData[tri.i].face, tri.flatColor);
                left = tri.p2;
                drawTriHalf(tri.p2.p_y, tri.p3.p_y, left, right, tri.edge23, tri.edge13, solid->faceData[tri.i].face, tri.flatColor);
            }
        };

        inline void orderVertices(vertex *p1, vertex *p2, vertex *p3) {

            if (p1->p_y > p2->p_y) std::swap(*p1,*p2);
            if (p2->p_y > p3->p_y) std::swap(*p2,*p3);
            if (p1->p_y > p2->p_y) std::swap(*p1,*p2);
        };

        inline vertex gradientDy(vertex p1, vertex p2) {

            int dy = p2.p_y - p1.p_y;
            if (dy > 0) {
                float oneOverDy = 1.0f / dy;
                return (p2 - p1) * oneOverDy;
            } else {
                return p2.p_x - p1.p_x > 0 ? vertex(INT32_MAX) : vertex(INT32_MIN);
            }
        };

        inline void drawTriHalf(int32_t top, int32_t bottom, vertex& left, vertex& right, vertex leftDy, vertex rightDy, const Face& face, uint32_t flatColor) {

            auto* pixels = static_cast<uint32_t*>(scene->sdlSurface->pixels);
        
            for(int hy=(top * scene->screen.width); hy<(bottom * scene->screen.width); hy+=scene->screen.width) {
                int dx = (right.p_x - left.p_x) >> 16;

                if (dx != 0) {
                    float oneOverDx = 1.0f / dx;
                    vertex vRaster = left, vDx = (right - left) * oneOverDx;
                    for(int hx = left.p_x >> 16; hx < right.p_x >> 16; hx++) {
                        if (scene->zBuffer->TestAndSet(hy + hx, vRaster.p_z)) {
                            pixels[hy + hx] = effect.ps(vRaster, *scene, face, flatColor);
                        }
                        vRaster += vDx;
                    }
                }

                left += leftDy;
                right += rightDy;
            }
        };
        
    };
    

