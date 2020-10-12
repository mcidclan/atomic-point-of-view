/*
 * APoV project
 * By m-c/d in 2020
 */

#include "./headers/math.hpp"
#include "./headers/utils.hpp"

static float COLOR_DEPTH_STEP;
static float PROJECTION_FACTOR;
static u8 SPACE_Y_BIT_OFFSET;
static u8 SPACE_Z_BIT_OFFSET;
static u16 SPACE_WIDTH;
static u16 SPACE_HEIGHT;
static u16 SPACE_DEPTH;
static u16 SPACE_HALF_WIDTH;
static u16 SPACE_HALF_HEIGHT;
static u16 SPACE_HALF_DEPTH;
static u32 SPACE_SIZE_DIGITS_X;
static u32 SPACE_SIZE_DIGITS_Y;
static u32 SPACE_SIZE_DIGITS_Z;
static u32 SPACE_ATOM_QUANTITY;
static u32 FRAME_SIZE;
static u32 ATOMIC_POV_COUNT;
static u32* space;
static Vec4<float>* spinAxis;

void init() {
    PROJECTION_FACTOR = 1.0f / Options::MAX_PROJECTION_DEPTH;
    COLOR_DEPTH_STEP = (1.0f/(float)Options::MAX_RAY_DEPTH);

    SPACE_WIDTH = Options::SPACE_BLOCK_SIZE * Options::WIDTH_BLOCK_COUNT;
    SPACE_HEIGHT = Options::SPACE_BLOCK_SIZE;
    SPACE_DEPTH = Options::SPACE_BLOCK_SIZE * Options::DEPTH_BLOCK_COUNT;
    
    SPACE_HALF_WIDTH = SPACE_WIDTH / 2;
    SPACE_HALF_HEIGHT = SPACE_HEIGHT / 2;
    SPACE_HALF_DEPTH = SPACE_DEPTH / 2;
    
    SPACE_Y_BIT_OFFSET = math::getPower<u16>(SPACE_WIDTH);
    SPACE_Z_BIT_OFFSET = SPACE_Y_BIT_OFFSET + math::getPower<u16>(SPACE_HEIGHT);

    SPACE_SIZE_DIGITS_X = SPACE_WIDTH - 1;
    SPACE_SIZE_DIGITS_Y = (SPACE_HEIGHT - 1) << SPACE_Y_BIT_OFFSET;
    SPACE_SIZE_DIGITS_Z = (SPACE_DEPTH - 1) << SPACE_Z_BIT_OFFSET;

    FRAME_SIZE = SPACE_WIDTH * SPACE_HEIGHT;
    SPACE_ATOM_QUANTITY = SPACE_WIDTH * SPACE_HEIGHT * SPACE_DEPTH;
    
    printf("Atom quantity: %u\n", SPACE_ATOM_QUANTITY);
    
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
    if(math::abs(coordinates->x) < SPACE_HALF_WIDTH &&
        math::abs(coordinates->y) < SPACE_HALF_HEIGHT &&
        math::abs(coordinates->z) < SPACE_HALF_DEPTH) {
        return true;
    }
    return false;
}

template <typename T>
u32 getOffset(T* const coordinates) {
    if(isContained(coordinates)) {
        return (((i16)coordinates->x + SPACE_HALF_WIDTH) |
            (((i16)coordinates->y + SPACE_HALF_HEIGHT) << SPACE_Y_BIT_OFFSET) |
            (((i16)coordinates->z + SPACE_HALF_DEPTH) << SPACE_Z_BIT_OFFSET));
    }
    return u32max;
}

template<typename T>
Vec4<T> getCoordinates(const u32 offset) {
    return {
        ((T)(offset & SPACE_SIZE_DIGITS_X)) - SPACE_HALF_WIDTH,
        ((T)((offset & SPACE_SIZE_DIGITS_Y) >> SPACE_Y_BIT_OFFSET)) - SPACE_HALF_HEIGHT,
        ((T)((offset & SPACE_SIZE_DIGITS_Z) >> SPACE_Z_BIT_OFFSET)) - SPACE_HALF_DEPTH,
        0
    };
}

void fillSpace(Voxel* const voxels, const u32 quantity) {
    printf("Starts filling space...\n");
    u32 i = quantity;
    while(i--) {
        Voxel* const voxel = &voxels[i];
        const u32 offset = getOffset(&voxel->coordinates);
        if(offset != u32max) {
            space[offset] = voxel->color;
        }
    }
    printf("Space filled.\n");
}

