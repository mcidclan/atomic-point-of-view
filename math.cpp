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
};
