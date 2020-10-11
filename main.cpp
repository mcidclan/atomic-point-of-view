/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/math.hpp"
#include "./headers/utils.hpp"

static float COLOR_DEPTH_STEP;
static float PROJECTION_FACTOR;

static u16 SPACE_HALF_SIZE; 
static u16 SPACE_Z_HALF_SIZE; 
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
    SPACE_Z_HALF_SIZE = (SPACE_HALF_SIZE * Options::SPACE_BLOCK_COUNT);
    SPACE_2D_SIZE = (u32)pow(Options::SPACE_SIZE, 2);
    SPACE_ATOM_QUANTITY = (u32)pow(Options::SPACE_SIZE, 3) * Options::SPACE_BLOCK_COUNT; //
    SPACE_SIZE_POWER_VALUE = math::getPower<u16>(Options::SPACE_SIZE);
    SPACE_SIZE_POWER_VALUE_X2 = SPACE_SIZE_POWER_VALUE * 2;
    SPACE_SIZE_DIGITS_X = Options::SPACE_SIZE - 1;
    SPACE_SIZE_DIGITS_Y = SPACE_SIZE_DIGITS_X << SPACE_SIZE_POWER_VALUE;
    SPACE_SIZE_DIGITS_Z = (((u32)pow(Options::SPACE_SIZE, Options::SPACE_BLOCK_COUNT)) - 1) << SPACE_SIZE_POWER_VALUE_X2;

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
        math::abs(coordinates->z) < SPACE_Z_HALF_SIZE) {
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
            (((i16)coordinates->z + SPACE_Z_HALF_SIZE) << SPACE_SIZE_POWER_VALUE_X2));
    }
    return u32max;
}

