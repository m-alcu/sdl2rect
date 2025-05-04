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
                    solid->faceData[i].face,
                    solid->faceData[i].faceNormal
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
                Triangle<vertex> tri(polygon[0], polygon[i], polygon[i + 1], t.face, t.faceNormal);
                draw(tri,
                    [&](const vertex from, const vertex to, int num_steps)
                    {
                        // Retrieve X coordinates for begin and end.
                        // Number of steps = number of scanlines
                        return Slope( from, to, num_steps );
                    } 
                );
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
        Drawing a triangle with scanline rasterization.
        The algorithm works by iterating through each scanline of the triangle and determining the left and right edges of the triangle at that scanline.
        For each scanline, the algorithm calculates the x-coordinates of the left and right edges of the triangle and fills in the pixels between them.
        The algorithm uses a slope to determine the x-coordinates of the left and right edges of the triangle at each scanline.
        */

        class Slope
        {
            vertex begin, step;
        public:
            Slope() {}
            Slope(vertex from, vertex to, int num_steps)
            {
                float inv_step = 1.f / num_steps;
                begin = from;                   // Begin here
                step  = (to - from) * inv_step; // Stepsize = (end-begin) / num_steps
            }
            vertex get() const { return begin; }
            int getx() const { return begin.p_x >> 16; }
            void advance()    { begin += step; }
        };

        void draw(Triangle<vertex>& tri, auto&& MakeSlope) {

            orderVertices(&tri.p1, &tri.p2, &tri.p3);
            if(tri.p1.p_y == tri.p3.p_y) return;
            bool shortside = (tri.p2.p_y - tri.p1.p_y) * (tri.p3.p_x - tri.p1.p_x) < (tri.p2.p_x - tri.p1.p_x) * (tri.p3.p_y - tri.p1.p_y); // false=left side, true=right side

            effect.gs(tri, *scene, normalTransformMat);

            tri.p1.p_x = tri.p1.p_x << 16; // shift to 16.16 space
            tri.p2.p_x = tri.p2.p_x << 16; // shift to 16.16 space
            tri.p3.p_x = tri.p3.p_x << 16; // shift to 16.16 space

            std::invoke_result_t<decltype(MakeSlope), vertex, vertex,int> sides[2];

            sides[!shortside] = MakeSlope(tri.p1,tri.p3, tri.p3.p_y - tri.p1.p_y);

            for(auto y = tri.p1.p_y, endy = tri.p1.p_y, hy = y * scene->screen.width; ; ++y)
            {
                if(y >= endy)
                {
                    // If y of p2 is reached, the triangle is complete.
                    if(y >= tri.p3.p_y) break;
                    // Recalculate slope for short side. The number of lines cannot be zero.
                    sides[shortside] = std::apply(MakeSlope, (y < tri.p2.p_y) ? std::tuple(tri.p1, tri.p2, (endy=tri.p2.p_y) - tri.p1.p_y)
                                                                              : std::tuple(tri.p2, tri.p3, (endy=tri.p3.p_y) - tri.p2.p_y) );
                }
                // On a single scanline, we go from the left X coordinate to the right X coordinate.
                DrawScanline(hy, sides[0], sides[1], tri.face, tri.flatColor);
                hy += scene->screen.width; 
            }

        };
 
        inline void orderVertices(vertex *p1, vertex *p2, vertex *p3) {
            if (p1->p_y > p2->p_y) std::swap(*p1,*p2);
            if (p2->p_y > p3->p_y) std::swap(*p2,*p3);
            if (p1->p_y > p2->p_y) std::swap(*p1,*p2);
        };
        
        inline void DrawScanline(const int& y, Slope& left, Slope& right, const Face& face, const uint32_t flatColor) {
            auto* pixels = static_cast<uint32_t*>(scene->sdlSurface->pixels);
            int xStart = left.getx();
            int xEnd = right.getx();
            int dx = xEnd - xStart;
        
            if (dx != 0) {
                float invDx = 1.0f / dx;
                vertex vStart = left.get();
                vertex vStep = (right.get() - vStart) * invDx;
        
                for (int x = xStart; x < xEnd; ++x) {
                    int index = y + x;
                    if (scene->zBuffer->TestAndSet(index, vStart.p_z)) {
                        pixels[index] = effect.ps(vStart, *scene, face, flatColor);
                    }
                    vStart += vStep;
                }
            }
        
            left.advance();
            right.advance();
        }

    };
    

