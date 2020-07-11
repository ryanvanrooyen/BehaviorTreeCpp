
#ifndef BEHAVIOR_TREE_MEMORY_H
#define BEHAVIOR_TREE_MEMORY_H

#include <cstdint>
#include <cstring>

namespace bt
{

class Memory
{
public:
    Memory(const size_t maxBytes)
        : buffer(new uint8_t[maxBytes]), maxBytes(maxBytes) {}

    Memory(const Memory& m)
        : buffer(new uint8_t[m.maxBytes]), maxBytes(m.maxBytes), offset(m.offset)
    {
        if (offset > 0)
            memcpy(buffer, m.buffer, offset);
    }

    Memory(Memory&& m) noexcept
        : buffer(m.buffer), offset(m.offset), maxBytes(m.maxBytes)
    {
        m.buffer = nullptr;
    }

    ~Memory() { delete[] buffer; }

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

#endif
