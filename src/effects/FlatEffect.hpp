#pragma once
#include "../slib.hpp"
#include "../color.hpp"

// solid color attribute not interpolated
class FlatEffect
{
public:
	// the vertex type that will be input into the pipeline
	class Vertex
	{
	public:
    Vertex() {}

    Vertex(int32_t px, int32_t py, float pz, slib::vec4 vp) :
    p_x(px), p_y(py), p_z(pz), ndc(vp) {}

    Vertex operator+(const Vertex &v) const {
        return Vertex(p_x + v.p_x, p_y + v.p_y, p_z + v.p_z, ndc + v.ndc);
    }

    Vertex operator-(const Vertex &v) const {
        return Vertex(p_x - v.p_x, p_y - v.p_y, p_z - v.p_z, ndc - v.ndc);
    }

    Vertex operator*(const float &rhs) const {
        return Vertex(p_x * rhs, p_y * rhs, p_z * rhs, ndc * rhs);
    }

    Vertex operator/(const float &rhs) const {
        return Vertex(p_x / rhs, p_y / rhs, p_z / rhs, ndc / rhs);
    }

    Vertex(int32_t px) :
    p_x(px) {}
    

    Vertex& operator+=(const Vertex &v) {
        p_x += v.p_x;
        p_y += v.p_y;
        p_z += v.p_z;
        ndc += v.ndc;
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
		template<class I>
		Color operator()( const I& in ) const
		{
			return in.color;
		}
	};
public:
	PixelShader ps;
};