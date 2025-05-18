#pragma once
#include "../slib.hpp"
#include "../color.hpp"

// solid color attribute not interpolated
class TexturedFlatEffect
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

            const auto& Ka = tri.material.Ka; // vec3
            const auto& Kd = tri.material.Kd; // vec3
            const auto& light = scene.lux;         // vec3

            tri.flatDiffuse = std::max(0.0f, smath::dot(tri.faceNormal,light));
            slib::vec3 color = Ka + Kd * tri.flatDiffuse;
            tri.flatColor = Color(color).toBgra(); // assumes vec3 uses .r/g/b or [0]/[1]/[2]
		}
	};

	class PixelShader
	{
	public:
		uint32_t operator()(Vertex& vRaster, const Scene& scene, Triangle<Vertex>& tri) const
		{

            float w = 1 / vRaster.tex.w;

            if (tri.material.map_Kd.textureFilter != slib::TextureFilter::BILINEAR) {

                auto tx = static_cast<int>(vRaster.tex.x * w * (tri.material.map_Kd.w - 1));
                auto ty = static_cast<int>(vRaster.tex.y * w * (tri.material.map_Kd.h - 1));
                int index = ( tx + ty * tri.material.map_Kd.w ) * tri.material.map_Kd.bpp;
                return Color(
                    tri.material.map_Kd.data[index] * tri.flatDiffuse,
                    tri.material.map_Kd.data[index + 1] * tri.flatDiffuse,
                    tri.material.map_Kd.data[index + 2] * tri.flatDiffuse).toBgra(); // assumes vec3 uses .r/g/b or [0]/[1]/[2]
            }  else {

                float tx = vRaster.tex.x * w * tri.material.map_Kd.w - 0.5f;
                float ty = vRaster.tex.y * w * tri.material.map_Kd.w - 0.5f;

                int left = std::clamp(static_cast<int>(tx), 0, tri.material.map_Kd.w - 2);
                int top = std::clamp(static_cast<int>(ty), 0, tri.material.map_Kd.h - 2);
                int right = left + 1;
                int bottom = top + 1;

                // Get the mantissa of the u/v
                float fracU = tx - static_cast<float>(left);
                float fracV = ty - static_cast<float>(top);

                // Calculate the distance (weight) for each corner
                float ul = (1.0f - fracU) * (1.0f - fracV);
                float ll = (1.0f - fracU) * fracV;
                float ur = fracU * (1.0f - fracV);
                float lr = fracU * fracV;

                // Texture index of above pixel samples
                auto topLeft = (top * tri.material.map_Kd.w + left) * tri.material.map_Kd.bpp;
                auto topRight = (top * tri.material.map_Kd.w + right) * tri.material.map_Kd.bpp;
                auto bottomLeft = (bottom * tri.material.map_Kd.w + left) * tri.material.map_Kd.bpp;
                auto bottomRight = (bottom * tri.material.map_Kd.w + right) * tri.material.map_Kd.bpp; 
                
                float red = ul * tri.material.map_Kd.data[topLeft] + 
                            ll * tri.material.map_Kd.data[bottomLeft] + 
                            ur * tri.material.map_Kd.data[topRight] +
                            lr * tri.material.map_Kd.data[bottomRight];
                float green = ul * tri.material.map_Kd.data[topLeft + 1] + 
                            ll * tri.material.map_Kd.data[bottomLeft + 1] +
                            ur * tri.material.map_Kd.data[topRight + 1] + 
                            lr * tri.material.map_Kd.data[bottomRight + 1];
                float blue = ul * tri.material.map_Kd.data[topLeft + 2] + 
                            ll * tri.material.map_Kd.data[bottomLeft + 2] +
                            ur * tri.material.map_Kd.data[topRight + 2] + 
                            lr * tri.material.map_Kd.data[bottomRight + 2];

                return Color(
                    red * tri.flatDiffuse,
                    green * tri.flatDiffuse,
                    blue * tri.flatDiffuse).toBgra(); // assumes vec3 uses .r/g/b or [0]/[1]/[2]     

            }

		}
	};
public:
    VertexShader vs;
    GeometryShader gs;
	PixelShader ps;
};