#ifndef APOV_TYPES_HPP
#define APOV_TYPES_HPP

    #include <cstdio>
    #include <cstring>
    #include <string>
    #include <map>
    #include <vector>
    #include <algorithm>
    
    #define i8 char
    #define i16 short int
    #define i32 int
    
    #define u8 unsigned i8
    #define u16 unsigned i16
    #define u32 unsigned i32
    #define u64 uint64_t
    
    #define u32max ((u32)-1)
    
    template<typename T>
    struct Vec2 {
        T x, y;
    };
    
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
    
    struct Quanta {
        u32 color;
        float depth;
    };
    
    struct Pov {
        Vec4<float> raystep;
        Vec4<float> q;
    };
#endif
