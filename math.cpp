#include "headers/math.hpp"

namespace math {
    template<typename T>
    T abs(const T v) {        
        return v < 0 ? -v : v;
    }
    
    template i16 abs(const i16);
    template float abs(const float);

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
    
    
    template <typename T1, typename T2>
    Vec3<T2> mulVector(Vec3<T1>* const v, const float s) {
        return {
            (T2)(v->x * s),
            (T2)(v->y * s),
            (T2)(v->z * s)
        };
    }
    
    template Vec3<i16> mulVector(Vec3<float>* const, const float);


    template <typename T1, typename T2>
    void addVector(Vec3<T1>* const va, Vec3<T2>* const vb) {
        va->x += vb->x;
        va->y += vb->y;
        va->z += vb->z;
    }

    template void addVector(Vec3<i16>* const, Vec3<float>* const);

    
    // Quaternions related functions
    Vec4<float> getNormalized4(Vec4<float> v) {
        const float norm = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
        return {
            v.x / norm, v.y / norm, v.z / norm, v.w / norm
        };
    }

    Vec4<float> getConjugate(const Vec4<float> qa) {
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

    Vec4<float> getSandwichProduct(const Vec4<float> v, const Vec4<float> q) {
        return mulQuat(mulQuat(q, v), getConjugate(q));
    }
    
    Vec4<float> getOrientedQuat(const Vec4<float> axis) {
        float a = axis.w / 2.0;
        return getNormalized4({
            sinf(a) * axis.x,
            sinf(a) * axis.y,
            sinf(a) * axis.z,
            cosf(a)
        });
    }

    Vec4<float> getReoriented(const Vec4<float> v, const Vec4<float> orientation) {        
        return getSandwichProduct(v, getOrientedQuat(orientation));
    }
};
