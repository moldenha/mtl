#ifndef __NT_UTILS_MTL_H__
#define __NT_UTILS_MTL_H__

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

namespace nt{
namespace utils{
namespace mtl{

struct ThreadDispatchConfig {
    MTL::Size gridSize;
    MTL::Size threadgroupSize;
};

ThreadDispatchConfig computeThreadDispatchConfig(int64_t N);
extern MTL::Device* nt_mtl_device;
extern NS::AutoreleasePool* nt_mtl_pool;
void nt_mtl_init();
void nt_metal_shutdown();
bool mtl_initialized();



}
}
}

#endif
