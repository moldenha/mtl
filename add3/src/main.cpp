//main_shared.cpp showed memory that was shared betweent the CPU and the GPU
//this file shows how to use memory entirely on the GPU

#include <iostream>
#include <vector>
#include <string>
#include "utils/mtl_utils.h"
#include "memory/mtl_devices.h"
#include <complex>

int contiguous_shared_gpu_transfer(){
    //Data
    const uint32_t N = 8;
    std::vector<float> input(N);
    for(uint32_t i = 0; i < N; ++i) input[i] = static_cast<float>(i);

    nt::DeviceMTLShared stagingIn = nt::mtl_shared_from_vector(input);

    nt::DeviceMTLPrivate outBuffer;
    outBuffer.allocate_memory(stagingIn.Size());


    //loading the shader library
    NS::Error* error = nullptr;
    MTL::Library* library = nt::utils::mtl::nt_mtl_device->newLibrary(NS::String::string(METALLIB_PATH, NS::UTF8StringEncoding), nullptr);
    /* MTL::Library* library = device->newDefaultLibrary(); */
    if(!library){
        std::cerr << "Failed to load Metal library\n";
        return -1;
    }

    MTL::Function* kernelFunc = library->newFunction(NS::String::string("add3_kernel_float", NS::UTF8StringEncoding));
    MTL::ComputePipelineState* pipeline = nt::utils::mtl::nt_mtl_device->newComputePipelineState(kernelFunc, &error);

    //Command Queue
    MTL::CommandQueue* queue = nt::utils::mtl::nt_mtl_device->newCommandQueue();
    MTL::CommandBuffer* commandBuffer = queue->commandBuffer();

    //automatically convert to private GPU function
    nt::DeviceMTLPrivate inBuffer = nt::mtl_shared_to_private(stagingIn, commandBuffer);


    //compute pass
    MTL::ComputeCommandEncoder* encoder = commandBuffer->computeCommandEncoder();
    encoder->setComputePipelineState(pipeline);
    encoder->setBuffer(inBuffer.get_buffer(), 0, 0);
    encoder->setBuffer(outBuffer.get_buffer(), 0, 1);

    nt::utils::mtl::ThreadDispatchConfig config = nt::utils::mtl::computeThreadDispatchConfig(N);
    encoder->dispatchThreads(config.gridSize, config.threadgroupSize);
    encoder->endEncoding();

    // 3. Copy GPU → CPU readback buffer
    nt::DeviceMTLShared stagingOut = nt::mtl_private_to_shared(outBuffer, commandBuffer);

    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    // Read results
    float* outData = static_cast<float*>(stagingOut.get_memory());
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
    return 0;
}


//this would be like a tensor t and indices i returning t[i] + 3;
int indiced_add3_function(){
    int64_t num_elements = 30;
    int64_t num_indices = 5;
    std::vector<float> vec(num_elements);
    for(int64_t i = 0; i < num_elements; ++i){vec[i] = static_cast<float>(i);}
    std::vector<int64_t> indices(num_indices);
    for(int64_t i = 0; i < num_indices; ++i){indices[i] = i * 4;}

    //transfering them to shared devices for simplicity
    //it was already shown above how to transfer from shared to private GPU

    nt::DeviceMTLShared inBuffer = nt::mtl_shared_from_vector(vec);
    nt::DeviceMTLShared indicesBuffer = nt::mtl_shared_from_vector(indices);
    nt::DeviceMTLShared outBuffer;
    outBuffer.allocate_memory(num_indices * sizeof(float));

    //loading the shader library
    NS::Error* error = nullptr;
    MTL::Library* library = nt::utils::mtl::nt_mtl_device->newLibrary(NS::String::string(METALLIB_PATH, NS::UTF8StringEncoding), nullptr);
    /* MTL::Library* library = device->newDefaultLibrary(); */
    if(!library){
        std::cerr << "Failed to load Metal library\n";
        return -1;
    }

    MTL::Function* kernelFunc = library->newFunction(NS::String::string("add3_kernel_indiced_float", NS::UTF8StringEncoding));
    MTL::ComputePipelineState* pipeline = nt::utils::mtl::nt_mtl_device->newComputePipelineState(kernelFunc, &error);

    //Command Queue
    MTL::CommandQueue* queue = nt::utils::mtl::nt_mtl_device->newCommandQueue();
    MTL::CommandBuffer* commandBuffer = queue->commandBuffer();

    //compute pass
    MTL::ComputeCommandEncoder* encoder = commandBuffer->computeCommandEncoder();
    encoder->setComputePipelineState(pipeline);
    encoder->setBuffer(indicesBuffer.get_buffer(), 0, 0);
    encoder->setBuffer(inBuffer.get_buffer(), 0, 1);
    encoder->setBuffer(outBuffer.get_buffer(), 0, 2);

    nt::utils::mtl::ThreadDispatchConfig config = nt::utils::mtl::computeThreadDispatchConfig(num_indices);
    encoder->dispatchThreads(config.gridSize, config.threadgroupSize);
    encoder->endEncoding();

    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
    

    // Read results
    float* outData = static_cast<float*>(outBuffer.get_memory());
    for (int i = 0; i < num_indices; ++i)
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
    return 0;

}


