#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char *argv[])
{
    std::ifstream ifs("challenge.bin", std::ios::binary);
    std::vector<uint16_t> program;
    char buf[2];
    while (ifs && ifs.read(buf, 2))
    {
        uint16_t w = static_cast<uint8_t>(buf[1]);
        w <<= 8;
        w |= static_cast<uint8_t>(buf[0]);
        program.push_back(w);
    }

    uint16_t regs[8] = {};
    size_t ip{};

    auto getVal = [&regs](uint16_t &w) -> uint16_t& {
        if (w < 0x8000)
            return w;
        return regs[w - 0x8000];
    };

    while (true)
    {
        auto op = program[ip++];
        switch (op)
        {
        case 0:  // halt
            return 0;
        case 19:  // out
            std::cout << static_cast<char>(getVal(program[ip++])) << std::flush;
            break;
        case 21:  // noop
            break;
        default:
            throw std::runtime_error(std::to_string(op) + " not implemented");
        }
    }
}
