/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/math.hpp"
#include "./headers/utils.hpp"

#define SPACE_SIZE 512
#define MAX_RAY_DEPTH 64
#define CURRENT_SPIN 0
static const float COLOR_DEPTH_STEP = (1.0f/(float)MAX_RAY_DEPTH);

static const u16 SPACE_HALF_SIZE = SPACE_SIZE / 2; 
static const u32 SPACE_2D_SIZE = (u32)pow(SPACE_SIZE, 2);
static const u32 SPACE_ATOM_QUANTITY = (u32)pow(SPACE_SIZE, 3);
static const u16 SPACE_SIZE_POWER_VALUE = math::getPower<u16>(SPACE_SIZE);
static const u16 SPACE_SIZE_POWER_VALUE_X2 = SPACE_SIZE_POWER_VALUE * 2;
static const u32 SPACE_SIZE_DIGITS_X = SPACE_SIZE - 1;
static const u32 SPACE_SIZE_DIGITS_Y = SPACE_SIZE_DIGITS_X << SPACE_SIZE_POWER_VALUE;
static const u32 SPACE_SIZE_DIGITS_Z = SPACE_SIZE_DIGITS_X << SPACE_SIZE_POWER_VALUE_X2;

static u32 space[SPACE_ATOM_QUANTITY] = {0};
//static Atom apov[SPACE_ATOM_QUANTITY] = {{0}};

static const Vec3<float> spinAxis[ATOMIC_POV_QUANTITY] = {
    {0.0f, 0.0f, 1.0f},
    {-1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, -1.0f},
    {1.0f, 0.0f, 0.0f}    
//    {0.0f, 1.0f, 0.0f},
//    {0.0f, -1.0f, 0.0f},
};
    
template <typename T>
bool isContained(T* const coordinates) {
    if(math::abs(coordinates->x) < SPACE_HALF_SIZE &&
        math::abs(coordinates->y) < SPACE_HALF_SIZE &&
        math::abs(coordinates->z) < SPACE_HALF_SIZE) {
        return true;
    }
    return false;
}

//
template <typename T>
u32 getOffset(T* const coordinates) {
    if(isContained(coordinates)) {
        return (((i16)coordinates->x + SPACE_HALF_SIZE) |
            (((i16)coordinates->y + SPACE_HALF_SIZE) << SPACE_SIZE_POWER_VALUE) |
            (((i16)coordinates->z + SPACE_HALF_SIZE) << SPACE_SIZE_POWER_VALUE_X2));
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
        ((T)(offset & SPACE_SIZE_DIGITS_X)) - SPACE_HALF_SIZE,
        ((T)((offset & SPACE_SIZE_DIGITS_Y) >> SPACE_SIZE_POWER_VALUE)) - SPACE_HALF_SIZE,
        ((T)((offset & SPACE_SIZE_DIGITS_Z) >> SPACE_SIZE_POWER_VALUE_X2)) - SPACE_HALF_SIZE
    };
}

void fillSpace(Voxel* const voxels, const u32 quantity) {
    printf("Starts filling space\n");
    u32 i = quantity;
    while(i--) {
        Voxel* const voxel = &voxels[i];
        const u32 offset = getOffset(&voxel->coordinates);
        if(offset != u32max) {
            space[offset] = voxel->color;
        }
    }
}

u32 getQuantum(const u32 color, const u8 depth) {
    const float darkness = 1.0f - depth * COLOR_DEPTH_STEP;    
    return {
        ((u32)(((color & 0xFF000000) >> 24) * darkness) << 24) |
        ((u32)(((color & 0x00FF0000) >> 16) * darkness) << 16) |
        ((u32)(((color & 0x0000FF00) >> 8) * darkness) << 8) |
        depth
    };
}

void genApovSpace(const char* const filename) {
    FILE* const file = fopen(filename, "wb");
    if(file != NULL) {
        u8 percent = 0;
        u32 i = SPACE_ATOM_QUANTITY;
        //const u8 spin = CURRENT_SPIN;
        const u32 step = SPACE_ATOM_QUANTITY / 100;
        while(--i) {
            if((step > 0) && (i % (step * 5) == 0)) {
                percent+=5;
                printf(">%d ", percent);
            }
            float depth = 0.0f;
            const Vec3<float> coordinates = getCoordinates<float>(i);
            
            u8 spin = 0;
            Atom atom;
            while(spin < ATOMIC_POV_QUANTITY) {
                const Vec3<float> axis = spinAxis[spin];
                while(depth < MAX_RAY_DEPTH) {
                    // Todo: rotation
                    Vec3<float> ray = {
                        coordinates.x + depth * axis.x,
                        coordinates.y + depth * axis.y,
                        coordinates.z + depth * axis.z
                    };
                    //printf("coordinates %f %f %f\n", coordinates.x,coordinates.y,coordinates.z);
                    const u32 offset = getOffset(&ray);
                    if(offset != u32max) {
                        if(space[offset] != 0) {
                            atom.quanta[spin] = getQuantum(space[offset], (u8)depth);
                            //apov[i].quanta[spin] = getQuantum(space[offset], (u8)depth);
                            break;
                        }
                    }
                    depth++;
                }
                spin++;
            }
            fwrite((void*)&atom, sizeof(Atom), 1, file);
        }
        printf("\n");
        fclose(file);
    }
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
    const u32 offset = getOffset(&coordinates);
    if(offset != u32max) {
        const u32 color = space[offset];
        printf("Color: 0x%08x\n", color);
    } else printf("!!!Out of space!!!");
    
    genApovSpace("atoms.bin");
    //utils::genBinaryContent<Atom>("atoms.bin", apov, SPACE_ATOM_QUANTITY);
    printf("Generation done!\n");
    return 0;
}
