/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/math.hpp"
#include "./headers/utils.hpp"

static float COLOR_DEPTH_STEP;
static float PROJECTION_FACTOR;

static u16 SPACE_HALF_SIZE; 
static u32 SPACE_2D_SIZE;
static u32 SPACE_ATOM_QUANTITY;
static u16 SPACE_SIZE_POWER_VALUE;
static u16 SPACE_SIZE_POWER_VALUE_X2;
static u32 SPACE_SIZE_DIGITS_X;
static u32 SPACE_SIZE_DIGITS_Y;
static u32 SPACE_SIZE_DIGITS_Z;
static u32 ATOMIC_POV_COUNT;
static u32* space;
static Vec4<float>* spinAxis;

void init() {
    PROJECTION_FACTOR = 1.0f / Options::MAX_PROJECTION_DEPTH;
    COLOR_DEPTH_STEP = (1.0f/(float)Options::MAX_RAY_DEPTH);
    SPACE_HALF_SIZE = Options::SPACE_SIZE / 2; 
    SPACE_2D_SIZE = (u32)pow(Options::SPACE_SIZE, 2);
    SPACE_ATOM_QUANTITY = (u32)pow(Options::SPACE_SIZE, 3);
    SPACE_SIZE_POWER_VALUE = math::getPower<u16>(Options::SPACE_SIZE);
    SPACE_SIZE_POWER_VALUE_X2 = SPACE_SIZE_POWER_VALUE * 2;
    SPACE_SIZE_DIGITS_X = Options::SPACE_SIZE - 1;
    SPACE_SIZE_DIGITS_Y = SPACE_SIZE_DIGITS_X << SPACE_SIZE_POWER_VALUE;
    SPACE_SIZE_DIGITS_Z = SPACE_SIZE_DIGITS_X << SPACE_SIZE_POWER_VALUE_X2;

    space = new u32[SPACE_ATOM_QUANTITY];
    memset(space, 0, SPACE_ATOM_QUANTITY * sizeof(u32));
    
    ATOMIC_POV_COUNT = Options::CAM_HEMISPHERE ?
    Options::ATOMIC_POV_COUNT / 2 : Options::ATOMIC_POV_COUNT;
    spinAxis = new Vec4<float>[ATOMIC_POV_COUNT];
    
    u16 i = 0;
    u16 id = 0;
    while(i < Options::ATOMIC_POV_COUNT) {
        float spin = (float)(i * (360 / Options::ATOMIC_POV_COUNT));
        if(!Options::CAM_HEMISPHERE || spin <= 90 || spin > 270) {
            spinAxis[id] = {0.0f, 1.0f, 0.0f, _ang(spin)};
            id++;
        }
        i++;
    }
}

void clean() {
    delete [] space;
    delete [] spinAxis;    
}

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

template<typename T>
Vec4<T> getCoordinates(const u32 offset) {
    return {
        ((T)(offset & SPACE_SIZE_DIGITS_X)) - SPACE_HALF_SIZE,
        ((T)((offset & SPACE_SIZE_DIGITS_Y) >> SPACE_SIZE_POWER_VALUE)) - SPACE_HALF_SIZE,
        ((T)((offset & SPACE_SIZE_DIGITS_Z) >> SPACE_SIZE_POWER_VALUE_X2)) - SPACE_HALF_SIZE,
        0
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
        u16 spin = 0;
        const u32 step = SPACE_ATOM_QUANTITY / 100;
        while(spin < ATOMIC_POV_COUNT) {
            u32 i = 0;
            while(i < SPACE_ATOM_QUANTITY) {
                if((i > 0) && (i % step == 0)) {
                    printf("\r%u%%", i / step);
                }
                u32 quanta = 0;
                float depth = 0.0f;
                const Vec4<float> axis = spinAxis[spin];
                const Vec4<float> normalized = math::getReoriented({0.0f, 0.0f, 1.0f}, axis);
                Vec4<float> coordinates = math::getReoriented(getCoordinates<float>(i), axis);
                
                if(!Options::CAM_LOCKED) {
                    //
                    coordinates.z -= Options::MAX_RAY_DEPTH;
                }
                
                while(depth < Options::MAX_RAY_DEPTH) {
                    Vec3<float> ray = {
                        coordinates.x + depth * normalized.x,
                        coordinates.y + depth * normalized.y,
                        coordinates.z + depth * normalized.z
                    };
                    if(Options::MAX_PROJECTION_DEPTH != 0.0f) {
                        const float scale = 1.0f + (depth * PROJECTION_FACTOR);
                        ray.x *= scale;
                        ray.y *= scale;
                        ray.z *= scale;
                    }
                    
                    const u32 offset = getOffset(&ray);
                    if(offset != u32max) {
                        if(space[offset] != 0) {
                            quanta = getQuantum(space[offset], (u8)depth);
                            break;
                        }
                    }
                    depth++;
                }
                fwrite((void*)&quanta, sizeof(u32), 1, file);
                i++;
                
                if(Options::RAY_STEP > 1) {
                    if(!(i % SPACE_2D_SIZE)) {
                        i += (SPACE_2D_SIZE * (Options::RAY_STEP - 1));
                    }
                }
                
            }
            spin++;
            printf("\r100%%");
            printf(" | %d/%d\n", spin, ATOMIC_POV_COUNT);
        }
        printf("\n");
        fclose(file);
    }
}

int main(int argc, char** argv) {
    Options::process(argc, argv);
    init();
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
        //
        
        genApovSpace("atoms.bin");
        printf("Generation done!\n");
    clean();
    return 0;
}
