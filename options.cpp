/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/Options.hpp"

u8 Options::SPACE_BLOCK_COUNT = 1;
u32 Options::MAX_RAY_DEPTH = 64;
u32 Options::SPACE_SIZE = 128;
u32 Options::ATOMIC_POV_COUNT = 360;
u32 Options::RAY_STEP = 1;
u32 Options::PROJECTION_GAPS_REDUCER = 0;
bool Options::CAM_LOCKED = false;
bool Options::ANTI_ALIASING = false;
bool Options::CAM_HEMISPHERE = false;
float Options::MAX_PROJECTION_DEPTH = 0.0f;

void Options::process(int argc, char **argv) {
    int i = 1;
    while(i < argc){    
        const std::string name = argv[i];
        if(name.find("space-size:") == 0) {
           Options::SPACE_SIZE = std::stoi(name.substr(11)); 
        } else if(name.find("atomic-pov-count:") == 0) {
            Options::ATOMIC_POV_COUNT = std::stoi(name.substr(17));
        } else if(name.find("ray-step:") == 0) {
            Options::RAY_STEP = std::stoi(name.substr(9));
        } else if(name.find("max-ray-depth:") == 0) {
            Options::MAX_RAY_DEPTH = std::stoi(name.substr(14));
        } else if(name.find("space-block-count:") == 0) {
            Options::SPACE_BLOCK_COUNT = std::stoi(name.substr(18));
        } else if(name.find("projection-depth:") == 0) {
            Options::MAX_PROJECTION_DEPTH = (float)std::stoi(name.substr(17));
        } else if(name.find("projection-gaps-reducer:") == 0) {
            Options::PROJECTION_GAPS_REDUCER = std::stoi(name.substr(24));
            if(Options::PROJECTION_GAPS_REDUCER > 3) {
                Options::PROJECTION_GAPS_REDUCER = 3;
            }
        } else if(name.find("cam-locked") == 0) {
            Options::CAM_LOCKED = true;
        } else if(name.find("cam-hemisphere") == 0) {
            Options::CAM_HEMISPHERE = true;
        } else if(name.find("anti-aliasing") == 0) {
            Options::ANTI_ALIASING = true;
        }
        i++;
    }
    
    if(Options::PROJECTION_GAPS_REDUCER != 0) {
        Options::PROJECTION_GAPS_REDUCER = 1 + Options::PROJECTION_GAPS_REDUCER * 2;
    }
}
