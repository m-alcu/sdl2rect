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

    class VertexShader
	{
	public:
        std::unique_ptr<Vertex> operator()(const VertexData& vData, const slib::mat4& fullTransformMat, const slib::mat4& normalTransformMat, const Scene& scene) const
		{
            Vertex screenPoint;
            screenPoint.point = fullTransformMat * slib::vec4(vData.vertex, 1);
            screenPoint.normal = normalTransformMat * slib::vec4(vData.normal, 0);
            screenPoint.ndc = screenPoint.point * scene.projectionMatrix;
            screenPoint.p_x = (int32_t) ceil((screenPoint.ndc.x / screenPoint.ndc.w + 1.0f) * (scene.screen.width / 2.0f) - 0.5f);
            screenPoint.p_y = (int32_t) ceil((screenPoint.ndc.y / screenPoint.ndc.w + 1.0f) * (scene.screen.height / 2.0f) - 0.5f);
            screenPoint.p_z = screenPoint.ndc.z / screenPoint.ndc.w;
            return std::make_unique<Vertex>(screenPoint);
		}
	};
    
    class GeometryShader
	{
	public:
    
        void operator()(Triangle<Vertex>& tri, const Solid& solid, const Scene& scene, const slib::mat4& normalTransformMat) const
		{
            float ds = std::max(0.0f, smath::dot(tri.p1.normal, scene.lux));
            tri.p1.color = Color(
                std::min(solid.faceData[tri.i].face.material.Ka[2] + solid.faceData[tri.i].face.material.Kd[2] * ds, 255.0f),
                std::min(solid.faceData[tri.i].face.material.Ka[1] + solid.faceData[tri.i].face.material.Kd[1] * ds, 255.0f),
                std::min(solid.faceData[tri.i].face.material.Ka[0] + solid.faceData[tri.i].face.material.Kd[0] * ds, 255.0f));
            ds = std::max(0.0f, smath::dot(tri.p2.normal, scene.lux));
            tri.p2.color = Color(
                std::min(solid.faceData[tri.i].face.material.Ka[2] + solid.faceData[tri.i].face.material.Kd[2] * ds, 255.0f),
                std::min(solid.faceData[tri.i].face.material.Ka[1] + solid.faceData[tri.i].face.material.Kd[1] * ds, 255.0f),
                std::min(solid.faceData[tri.i].face.material.Ka[0] + solid.faceData[tri.i].face.material.Kd[0] * ds, 255.0f));
            ds = std::max(0.0f, smath::dot(tri.p3.normal, scene.lux));
            tri.p3.color = Color(
                std::min(solid.faceData[tri.i].face.material.Ka[2] + solid.faceData[tri.i].face.material.Kd[2] * ds, 255.0f),
                std::min(solid.faceData[tri.i].face.material.Ka[1] + solid.faceData[tri.i].face.material.Kd[1] * ds, 255.0f),
                std::min(solid.faceData[tri.i].face.material.Ka[0] + solid.faceData[tri.i].face.material.Kd[0] * ds, 255.0f));
		}
	};    

	class PixelShader
	{
	public:
		uint32_t operator()(Vertex& vRaster, const Scene& scene, const Face& face, uint32_t flatColor) const
		{
			return vRaster.color.toBgra();
		}
	};
public:
    VertexShader vs;
    GeometryShader gs;
	PixelShader ps;
};