int blocked_add3_function(){
    int64_t num_elements = 30;
    int64_t num_indices = 5;
    std::vector<float> vec(num_elements);
    for(int64_t i = 0; i < num_elements; ++i){vec[i] = static_cast<float>(i);}
    std::vector<std::complex<int64_t>> blocks = {{1, 3}, {5, 9}, {11, 12}, {19, 25}, {15, 17}};

    //transfering them to shared devices for simplicity
    //it was already shown above how to transfer from shared to private GPU

    nt::DeviceMTLShared inBuffer = nt::mtl_shared_from_vector(vec);
    nt::DeviceMTLShared blockedBuffer = nt::mtl_shared_from_vector(blocks);
    nt::DeviceMTLShared outBuffer;
    outBuffer.allocate_memory(num_elements * sizeof(float));
    std::memset(outBuffer.get_memory(), 0, num_elements * sizeof(float));

    //loading the shader library
    NS::Error* error = nullptr;
    MTL::Library* library = nt::utils::mtl::nt_mtl_device->newLibrary(NS::String::string(METALLIB_PATH, NS::UTF8StringEncoding), nullptr);
    /* MTL::Library* library = device->newDefaultLibrary(); */
    if(!library){
        std::cerr << "Failed to load Metal library\n";
        return -1;
    }

    MTL::Function* kernelFunc = library->newFunction(NS::String::string("add3_kernel_blocked_float", NS::UTF8StringEncoding));
    MTL::ComputePipelineState* pipeline = nt::utils::mtl::nt_mtl_device->newComputePipelineState(kernelFunc, &error);

    //Command Queue
    MTL::CommandQueue* queue = nt::utils::mtl::nt_mtl_device->newCommandQueue();
    MTL::CommandBuffer* commandBuffer = queue->commandBuffer();

    //compute pass
    MTL::ComputeCommandEncoder* encoder = commandBuffer->computeCommandEncoder();
    encoder->setComputePipelineState(pipeline);
    encoder->setBuffer(blockedBuffer.get_buffer(), 0, 0);
    encoder->setBuffer(inBuffer.get_buffer(), 0, 1);
    encoder->setBuffer(outBuffer.get_buffer(), sizeof(float), 2);

    nt::utils::mtl::ThreadDispatchConfig config = nt::utils::mtl::computeThreadDispatchConfig(num_indices);
    encoder->dispatchThreads(config.gridSize, config.threadgroupSize);
    encoder->endEncoding();

    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
    

    // Read results
    float* outData = static_cast<float*>(outBuffer.get_memory());
    for (int i = 0; i < num_elements; ++i)
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
    return 0;

}


int main(){
    return blocked_add3_function();
}
