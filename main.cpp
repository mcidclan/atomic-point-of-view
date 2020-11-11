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

static u16 COLOR_MAP_WIDTH;
static u16 COLOR_MAP_HEIGHT;
static u16 COLOR_MAP_WIDTH_SCALE;
static u16 COLOR_MAP_HEIGHT_SCALE;
static u32 COLOR_MAP_SIZE;
//static u32 COLOR_MAP_SIZE_SCALE;

static u32 SPACE_SIZE_DIGITS_X;
static u32 SPACE_SIZE_DIGITS_Y;
static u32 SPACE_SIZE_DIGITS_Z;
static u32 SPACE_ATOM_QUANTITY;
static u32 FRAME_SIZE;
static u32 VERTICAL_POV_COUNT;
static u32 HORIZONTAL_POV_COUNT;

static u32* space;
static Pov* povs;

void init() {
    PROJECTION_FACTOR = 1.0f / Options::MAX_PROJECTION_DEPTH;
    COLOR_DEPTH_STEP = (1.0f / (float)Options::MAX_RAY_DEPTH);

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
    
    if(Options::EXPORT_ONE_BIT_COLOR_MAPPING) {
        COLOR_MAP_WIDTH = Options::COLOR_MAP_SIZE * Options::WIDTH_BLOCK_COUNT;
        COLOR_MAP_HEIGHT = Options::COLOR_MAP_SIZE;
        COLOR_MAP_SIZE = COLOR_MAP_WIDTH * COLOR_MAP_HEIGHT;
        COLOR_MAP_WIDTH_SCALE = SPACE_WIDTH / COLOR_MAP_WIDTH;
        COLOR_MAP_HEIGHT_SCALE = SPACE_HEIGHT / COLOR_MAP_WIDTH;
        //COLOR_MAP_SIZE_SCALE = COLOR_MAP_WIDTH_SCALE * COLOR_MAP_HEIGHT_SCALE;
    }
    
    space = new u32[SPACE_ATOM_QUANTITY];
    memset(space, 0, SPACE_ATOM_QUANTITY * sizeof(u32));
    
    printf("Caching spining point of views... ");
    HORIZONTAL_POV_COUNT = Options::CAM_HEMISPHERE ?
    Options::HORIZONTAL_POV_COUNT / 2 : Options::HORIZONTAL_POV_COUNT;
    VERTICAL_POV_COUNT = Options::CAM_HEMISPHERE ?
    Options::VERTICAL_POV_COUNT / 2 : Options::VERTICAL_POV_COUNT;    
    
    povs = new Pov[HORIZONTAL_POV_COUNT * VERTICAL_POV_COUNT];

    u16 hstep = 0;
    while(hstep < HORIZONTAL_POV_COUNT) {
        u16 vstep = 0;
        float hspin = (float)(hstep * (360 / Options::HORIZONTAL_POV_COUNT));
        if(Options::CAM_HEMISPHERE && hspin >= 90.0f) {
            hspin += 270.0f;
        }
        const Vec4<float> qa = math::getOrientedQuat({0.0f, 1.0f, 0.0f, _ang(hspin)});
        while(vstep < VERTICAL_POV_COUNT) {
            float vspin = (float)(vstep * (360 / Options::VERTICAL_POV_COUNT));
            if(Options::CAM_HEMISPHERE && vspin >= 90.0f) {
                vspin += 270.0f;
            }
                
            const Vec4<float> qb = math::getOrientedQuat({1.0f, 0.0f, 0.0f, _ang(vspin)});
            const Vec4<float> qc = math::mulQuat(qa, qb);
            const Vec4<float> rayStep = math::getNormalized4(
                math::getSandwichProduct({0.0f, 0.0f, 1.0f, 0.0f}, qc));
            povs[vstep + hstep * VERTICAL_POV_COUNT] = {rayStep, qc};
            vstep++;
        }
        hstep++;
    }
    printf("done!\n");
}

