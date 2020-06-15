
#include "memory.h"
#include <cstring>

namespace bt
{

Memory::Memory(const size_t maxBytes)
    : buffer(new uint8_t[maxBytes]), maxBytes(maxBytes) {}

Memory::Memory(const Memory& m)
    : buffer(new uint8_t[m.maxBytes]), maxBytes(m.maxBytes), offset(m.offset)
{
    if (offset > 0)
        memcpy(buffer, m.buffer, offset);
}

Memory::Memory(Memory&& m) noexcept
    : buffer(m.buffer), offset(m.offset), maxBytes(m.maxBytes)
{
    m.buffer = nullptr;
}

Memory::~Memory()
{
    delete[] buffer;
}

}
