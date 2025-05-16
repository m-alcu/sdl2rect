#pragma once
#include <algorithm>
#include "../slib.hpp"
#include "../color.hpp"

// solid color attribute not interpolated
class GouraudEffect
{
public:
	// the vertex type that will be input into the pipeline
	class Vertex
	{
	public:
    Vertex() {}

    Vertex(int32_t px, int32_t py, float pz, slib::vec3 n, slib::vec4 vp, slib::zvec2 _tex, Color _color) :
    p_x(px), p_y(py), p_z(pz), normal(n), ndc(vp), tex(_tex), color(_color) {}

    Vertex operator+(const Vertex &v) const {
        return Vertex(p_x + v.p_x, p_y + v.p_y, p_z + v.p_z, normal + v.normal, ndc + v.ndc, tex + v.tex, color + v.color);
    }

    Vertex operator-(const Vertex &v) const {
        return Vertex(p_x - v.p_x, p_y - v.p_y, p_z - v.p_z, normal - v.normal, ndc - v.ndc, tex - v.tex, color - v.color);
    }

    Vertex operator*(const float &rhs) const {
        return Vertex(p_x * rhs, p_y * rhs, p_z * rhs, normal * rhs, ndc * rhs, tex * rhs, color * rhs);
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
        color += v.color;
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
        Color color;
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
            screenPoint.normal = normalTransformMat * slib::vec4(vData.normal, 0);
            return std::make_unique<Vertex>(screenPoint);
		}
	};
    
    class GeometryShader
	{
	public:
    
        void operator()(Triangle<Vertex>& tri, const Scene& scene) const
        {
            const auto& Ka = tri.material.Ka; // vec3
            const auto& Kd = tri.material.Kd; // vec3
            const auto& light = scene.lux;         // vec3
        
            auto computeColor = [&](const slib::vec3& normal) -> Color {
                float ds = std::max(0.0f, smath::dot(normal, light)); // diffuse scalar
                return Color(Ka + Kd * ds); // assumes vec3 uses .r/g/b or [0]/[1]/[2]
            };
        
            tri.p1.color = computeColor(tri.p1.normal);
            tri.p2.color = computeColor(tri.p2.normal);
            tri.p3.color = computeColor(tri.p3.normal);
        }
	};    

	class PixelShader
	{
	public:
		uint32_t operator()(Vertex& vRaster, const Scene& scene, Triangle<Vertex>& tri) const
		{
			return vRaster.color.toBgra();
		}
	};
public:
    VertexShader vs;
    GeometryShader gs;
	PixelShader ps;
};