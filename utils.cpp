#include "./headers/utils.hpp"

namespace utils {
    template <typename T>
    T* getBinaryContent(const char* const filename, u32* size) {
        FILE* const file = fopen(filename, "rb");
        if(file != NULL) {
            fseek(file, 0, SEEK_END);
            *size = ftell(file) / sizeof(T);
            log("%s contents %u elements with a size of %u.\n",
            filename, *size, sizeof(T));
            if(*size > 0) {
                T* const content = new T[*size];
                rewind(file);
                fread((void*)content, sizeof(T), *size, file);
                return content;
            }
            fclose(file);
        }
        return NULL;
    }
    
    template Voxel* getBinaryContent(const char* const, u32*);
    
    template <typename T>
    void genBinaryContent(const char* const filename, T* const data, const u32 count) {
        FILE* const file = fopen(filename, "wb");
        if(file != NULL) {
            fwrite((void*)data, sizeof(T), count, file);
            fclose(file);
        }
    }    
}