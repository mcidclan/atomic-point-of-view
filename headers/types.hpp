#ifndef APOV_TYPES_HPP
#define APOV_TYPES_HPP

    #include <cstdio>
    
    #define i8 char
    #define i16 short int
    #define i32 int
    
    #define u8 unsigned i8
    #define u16 unsigned i16
    #define u32 unsigned i32
    
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
        Vec4<i16> coordinates;
    };
    
#endif
