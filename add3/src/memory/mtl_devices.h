#ifndef __NT_DEVICES_MTL_H__
#define __NT_DEVICES_MTL_H__

#include "../utils/mtl_utils.h"
#include <string>
#include <vector>

namespace nt{
class DeviceMTLPrivate{
    MTL::Buffer* memory;
    int64_t size;
public:
    DeviceMTLPrivate()
    :memory(nullptr), size(0) {}
    ~DeviceMTLPrivate();
    void allocate_memory(int64_t size);
    void release_memory();
    void* get_memory();
    void* get_end_memory();
    const void* get_memory() const;
    const void* get_end_memory() const;
    inline std::string get_name() const {return "MTLPrivate";}
    inline const int64_t& Size() const {return size;}
    inline MTL::Buffer* get_buffer() {return memory;}
};

class DeviceMTLShared{
    MTL::Buffer* memory;
    int64_t size;
public:
    DeviceMTLShared()
    :memory(nullptr), size(0) {}
    ~DeviceMTLShared();
    void allocate_memory(int64_t size);
    void release_memory();
    void* get_memory();
    void* get_end_memory();
    const void* get_memory() const;
    const void* get_end_memory() const;
    inline std::string get_name() const {return "MTLShared";}
    inline const int64_t& Size() const {return size;}
    inline MTL::Buffer* get_buffer() {return memory;}
};

DeviceMTLPrivate mtl_shared_to_private(DeviceMTLShared& device, MTL::CommandBuffer*);
DeviceMTLShared mtl_private_to_shared(DeviceMTLPrivate& device, MTL::CommandBuffer*);

template<typename T>
inline DeviceMTLShared mtl_shared_from_vector(const std::vector<T>& vec){
    DeviceMTLShared shared;
    shared.allocate_memory(vec.size() * sizeof(T));
    std::memcpy(shared.get_memory(), vec.data(), sizeof(T) * vec.size());
    return shared;
}

}

#endif
