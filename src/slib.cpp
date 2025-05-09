#include "slib.hpp"
#include <iostream>

namespace slib
{

    vec2& vec2::operator*=(const vec2& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    vec2& vec2::operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    vec2 vec2::operator-(const vec2& rhs) const
    {
        return {x - rhs.x, y - rhs.y};
    }

    vec2 vec2::operator+(const vec2& rhs) const
    {
        return {x + rhs.x, y + rhs.y};
    }

    zvec2& zvec2::operator*=(const zvec2& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        w *= rhs.w;
        return *this;
    }

    zvec2& zvec2::operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        w *= rhs;
        return *this;
    }

    zvec2& zvec2::operator/=(const float rhs)
    {
        x /= rhs;
        y /= rhs;
        w /= rhs;
        return *this;
    }

    zvec2& zvec2::operator+=(const zvec2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        w += rhs.w;
        return *this;
    }

    zvec2 zvec2::operator-(const zvec2& rhs) const
    {
        return {x - rhs.x, y - rhs.y, w - rhs.w};
    }

    zvec2 zvec2::operator+(const zvec2& rhs) const
    {
        return {x + rhs.x, y + rhs.y, w + rhs.w};
    }

    zvec2 zvec2::operator*(const float rhs) const
    {
        return {x * rhs, y * rhs, w * rhs};
    }
    
    zvec2 zvec2::operator/(const float rhs) const
    {
        return {x / rhs, y / rhs, w / rhs};
    }


    vec3& vec3::operator+=(const float rhs)
    {
        x += rhs;
        y += rhs;
        z += rhs;
        return *this;
    }

    vec3& vec3::operator-=(const float rhs)
    {
        x -= rhs;
        y -= rhs;
        z -= rhs;
        return *this;
    }

