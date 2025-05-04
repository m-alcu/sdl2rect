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
        slib::vec3 point;
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
            screenPoint.point = viewMatrix * fullTransformMat * slib::vec4(vData.vertex, 1);
            screenPoint.ndc = slib::vec4(screenPoint.point, 1) * scene.projectionMatrix;
            float oneOverW = 1.0f / screenPoint.ndc.w;
            screenPoint.p_x = (int32_t) ((screenPoint.ndc.x * oneOverW + 1.0f) * (scene.screen.width / 2.0f));
            screenPoint.p_y = (int32_t) ((screenPoint.ndc.y * oneOverW + 1.0f) * (scene.screen.height / 2.0f));
            screenPoint.p_z = screenPoint.ndc.z * oneOverW;
            return std::make_unique<Vertex>(screenPoint);
		}
	};

    class GeometryShader
	{
	public:
    
        void operator()(Triangle<Vertex>& tri, const Scene& scene, const slib::mat4& normalTransformMat) const
		{

            const auto& Ka = tri.face.material.Ka; // vec3
            const auto& Kd = tri.face.material.Kd; // vec3
            const auto& light = scene.lux;         // vec3

            slib::vec3 rotatedFacenormal;
            rotatedFacenormal = normalTransformMat * slib::vec4(tri.faceNormal, 0);
            float diff = std::max(0.0f, smath::dot(rotatedFacenormal,light));
            slib::vec3 color = Ka + Kd * diff;
            tri.flatColor = Color(color).toBgra(); // assumes vec3 uses .r/g/b or [0]/[1]/[2]
		}
	};

	class PixelShader
	{
	public:
		uint32_t operator()(Vertex& vRaster, const Scene& scene, const Face& face, uint32_t flatColor) const
		{
			return flatColor;
		}
	};
public:
    VertexShader vs;
    GeometryShader gs;
	PixelShader ps;
};