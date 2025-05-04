#pragma once
#include <array>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "slib.hpp"

namespace slib
{
    struct vec2;
    struct vec3;
    struct vec4;
    struct mat4;

    struct material;

    struct texture
    {
        int w, h;
        std::vector<unsigned char> data;
        unsigned int bpp;
    };

    struct zvec2
    {
        float x, y, w;
        zvec2& operator*=(const zvec2& rhs);
        zvec2& operator*=(float rhs);
        zvec2& operator/=(float rhs);
        zvec2 operator-(const zvec2& rhs) const;
        zvec2 operator+(const zvec2& rhs) const;
        zvec2 operator*(float rhs) const;
        zvec2 operator/(float rhs) const;
        zvec2& operator+=(const zvec2& rhs);
    };

    struct vec2
    {
        float x, y;
        vec2& operator*=(const vec2& rhs);
        vec2& operator*=(float rhs);
        vec2 operator-(const vec2& rhs) const;
        vec2 operator+(const vec2& rhs) const;
    };

    struct vec3
    {
        float x, y, z;
        vec3& operator+=(float rhs);
        vec3& operator-=(float rhs);
        vec3& operator*=(float rhs);
        vec3& operator/=(float rhs);
        vec3 operator-(float rhs) const;
        vec3 operator+(float rhs) const;
        vec3 operator*(float rhs) const;
        vec3 operator/(float rhs) const;
        vec3& operator+=(const vec3& rhs);
        vec3& operator-=(const vec3& rhs);
        vec3& operator*=(const vec3& rhs);
        vec3& operator/=(const vec3& rhs);
        vec3 operator-(const vec3& rhs) const;
        vec3 operator+(const vec3& rhs) const;
        vec3 operator*(const vec3& rhs) const;
        vec3 operator/(const vec3& rhs) const;
        vec3 operator*(const mat4& rhs) const;
        vec3& operator*=(const mat4& rhs);
        bool operator==(const vec3& rhs) const;
        bool operator==(float rhs) const;
        bool operator<(const vec3& rhs) const;
        bool operator>(const vec3& rhs) const;
        bool operator<=(const vec3& rhs) const;
        bool operator>=(const vec3& rhs) const;
        vec3& operator=(const vec4& rhs);
    };

    struct vec4
    {
        float x, y, z, w;
        vec4 operator*(const mat4& rhs) const;
        vec4 operator*=(const mat4& rhs);
        vec4 operator-(const vec4& rhs) const;
        vec4 operator+(const vec4& rhs) const;
        vec4& operator+=(const vec4& rhs);
        vec4& operator-=(const vec4& rhs);
        vec4 operator*(float rhs) const;
        vec4 operator/(float rhs) const;
        vec4() = default;
        vec4(const vec3& v3, const float _w) : x(v3.x), y(v3.y), z(v3.z), w(_w)
        {
        }
        vec4(const float _x, const float _y, const float _z, const float _w) : x(_x), y(_y), z(_z), w(_w)
        {
        }
    };

    struct mat4
    {
        std::vector<std::vector<float>> data;
        explicit mat4(std::vector<std::vector<float>> _data) : data(std::move(_data)){};

        mat4& operator+=(const mat4& rhs);
        mat4& operator*=(const mat4& rhs);
        mat4 operator*(const mat4& rhs) const;
        vec4 operator*(const vec4& rhs) const;
    };

    struct material
    {
        float Ns{};
        vec3 Ka{};
        vec3 Kd{};
        vec3 Ks{};
        vec3 Ke{};
        float Ni{};
        float d{};
        int illum{};
        texture map_Kd;
        texture map_Ks;
        texture map_Ns;
    };
} // namespace slib
