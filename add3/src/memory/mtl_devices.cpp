#include "mtl_devices.h"
#include "../utils/mtl_utils.h"

namespace nt{


DeviceMTLPrivate::~DeviceMTLPrivate(){
    this->release_memory();
}

void DeviceMTLPrivate::allocate_memory(int64_t size){
    if(!nt::utils::mtl::mtl_initialized()){utils::mtl::nt_mtl_init();}
    this->memory = nt::utils::mtl::nt_mtl_device->newBuffer(size, MTL::ResourceStorageModePrivate);
    this->size = size;
}

void DeviceMTLPrivate::release_memory(){
    if(this->memory) this->memory->release();
}

void* DeviceMTLPrivate::get_memory(){return this->memory->contents();}
void* DeviceMTLPrivate::get_end_memory(){
    return reinterpret_cast<char*>(this->memory->contents())
    + this->size;
}
const void* DeviceMTLPrivate::get_memory() const {return this->memory->contents();}
const void* DeviceMTLPrivate::get_end_memory() const {
    return reinterpret_cast<const char*>(this->memory->contents())
    + this->size;
}


DeviceMTLShared::~DeviceMTLShared(){
    this->release_memory();
}

void DeviceMTLShared::allocate_memory(int64_t size){
    if(!nt::utils::mtl::mtl_initialized()){utils::mtl::nt_mtl_init();}
    this->memory = nt::utils::mtl::nt_mtl_device->newBuffer(size, MTL::ResourceStorageModeShared);
    this->size = size;
}

void DeviceMTLShared::release_memory(){
    if(this->memory) this->memory->release();
}

void* DeviceMTLShared::get_memory(){return this->memory->contents();}
void* DeviceMTLShared::get_end_memory(){
    return reinterpret_cast<char*>(this->memory->contents())
    + this->size;
}
const void* DeviceMTLShared::get_memory() const {return this->memory->contents();}
const void* DeviceMTLShared::get_end_memory() const {    
    return reinterpret_cast<const char*>(this->memory->contents())
    + this->size;
}

DeviceMTLPrivate mtl_shared_to_private(DeviceMTLShared& device, MTL::CommandBuffer* buffer){
    // Copy CPU → GPU buffer
    DeviceMTLPrivate outBuffer;
    outBuffer.allocate_memory(device.Size());
    MTL::BlitCommandEncoder* blit = buffer->blitCommandEncoder();
    blit->copyFromBuffer(device.get_buffer(), 0, outBuffer.get_buffer(), 0, device.Size());
    blit->endEncoding();
    return outBuffer;
}

DeviceMTLShared mtl_private_to_shared(DeviceMTLPrivate& device, MTL::CommandBuffer* buffer){
    // Copy GPU → CPU buffer
    DeviceMTLShared outBuffer;
    outBuffer.allocate_memory(device.Size());
    MTL::BlitCommandEncoder* blit = buffer->blitCommandEncoder();
    blit->copyFromBuffer(device.get_buffer(), 0, outBuffer.get_buffer(), 0, device.Size());
    blit->endEncoding();
    return outBuffer;
}


}
