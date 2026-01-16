//shaders/add3_kernel.metal

#include <metal_stdlib>

#define NT_MAKE_ADD3_KERNEL(Type)\
kernel void add3_kernel_##Type(\
        device const Type* in  [[ buffer(0) ]], \
        device Type* out       [[ buffer(1) ]], \
        uint3 tid                 [[ thread_position_in_grid ]]){ \
    out[tid.x] = in[tid.x] + Type(3.0); \
} \

NT_MAKE_ADD3_KERNEL(float)
NT_MAKE_ADD3_KERNEL(int64_t)

#undef NT_MAKE_ADD3_KERNEL

#define NT_MAKE_ADD3_KERNEL_INDICES(Type)\
kernel void add3_kernel_indiced_##Type(\
        device const int64_t* indices   [[ buffer(0) ]],\
        device const Type* in           [[ buffer(1) ]],\
        device Type* out                [[ buffer(2) ]],\
        uint3 tid                 [[ thread_position_in_grid ]]){ \
    out[tid.x] = in[indices[tid.x]] + Type(3.0); \
} \

NT_MAKE_ADD3_KERNEL_INDICES(float)

#undef NT_MAKE_ADD3_KERNEL_INDICES


struct _NTBlockedRange{
    int64_t start;
    int64_t end;
};

#define NT_MAKE_ADD3_KERNEL_BLOCKED(Type)\
kernel void add3_kernel_blocked_##Type(\
        device const _NTBlockedRange* ranges    [[ buffer(0) ]],\
        device const Type* in                   [[ buffer(1) ]],\
        device Type* out                        [[ buffer(2) ]],\
        uint3 tid                           [[ thread_position_in_grid ]]){ \
    int64_t i = ranges[tid.x].start; \
    for(; i < ranges[tid.x].end; ++i){ \
        out[i] = in[i] + Type(3.0); \
    } \
}

NT_MAKE_ADD3_KERNEL_BLOCKED(float)

#undef NT_MAKE_ADD3_KERNEL_BLOCKED 
// kernel void add3_kernel_float(
//     device const float* in  [[ buffer(0) ]],
//     device float* out       [[ buffer(1) ]],
//     uint id                 [[ thread_position_in_grid ]]){
//
//     out[id] = in[id] + 3.0f;
// }
