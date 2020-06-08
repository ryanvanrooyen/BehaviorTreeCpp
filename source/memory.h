
#pragma once

#include <cstdint>
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
    T* allocate(Args... args)
    {
        size_t size = sizeof(T);
        if ((offset + size) >= maxBytes)
            throw std::bad_alloc();
		T* instance = new ((void*)((uintptr_t)buffer + offset)) T(args...);
        offset += size;
        return instance;
    }

    template <typename T>
    T* allocateArray(int length)
    {
        size_t size = sizeof(T) * length;
        if ((offset + size) >= maxBytes)
            throw std::bad_alloc();
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
