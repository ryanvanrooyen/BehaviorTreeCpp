
#pragma once

#include <utility>
#include <cstdint>
#include <stdexcept>
#include <new>

namespace bt
{

class Memory
{
public:
    Memory(const size_t maxBytes);
    Memory(const Memory& m);
    Memory(Memory&& m) noexcept;
    ~Memory();

    size_t size() const { return offset; }
    size_t maxSize() const { return maxBytes; }

    template <typename T, typename... Args>
    T* allocate(Args&&... args)
    {
        size_t size = sizeof(T);
        if ((offset + size) >= maxBytes)
            throw std::runtime_error("BehaviorTree Memory capacity exceeded.");
		T* instance = new ((void*)((uintptr_t)buffer + offset)) T(std::forward<Args>(args)...);
        offset += size;
        return instance;
    }

    template <typename T>
    T* allocateArray(int length)
    {
        size_t size = sizeof(T) * length;
        if ((offset + size) >= maxBytes)
            throw std::runtime_error("BehaviorTree Memory capacity exceeded.");
		T* instance = new ((void*)((uintptr_t)buffer + offset)) T [length];
        offset += size;
        return instance;
    }

private:
	uint8_t* buffer;
    size_t offset = 0;
    const size_t maxBytes;
};

}