u32 getQuantum(const u32 color, const float depth) {
    const float darkness = 1.0f - (COLOR_DEPTH_STEP * depth);
    const u8 r = darkness * ((color >> 24) & 0x000000FF);
    const u8 g = darkness * ((color >> 16) & 0x000000FF);
    const u8 b = darkness * ((color >> 8) & 0x000000FF);
    const u8 d = depth > 255.0f ? 255 : (u8)depth;
    return (d << 24) | (b << 16) | (g << 8) | r;
}

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
    u32 x = SPACE_WIDTH - (s + 1);
    while(--x > s) {
        u32 y = SPACE_HEIGHT - (s + 1);
        while(--y > s) {
            const u32 i = x | y << SPACE_Y_BIT_OFFSET;
            if(mask == NULL || mask[i]) {
                const u32 o = src[i];
                const u32 a = m ? src[(x + (s + r)) | y << SPACE_Y_BIT_OFFSET] & 0x00FFFFFF : 0;
                const u32 b = m ? src[(x - (s + r)) | y << SPACE_Y_BIT_OFFSET] & 0x00FFFFFF : 0;
                const u32 c = m ? src[x | (y + (s + r)) << SPACE_Y_BIT_OFFSET] & 0x00FFFFFF : 0;
                const u32 d = m ? src[x | (y - (s + r)) << SPACE_Y_BIT_OFFSET] & 0x00FFFFFF : 0;
                
                const u32 e = src[(x + s) | (y + s) << SPACE_Y_BIT_OFFSET] & 0x00FFFFFF;
                const u32 f = src[(x - s) | (y + s) << SPACE_Y_BIT_OFFSET] & 0x00FFFFFF;
                const u32 g = src[(x + s) | (y - s) << SPACE_Y_BIT_OFFSET] & 0x00FFFFFF;
                const u32 h = src[(x - s) | (y - s) << SPACE_Y_BIT_OFFSET] & 0x00FFFFFF;
                
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
    u32* _frame = new u32[FRAME_SIZE];
    u32* __frame = new u32[FRAME_SIZE];
    memset(_frame, 0, FRAME_SIZE * sizeof(u32));
    memset(__frame, 0, FRAME_SIZE * sizeof(u32));

    u32 x = SPACE_WIDTH - 1;
    while(--x) {
        u32 y = SPACE_HEIGHT - 1;
        while(--y) {
            const u32 i = x | y << SPACE_Y_BIT_OFFSET;
            const u32 o = frame[i];
            
            const u32 n[8] = {
                (x + 1) | y << SPACE_Y_BIT_OFFSET,
                (x - 1) | y << SPACE_Y_BIT_OFFSET,
                x | (y + 1) << SPACE_Y_BIT_OFFSET,
                x | (y - 1) << SPACE_Y_BIT_OFFSET,
                
                (x + 1) | (y + 1) << SPACE_Y_BIT_OFFSET,
                (x - 1) | (y + 1) << SPACE_Y_BIT_OFFSET,
                (x + 1) | (y - 1) << SPACE_Y_BIT_OFFSET,
                (x - 1) | (y - 1) << SPACE_Y_BIT_OFFSET
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
    
    memcpy(__frame, frame, FRAME_SIZE * sizeof(u32));
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

void genApovSpace() {
    printf("Starts generating APoV region...\n");
    FILE* file = NULL;
    FILE* clutfile = NULL;
    u8* indexes = NULL;
    
    u32* const frame = new u32[FRAME_SIZE];
    const float MIN_SCALE_STEP = 1.0f / Options::SPACE_BLOCK_SIZE; //

    if(Options::USE_CLUT) {
        clutfile = fopen("clut-indexes.bin", "wb"); //
        indexes = new u8[FRAME_SIZE];
    } else {
        file = fopen("atoms.bin", "wb");
    }
    
    if(file != NULL) {
        u16 spin = 0;
        const float step = 100.0f / SPACE_ATOM_QUANTITY ;
        while(spin < ATOMIC_POV_COUNT) {
            u32 i = 0;
            u8 percent = 0;
            while(i < SPACE_ATOM_QUANTITY) {
                if((u8)(i * step) > percent) {
                    percent++;
                    printf("\r%u%%", percent);
                }

                const Vec4<float> axis = spinAxis[spin];
                const Vec4<float> normalized = math::getReoriented({0.0f, 0.0f, 1.0f}, axis);
                Vec4<float> coordinates = getCoordinates<float>(i);
                
                if(Options::CAM_LOCK) {
                    coordinates = math::getReoriented(coordinates, axis);
                }
                if(Options::CAM_DISTANCE) {
                    coordinates.z -= Options::CAM_DISTANCE;
                }
                
                u32 quanta = 0;
                float depth = 0.0f;
                while(depth < Options::MAX_RAY_DEPTH) {
                    u8 r = 0; //Todo
                    static const int wraper[7] = {0, 1, -1, 2, -2, 3, -3};
                    do {
                        Vec4<float> c = coordinates;
                        if(Options::MAX_PROJECTION_DEPTH != 0.0f) {
                            const float scale = 1.0f + (depth * PROJECTION_FACTOR);
                            c.x *= scale + wraper[r] * MIN_SCALE_STEP;
                            c.y *= scale + wraper[r] * MIN_SCALE_STEP;
                            c.z *= scale + wraper[r] * MIN_SCALE_STEP;
                        }
                        
                        Vec3<float> ray = {
                            c.x + depth * normalized.x,
                            c.y + depth * normalized.y,
                            c.z + depth * normalized.z 
                        };
                        
                        const u32 offset = getOffset(&ray);
                        
                        if(offset != u32max) {
                            if(space[offset] != 0) {
                                quanta = getQuantum(space[offset], depth);
                                goto write_quanta;
                            }
                        }
                    } while(r++ < Options::PROJECTION_GAPS_REDUCER);
                    depth++;
                }
                write_quanta:
                const u32 fid = i % FRAME_SIZE;
                frame[fid] = quanta;
                if(Options::USE_CLUT) {
                    indexes[fid] = updateClut(quanta);
                }
                if(fid == FRAME_SIZE - 1) {
                    applyFilters(frame);
                    if(Options::USE_CLUT) {
                        fwrite(indexes, sizeof(u8), FRAME_SIZE, clutfile);
                    } else {
                        fwrite(frame, sizeof(u32), FRAME_SIZE, file);
                    }
                }
                
                i++;
                if(Options::RAY_STEP > 1) {
                    if(!(i % FRAME_SIZE)) {
                        i += (FRAME_SIZE * (Options::RAY_STEP - 1));
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
    printf("Generation done!\n");
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
    
    genApovSpace();
    rename("atoms.bin", "atoms-done.bin");
    clean();
    return 0;
}
