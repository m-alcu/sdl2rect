#pragma once
#include <cmath>
#include "../slib.hpp"
#include "../color.hpp"

// solid color attribute not interpolated
class BlinnPhongEffect
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
            screenPoint.point = viewMatrix * slib::vec4(screenPoint.world, 1);
            screenPoint.normal = normalTransformMat * slib::vec4(vData.normal, 0);
            screenPoint.ndc = slib::vec4(screenPoint.point, 1) * scene.projectionMatrix;
            return std::make_unique<Vertex>(screenPoint);
		}
	};

    class GeometryShader
	{
	public:
    
        void operator()(Triangle<Vertex>& tri, const Scene& scene) const
		{
		}
	};      

	class PixelShader
	{
	public:
		uint32_t operator()(Vertex& vRaster, const Scene& scene, const Face& face, uint32_t flatColor) const
		{

            const auto& Ka = face.material.Ka; // vec3
            const auto& Kd = face.material.Kd; // vec3
            const auto& Ks = face.material.Ks; // vec3
            // Normalize vectors
            slib::vec3 N = smath::normalize(vRaster.normal); // Normal at the fragment
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
                
            slib::vec3 color = Ka + Kd * diff + Ks * spec;
            return Color(color).toBgra();
        }
	};
public:
    VertexShader vs;
    GeometryShader gs;
    PixelShader ps;
};