#pragma once

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

    enum State
    {
        S_RUN = std::numeric_limits<int>::min(),
        S_HALT = S_RUN + 1,
        S_INPUT = S_RUN + 2,
        S_OUTPUT = S_RUN + 3,
        // Every other value is output.
    };

    int Advance(int);

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
    State _state = S_RUN;
    int _ip = 0;

    int _GetArgIdx(int state_flags, int num);
};
