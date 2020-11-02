/*
 * APoV project: Generator
 * By m-c/d in 2020
 */

#include "./headers/Options.hpp"

u8 Options::BLUR_MODE = 1;
u8 Options::CLUT_COMPRESSION_MODE = 0;
u8 Options::ANTI_ALIASING_THRESHOLD = 24;
u8 Options::WIDTH_BLOCK_COUNT = 1;
u8 Options::DEPTH_BLOCK_COUNT = 1;
u16 Options::SPACE_BLOCK_SIZE = 128;
u32 Options::MAX_RAY_DEPTH = 64;
u32 Options::MAX_BLEND_DEPTH = 64;
u32 Options::HORIZONTAL_POV_COUNT = 360;
u32 Options::VERTICAL_POV_COUNT = 1;
u32 Options::RAY_STEP = 1;
u32 Options::PROJECTION_GAPS_REDUCER = 0;
int Options::CAM_LOCK_AT = 0;
int Options::CAM_DISTANCE = 0;
bool Options::EXPORT_CLUT = false;
bool Options::COMPRESS_CLUT = false;
bool Options::ANTI_ALIASING = false;
bool Options::CAM_HEMISPHERE = false;
bool Options::CAM_LOCK_AHEAD = false;
bool Options::EXPORT_HEADER = false;
bool Options::ENABLE_BLENDING = false;
bool Options::BLUR_SMOOTH_MATRIX = false;
float Options::MAX_PROJECTION_DEPTH = 0.0f;
float Options::CLUT_COMPRESSION_FACTOR = 0.0f;

std::string Options::GENERATOR_TYPE = "path";

void Options::process(int argc, char **argv) {
    int i = 1;
    while(i < argc){
        const std::string name = argv[i];
        if(name.find("space-block-size:") == 0) {
           Options::SPACE_BLOCK_SIZE = std::stoi(name.substr(17)); 
        } else if(name.find("horizontal-pov-count:") == 0) {
            const u16 count = std::stoi(name.substr(21));
            if(count > 0) {
                Options::HORIZONTAL_POV_COUNT = count < 360 ? count : 360;
            }
        } else if(name.find("vertical-pov-count:") == 0) {
            const u16 count = std::stoi(name.substr(19));
            if(count > 0) {
                Options::VERTICAL_POV_COUNT = count < 360 ? count : 360;
            }
        } else if(name.find("ray-step:") == 0) {
            Options::RAY_STEP = std::stoi(name.substr(9));
            if((Options::RAY_STEP != 1) && !math::getPower((u16)Options::RAY_STEP)) {
                printf("!!!ray-step must be a power of 2!!!\n");
                printf("!!!ray-step sets to 1!!!\n");
                Options::RAY_STEP = 1;
            }
            
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
        } else if(name.find("anti-aliasing-threshold:") == 0) {
            Options::ANTI_ALIASING_THRESHOLD = std::stoi(name.substr(24));
        } else if(name.find("clut-compression-factor:") == 0) {
            Options::CLUT_COMPRESSION_FACTOR = std::stof(name.substr(24));
        } else if(name.find("clut-compression-mode:") == 0) {
            if(name.substr(22) == "ycbcr") {
                Options::CLUT_COMPRESSION_MODE = 1;
                if(!Options::CLUT_COMPRESSION_FACTOR) {
                    Options::CLUT_COMPRESSION_FACTOR = 32;
                }
            } else if(name.substr(22) == "rgb") {
                Options::CLUT_COMPRESSION_MODE = 2;
                if(!Options::CLUT_COMPRESSION_FACTOR) {
                    Options::CLUT_COMPRESSION_FACTOR = 9;
                }
            } else if(name.substr(22) == "luminance") {
                Options::CLUT_COMPRESSION_MODE = 3;
                if(!Options::CLUT_COMPRESSION_FACTOR) {
                    Options::CLUT_COMPRESSION_FACTOR = 4;
                }
            } else if(name.substr(22) == "average") {
                Options::CLUT_COMPRESSION_MODE = 0;
            }
        } else if(name.find("blur-mode:") == 0) {
            Options::BLUR_MODE = std::stoi(name.substr(10));
        } else if(name.find("max-blend-depth:") == 0) {
            Options::MAX_BLEND_DEPTH = std::stoi(name.substr(16));
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
        } else if(name.find("export-header") == 0) {
            Options::EXPORT_HEADER = true;
        } else if(name.find("blur-smooth-matrix") == 0) {
            Options::BLUR_SMOOTH_MATRIX = true;
        } else if(name.find("enable-blending") == 0) {
            Options::ENABLE_BLENDING = true;
        }
        i++;
    }
    
    if(Options::MAX_RAY_DEPTH > 255) {
        printf("!!!max-ray-depth greater than 255!!!\n");
        printf("!!!Depth greater than 255 would not be exported!!!\n");
    }
    
    if(Options::ENABLE_BLENDING &&
        (Options::MAX_RAY_DEPTH < Options::MAX_BLEND_DEPTH)) {
        Options::MAX_BLEND_DEPTH = Options::MAX_RAY_DEPTH;
        printf("!!!max-blend-depth sets to max-ray-length!!\n");
    }
    
    if(Options::PROJECTION_GAPS_REDUCER != 0) {
        Options::PROJECTION_GAPS_REDUCER = 1 + Options::PROJECTION_GAPS_REDUCER * 2;
    }
    
    if(Options::CAM_LOCK_AHEAD) {
        const float d = Options::SPACE_BLOCK_SIZE * Options::DEPTH_BLOCK_COUNT;
        Options::CAM_DISTANCE = (int)sqrt(d*d + d*d);
        Options::CAM_LOCK_AT = Options::CAM_DISTANCE;
    }
    
    if(Options::EXPORT_HEADER) {
        if(!Options::EXPORT_CLUT) {
            printf("Export header... ");
            FILE* file = fopen("_atoms.bin", "wb");
            if(file != NULL) {
                u32* const header = new u32[HEADER_LENGTH];
                memset(header, 0, HEADER_LENGTH*sizeof(u32));
                header[0] = Options::SPACE_BLOCK_SIZE;
                header[1] = Options::HORIZONTAL_POV_COUNT;
                header[2] = Options::VERTICAL_POV_COUNT;
                header[3] = Options::RAY_STEP;
                header[4] = Options::WIDTH_BLOCK_COUNT;
                header[5] = Options::DEPTH_BLOCK_COUNT;
                fwrite(header, sizeof(u32), HEADER_LENGTH, file);
                fclose(file);
                delete header;
                printf("Done!\n");
            }
        } else printf("!!!Header export not available width clut!!!\n");
    }
    
    printf("\n");
    printf("cam-hemisphere ...... %s\n", Options::CAM_HEMISPHERE ? "On" : "Off");
    printf("cam-lock-ahead ...... %s\n", Options::CAM_LOCK_AHEAD ? "On" : "Off");
    printf("export-clut ......... %s\n", Options::EXPORT_CLUT ? "On" : "Off");
    printf("compress-clut ....... %s\n", Options::COMPRESS_CLUT ? "On" : "Off");
    printf("export-header ....... %s\n", Options::EXPORT_HEADER? "On" : "Off");
    printf("anti-aliasing ....... %s\n", Options::ANTI_ALIASING ? "On" : "Off");
    printf("blur-smooth-matrix .. %s\n", Options::BLUR_SMOOTH_MATRIX ? "On" : "Off");
    printf("enable-blending ..... %s\n", Options::ENABLE_BLENDING ? "On" : "Off");
    printf("\n");
}
