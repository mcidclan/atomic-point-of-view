/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/math.hpp"
#include "./headers/utils.hpp"

#define SPACE_SIZE 512
static const u16 SPACE_HALF_SIZE = SPACE_SIZE / 2; 
static const u32 SPACE_ATOM_QUANTTY = (u32)pow(SPACE_SIZE, 3);
static const u16 SPACE_SIZE_POWER_VALUE = math::getPower<u16>(SPACE_SIZE);

static u32 space[SPACE_ATOM_QUANTTY] = {0};

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
            const u32 offset = ((voxel->coordinates.x + SPACE_HALF_SIZE) &
            ((voxel->coordinates.y + SPACE_HALF_SIZE) << SPACE_SIZE_POWER_VALUE) &
            ((voxel->coordinates.z + SPACE_HALF_SIZE) << (SPACE_SIZE_POWER_VALUE * 2)));
            space[offset] = voxel->color;      
        }
    }
}

u32 getColorFromSpace(const u16 x, const u16 y, const u16 z) {
    const u32 offset = ((x + SPACE_HALF_SIZE) &
    ((y + SPACE_HALF_SIZE) << SPACE_SIZE_POWER_VALUE) &
    ((z + SPACE_HALF_SIZE) << (SPACE_SIZE_POWER_VALUE * 2)));
    return space[offset];
}

int main(int argc, char** argv) {
    
    u32 size = 0;
    Voxel* voxels = utils::getBinaryContent<Voxel>("voxels.bin", &size);
    
    if(voxels == NULL) {
        printf("File content is null");
    }
    
    fillSpace(voxels, size);
    const u32 color = getColorFromSpace(-15,-48,-33);
    printf("Color a: 0x%08x\n", voxels[0].color);
    printf("Color b: 0x%08x\n", color);
    
    delete [] voxels;
    return 0;
}
