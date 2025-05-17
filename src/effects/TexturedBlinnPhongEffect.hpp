#pragma once
#include <cmath>
#include "../slib.hpp"
#include "../color.hpp"

// solid color attribute not interpolated
class TexturedBlinnPhongEffect
{
public:
	// the vertex type that will be input into the pipeline
	class Vertex
	{
	public:
    Vertex() {}

    Vertex(int32_t px, int32_t py, float pz, slib::vec3 n, slib::vec4 vp, slib::zvec2 _tex) :
    p_x(px), p_y(py), p_z(pz), normal(n), ndc(vp), tex(_tex) {}

    Vertex operator+(const Vertex &v) const {
        return Vertex(p_x + v.p_x, p_y + v.p_y, p_z + v.p_z, normal + v.normal, ndc + v.ndc, tex + v.tex);
    }

    Vertex operator-(const Vertex &v) const {
        return Vertex(p_x - v.p_x, p_y - v.p_y, p_z - v.p_z, normal - v.normal, ndc - v.ndc, tex - v.tex);
    }

    Vertex operator*(const float &rhs) const {
        return Vertex(p_x * rhs, p_y * rhs, p_z * rhs, normal * rhs, ndc * rhs, tex * rhs);
    }


    Vertex& operator+=(const Vertex &v) {
        p_x += v.p_x;
        p_y += v.p_y;
        p_z += v.p_z;
        normal += v.normal;
        ndc += v.ndc;
        tex += v.tex;
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

        void viewProjection(const Scene& scene, Vertex& p) {
            float oneOverW = 1.0f / p.ndc.w;
            p.p_x = static_cast<int>((p.ndc.x * oneOverW + 1.0f) * (scene.screen.width / 2.0f)); // Convert from NDC to screen coordinates
            p.p_y = static_cast<int>((p.ndc.y * oneOverW + 1.0f) * (scene.screen.height / 2.0f)); // Convert from NDC to screen coordinates
            p.p_z = p.ndc.z * oneOverW; // Store the depth value in the z-buffer
            p.tex.x = p.tex.x * oneOverW;
            p.tex.y = p.tex.y * oneOverW;
            p.tex.w = oneOverW;   
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
		uint32_t operator()(Vertex& vRaster, const Scene& scene, Triangle<Vertex>& tri) const
		{

            const auto& Ka = tri.material.Ka; // vec3
            const auto& Kd = tri.material.Kd; // vec3
            const auto& Ks = tri.material.Ks; // vec3
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
            float spec = std::powf(specAngle, tri.material.Ns); // Blinn Phong shininess needs *4 to be like Phong
        
            float w = 1 / vRaster.tex.w;
            auto tx = static_cast<int>(vRaster.tex.x * w * (tri.material.map_Kd.w - 1));
            auto ty = static_cast<int>(vRaster.tex.y * w * (tri.material.map_Kd.h - 1));
            int index = ( tx + ty * tri.material.map_Kd.w ) * tri.material.map_Kd.bpp;

            if (spec > 0.95) { 
                return 0xffffffff; // White point if the light is too close to the normal
            }
                
            return Color(
                tri.material.map_Kd.data[index] * diff + Ks.x * spec,
                tri.material.map_Kd.data[index + 1] * diff + Ks.y * spec,  
                tri.material.map_Kd.data[index + 2] * diff + Ks.z * spec).toBgra(); // assumes vec3 uses .r/g/b or [0]/[1]/[2]

        }
	};
public:
    VertexShader vs;
    GeometryShader gs;
    PixelShader ps;
};