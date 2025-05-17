#pragma once
#include <algorithm>
#include "../slib.hpp"
#include "../color.hpp"

// solid color attribute not interpolated
class TexturedGouraudEffect
{
public:
	// the vertex type that will be input into the pipeline
	class Vertex
	{
	public:
    Vertex() {}

    Vertex(int32_t px, int32_t py, float pz, slib::vec3 n, slib::vec4 vp, slib::zvec2 _tex, float _diffuse) :
    p_x(px), p_y(py), p_z(pz), normal(n), ndc(vp), tex(_tex), diffuse(_diffuse) {}

    Vertex operator+(const Vertex &v) const {
        return Vertex(p_x + v.p_x, p_y + v.p_y, p_z + v.p_z, normal + v.normal, ndc + v.ndc, tex + v.tex, diffuse + v.diffuse);
    }

    Vertex operator-(const Vertex &v) const {
        return Vertex(p_x - v.p_x, p_y - v.p_y, p_z - v.p_z, normal - v.normal, ndc - v.ndc, tex - v.tex, diffuse - v.diffuse);
    }

    Vertex operator*(const float &rhs) const {
        return Vertex(p_x * rhs, p_y * rhs, p_z * rhs, normal * rhs, ndc * rhs, tex * rhs, diffuse * rhs);
    }

    Vertex(int32_t px) :
    p_x(px) {}
    

    Vertex& operator+=(const Vertex &v) {
        p_x += v.p_x;
        p_y += v.p_y;
        p_z += v.p_z;
        normal += v.normal;
        ndc += v.ndc;
        tex += v.tex;
        diffuse += v.diffuse;
        return *this;
    }
        
	public:
        int32_t p_x;
        int32_t p_y;
        float p_z; 
        slib::vec3 world;
        slib::vec3 point;
        slib::vec3 normal;
        slib::vec4 ndc;
        slib::zvec2 tex; // Texture coordinates
        float diffuse; // Diffuse color
	};

    class VertexShader
	{
	public:
        std::unique_ptr<Vertex> operator()(const VertexData& vData, const slib::mat4& fullTransformMat, const slib::mat4& viewMatrix, const slib::mat4& normalTransformMat, const Scene& scene) const
		{
            Vertex screenPoint;
            screenPoint.world = fullTransformMat * slib::vec4(vData.vertex, 1);
            screenPoint.point =  slib::vec4(screenPoint.world, 1) * viewMatrix;
            screenPoint.ndc = slib::vec4(screenPoint.point, 1) * scene.projectionMatrix;
            screenPoint.tex = slib::zvec2(vData.texCoord.x, vData.texCoord.y, 1);
            screenPoint.normal = normalTransformMat * slib::vec4(vData.normal, 0);
            return std::make_unique<Vertex>(screenPoint);
		}
	};
    
    class GeometryShader
	{
	public:
    
        void operator()(Triangle<Vertex>& tri, const Scene& scene) const
        {
            tri.p1.diffuse = std::max(0.0f, smath::dot(tri.p1.normal, scene.lux));
            tri.p2.diffuse = std::max(0.0f, smath::dot(tri.p2.normal, scene.lux));
            tri.p3.diffuse = std::max(0.0f, smath::dot(tri.p3.normal, scene.lux));
        }
	};    

	class PixelShader
	{
	public:
		uint32_t operator()(Vertex& vRaster, const Scene& scene, Triangle<Vertex>& tri) const
		{

            float w = 1 / vRaster.tex.w;
            auto tx = static_cast<int>(vRaster.tex.x * w * (tri.material.map_Kd.w - 1));
            auto ty = static_cast<int>(vRaster.tex.y * w * (tri.material.map_Kd.h - 1));
            int index = ( tx + ty * tri.material.map_Kd.w ) * tri.material.map_Kd.bpp;

            return Color(
                tri.material.map_Kd.data[index] * vRaster.diffuse,
                tri.material.map_Kd.data[index + 1] * vRaster.diffuse,  
                tri.material.map_Kd.data[index + 2] * vRaster.diffuse).toBgra(); // assumes vec3 uses .r/g/b or [0]/[1]/[2]
		}
	};
public:
    VertexShader vs;
    GeometryShader gs;
	PixelShader ps;
};