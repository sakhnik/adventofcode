#include "IntCode.h"
#include <iostream>
#include <boost/scope_exit.hpp>


IntCode::IntCode(std::istream &is)
{
    int idx{0};
    while (is)
    {
        int val{};
        char comma;
        is >> val >> comma;
        _memory[idx++] = val;
    }
}

int IntCode::_GetArgIdx(int state_flags, int num)
{
    for (int i = 0; i < num; ++i)
    {
        state_flags /= 10;
    }
    int mode = state_flags % 10;

    switch (mode)
    {
    case 0:
        return _memory[_ip + num + 1];
    case 1:
        return _ip + num + 1;
    default:
        throw "Mode not implemented";
    }
}

int IntCode::Advance(int input)
{
    while (true)
    {
        int cmd = _memory[_ip];
        int opcode = cmd % 100;
        int state_flags = cmd / 100;

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
            return _state = S_INPUT;
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
                    _ip = _memory[addr];
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
                    _ip = _memory[addr];
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
        case 99:
            return _state = S_HALT;
        default:
            std::cout << "HCF" << std::endl;
            break;
        }
    }
}
