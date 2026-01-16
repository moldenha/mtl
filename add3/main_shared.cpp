#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include <iostream>
#include <vector>
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp> // Only needed if rendering; safe to ignore here

int main(){
    //Setup Metal
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
    MTL::Device* device = MTL::CreateSystemDefaultDevice();


    //Data
    const uint32_t N = 8;
    std::vector<float> input(N);
    for(uint32_t i = 0; i < N; ++i) input[i] = static_cast<float>(i);

    //create buffers
    MTL::Buffer* inBuffer = device->newBuffer(sizeof(float) * N, MTL::ResourceStorageModeShared);
    MTL::Buffer* outBuffer = device->newBuffer(sizeof(float) * N, MTL::ResourceStorageModeShared);

    memcpy(inBuffer->contents(), input.data(), sizeof(float) * N);
    
    //loading the shader library
    std::cout << "Metal lib path: "<<METALLIB_PATH << std::endl;
    NS::Error* error = nullptr;
    MTL::Library* library = device->newLibrary(NS::String::string(METALLIB_PATH, NS::UTF8StringEncoding), nullptr);
    /* MTL::Library* library = device->newDefaultLibrary(); */
    if(!library){
        std::cerr << "Failed to load Metal library\n";
        return -1;
    }

    MTL::Function* kernelFunc = library->newFunction(NS::String::string("add3_kernel_float", NS::UTF8StringEncoding));
    MTL::ComputePipelineState* pipeline = device->newComputePipelineState(kernelFunc, &error);

    //Command Queue
    MTL::CommandQueue* queue = device->newCommandQueue();
    MTL::CommandBuffer* commandBuffer = queue->commandBuffer();
    MTL::ComputeCommandEncoder* encoder = commandBuffer->computeCommandEncoder();

    encoder->setComputePipelineState(pipeline);
    encoder->setBuffer(inBuffer, 0, 0);
    encoder->setBuffer(outBuffer, 0, 1);

    MTL::Size gridSize = MTL::Size::Make(N, 1, 1);
    MTL::Size threadgroupSize = MTL::Size::Make(1,1,1);
    encoder->dispatchThreads(gridSize, threadgroupSize);

    encoder->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    // Read results
    float* outData = static_cast<float*>(outBuffer->contents());
    for (int i = 0; i < N; ++i)
        std::cout << "output[" << i << "] = " << outData[i] << std::endl;
    std::cout << "cleaning up now"<<std::endl;
    // Cleanup
    // Safe cleanup: only release explicitly created buffers and encoder
    /* if (encoder) encoder->release(); */
    /* if (commandBuffer) commandBuffer->release(); */
    if (queue) queue->release();

    // These may already be autoreleased. Avoid releasing unless explicitly retained.
    if (pipeline) pipeline->release();
    if (kernelFunc) kernelFunc->release() ;
    if (library) library->release();

    // Buffers are usually safe to release if created with newBuffer
    if (inBuffer) inBuffer->release();
    if (outBuffer) outBuffer->release();

    std::cout << "draining pool..."<<std::endl;
    // Only call release if pool was created using alloc/init
    if (pool) pool->drain();  // safer than release
    std::cout << "drained"<<std::endl;
    return 0;
}
