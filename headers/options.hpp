/*
 * APoV project
 * By m-c/d in 2020
 */

#ifndef OPTIONS_HPP
#define OPTIONS_HPP
    #include "types.hpp"

    class Options {
        public:
        static u32 MAX_RAY_DEPTH;
        static u32 SPACE_SIZE;
        static u32 ATOMIC_POV_COUNT;
        static u32 RAY_STEP;
        static bool FREE_CAM;
        //
        static void process(int argc, char **argv);
    };
#endif

