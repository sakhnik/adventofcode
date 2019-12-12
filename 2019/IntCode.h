#pragma once

#include <unordered_map>
#include <iostream>
#include <cassert>
#include <boost/multiprecision/gmp.hpp>


template <typename IntT>
int to_int(IntT v)
{
    return v;
}

template <typename IntT>
class IntCodeImpl
{
public:
    IntCodeImpl(std::istream &is)
    {
        int idx{0};
        while (is)
        {
            IntT val{};
            char comma;
            is >> val >> comma;
            _memory[idx++] = val;
        }
    }

    IntCodeImpl(std::istream &&is)
        : IntCodeImpl(is)
    {
    }

    IntCodeImpl(const IntCodeImpl &) = default;
    IntCodeImpl& operator=(const IntCodeImpl &) = default;
    IntCodeImpl(IntCodeImpl &&) = default;
    IntCodeImpl& operator=(IntCodeImpl &&) = default;

    enum State
    {
        S_RUN = 0,
        S_HALT,
        S_INPUT,
        S_OUTPUT,
    };
    State GetState() { return _state; }

    IntT Advance(IntT input)
    {
        while (true)
        {
            auto cmd = _memory[_ip];
            int opcode = to_int(cmd) % 100;
            int state_flags = to_int(cmd) / 100;

            switch (opcode)
            {
            case 1:
                {
                    auto a = _GetArgIdx(state_flags, 0);
                    auto b = _GetArgIdx(state_flags, 1);
                    auto c = _GetArgIdx(state_flags, 2);
                    _memory[c] = _memory[a] + _memory[b];
                    _ip += 4;
                    break;
                }
            case 2:
                {
                    auto a = _GetArgIdx(state_flags, 0);
                    auto b = _GetArgIdx(state_flags, 1);
                    auto c = _GetArgIdx(state_flags, 2);
                    _memory[c] = _memory[a] * _memory[b];
                    _ip += 4;
                    break;
                }
            case 3:
                if (_state == S_INPUT)
                {
                    auto a = _GetArgIdx(state_flags, 0);
                    _memory[a] = input;
                    _state = S_RUN;
                    _ip += 2;
                    break;
                }
                assert(_state == S_RUN);
                _state = S_INPUT;
                return 0;
            case 4:
                if (_state == S_RUN)
                {
                    _state = S_OUTPUT;
                    auto v = _GetArgIdx(state_flags, 0);
                    return _memory[v];
                }
                assert(_state = S_OUTPUT);
                _ip += 2;
                _state = S_RUN;
                break;
            case 5:
                {
                    auto cond = _GetArgIdx(state_flags, 0);
                    auto addr = _GetArgIdx(state_flags, 1);
                    if (_memory[cond])
                    {
                        _ip = to_int(_memory[addr]);
                        continue;
                    }
                    _ip += 3;
                    break;
                }
            case 6:
                {
                    auto cond = _GetArgIdx(state_flags, 0);
                    auto addr = _GetArgIdx(state_flags, 1);
                    if (!_memory[cond])
                    {
                        _ip = to_int(_memory[addr]);
                        continue;
                    }
                    _ip += 3;
                    break;
                }
            case 7:
                {
                    auto a = _GetArgIdx(state_flags, 0);
                    auto b = _GetArgIdx(state_flags, 1);
                    auto c = _GetArgIdx(state_flags, 2);
                    _memory[c] = _memory[a] < _memory[b];
                    _ip += 4;
                    break;
                }
            case 8:
                {
                    auto a = _GetArgIdx(state_flags, 0);
                    auto b = _GetArgIdx(state_flags, 1);
                    auto c = _GetArgIdx(state_flags, 2);
                    _memory[c] = _memory[a] == _memory[b];
                    _ip += 4;
                    break;
                }
            case 9:
                {
                    auto a = _GetArgIdx(state_flags, 0);
                    _rb += to_int(_memory[a]);
                    _ip += 2;
                    break;
                }
            case 99:
                _state = S_HALT;
                return 0;
            default:
                std::cout << "Unknown opcode " << opcode << std::endl;
                throw "HCF";
            }
        }
    }

    IntT GetMemory(int addr) const
    {
        auto it = _memory.find(addr);
        if (it == _memory.end())
        {
            throw "Memory Fault";
        }
        return it->second;
    }

    void SetMemory(int addr, IntT val)
    {
        _memory[addr] = val;
    }

private:
    using MemoryT = std::unordered_map<int, IntT>;
    MemoryT _memory;
    State _state = S_RUN;
    int _ip = 0;
    int _rb = 0;

    int _GetArgIdx(int state_flags, int num)
    {
        for (int i = 0; i < num; ++i)
        {
            state_flags /= 10;
        }
        int mode = state_flags % 10;

        switch (mode)
        {
        case 0:
            return to_int(_memory[_ip + num + 1]);
        case 1:
            return _ip + num + 1;
        case 2:
            return _rb + to_int(_memory[_ip + num + 1]);
        default:
            throw "Mode not implemented";
        }
    }
};

using IntCode = IntCodeImpl<int>;

using BigIntT = boost::multiprecision::mpz_int;
using IntCodeB = IntCodeImpl<BigIntT>;

template<>
inline int to_int(BigIntT v)
{
    return v.convert_to<int>();
}
