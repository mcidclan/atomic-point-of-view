/*
 * APoV project: Generator
 * By m-c/d in 2020
 */

#include "./headers/Options.hpp"

u8 Options::WIDTH_BLOCK_COUNT = 1;
u8 Options::DEPTH_BLOCK_COUNT = 1;
u16 Options::SPACE_BLOCK_SIZE = 128;
u32 Options::MAX_RAY_DEPTH = 64;
u32 Options::ATOMIC_POV_COUNT = 360;
u32 Options::RAY_STEP = 1;
u32 Options::PROJECTION_GAPS_REDUCER = 0;
int Options::CAM_LOCK_AT = 0;
int Options::CAM_DISTANCE = 0;
bool Options::EXPORT_CLUT = false;
bool Options::COMPRESS_CLUT = false;
bool Options::ANTI_ALIASING = false;
bool Options::CAM_HEMISPHERE = false;
bool Options::CAM_LOCK_AHEAD = false;
float Options::MAX_PROJECTION_DEPTH = 0.0f;
std::string Options::GENERATOR_TYPE = "path";

void Options::process(int argc, char **argv) {
    int i = 1;
    while(i < argc){
        const std::string name = argv[i];
        if(name.find("space-block-size:") == 0) {
           Options::SPACE_BLOCK_SIZE = std::stoi(name.substr(17)); 
        } else if(name.find("atomic-pov-count:") == 0) {
            Options::ATOMIC_POV_COUNT = std::stoi(name.substr(17));
        } else if(name.find("ray-step:") == 0) {
            Options::RAY_STEP = std::stoi(name.substr(9));
        } else if(name.find("max-ray-depth:") == 0) {
            Options::MAX_RAY_DEPTH = std::stoi(name.substr(14));
        } else if(name.find("width-block-count:") == 0) {
            Options::WIDTH_BLOCK_COUNT = std::stoi(name.substr(18));
        } else if(name.find("depth-block-count:") == 0) {
            Options::DEPTH_BLOCK_COUNT = std::stoi(name.substr(18));
        } else if(name.find("projection-depth:") == 0) {
            Options::MAX_PROJECTION_DEPTH = (float)std::stoi(name.substr(17));
        } else if(name.find("projection-gaps-reducer:") == 0) {
            Options::PROJECTION_GAPS_REDUCER = std::stoi(name.substr(24));
            if(Options::PROJECTION_GAPS_REDUCER > 3) {
                Options::PROJECTION_GAPS_REDUCER = 3;
            }
        } else if(name.find("cam-distance:") == 0) {
            Options::CAM_DISTANCE = std::stoi(name.substr(13));
        } else if(name.find("cam-lock-at:") == 0) {
            Options::CAM_LOCK_AT = std::stoi(name.substr(12));
        } else if(name.find("compress-clut") == 0) {
            Options::COMPRESS_CLUT = true;
        } else if(name.find("cam-hemisphere") == 0) {
            Options::CAM_HEMISPHERE = true;
        } else if(name.find("anti-aliasing") == 0) {
            Options::ANTI_ALIASING = true;
        } else if(name.find("cam-lock-ahead") == 0) {
            Options::CAM_LOCK_AHEAD = true;
        } else if(name.find("export-clut") == 0) {
            Options::EXPORT_CLUT = true;
        }
        i++;
    }
    
    if(Options::MAX_RAY_DEPTH > 256) {
        printf("!!!max-ray-depth greater than 256!!!\n");
    }
    
    if(Options::PROJECTION_GAPS_REDUCER != 0) {
        Options::PROJECTION_GAPS_REDUCER = 1 + Options::PROJECTION_GAPS_REDUCER * 2;
    }
    
    if(Options::CAM_LOCK_AHEAD) {
        const float d = Options::SPACE_BLOCK_SIZE * Options::DEPTH_BLOCK_COUNT;
        Options::CAM_DISTANCE = (int)sqrt(d*d + d*d);
        Options::CAM_LOCK_AT = Options::CAM_DISTANCE;
    }
    
    printf("Options set.\n");
}