void clean() {
    delete [] space;
    delete [] povs;
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
    printf("Filling the space with voxels...\n");
    u32 i = quantity;
    while(i--) {
        Voxel* const voxel = &voxels[i];
        const u32 offset = getOffset(&voxel->coordinates);
        if(offset != u32max) {
            space[offset] = voxel->color;
        }
    }
    printf("  Done!\n\n");
}

float getDarkness(const float depth) {
    return 1.0f - (COLOR_DEPTH_STEP * depth);
}

u32 getQuantum(const u32 color, const float depth) {
    const float darkness = getDarkness(depth);
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
    const u32 s = 1; //size
    const u8 r = Options::BLUR_SMOOTH_MATRIX ? 1 : 0; //rounded
    const u8 m = Options::BLUR_MODE ? 1 : 0;//mode 0, lite, full.
    u32 x = SPACE_WIDTH - (s + r);
    while(--x > (s + r)) {
        u32 y = SPACE_HEIGHT - (s + r);
        while(--y > (s + r)) {
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

static void getEdges(u32* const frame, u32* const _frame,
    const u8 threshold, const u8 size, const bool thin = false) {
    const u8 MAT_UNIT_COUNT = size > 1 ? 16 : 8;
    u32 x = SPACE_WIDTH - size;
    while(--x > size) {
        u32 y = SPACE_HEIGHT - size;
        while(--y > size) {
            const u32 i = x | y << SPACE_Y_BIT_OFFSET;
            const u32 o = frame[i];    

            u32 n[MAT_UNIT_COUNT] = {
                (x + size) | y << SPACE_Y_BIT_OFFSET,
                (x - size) | y << SPACE_Y_BIT_OFFSET,
                x | (y + size) << SPACE_Y_BIT_OFFSET,
                x | (y - size) << SPACE_Y_BIT_OFFSET,
                
                (x + size) | (y + size) << SPACE_Y_BIT_OFFSET,
                (x - size) | (y + size) << SPACE_Y_BIT_OFFSET,
                (x + size) | (y - size) << SPACE_Y_BIT_OFFSET,
                (x - size) | (y - size) << SPACE_Y_BIT_OFFSET
            };
            
            if(MAT_UNIT_COUNT > 8) {
                const u8 _size = size - 1;
                n[8] = (x + _size) | y << SPACE_Y_BIT_OFFSET;
                n[9] = (x - _size) | y << SPACE_Y_BIT_OFFSET;
                n[10] = x | (y + _size) << SPACE_Y_BIT_OFFSET;
                n[11] = x | (y - _size) << SPACE_Y_BIT_OFFSET;
                
                n[12] = (x + _size) | (y + _size) << SPACE_Y_BIT_OFFSET;
                n[13] = (x - _size) | (y + _size) << SPACE_Y_BIT_OFFSET;
                n[14] = (x + _size) | (y - _size) << SPACE_Y_BIT_OFFSET;
                n[15] = (x - _size) | (y - _size) << SPACE_Y_BIT_OFFSET;
            }
            
            const u8 lumo = lumRGB(o);
            const u8 a = math::abs<i16>(lumo - lumRGB(frame[n[0]])) > threshold;
            const u8 b = math::abs<i16>(lumo - lumRGB(frame[n[1]])) > threshold;
            const u8 c = math::abs<i16>(lumo - lumRGB(frame[n[2]])) > threshold;
            const u8 d = math::abs<i16>(lumo - lumRGB(frame[n[3]])) > threshold;
            
            const u8 e = math::abs<i16>(lumo - lumRGB(frame[n[0]])) > threshold;
            const u8 f = math::abs<i16>(lumo - lumRGB(frame[n[1]])) > threshold;
            const u8 g = math::abs<i16>(lumo - lumRGB(frame[n[2]])) > threshold;
            const u8 h = math::abs<i16>(lumo - lumRGB(frame[n[3]])) > threshold;
            
            u8 _a = 0, _b = 0, _c = 0, _d = 0, _e = 0, _f = 0, _g = 0, _h = 0;
            if(MAT_UNIT_COUNT > 8) {
                _a = math::abs<i16>(lumo - lumRGB(frame[n[0]])) > threshold;
                _b = math::abs<i16>(lumo - lumRGB(frame[n[1]])) > threshold;
                _c = math::abs<i16>(lumo - lumRGB(frame[n[2]])) > threshold;
                _d = math::abs<i16>(lumo - lumRGB(frame[n[3]])) > threshold;
                
                _e = math::abs<i16>(lumo - lumRGB(frame[n[0]])) > threshold;
                _f = math::abs<i16>(lumo - lumRGB(frame[n[1]])) > threshold;
                _g = math::abs<i16>(lumo - lumRGB(frame[n[2]])) > threshold;
                _h = math::abs<i16>(lumo - lumRGB(frame[n[3]])) > threshold;
            }
            
            if(o && (a || b || c || d || e || f || g || h ||
                _a || _b || _c || _d || _e || _f || _g || _h)) {
                _frame[i] = 0xFFFFFFFF;
                if(!thin) {
                    u8 p = 8;
                    while(p--) {
                        _frame[n[p]] = 0xFFFFFFFF;
                    }
                }
            }
        }
    }
}

static void applyAntiAliasing(u32* const frame) {    
    u32* _frame = new u32[FRAME_SIZE];
    u32* __frame = new u32[FRAME_SIZE];
    memset(_frame, 0, FRAME_SIZE * sizeof(u32));
    memset(__frame, 0, FRAME_SIZE * sizeof(u32));
    getEdges(frame, _frame, Options::ANTI_ALIASING_THRESHOLD, 1);
    memcpy(__frame, frame, FRAME_SIZE * sizeof(u32));
    applyBlur(frame, __frame, _frame);
    delete [] _frame;
    delete [] __frame;
}

static void removePixels(u32* const frame, u32* const mask) {
    u32 i = 0;
    while(i < FRAME_SIZE) {
        if(mask[i]) {
            frame[i] = 0;
        }
        i++;
    }
}

static void applyVoidToEgdes(u32* const frame) {
    u32* _frame = new u32[FRAME_SIZE];
    memset(_frame, 0, FRAME_SIZE * sizeof(u32));
    getEdges(frame, _frame, Options::TRACE_EGDES_THRESHOLD, 1, true);
    removePixels(frame, _frame);
    delete [] _frame;
}

static void applyFilters(void* const frame) {
    if(Options::EXPORT_ONE_BIT_COLOR_MAPPING) {
        if(Options::ENABLE_TRACE_EDGES) {
            applyVoidToEgdes((u32*)frame);
        }
    } else {
        if(Options::ENABLE_ANTI_ALIASING) {
            applyAntiAliasing((u32*)frame);
        }
    }    
}

static void getOBCM(u32* const frame, u8* const obcm) {
    u32 i = 0;
    while(i < FRAME_SIZE) {
        const u8 slot = i % 8;
        const u32 offset = (i / 8);
        const u32 quanta = frame[i];
        if(slot == 0) {
            if(quanta) {
                obcm[offset] = 0b1;
            } else obcm[offset] = 0b0;
        } else {
            if(quanta) {
                obcm[offset] |= 0b1 << slot;
            } else obcm[offset] &= ~(0b1 << slot);
        }
        i++;
    }
}

#define CLUT_COLOR_COUNT 256
#define CLUT_MAX_COLOR_COUNT 16384

typedef struct {
    std::vector<u32> values;
    u32 index;
} ClutChunk;

static u32 clut[CLUT_MAX_COLOR_COUNT] = {0};
static std::map<u32, ClutChunk> clutMap;
u16 updateClut(u32 value) {
    value = 0xFF000000 | value;
    u32 key = ((u16)-1); // Todo
    
    if(Options::COMPRESS_CLUT) {
        static const float f = Options::CLUT_COMPRESSION_FACTOR;        
        const float r = value & 0xFF;
        const float g = (value >> 8) & 0xFF;
        const float b = (value >> 16) & 0xFF;
        if(Options::CLUT_COMPRESSION_MODE == 1) {
            static const float _f = f / 8.0f;
            const u8 y = (0.299f * r + 0.587f * g + 0.114f * b - 128.0f) / _f;
            const u8 cb = (-0.148f * r - 0.291f * g + 0.439f * b + 128.0f) / f;
            const u8 cr = (0.439f * r - 0.368f * g - 0.071f * b + 128.0f) / f;
            key = (y << 16) | (cb << 8) | cr;
        } else if(Options::CLUT_COMPRESSION_MODE == 2) {
            const u8 _r = r / f;
            const u8 _g = g / f;
            const u8 _b = b / f;
            key = (_r << 16) | (_g  << 8) | _b;
        } else if(Options::CLUT_COMPRESSION_MODE == 3) {
            const u8 _r = 0.299f * (r / f);
            const u8 _g = 0.587f * (g / f);
            const u8 _b = 0.114f * (b / f);
            key = (_r << 16) | (_g << 8) | _b;
        } else {
            const float l = (r + g + b) / 3;
            key = l;
        }
    }
    
    if(!clutMap.count(key)) {
        std::vector<u32> v;
        v.push_back(value);
        const ClutChunk chunk = {
            v,
            clutMap.size()
        };
        clutMap[key] = chunk;
    } else {
        std::vector<u32>& v = clutMap[key].values;
        if(std::find(v.begin(), v.end(), value) == v.end()) {
            v.push_back(value);
        }
    }   
    
    u16 index = clutMap[key].index;
    if(Options::COMPRESS_CLUT) {
        if(index >= CLUT_COLOR_COUNT) {
            index = 0;//CLUT_COLOR_COUNT - 1;
        }
    } else {
        if(index >= CLUT_MAX_COLOR_COUNT) {
            index = CLUT_MAX_COLOR_COUNT - 1;
        }
    }
    return index;
}

u32 getAverage(std::vector<u32>& colors) {
    u16 i = 0;
    u32 r = 0, g = 0, b = 0, a = 0;
    const u16 count = colors.size();
    while(i < count) {
        const u32 color = colors[i];
        r += (color >> 24) & 0xFF;
        g += (color >> 16) & 0xFF;
        b += (color >> 8) & 0xFF;
        a += color & 0xFF;
        i++;
    }
    return ((r / count) << 24) | ((g / count) << 16) |
           ((b / count) << 8 ) | (a / count);
}

void genAPoVSpace() {
    printf("Generating APoV region... \n");
    printf("  Scanning space...\n");
    
    FILE* file = NULL;
    FILE* mfile = NULL;
    
    u32* map = NULL;
    u8* obcm = NULL;
    void* indexes = NULL;    
    u32 FRAME_BIT_COUNT = 0;
    std::vector<Quanta> quantas;
    std::vector<u32>* _map = NULL;
    u32* frame = new u32[FRAME_SIZE];
    
    if(Options::EXPORT_ONE_BIT_COLOR_MAPPING) {
        FRAME_BIT_COUNT = FRAME_SIZE / 8;
        if(FRAME_SIZE % 8 != 0) {
            printf("!!!Frame size should be a multiple of 8!!!\n");
            return;
        }
        obcm = new u8[FRAME_SIZE / 8];
    }
    
    const float MIN_SCALE_STEP = 1.0f / Options::SPACE_BLOCK_SIZE; //
    const float wrapper[7] = {
        0 * MIN_SCALE_STEP,
        1 * MIN_SCALE_STEP, -1 * MIN_SCALE_STEP,
        2 * MIN_SCALE_STEP, -2 * MIN_SCALE_STEP,
        3 * MIN_SCALE_STEP, -3 * MIN_SCALE_STEP
    };
    
    if(Options::EXPORT_CLUT) {
        file = fopen("_clut-indexes.bin", "wb");
        if(Options::COMPRESS_CLUT) {
            indexes = new u8[FRAME_SIZE];
        } else indexes = new u16[FRAME_SIZE];
    } else if(Options::EXPORT_HEADER) {
        file = fopen("_atoms.bin", "ab");
    } else file = fopen("_atoms.bin", "wb");
    
    if(Options::EXPORT_ONE_BIT_COLOR_MAPPING) {
        //_8888
        map = new u32[COLOR_MAP_SIZE];
        _map = new std::vector<u32>[COLOR_MAP_SIZE];
        if(!Options::EXPORT_SINGLE_FILE) {
            mfile = fopen("_map.bin", "wb");
        }
    }
    
    if(file != NULL) {
        u16 hstep = 0;
        const float step = 100.0f / SPACE_ATOM_QUANTITY ;
        while(hstep < HORIZONTAL_POV_COUNT) {
            
            u16 vstep = 0;            
            while(vstep < VERTICAL_POV_COUNT) {
                Pov* const pov = &povs[vstep + hstep * VERTICAL_POV_COUNT];
                
                u32 i = 0;
                u8 percent = 0;
                u32 writtenFrameCount = 0;
                while(i < SPACE_ATOM_QUANTITY) {
                    if((u8)(i * step) > percent) {
                        percent++;
                        printf("\r    %u%%", percent);
                    }
                    Vec4<float> coordinates = getCoordinates<float>(i);
                    if(Options::CAM_DISTANCE) {
                        coordinates.z -= Options::CAM_DISTANCE;
                    }
                    
                    coordinates = math::getSandwichProduct(coordinates, pov->q);
                    
                    if(Options::CAM_LOCK_AT) {
                        coordinates.z += Options::CAM_LOCK_AT;
                    }
                    
                    u32 quanta = 0;
                    float depth = 0.0f, rayLength = 0.0f;
                    bool blendRayStarted = false;
                    float blendDepthStart = 0.0f;
                    Vec4<float>* const rayStep = &pov->rayStep;
                    while(rayLength < Options::MAX_RAY_DEPTH) {
                        //Todo wrapper
                        u8 r = 0;
                        do {
                            Vec4<float> ray = coordinates;
                            
                            if(Options::MAX_PROJECTION_DEPTH != 0.0f) {
                                 //Todo pre-calc
                                const float scale = 1.0f + (depth * PROJECTION_FACTOR);
                                ray.x *= scale + wrapper[r];
                                ray.y *= scale + wrapper[r];
                                ray.z *= scale + wrapper[r];
                            }
                            
                            if(Options::SCALE) {
                                ray.x *= Options::SCALE;
                                ray.y *= Options::SCALE;
                                ray.z *= Options::SCALE;
                            }
                                
                            ray.x += depth * rayStep->x;
                            ray.y += depth * rayStep->y;
                            ray.z += depth * rayStep->z;
                            
                            rayLength = math::getNorm(math::sub(coordinates, ray));

                            u32 color = 0;
                            const u32 offset = getOffset(&ray);
                            if(offset != u32max) {
                                color = space[offset];
                                if(color != 0) {
                                    if(Options::ENABLE_BLENDING) {
                                        if(!blendRayStarted) {
                                            blendDepthStart = rayLength;
                                            blendRayStarted = true;
                                        }
                                        quantas.push_back({color, rayLength});
                                    } else {
                                        quanta = getQuantum(color, rayLength);
                                        goto write_quanta;
                                    }
                                }
                            }
                            
                            if(Options::ENABLE_BLENDING) {
                                if((
                                    ((color & 0xFF) == 0xFF) ||
                                    ((rayLength - blendDepthStart) >= (Options::MAX_BLEND_DEPTH - 1.0f)) ||
                                    (rayLength >= (Options::MAX_RAY_DEPTH - 1.0f)))
                                ) {
                                    u16 qstep = quantas.size();
                                    if(qstep) {
                                        float _depth = 0.0f;
                                        u8 r = 0, g = 0, b = 0;
                                        while(qstep--) {
                                            const Quanta _q = quantas[qstep];
                                            const float _d = getDarkness(_q.depth);
                                            const u8 _r = (u8)(_d * ((_q.color >> 24) & 0xFF));
                                            const u8 _g = (u8)(_d * ((_q.color >> 16) & 0xFF));
                                            const u8 _b = (u8)(_d * ((_q.color >> 8) & 0xFF));
                                            float _a = ((_q.color & 0xFF) / 255.0f);
                                            if(Options::ENABLE_DEEP_TRANSPARENCY) {
                                                _a = powf(_a, Options::TRANSPARENCY_DEPTH);
                                            }
                                            if(_q.depth > _depth) {
                                                _depth = _q.depth;
                                            }
                                            r = (u8)(_r * _a + (1.0f - _a) * r);
                                            g = (u8)(_g * _a + (1.0f - _a) * g);
                                            b = (u8)(_b * _a + (1.0f - _a) * b);
                                        }
                                        const u8 d = _depth > 255.0f ? 255 : (u8)_depth;
                                        quanta = (d << 24) | (b << 16) | (g << 8) | r;
                                    }
                                    quantas.clear();
                                    goto write_quanta;
                                }
                            }
                        } while(r++ < Options::PROJECTION_GAPS_REDUCER);
                        depth++;
                    }
                    write_quanta:
                    const u32 fid = i % FRAME_SIZE;
                    frame[fid] = quanta;
                    if(Options::EXPORT_ONE_BIT_COLOR_MAPPING) {
                        if(quanta) {
                            const u16 x = (fid % SPACE_WIDTH) / COLOR_MAP_WIDTH_SCALE;
                            const u16 y = (fid / SPACE_WIDTH) / COLOR_MAP_HEIGHT_SCALE;
                            _map[x + y * COLOR_MAP_WIDTH].push_back(quanta);
                        }
                        if(fid == FRAME_SIZE - 1) {
                            u32 i = 0;
                            while(i < COLOR_MAP_SIZE) {
                                map[i] = getAverage(_map[i]);
                                _map[i].clear();
                                i++;
                            }
                            applyFilters(frame);
                            getOBCM(frame, obcm);
                            fwrite(obcm, sizeof(u8), FRAME_BIT_COUNT, file);
                            if(Options::EXPORT_SINGLE_FILE) {
                                fwrite(map, sizeof(u32), COLOR_MAP_SIZE, file);
                            } else fwrite(map, sizeof(u32), COLOR_MAP_SIZE, mfile);
                            writtenFrameCount++;
                        }
                    } else {
                        if(fid == FRAME_SIZE - 1) {
                            applyFilters(frame);   
                            if(Options::EXPORT_CLUT) {
                                u32 i = FRAME_SIZE;
                                while(i--) {
                                    if(Options::COMPRESS_CLUT) {
                                        ((u8*)indexes)[i] = updateClut(frame[i]);
                                    } else ((u16*)indexes)[i] = updateClut(frame[i]);
                                }
                                if(Options::COMPRESS_CLUT) {
                                    fwrite(indexes, sizeof(u8), FRAME_SIZE, file);
                                } else fwrite(indexes, sizeof(u16), FRAME_SIZE, file);
                            } else fwrite(frame, sizeof(u32), FRAME_SIZE, file);
                            writtenFrameCount++;
                        }
                    }
                    
                    i++;
                    if(Options::RAY_STEP > 1) {
                        if(!(i % FRAME_SIZE)) {
                            i += (FRAME_SIZE * (Options::RAY_STEP - 1));
                        }
                    }
                }
                vstep++;
                printf("\r    100%% | %u frames in depth.", writtenFrameCount);
                printf(" | v scan | %d/%d\n", vstep, VERTICAL_POV_COUNT);
            }
            hstep++;
            printf("        100%% | h scan | %d/%d\n\n", hstep, HORIZONTAL_POV_COUNT);
        }
        printf("Done!\n\n");
        fclose(file);
        if(Options::EXPORT_ONE_BIT_COLOR_MAPPING && !Options::EXPORT_SINGLE_FILE) {
            fclose(mfile);
        }
        if(Options::EXPORT_CLUT) {
            fclose(file);
            printf("  Generating clut file...\n");

            u32 count = clutMap.size();
            printf("    %u colors available.\n", count);
            if(count >= CLUT_MAX_COLOR_COUNT) {
                printf("    !!!Clut will be satured!!!\n");
                count = CLUT_MAX_COLOR_COUNT;
            }
            
            if(Options::COMPRESS_CLUT && count >= CLUT_COLOR_COUNT) {
                printf("    !!!Clut will be satured!!!\n");
                count = CLUT_COLOR_COUNT;
            }
            
            std::map<u32, ClutChunk>::iterator it = clutMap.begin();
            
            while(it != clutMap.end()) {
                if(it->second.index < count) {
                    u32 r = 0, g = 0, b = 0, j = 0;
                    const u32 s = it->second.values.size();
                    while(j < s) {
                        const u32 v = it->second.values[j];                  
                        r += v & 0x000000FF;
                        g += (v >> 8) & 0x000000FF;
                        b += (v >> 16) & 0x000000FF;
                        j++;
                    }
                    r /= s; g /= s; b /= s;
                    clut[it->second.index] = 0xFF000000 | (b << 16) | (g << 8) | r;
                }
                it++;
            }
            
            file = fopen("_clut.bin", "wb");
            fwrite(clut, sizeof(u32), count, file);          
            fclose(file);
            
            if(Options::COMPRESS_CLUT) {
                delete [] ((u8*)indexes);
            } else delete [] ((u16*)indexes);
            printf("    Clut file generated!\n\n");
        }
    }
    
    if(Options::EXPORT_ONE_BIT_COLOR_MAPPING) {
        delete [] ((u8*)frame);
    } else delete [] ((u32*)frame);
    
    if(Options::EXPORT_ONE_BIT_COLOR_MAPPING) {
        delete [] map;
        delete [] _map;
    }
    printf("  APoV region generated!\n\n");
}

// Path mode APoV generation
// Point of views along the path described by the z axis.
void genPathPoV() {
    genAPoVSpace();
}

int main(int argc, char** argv) {
    Options::process(argc, argv);
    init();
    
    u32 size = 0;
    Voxel* voxels = utils::getBinaryContent<Voxel>("voxels.bin", &size);
    
    if(voxels == NULL) {
        printf("File content is null");
        return 0;
    }
    
    printf("\n");
    fillSpace(voxels, size);
    delete [] voxels;
    
    if(Options::GENERATOR_TYPE == "surface") {
    } else if(Options::GENERATOR_TYPE == "volume") {
    } else {
        genPathPoV();
    }
    if(Options::EXPORT_CLUT) {
        remove("clut-indexes.bin");
        rename("_clut-indexes.bin", "clut-indexes.bin");    
        remove("clut.bin");
        rename("_clut.bin", "clut.bin");
    } else {
        remove("atoms.apov");
        rename("_atoms.bin", "atoms.apov");
        if(Options::EXPORT_ONE_BIT_COLOR_MAPPING && !Options::EXPORT_SINGLE_FILE) {
            remove("map.apov");
            rename("_map.bin", "map.apov");    
        }
    }
    clean();
    return 0;
}
