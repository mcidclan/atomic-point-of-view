/*
 * APoV project
 * By m-c/d in 2020
 */

#ifndef OPTIONS_HPP
#define OPTIONS_HPP
    #include "types.hpp"

    class Options {
        public:
        static u8 GENERATOR_TYPE;
        static u8 WIDTH_BLOCK_COUNT;
        static u8 DEPTH_BLOCK_COUNT;
        static u16 SPACE_BLOCK_SIZE;
        static u32 CAM_DISTANCE;
        static u32 MAX_RAY_DEPTH;
        static u32 ATOMIC_POV_COUNT;
        static u32 RAY_STEP;
        static u32 PROJECTION_GAPS_REDUCER;
        static bool CAM_LOCK;
        static bool USE_CLUT;
        static bool ANTI_ALIASING;
        static bool CAM_HEMISPHERE;
        static float MAX_PROJECTION_DEPTH;
        //
        static void process(int argc, char **argv);
    };
#endif

