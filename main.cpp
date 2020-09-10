/*
 * APoV project
 * By m-c/d in 2020
 */
 
#include <stdio.h>
#include "./headers/math.hpp"

#define ENABLE_LOGS true
#define log ENABLE_LOGS && printf

#define SPACE_SIZE 512
#define SPACE_ATOM_QUANTTY 0x8000000 // 512x512x512

//static u32 space[SPACE_ATOM_QUANTTY] = {0};

template <typename T>
T* getBinaryContent(const char* const filename) {
    FILE* const file = fopen(filename, "rb");
    if(file != NULL) {
        fseek(file, 0, SEEK_END);
        const u32 size = ftell(file) / sizeof(T);
        log("%s contents %u elements.\n", filename, size);
        if(size > 0) {
            T* const content = new T[size];
            rewind(file);
            fread((void*)content, sizeof(T), size, file);
            return content;
        }
    }
    return NULL;
}

template <typename T>
bool isContained(Vec4<T>* const coordinates, const T size) {
    if(math::abs(coordinates->x) < size &&
        math::abs(coordinates->y) < size &&
        math::abs(coordinates->z) < size) {
        return true;
    }
    return false;
}


void fillSpace(Voxel* const voxels, const u32 quantity) {
    u32 i = quantity;
    while(i--) {
        Voxel* const voxel = &voxels[i];
                
        if(isContained<u16>(&voxel->coordinates, SPACE_SIZE)) {
            
            /*voxel.coordinates.x
            voxel.coordinates.y
            voxel.coordinates.z
            
            space[] = voxel->color;*/         
        }
    }
}

int main(int argc, char** argv) {
    /*u32 i = 0;
    while(i < SPACE_ATOM_QUANTTY) {
        i++;
    }*/
    /*u32* content = getBinaryContent<u32>("voxels.bin");
    if(content != NULL) {
        printf("size: %u\n", content[0]);
    } else {
        printf("content is null");
    }*/
    
    printf("power %u %u\n", i, math::getPower<u16>(0));
    
    return 0;
}
