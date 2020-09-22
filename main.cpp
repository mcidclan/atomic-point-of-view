/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/math.hpp"
#include "./headers/utils.hpp"

#define SPACE_SIZE 512
#define MAX_RAY_DEPTH 32
#define CURRENT_SPIN 0
static const float COLOR_DEPTH_STEP = (1.0f/(float)MAX_RAY_DEPTH);

static const u16 SPACE_HALF_SIZE = SPACE_SIZE / 2; 
static const u32 SPACE_2D_SIZE = (u32)pow(SPACE_SIZE, 2);
static const u32 SPACE_ATOM_QUANTITY = (u32)pow(SPACE_SIZE, 3);
static const u16 SPACE_SIZE_POWER_VALUE = math::getPower<u16>(SPACE_SIZE);
static const u16 SPACE_SIZE_POWER_VALUE_X2 = SPACE_SIZE_POWER_VALUE * 2;
static const u16 SPACE_SIZE_DIGITS = SPACE_SIZE - 1;

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
    
template <typename T1, typename T2>
bool isContained(T1* const coordinates, const T2 size) {
    if(math::abs(coordinates->x) < size + SPACE_HALF_SIZE &&
        math::abs(coordinates->y) < size + SPACE_HALF_SIZE &&
        math::abs(coordinates->z) < size + SPACE_HALF_SIZE) {
        return true;
    }
    return false;
}

//
template <typename T>
u32 getOffset(T* const coordinates) {
    if(isContained(coordinates, SPACE_SIZE)) {
        return ((coordinates->x + SPACE_HALF_SIZE) |
            (coordinates->y + SPACE_HALF_SIZE) << SPACE_SIZE_POWER_VALUE |
            (coordinates->z + SPACE_HALF_SIZE) << SPACE_SIZE_POWER_VALUE_X2);
    }
    return u32max;
}

// slow
/*template<typename T>
Vec3<T> getCoordinates(const u32 offset) {
    const T x = ((T)(offset % SPACE_SIZE)) - SPACE_HALF_SIZE;
    const T y = ((T)((offset / SPACE_SIZE) % SPACE_SIZE)) - SPACE_HALF_SIZE;
    const T z = ((T)(offset / SPACE_2D_SIZE)) - SPACE_HALF_SIZE;
    return {x, y, z};
}*/

template<typename T>
Vec3<T> getCoordinates(const u32 offset) {
    return {
        ((T)(offset & SPACE_SIZE_DIGITS)) - SPACE_HALF_SIZE,
        ((T)(offset & (SPACE_SIZE_DIGITS << SPACE_SIZE_POWER_VALUE))) - SPACE_HALF_SIZE,
        ((T)(offset & (SPACE_SIZE_DIGITS << SPACE_SIZE_POWER_VALUE_X2))) - SPACE_HALF_SIZE
    };
}

void fillSpace(Voxel* const voxels, const u32 quantity) {
    u32 i = quantity;
    while(i--) {
        Voxel* const voxel = &voxels[i];
        const u32 offset = getOffset(&voxel->coordinates);
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
    u32 i = 0;
    u8 percent = 0;
    const u8 spin = CURRENT_SPIN;
    const u32 step = SPACE_ATOM_QUANTITY / 100;
    while(i < SPACE_ATOM_QUANTITY) {
        if((step > 0) && (i % (step * 10) == 0)) {
            printf(">%d ", percent);
            percent+=10;
        }
        u16 depth = 0;
        Vec3<float> axis = spinAxis[spin];
        Vec3<float> coordinates = getCoordinates<float>(i);
        while(depth < MAX_RAY_DEPTH) {
            // Todo: rotation
            Vec3<i16> ray = math::mulVector<float, i16>(&axis, (float)depth);
            math::addVector(&ray, &coordinates);
            const u32 offset = getOffset(&ray);
            if(offset != u32max) {
                if(space[offset] != 0) {
                    apov[i].quanta[spin] = getQuantum(space[offset], depth);
                }
            }
            depth++;
        }
        i++;
    }
    printf("\n");
}

int main(int argc, char** argv) {
    u32 size = 0;
    Voxel* voxels = utils::getBinaryContent<Voxel>("voxels.bin", &size);
    
    if(voxels == NULL) {
        printf("File content is null");
    }
    
    fillSpace(voxels, size);
    delete [] voxels;
    
    // Test
    Vec4<i16> coordinates = {-15,-48,-33, 0};
    const u32 color = space[getOffset(&coordinates)];
    printf("Color: 0x%08x\n", color);
    
    genApovSpace();
    printf("Generation done!\n");
    return 0;
}
