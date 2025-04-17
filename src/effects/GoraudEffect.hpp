#pragma once
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

        Vertex(int32_t px, int32_t py, float pz, float _ds) :
        p_x(px), p_y(py), p_z(pz), ds(_ds) {}

        Vertex operator+(const Vertex &v) const {
            return Vertex(p_x + v.p_x, p_y + v.p_y, p_z + v.p_z, v.ds);
        }

        Vertex operator-(const Vertex &v) const {
            return Vertex(p_x - v.p_x, p_y - v.p_y, p_z - v.p_z, ds - v.ds);
        }

        Vertex operator*(const float &rhs) const {
            return Vertex(p_x * rhs, p_y * rhs, p_z * rhs, ds * rhs);
        }

        Vertex operator/(const float &rhs) const {
            return Vertex(p_x / rhs, p_y / rhs, p_z / rhs, ds / rhs);
        }
        
	public:
        int32_t p_x;
        int32_t p_y;
        float p_z; 
        float ds;
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