/*
 * APoV project: Generator
 * By m-c/d in 2020
 */

#ifndef OPTIONS_HPP
#define OPTIONS_HPP
    #include "types.hpp"
    #include <cmath>
    
    class Options {
        public:
        static std::string GENERATOR_TYPE;
        static u8 WIDTH_BLOCK_COUNT;
        static u8 DEPTH_BLOCK_COUNT;
        static u16 SPACE_BLOCK_SIZE;
        static u32 MAX_RAY_DEPTH;
        static u32 ATOMIC_POV_COUNT;
        static u32 RAY_STEP;
        static u32 PROJECTION_GAPS_REDUCER;
        static int CAM_LOCK_AT;
        static int CAM_DISTANCE;        
        static bool EXPORT_CLUT;
        static bool COMPRESS_CLUT;
        static bool CAM_LOCK_AHEAD;
        static bool ANTI_ALIASING;
        static bool CAM_HEMISPHERE;
        static float MAX_PROJECTION_DEPTH;
        //
        static void process(int argc, char **argv);
    };
#endif