    vec3& vec3::operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    vec3& vec3::operator/=(const float rhs)
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }

    vec3 vec3::operator-(const float rhs) const
    {
        return {x - rhs, y - rhs, z - rhs};
    }

    vec3 vec3::operator+(const float rhs) const
    {
        return {x + rhs, y + rhs, z + rhs};
    }

    vec3 vec3::operator*(const float rhs) const
    {
        return {x * rhs, y * rhs, z * rhs};
    }

    vec3 vec3::operator/(const float rhs) const
    {
        return {x / rhs, y / rhs, z / rhs};
    }

    vec3& vec3::operator+=(const vec3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    vec3& vec3::operator-=(const vec3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    vec3& vec3::operator*=(const vec3& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    vec3& vec3::operator/=(const vec3& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    vec3 vec3::operator-(const vec3& rhs) const
    {
        return {x - rhs.x, y - rhs.y, z - rhs.z};
    }

    vec3 vec3::operator+(const vec3& rhs) const
    {
        return {x + rhs.x, y + rhs.y, z + rhs.z};
    }

    vec3 vec3::operator*(const vec3& rhs) const
    {
        return {x * rhs.x, y * rhs.y, z * rhs.z};
    }

    vec3 vec3::operator/(const vec3& rhs) const
    {
        return {x / rhs.x, y / rhs.y, z / rhs.z};
    }

    bool vec3::operator==(const vec3& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    bool vec3::operator==(float rhs) const
    {
        return x == rhs && y == rhs && z == rhs;
    }

    vec3 vec3::operator*(const mat4& rhs) const
    {
        mat4 lhs({{this->x, this->y, this->z}});
        lhs *= rhs;
        return {lhs.data[0][0], lhs.data[0][1], lhs.data[0][2]};
    }

    vec3& vec3::operator*=(const mat4& rhs)
    {
        mat4 lhs({{this->x, this->y, this->z}});
        lhs *= rhs;
        *this = {lhs.data[0][0], lhs.data[0][1], lhs.data[0][2]};
        return *this;
    }

    bool vec3::operator<(const vec3& rhs) const
    {
        return x < rhs.x && y < rhs.y && z < rhs.z;
    }

    bool vec3::operator>(const vec3& rhs) const
    {
        return !(*this < rhs);
    }

    bool vec3::operator<=(const vec3& rhs) const
    {
        return x <= rhs.x && y <= rhs.y && z <= rhs.z;
    }

    bool vec3::operator>=(const vec3& rhs) const
    {
        return x >= rhs.x && y >= rhs.y && z >= rhs.z;
    }

    vec3& vec3::operator=(const vec4& rhs)
    {
        this->x = rhs.x;
        this->y = rhs.y;
        this->z = rhs.z;
        return *this;
    }

    mat4& mat4::operator+=(const mat4& rhs)
    {
        const auto rhsrows = rhs.data.size();
        const auto rhscols = rhs.data.at(0).size();
        const auto lhsrows = data.size();
        const auto lhscols = data.at(0).size();

        if (lhscols != rhsrows)
        {
            std::cout << "Error: mat-size mismatch." << std::endl;
            exit(1);
        }
        auto maxCols = lhscols >= rhscols ? lhscols : rhscols;
        for (int col = 0; col < static_cast<int>(maxCols); ++col)
        {
            for (int row = 0; row < static_cast<int>(lhsrows); ++row)
            {
                data[col][row] += rhs.data[col][row];
            }
        }
        return *this;
    }

    mat4& mat4::operator*=(const mat4& rhs)
    {
        mat4 result({{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}});
    
        for (size_t i = 0; i < 4; ++i)
        {
            for (size_t j = 0; j < 4; ++j)
            {
                result.data[i][j] = 0;
                for (size_t k = 0; k < 4; ++k)
                {
                    result.data[i][j] += data[i][k] * rhs.data[k][j];
                }
            }
        }
    
        data = result.data;
        return *this;
    }
    

    mat4 mat4::operator*(const mat4& rhs) const
    {
        mat4 toReturn(data);
        return toReturn *= rhs;
    }

    vec4 mat4::operator*(const vec4& v) const
    {
        float res_x = data[0][0] * v.x + data[0][1] * v.y + data[0][2] * v.z + data[0][3] * v.w;
        float res_y = data[1][0] * v.x + data[1][1] * v.y + data[1][2] * v.z + data[1][3] * v.w;
        float res_z = data[2][0] * v.x + data[2][1] * v.y + data[2][2] * v.z + data[2][3] * v.w;
        float res_w = data[3][0] * v.x + data[3][1] * v.y + data[3][2] * v.z + data[3][3] * v.w;
        return vec4{res_x, res_y, res_z, res_w};
    }

    vec4 vec4::operator*(const mat4& m) const
    {
        float res_x = m.data[0][0] * (this->x) + m.data[1][0] * this->y + m.data[2][0] * this->z + m.data[3][0] * this->w;
        float res_y = m.data[0][1] * this->x + m.data[1][1] * this->y + m.data[2][1] * this->z + m.data[3][1] * this->w;
        float res_z = m.data[0][2] * this->x + m.data[1][2] * this->y + m.data[2][2] * this->z + m.data[3][2] * this->w;
        float res_w = m.data[0][3] * this->x + m.data[1][3] * this->y + m.data[2][3] * this->z + m.data[3][3] * this->w;
        return vec4{res_x, res_y, res_z, res_w};
    }

    vec4 vec4::operator*=(const mat4& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    vec4 vec4::operator-(const vec4& rhs) const
    {
        return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
    }

    vec4 vec4::operator+(const vec4& rhs) const
    {
        return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
    }

    vec4& vec4::operator+=(const vec4& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    vec4& vec4::operator-=(const vec4& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    vec4 vec4::operator*(const float rhs) const
    {
        return {x * rhs, y * rhs, z * rhs, w * rhs};	
    }

    vec4 vec4::operator/(const float rhs) const
    {
        return {x / rhs, y / rhs, z / rhs, w / rhs};
    }


} // namespace slib