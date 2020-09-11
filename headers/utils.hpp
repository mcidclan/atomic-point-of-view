#ifndef APOV_UTILS_HPP
#define APOV_UTILS_HPP

    #include "./types.hpp"

    #define ENABLE_LOGS true
    #define log ENABLE_LOGS && printf

    namespace utils {
        template <typename T> T* getBinaryContent(const char* const, u32*);
    }

#endif
