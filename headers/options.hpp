/*
 * APoV project: Generator
 * By m-c/d in 2020
 */

#ifndef OPTIONS_HPP
#define OPTIONS_HPP
    #include "math.hpp"
    #include <cmath>
    
    #define HEADER_LENGTH 20
    
    class Options {
        public:
        static std::string GENERATOR_TYPE;
        static u8 BLUR_MODE;
        static u8 CLUT_COMPRESSION_MODE;
        static u8 ANTI_ALIASING_THRESHOLD;
        static u8 TRACE_EGDES_THRESHOLD;
        static u8 WIDTH_BLOCK_COUNT;
        static u8 DEPTH_BLOCK_COUNT;
        static u8 TRANSPARENCY_DEPTH;
        static u16 SPACE_BLOCK_SIZE;
        static u16 COLOR_MAP_SIZE;
        static float MAX_RAY_DEPTH;
        static float MAX_BLEND_DEPTH;
        static u32 HORIZONTAL_POV_COUNT;
        static u32 VERTICAL_POV_COUNT;
        static u32 RAY_STEP;
        static u32 PROJECTION_GAPS_REDUCER;
        static int CAM_LOCK_AT;
        static int CAM_DISTANCE;
        static bool RENDER_BY_SPACE_BLOCK;
        static bool BLUR_SMOOTH_MATRIX;
        static bool EXPORT_CLUT;
        static bool COMPRESS_CLUT;
        static bool CAM_LOCK_AHEAD;
        static bool ENABLE_ANTI_ALIASING;
        static bool ENABLE_TRACE_EDGES;
        static bool CAM_HEMISPHERE;
        static bool EXPORT_HEADER;
        static bool ENABLE_BLENDING;
        static bool EXPORT_ONE_BIT_COLOR_MAPPING;
        static bool EXPORT_SINGLE_FILE;
        static bool ENABLE_DEEP_TRANSPARENCY;
        static float MAX_PROJECTION_DEPTH;
        static float CLUT_COMPRESSION_FACTOR;
        static float SCALE;
        //
        static void process(int argc, char **argv);
    };
#endif

