#include "../test.hpp"
#include <bitset>
#include <unordered_map>
#include <numeric>
#include <fstream>

namespace {

using namespace boost::ut;

const int N = 36;
using BitsT = std::bitset<N>;

class Task1
{
public:
    void SetMask(BitsT mask, BitsT vals)
    {
        _mask = mask;
        _vals = vals;
    }

    void SetMem(uint64_t addr, uint64_t val)
    {
        uint64_t m = _mask.to_ullong();
        val &= ~m;  // clear the masked bits
        val |= _vals.to_ullong();  // override the bits from the mask
        _mem[addr] = val;
    }

    uint64_t GetResult() const
    {
        return std::accumulate(_mem.begin(), _mem.end(), 0ull,
                               [](auto a, auto b) { return a + b.second; });
    }

protected:
    BitsT _mask, _vals;
    std::unordered_map<uint64_t, uint64_t> _mem;
};

class Task2
    : public Task1
{
public:
    void SetMem(uint64_t addr, uint64_t val)
    {
        uint64_t v = _vals.to_ullong();
        addr |= v;

        auto m1 = BitsT{_mask}.flip();
        size_t count = m1.count();
        _Set(0, count, m1, addr, val);
    }

private:
    void _Set(size_t i, size_t count, BitsT m, uint64_t addr, uint64_t val)
    {
        if (!count)
        {
            _mem[addr] = val;
            return;
        }

        for (; i < N; ++i)
        {
            if (m.test(i))
            {
                addr &= ~(1ull << i);
                _Set(i + 1, count - 1, m, addr, val);
                _Set(i + 1, count - 1, m, addr | (1ull << i), val);
                break;
            }
        }
    }
};

template <typename TaskT>
uint64_t Initialize(std::istream &&is)
{
    TaskT task;

    std::string line;
    uint64_t addr{}, val{};
    while (getline(is, line))
    {
        if (line.starts_with("mask = "))
        {
            BitsT mask = 0, vals = 0;
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

            task.SetMask(mask, vals);
        }
        else if (2 == sscanf(line.c_str(), "mem[%ld] = %ld", &addr, &val))
        {
            task.SetMem(addr, val);
        }
        else
            expect(false);
    }

    return task.GetResult();
}

suite s = [] {
    "2020-14"_test = [] {
        {
            const char *const TEST =
                "mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X\n"
                "mem[8] = 11\n"
                "mem[7] = 101\n"
                "mem[8] = 0";
            expect(165_u == Initialize<Task1>(std::istringstream{TEST}));
        }

        Printer::Print(__FILE__, "1", Initialize<Task1>(std::ifstream{INPUT}));

        {
            const char *const TEST =
                "mask = 000000000000000000000000000000X1001X\n"
                "mem[42] = 100\n"
                "mask = 00000000000000000000000000000000X0XX\n"
                "mem[26] = 1";
            expect(208_u == Initialize<Task2>(std::istringstream{TEST}));
        }

        Printer::Print(__FILE__, "2", Initialize<Task2>(std::ifstream{INPUT}));
    };
};

} //namespace;
