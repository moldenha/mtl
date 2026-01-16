#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "mtl_utils.h"
#include <iostream>

namespace nt{
namespace utils{
namespace mtl{

MTL::Device* nt_mtl_device = nullptr;
NS::AutoreleasePool* nt_mtl_pool = nullptr;

void nt_mtl_init(){
    nt_mtl_pool = NS::AutoreleasePool::alloc()->init();
    nt_mtl_device = MTL::CreateSystemDefaultDevice();
}

void nt_metal_shutdown() {
    if (nt_mtl_pool) {
        nt_mtl_pool->drain();
        nt_mtl_pool = nullptr;
    }
}

bool mtl_initialized() {return nt_mtl_pool != nullptr;}

ThreadDispatchConfig computeThreadDispatchConfig(int64_t N) {
    // Get max total threads allowed in a single threadgroup
    MTL::Size maxSize = nt_mtl_device->maxThreadsPerThreadgroup();
    uint32_t maxThreadsPerGroup =
    maxSize.width * maxSize.height * maxSize.depth;
    uint32_t max1DThreads = maxSize.width;


    // Pick a reasonable power-of-two size <= max
    uint32_t groupSize = 1;
    while (groupSize * 2 <= max1DThreads && groupSize * 2 <= N)
        groupSize *= 2;

    uint32_t numGroups = (N + groupSize - 1) / groupSize;

    return ThreadDispatchConfig {
        .gridSize = MTL::Size::Make(N, 1, 1),
        .threadgroupSize = MTL::Size::Make(groupSize, 1, 1)
    };
}


//the following class ensures that mtl is shut down properly
class NTMetalFinalizer {
public:
    ~NTMetalFinalizer() {
        nt_metal_shutdown();  // This gets called at global teardown
    }
};

// Declare a global static object
static NTMetalFinalizer _nt_metal_finalizer;

}
}
}
