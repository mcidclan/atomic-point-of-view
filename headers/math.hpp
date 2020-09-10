/*
 * APoV project
 * By m-c/d in 2020
 */
 
#ifndef APOV_MATH_HPP
#define APOV_MATH_HPP

    #define u8 unsigned char
    #define u16 short unsigned int
    #define u32 unsigned int

    template<typename T>
    struct Vec3 {
        T x, y, z;
    };

    template<typename T>
    struct Vec4 {
        T x, y, z, w;
    };

    struct Voxel {
        u32 color;
        Vec4<u16> coordinates;
    };
    
    namespace math {
        template<typename T> T abs(const T);
        template<typename T> T getPower(T);
    };
#endif
