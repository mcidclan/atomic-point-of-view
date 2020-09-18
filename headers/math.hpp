/*
 * APoV project
 * By m-c/d in 2020
 */
 
#ifndef APOV_MATH_HPP
#define APOV_MATH_HPP

    #include "./types.hpp"
    #include <cmath>
    
    namespace math {
        template<typename T> T abs(const T);
        template<typename T> T getPower(T);
        template<typename T> Vec3<T> mulVector(Vec3<T>, const float);
        // Quaternions related functions
        Vec4<float> getNormalized4(Vec4<float>);
        Vec4<float> getConjugate(const Vec4);
        Vec4<float> mulQuat(const Vec4<float> qa, const Vec4<float> qb);
        Vec4<float> getSandwichProduct(const Vec4, const Vec4);
        Vec4<float> getOrientedQuat(const Vec2<float>);
        Vec4<float> getReoriented(const Vec4<float>, const Vec2<float>);
    };
#endif
