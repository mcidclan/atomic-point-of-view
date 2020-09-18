#include "headers/math.hpp"

namespace math {
    template<typename T>
    T abs(const T v) {        
        return v < 0 ? -v : v;
    }
    
    template i16 abs(const i16);


    template<typename T>
    T getPower(T value) {
        T power = 0;
        while(value > 1) {
            if((value & 1) != 0) {
                return 0;
            }
            power++;
            value >>= 1;
        }
        return power;
    }
    
    template u16 getPower(u16);
    
    
    template <typename T>
    Vec3<T> mulVector(Vec3<T> v, const float s) {
        v.x *= s;
        v.y *= s;
        v.z *= s;
        return v;
    }
    
    template Vec3<float> mulVector(Vec3<float>, const float);


    // Quaternions related functions
    Vec4<float> getNormalized4(Vec4<float> v) {
        const float norm = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
        return {
            v.x / norm, v.y / norm, v.z / norm, v.w / norm
        }
    }

    Vec4<float> getConjugate(const Vec4 qa) {
        return {
            -qa.x, -qa.y, -qa.z, qa.w
        };
    }

    Vec4<float> mulQuat(const Vec4<float> qa, const Vec4<float> qb) {
        return {
            qa.w * qb.x + qa.x * qb.w + qa.y * qb.z - qa.z * qb.y,
            qa.w * qb.y + qa.y * qb.w + qa.z * qb.x - qa.x * qb.z,
            qa.w * qb.z + qa.z * qb.w + qa.x * qb.y - qa.y * qb.x,
            qa.w * qb.w - qa.x * qb.x - qa.y * qb.y - qa.z * qb.z
        };
    }

    Vec4<float> getSandwichProduct(const Vec4 v, const Vec4 q) {
        return mulQuat(mulQuat(q, v), getConjugate(q));
    }

    Vec4<float> getOrientedQuat(const Vec2<float> orientation) {
        if(abs(y) > 1.0) {
            y = y / abs(y);
        }
        y = 1.0 - y;
        return normalize({x, y, -1.0, 0.0});
    }

    Vec4<float> getReoriented(const Vec4<float> v, const Vec2<float> orientation) {        
        return getSandwichProduct(position, getOrientedQuat(orientation.x);
    }
};
