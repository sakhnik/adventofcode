#include "../test.hpp"
#include <bitset>
#include <unordered_map>
#include <numeric>
#include <fstream>

namespace {

using namespace boost::ut;

uint64_t Initialize(std::istream &&is)
{
    const int N = 36;
    std::bitset<N> mask = 0, vals = 0;
    std::unordered_map<uint64_t, uint64_t> mem;

    std::string line;
    uint64_t addr{}, val{};
    while (getline(is, line))
    {
        if (line.starts_with("mask = "))
        {
            std::string_view str{line.data() + 7, N};
            for (size_t i = 0; i < mask.size(); ++i)
            {
                switch (char ch = str[i])
                {
                case 'X':
                    mask.set(N - i - 1, false);
                    vals.set(N - i - 1, false);
                    break;
                case '0':
                case '1':
                    mask.set(N - i - 1, true);
                    vals.set(N - i - 1, ch == '1');
                    break;
                }
            }
        }
        else if (2 == sscanf(line.c_str(), "mem[%ld] = %ld", &addr, &val))
        {
            uint64_t m = mask.to_ullong();
            val &= ~m;  // clear the masked bits
            val |= vals.to_ullong();  // override the bits from the mask
            mem[addr] = val;
        }
        else
            expect(false);
    }

    return std::accumulate(mem.begin(), mem.end(), 0ull,
                           [](auto a, auto b) { return a + b.second; });
}

suite s = [] {
    "2020-14"_test = [] {
        {
            const char *const TEST =
                "mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X\n"
                "mem[8] = 11\n"
                "mem[7] = 101\n"
                "mem[8] = 0";
            expect(165_u == Initialize(std::istringstream{TEST}));
        }

        Printer::Print(__FILE__, "1", Initialize(std::ifstream{INPUT}));
    };
};

} //namespace;
