/*
 * APoV project
 * By m-c/d in 2020
 */
 
#ifndef APOV_MATH_HPP
#define APOV_MATH_HPP

    #include "./types.hpp"
    #include <cmath>
    
    namespace math {
        template<typename T>T abs(const T);
        template<typename T> T getPower(T);
        template <typename T> Vec3<T> mulVector(Vec3<T> v, const float s);
    };
#endif