template<typename T>
Vec4<T> getCoordinates(const u32 offset) {
    return {
        ((T)(offset & SPACE_SIZE_DIGITS_X)) - SPACE_HALF_SIZE,
        ((T)((offset & SPACE_SIZE_DIGITS_Y) >> SPACE_SIZE_POWER_VALUE)) - SPACE_HALF_SIZE,
        ((T)((offset & SPACE_SIZE_DIGITS_Z) >> SPACE_SIZE_POWER_VALUE_X2)) - SPACE_Z_HALF_SIZE,
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
    const float darkness = 1.0f - (COLOR_DEPTH_STEP * (float)depth);
    const u32 r = darkness * ((color >> 24) & 0x000000FF);
    const u32 g = darkness * ((color >> 16) & 0x000000FF);
    const u32 b = darkness * ((color >> 8) & 0x000000FF);
    return (((u32)depth) << 24) | (b << 16) | (g << 8) | r;
}

/*static u32 divRGB(const u32 value, const float d) {
    const u8 r = (value & 0x00000FF) / d;
    const u8 g = ((value >> 8) & 0x00000FF) / d;
    const u8 b = ((value >> 16) & 0x00000FF) / d;
    return (value & 0xFF000000) | (b << 16) | (g << 8) | r;
}

static u32 aveRGB(const u32 v1, const u32 v2) {
    const u8 r = ((v1 & 0x000000FF) + (v2 & 0x000000FF)) / 2;
    const u8 g = (((v1 >> 8) & 0x000000FF) + ((v2 >> 8) & 0x000000FF)) / 2;
    const u8 b = (((v1 >> 16) & 0x000000FF) + ((v2 >> 16) & 0x000000FF)) / 2;
    return (v1 & 0xFF000000) | (b << 16) | (g << 8) | r;
}*/

static u8 lumRGB(const u32 value) {
    const u8 r = (value & 0x00000FF);
    const u8 g = ((value >> 8) & 0x00000FF);
    const u8 b = ((value >> 16) & 0x00000FF);
    return (r + g + b) / 3;
}

static void applyBlur(u32* const dst, u32* const src, u32* const mask = NULL) {
    const u8 s = 1; //size
    const u8 r = 0; //rounded
    const u8 m = 0; //mode 0, lite, full.
    u32 x = Options::SPACE_SIZE - (s + 1);
    while(--x > s) {
        u32 y = Options::SPACE_SIZE - (s + 1);
        while(--y > s) {
            const u32 i = x | y << SPACE_SIZE_POWER_VALUE;
            if(mask == NULL || mask[i]) {
                const u32 o = src[i];
                const u32 a = m ? src[(x + (s + r)) | y << SPACE_SIZE_POWER_VALUE] & 0x00FFFFFF : 0;
                const u32 b = m ? src[(x - (s + r)) | y << SPACE_SIZE_POWER_VALUE] & 0x00FFFFFF : 0;
                const u32 c = m ? src[x | (y + (s + r)) << SPACE_SIZE_POWER_VALUE] & 0x00FFFFFF : 0;
                const u32 d = m ? src[x | (y - (s + r)) << SPACE_SIZE_POWER_VALUE] & 0x00FFFFFF : 0;
                
                const u32 e = src[(x + s) | (y + s) << SPACE_SIZE_POWER_VALUE] & 0x00FFFFFF;
                const u32 f = src[(x - s) | (y + s) << SPACE_SIZE_POWER_VALUE] & 0x00FFFFFF;
                const u32 g = src[(x + s) | (y - s) << SPACE_SIZE_POWER_VALUE] & 0x00FFFFFF;
                const u32 h = src[(x - s) | (y - s) << SPACE_SIZE_POWER_VALUE] & 0x00FFFFFF;
                
                const u8 R = (
                    (o & 0x000000FF) + (a & 0x000000FF) + (b & 0x000000FF) +
                    (c & 0x000000FF) + (d & 0x000000FF) + (e & 0x000000FF) +
                    (f & 0x000000FF) + (g & 0x000000FF) + (h & 0x000000FF)
                ) / (m ? 9 : 5);
                const u8 G = (
                    ((o >> 8) & 0x000000FF) + ((a >> 8) & 0x000000FF) + ((b >> 8) & 0x000000FF) +
                    ((c >> 8) & 0x000000FF) + ((d >> 8) & 0x000000FF) + ((e >> 8) & 0x000000FF) +
                    ((f >> 8) & 0x000000FF) + ((g >> 8) & 0x000000FF) + ((h >> 8) & 0x000000FF)
                ) / (m ? 9 : 5);
                const u8 B = (
                    ((o >> 16) & 0x000000FF) + ((a >> 16) & 0x000000FF) + ((b >> 16) & 0x000000FF) +
                    ((c >> 16) & 0x000000FF) + ((d >> 16) & 0x000000FF) + ((e >> 16) & 0x000000FF) +
                    ((f >> 16) & 0x000000FF) + ((g >> 16) & 0x000000FF) + ((h >> 16) & 0x000000FF)
                ) / (m ? 9 : 5);
                
                dst[i] = (0xFF000000 & o) | (B << 16) | (G << 8) | R;
            }
        }
    }
}

static void applyAntiAliasing(u32* const frame) {
    const u8 threshold = 16;
    u32* _frame = new u32[SPACE_2D_SIZE];
    u32* __frame = new u32[SPACE_2D_SIZE];
    memset(_frame, 0, SPACE_2D_SIZE * sizeof(u32));
    memset(__frame, 0, SPACE_2D_SIZE * sizeof(u32));

    u32 x = Options::SPACE_SIZE - 1;
    while(--x) {
        u32 y = Options::SPACE_SIZE - 1;
        while(--y) {
            const u32 i = x | y << SPACE_SIZE_POWER_VALUE;
            const u32 o = frame[i];
            
            const u32 n[8] = {
                (x + 1) | y << SPACE_SIZE_POWER_VALUE,
                (x - 1) | y << SPACE_SIZE_POWER_VALUE,
                x | (y + 1) << SPACE_SIZE_POWER_VALUE,
                x | (y - 1) << SPACE_SIZE_POWER_VALUE,
                
                (x + 1) | (y + 1) << SPACE_SIZE_POWER_VALUE,
                (x - 1) | (y + 1) << SPACE_SIZE_POWER_VALUE,
                (x + 1) | (y - 1) << SPACE_SIZE_POWER_VALUE,
                (x - 1) | (y - 1) << SPACE_SIZE_POWER_VALUE
            };
            
            const u8 lumo = lumRGB(o);
            const u8 a = math::abs<i16>(lumo - lumRGB(frame[n[0]])) > threshold;
            const u8 b = math::abs<i16>(lumo - lumRGB(frame[n[1]])) > threshold;
            const u8 c = math::abs<i16>(lumo - lumRGB(frame[n[2]])) > threshold;
            const u8 d = math::abs<i16>(lumo - lumRGB(frame[n[3]])) > threshold;
            
            const u8 e = math::abs<i16>(lumo - lumRGB(frame[n[0]])) > threshold;
            const u8 f = math::abs<i16>(lumo - lumRGB(frame[n[1]])) > threshold;
            const u8 g = math::abs<i16>(lumo - lumRGB(frame[n[2]])) > threshold;
            const u8 h = math::abs<i16>(lumo - lumRGB(frame[n[3]])) > threshold;
            
            if(o && (a || b || c || d || e || f || g || h)) {
                _frame[i] = 0xFFFFFFFF;
                u8 p = 8;
                while(p--) {
                    _frame[n[p]] = 0xFFFFFFFF;
                }
            }
        }
    }
    
    memcpy(__frame, frame, SPACE_2D_SIZE * sizeof(u32));
    applyBlur(frame, __frame, _frame);

    delete [] _frame;
    delete [] __frame;
}

void applyFilters(u32* const frame) {
    if(Options::ANTI_ALIASING) {
        applyAntiAliasing(frame);
    }
}


#define CLUT_G_OFFSET 6
#define CLUT_B_OFFSET 36
#define CLUT_COLOR_UNIT 42
#define CLUT_COLOR_COUNT 216
static u32 clut[CLUT_COLOR_COUNT] = {0};
u8 updateClut(const u32 value) {
    const u8 r = (value & 0x000000FF) / CLUT_COLOR_UNIT;
    const u8 g = ((value >> 8) & 0x000000FF) / CLUT_COLOR_UNIT;
    const u8 b = ((value >> 24) & 0x000000FF) / CLUT_COLOR_UNIT;
    u8 i = r + g * CLUT_G_OFFSET + b * CLUT_B_OFFSET;
    if(!clut[i]) {
        clut[i] = 0xFF000000 | value;
    } else {
        u8 _r = value & 0x000000FF;
        u8 _g = (value >> 8) & 0x000000FF;
        u8 _b = (value >> 24) & 0x000000FF;
        _r = (r + _r) / 2;
        _g = (g + _g) / 2;
        _b = (b + _b) / 2;
        clut[i] = 0xFF000000 | (b << 16) | (g << 8) | r;
    }
    return i;
}

void genApovSpace(const char* const filename) {
    FILE* file = NULL;
    FILE* clutfile = NULL;
    u8* indexes = NULL;
    
    u32* const frame = new u32[SPACE_2D_SIZE];
    const float MIN_SCALE_STEP = 1.0f / Options::SPACE_SIZE;

    if(Options::USE_CLUT) {
        clutfile = fopen("clut-indexes.bin", "wb"); //
        indexes = new u8[SPACE_2D_SIZE];
    } else {
        file = fopen("atoms.bin", "wb");
    }
    
    if(file != NULL) {
        u16 spin = 0;
        const u32 step = SPACE_ATOM_QUANTITY / 100;
        while(spin < ATOMIC_POV_COUNT) {
            u32 i = 0;
            while(i < SPACE_ATOM_QUANTITY) {
                if((i > 0) && (i % step == 0)) {
                    printf("\r%u%%", i / step);
                }

                const Vec4<float> axis = spinAxis[spin];
                const Vec4<float> normalized = math::getReoriented({0.0f, 0.0f, 1.0f}, axis);
                Vec4<float> coordinates = math::getReoriented(getCoordinates<float>(i), axis);
                
                if(Options::CAM_LOCK_AT) {
                    //
                    coordinates.z += Options::CAM_LOCK_AT;
                }
                
                u32 quanta = 0;
                float depth = 0.0f;
                while(depth < Options::MAX_RAY_DEPTH) {
                    u8 r = 0;
                    static const int wraper[7] = {0, 1, -1, 2, -2, 3, -3};
                    do {
                        Vec4<float> c = coordinates;
                        if(Options::MAX_PROJECTION_DEPTH != 0.0f) {
                            const float scale = 1.0f + (depth * PROJECTION_FACTOR);
                            c.x *= scale + wraper[r] * MIN_SCALE_STEP;
                            c.y *= scale + wraper[r] * MIN_SCALE_STEP;
                            c.z *= scale + wraper[r] * MIN_SCALE_STEP;
                        }
                        
                        const Vec3<float> ray = {
                            c.x + depth * normalized.x,
                            c.y + depth * normalized.y,
                            c.z + depth * normalized.z 
                        };
                        
                        const u32 offset = getOffset(&ray);
                        
                        if(offset != u32max) {
                            if(space[offset] != 0) {
                                quanta = getQuantum(space[offset], (u8)depth);
                                goto write_quanta;
                                //break;
                            }
                        }
                    } while(r++ < Options::PROJECTION_GAPS_REDUCER);
                    depth++;
                }
                write_quanta:
                const u32 fid = i % SPACE_2D_SIZE;
                frame[fid] = quanta;
                if(Options::USE_CLUT) {
                    indexes[fid] = updateClut(quanta);
                }
                if(fid == SPACE_2D_SIZE - 1) {
                    applyFilters(frame);
                    if(Options::USE_CLUT) {
                        fwrite(indexes, sizeof(u8), SPACE_2D_SIZE, clutfile);
                    } else {
                        fwrite(frame, sizeof(u32), SPACE_2D_SIZE, file);
                    }
                }
                
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
        if(Options::USE_CLUT) {
            fclose(clutfile);
            clutfile = fopen("clut.bin", "wb");
            fwrite(clut, sizeof(u32), CLUT_COLOR_COUNT, clutfile);            
            fclose(clutfile);
            delete [] indexes;
        }
    }
    delete [] frame;
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
    
    genApovSpace("atoms.bin");
    printf("Generation done!\n");
    
    clean();
    return 0;
}
