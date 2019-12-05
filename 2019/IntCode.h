#pragma once

#include <iostream>
#include <unordered_map>


class IntCode
{
public:
    IntCode(std::istream &is);
    IntCode(std::istream &&is)
        : IntCode(is)
    {
    }

    IntCode(const IntCode &) = default;
    IntCode& operator=(const IntCode &) = default;
    IntCode(IntCode &&) = default;
    IntCode& operator=(IntCode &&) = default;

    void Run(std::istream & = std::cin, std::ostream & = std::cout);

    int GetMemory(int addr) const
    {
        auto it = _memory.find(addr);
        if (it == _memory.end())
        {
            throw "Memory Fault";
        }
        return it->second;
    }

    void SetMemory(int addr, int val)
    {
        _memory[addr] = val;
    }

private:
    using MemoryT = std::unordered_map<int, int>;
    MemoryT _memory;
};
