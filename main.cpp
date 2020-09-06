/*
 * APoV project
 * By m-c/d in 2020
 */
 
#include <stdio.h>

#define u8 unsigned char
#define u16 short unsigned int
#define u32 unsigned int
#define u64 long unsigned int

#define ENABLE_LOGS true
#define log ENABLE_LOGS && printf
#define SPACE_ATOM_QUANTTY 0x8000000

static u32 space[SPACE_ATOM_QUANTTY] = {0};

template <typename T>
T* getBinaryContent(const char* const filename) {
    FILE* const file = fopen(filename, "rb");
    if(file != NULL) {
        fseek(file, 0, SEEK_END);
        const u64 size = ftell(file) / sizeof(T);
        log("%s contents %lu elements.\n", filename, size);
        if(size > 0) {
            T* const content = new T[size];
            rewind(file);
            fread((void*)content, sizeof(T), size, file);
            return content;
        }
    }
    return NULL;
}

int main(int argc, char** argv) {
    /*u32 i = 0;
    while(i < SPACE_ATOM_QUANTTY) {
        i++;
    }*/
    u32* content = getBinaryContent<u32>("voxels.bin");
    if(content != NULL) {
        printf("size: %u\n", content[0]);
    } else {
        printf("content is null");
    }
    return 0;
}
