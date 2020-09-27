/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/Options.hpp"

u32 Options::MAX_RAY_DEPTH = 64;
u32 Options::SPACE_SIZE = 128;
u32 Options::ATOMIC_POV_COUNT = 360;
u32 Options::RAY_STEP = 1;
bool Options::FREE_CAM = false;
bool Options::CAM_HEMISPHERE = false;

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
        } else if(name.find("free-cam") == 0) {
            Options::FREE_CAM = true;
        } else if(name.find("cam-hemisphere") == 0) {
            Options::CAM_HEMISPHERE = true;
        }
        i++;
    }
}
