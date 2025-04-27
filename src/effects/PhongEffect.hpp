#pragma once
#include "../slib.hpp"
#include "../color.hpp"

// solid color attribute not interpolated
class PhongEffect
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

    Vertex operator/(const float &rhs) const {
        return Vertex(p_x / rhs, p_y / rhs, p_z / rhs, normal / rhs, ndc / rhs, tex / rhs, color / rhs);
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
        slib::vec4 point;
        slib::vec3 normal;
        slib::vec4 ndc;
        slib::zvec2 tex; // Texture coordinates
        Color color;
	};
	class PixelShader
	{
	public:
		uint32_t operator()(Vertex& vRaster, const Scene& scene, const Face& face, uint32_t flatColor) const
		{
            slib::vec3 normal = smath::normalize(vRaster.normal);
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
	};
public:
	PixelShader ps;
};