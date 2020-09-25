/*
 * APoV project
 * By m-c/d in 2020
 */

#ifndef OPTIONS_HPP
#define OPTIONS_HPP
    #include "types.hpp"

    class Options {
        public:
        static u32 SPACE_SIZE;
        static u32 ATOMIC_POV_COUNT;
        //
        static void process(int argc, char **argv);
    };
#endif

