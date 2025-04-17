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

        Vertex(int32_t px, int32_t py, float pz, slib::vec3 n, slib::vec4 vp, float _ds) :
        p_x(px), p_y(py), p_z(pz), normal(n), vertexPoint(vp), ds(_ds) {}

        Vertex operator+(const Vertex &v) const {
            return Vertex(p_x + v.p_x, p_y + v.p_y, p_z + v.p_z, normal + v.normal, vertexPoint + v.vertexPoint, ds + v.ds);
        }

        Vertex operator-(const Vertex &v) const {
            return Vertex(p_x - v.p_x, p_y - v.p_y, p_z - v.p_z, normal - v.normal, vertexPoint - v.vertexPoint, ds - v.ds);
        }

        Vertex operator*(const float &rhs) const {
            return Vertex(p_x * rhs, p_y * rhs, p_z * rhs, normal * rhs, vertexPoint * rhs, ds * rhs);
        }

        Vertex operator/(const float &rhs) const {
            return Vertex(p_x / rhs, p_y / rhs, p_z / rhs, normal / rhs, vertexPoint / rhs, ds / rhs);
        }

        Vertex& operator+=(const Vertex &v) {
            p_x += v.p_x;
            p_y += v.p_y;
            p_z += v.p_z;
            normal += v.normal;
            vertexPoint += v.vertexPoint;
            ds += v.ds;
            return *this;
        }
        
	public:
        int32_t p_x;
        int32_t p_y;
        float p_z; 
        slib::vec3 normal;
        slib::vec4 vertexPoint;
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