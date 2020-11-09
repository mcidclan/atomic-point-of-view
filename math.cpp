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

    template <typename T>
    float getNorm(const T v) {
        return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    }
    
    template float getNorm(Vec4<float>);


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
        return mulQuat(mulQuat(q, v), getConjugate(q)); //qvq-1
    }

    Vec4<float> getOrientedQuat(const Vec4<float> axis) {
        const float a = axis.w / 2.0f;
        const float s = sinf(a);
        return {
            s * axis.x,
            s * axis.y,
            s * axis.z,
            cosf(a)
        };
    }        
    
    Vec4<float> getReoriented(const Vec4<float> v, const Vec4<float> orientation) {        
        return getSandwichProduct(v, getOrientedQuat(orientation));
    }

/*    
    void mat4xmat4(float* const m1, float* const m2, float* const m3)
    {
        unsigned char
        i = 0,
        j = 0,
        k = 0;

        float
        *rw = NULL,
        *cl = NULL;


        for(i = 0; i < 16; i += 4)
        {
            rw = &(m1[i]);

            for(j = 0; j < 4; j++)
            {
                cl = &(m2[j]);
                m3[k] = cl[0]*rw[0] + cl[4]*rw[1] + cl[8]*rw[2] + cl[12]*rw[3];
                k++;
            }
        }
    }

    void eulerOrientation(const Vec4<float> axis, float* const m) {
        float
        h = sinf(axis.w),
        i = cosf(axis.w),
        j = 1.0f - i;

        m[0] = axis.x * axis.x * j + i;
        m[1] = axis.y * axis.x * j + axis.z * h;
        m[2] = axis.z * axis.x * j - axis.y * h;
        m[3] = 0.0f;
        m[4] = axis.x * axis.y * j - axis.z * h;
        m[5] = axis.y * axis.y * j + i;
        m[6] = axis.z * axis.y * j + axis.x * h;
        m[7] = 0.0f;

        m[8] = axis.x * axis.z * j + axis.y * h;
        m[9] = axis.y * axis.z * j - axis.x * h;
        m[10] = axis.z * axis.z * j + i;
        m[11] = 0.0f;

        m[12] = 0.0f;
        m[13] = 0.0f;
        m[14] = 0.0f;
        m[15] = 1.0f;
    }

    Vec4<float> transform(const Vec4<float> v, float* const m) {
        return {
            v.x * m[0] + v.y * m[1] + v.z * m[2] + m[3],
            v.x * m[4] + v.y * m[5] + v.z * m[6] + m[7],
            v.x * m[8] + v.y * m[9] + v.z * m[10] + m[11],
            0.0f
        };
    }
    */
};
