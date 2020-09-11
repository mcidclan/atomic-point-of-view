#ifndef APOV_TYPES_HPP
#define APOV_TYPES_HPP

    #include <cstdio>
    
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
    
#endif
