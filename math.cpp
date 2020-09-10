#include "headers/math.hpp"

namespace math {
    template<typename T>
    T abs(const T v) {        
        return v < 0 ? -v : v;
    }
    
    template u16 abs<u16>(const u16);


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
    
    template u16 getPower<u16>(u16);
};
