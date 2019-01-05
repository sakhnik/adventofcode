#include <iostream>
#include <fstream>
#include <vector>
#include <stack>

int main(int argc, char *argv[])
{
    std::ifstream ifs("challenge.bin", std::ios::binary);
    std::vector<uint16_t> memory;
    char buf[2];
    while (ifs && ifs.read(buf, 2))
    {
        uint16_t w = static_cast<uint8_t>(buf[1]);
        w <<= 8;
        w |= static_cast<uint8_t>(buf[0]);
        memory.push_back(w);
    }
    memory.resize(0x8000, 0);

    uint16_t regs[8] = {};
    size_t ip{};
    std::stack<uint16_t> st;

    auto getVal = [&regs](uint16_t &w) -> uint16_t& {
        if (w < 0x8000)
            return w;
        return regs[w - 0x8000];
    };

    auto getNext = [&]() -> uint16_t& {
        return getVal(memory[ip++]);
    };

    while (true)
    {
        auto op = getNext();
        switch (op)
        {
        case 0:  // halt
            return 0;
        case 1:  // set
            {
                auto &a = getNext();
                auto b = getNext();
                a = b;
                break;
            }
        case 2:  // push
            st.push(getNext());
            break;
        case 3:  // pop
            getNext() = st.top();
            st.pop();
            break;
        case 4:  // eq
            {
                auto &a = getNext();
                auto b = getNext();
                auto c = getNext();
                a = b == c;
                break;
            }
        case 5:  // gt
            {
                auto &a = getNext();
                auto b = getNext();
                auto c = getNext();
                a = b > c;
                break;
            }
        case 6:  // jmp
            ip = getNext();
            break;
        case 7:  // jt
            {
                auto a = getNext();
                auto b = getNext();
                if (a)
                    ip = b;
                break;
            }
        case 8:  // jf
            {
                auto a = getNext();
                auto b = getNext();
                if (!a)
                    ip = b;
                break;
            }
        case 9:  // add
            {
                auto &a = getNext();
                auto b = getNext();
                auto c = getNext();
                a = (b + c) & 0x7fff;
                break;
            }
        case 10:  // mult
            {
                auto &a = getNext();
                auto b = getNext();
                auto c = getNext();
                a = (b * c) & 0x7fff;
                break;
            }
        case 11:  // mod
            {
                auto &a = getNext();
                auto b = getNext();
                auto c = getNext();
                a = b % c;
                break;
            }
        case 12:  // and
            {
                auto &a = getNext();
                auto b = getNext();
                auto c = getNext();
                a = b & c;
                break;
            }
        case 13:  // or
            {
                auto &a = getNext();
                auto b = getNext();
                auto c = getNext();
                a = b | c;
                break;
            }
        case 14:  // not
            {
                auto &a = getNext();
                auto b = getNext();
                a = ~b & 0x7fff;
                break;
            }
        case 15:  // rmem
            {
                auto &a = getNext();
                auto b = getNext();
                a = memory[b];
                break;
            }
        case 16:  // wmem
            {
                auto a = getNext();
                auto b = getNext();
                memory[a] = b;
                break;
            }
        case 17:  // call
            {
                auto a = getNext();
                st.push(ip);
                ip = a;
                break;
            }
        case 18:  // ret
            {
                if (st.empty())
                {
                    return 0;
                }
                ip = st.top();
                st.pop();
                break;
            }
        case 19:  // out
            std::cout << static_cast<char>(getNext()) << std::flush;
            break;
        case 21:  // noop
            break;
        default:
            throw std::runtime_error(std::to_string(op) + " not implemented");
        }
    }
}
