/*
 * APoV project
 * By m-c/d in 2020
 */
 
#ifndef APOV_MATH_HPP
#define APOV_MATH_HPP

    #include "./types.hpp"
    #include <cmath>
    
    #define M_PI 3.14159265358979323846
    #define _ang(x) ((float)M_PI/180.0f) * x
    
    namespace math {
        template<typename T>
        T abs(const T);
        
        template<typename T>
        T getPower(T);
        
        template<typename T1, typename T2>
        Vec3<T2> mulVector(Vec3<T1>* const, const float);
        
        template <typename T1, typename T2>
        void addVector(Vec3<T1>* const, Vec3<T2>* const);
        
        // Quaternions related functions
        Vec4<float> getNormalized4(Vec4<float>);
        Vec4<float> getConjugate(const Vec4<float>);
        Vec4<float> mulQuat(const Vec4<float> qa, const Vec4<float> qb);
        Vec4<float> getSandwichProduct(const Vec4<float>, const Vec4<float>);
        Vec4<float> getOrientedQuat(const Vec4<float>);
        Vec4<float> getReoriented(const Vec4<float>, const Vec4<float>);
    };
#endif
