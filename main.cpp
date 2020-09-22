/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/math.hpp"
#include "./headers/utils.hpp"

#define SPACE_SIZE 512
#define MAX_RAY_DEPTH 32
#define CURRENT_SPIN 1
static const float COLOR_DEPTH_STEP = (1.0f/(float)MAX_RAY_DEPTH);

static const u16 SPACE_HALF_SIZE = SPACE_SIZE / 2; 
static const u32 SPACE_ATOM_QUANTITY = (u32)pow(SPACE_SIZE, 3);
static const u16 SPACE_SIZE_POWER_VALUE = math::getPower<u16>(SPACE_SIZE);

static u32 space[SPACE_ATOM_QUANTITY] = {0};
static Atom apov[SPACE_ATOM_QUANTITY] = {{0}};

static const Vec3<float> spinAxis[ATOMIC_POV_QUANTITY] = {
//    {1.0f, 0.0f, 0.0f},
//    {-1.0f, 0.0f, 0.0f},
//    {0.0f, 1.0f, 0.0f},
//    {0.0f, -1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
//    {0.0f, 0.0f, -1.0f}
};
    
template <typename T>
bool isContained(Vec4<T>* const coordinates, const T size) {
    if(math::abs(coordinates->x) < size + SPACE_HALF_SIZE &&
        math::abs(coordinates->y) < size + SPACE_HALF_SIZE &&
        math::abs(coordinates->z) < size + SPACE_HALF_SIZE) {
        return true;
    }
    return false;
}

u32 getVoxelOffset(Vec4<i16>* const coordinates) {
    if(isContained<i16>(coordinates, SPACE_SIZE)) {
        return ((coordinates->x + SPACE_HALF_SIZE) |
            (coordinates->y + SPACE_HALF_SIZE) << SPACE_SIZE_POWER_VALUE |
            (coordinates->z + SPACE_HALF_SIZE) << (SPACE_SIZE_POWER_VALUE * 2));
    }
    return u32max;
}

void fillSpace(Voxel* const voxels, const u32 quantity) {
    u32 i = quantity;
    while(i--) {
        Voxel* const voxel = &voxels[i];
        const u32 offset = getVoxelOffset(&voxel->coordinates);
        if(offset != u32max) {
            space[offset] = voxel->color;
        }
    }
}

Quantum getQuantum(const u32 color, const u16 depth) {
    const float darkness = 1.0f - depth * COLOR_DEPTH_STEP;    
    return {
        depth, (
        (u32)(((color & 0xFF000000) >> 24) * darkness) |
        (u32)(((color & 0x00FF0000) >> 16) * darkness) |
        (u32)(((color & 0x0000FF00) >> 8) * darkness) |
        (u32)(color & 0x000000FF))
    };
}

void genApovSpace() {
    u8 i = 0;
    const u8 spin = CURRENT_SPIN;
    while(i--) { // Todo: view
        u16 depth = 0;
        while(depth < MAX_RAY_DEPTH) {
            const Vec3<float> ray = math::mulVector(spinAxis[spin], depth);
            Vec4<i16> ray4 = {
                (i16)ray.x, (i16)ray.y, (i16)ray.z, 0
            };
            const u32 offset = getVoxelOffset(&ray4);
            if(offset != u32max) {
                if(space[offset] != 0) {
                    apov[offset].quanta[spin] = getQuantum(space[offset], depth);
                }
            }
            depth++;
        }
    }
}

int main(int argc, char** argv) {
    u32 size = 0;
    Voxel* voxels = utils::getBinaryContent<Voxel>("voxels.bin", &size);
    
    if(voxels == NULL) {
        printf("File content is null");
    }
    
    fillSpace(voxels, size);
    // Test
    Vec4<i16> coordinates = {-15,-48,-33, 0};
    const u32 color = space[getVoxelOffset(&coordinates)];
    printf("Color: 0x%08x\n", color);
    delete [] voxels;
    
    //genApovSpace();
    return 0;
}
