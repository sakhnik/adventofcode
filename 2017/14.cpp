#include <bitset>
#include <queue>
#include <algorithm>
#include "KnotHash.h"
#include "../test.hpp"

namespace {

unsigned CountBits(const std::string &key)
{
    unsigned count{0};
    for (unsigned i = 0; i < 128; ++i)
    {
        auto h = KnotHash(key + '-' + std::to_string(i));
        for (char x : h)
            count += std::bitset<8>(x).count();
    }
    return count;
}

std::string HexToBin(uint8_t d)
{
    std::string res;
    std::bitset<8> b(d);
    for (unsigned k = 0; k < 8; ++k)
        res.push_back('0' + b.test(7 - k));
    return res;
}

unsigned CountRegions(const std::string &key)
{
    std::vector<uint8_t> space;
    for (unsigned i = 0; i < 128; ++i)
    {
        auto h = KnotHash(key + '-' + std::to_string(i));
        for (auto d : h)
        {
            auto bin = HexToBin(d);
            space.insert(space.end(), bin.begin(), bin.end());
        }
    }

    // Let's implement some BFS
    unsigned count{0};
    unsigned labels[128 * 128] = {};
    std::queue<unsigned> to_discover;

    auto enqueue = [&](unsigned idx) {
        if (space[idx] == '1' && !labels[idx])
        {
            labels[idx] = count;
            to_discover.push(idx);
        }
    };

    // Find the next set bit, which is not marked
    for (unsigned i = 0; i < space.size(); ++i)
    {
        if (space[i] == '1' && !labels[i])
        {
            ++count;
            enqueue(i);
            while (!to_discover.empty())
            {
                unsigned i = to_discover.front();
                to_discover.pop();

                int row = i / 128;
                int col = i % 128;
                if (col > 0)
                    enqueue(i - 1);
                if (row > 0)
                    enqueue(i - 128);
                if (col < 127)
                    enqueue(i + 1);
                if (row < 127)
                    enqueue(i + 128);
            }
        }
    }

    return count;
}

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "14"_test = [] {
        expect(eq(HexToBin(0xa0), "10100000"s));

        expect(8108_u == CountBits("flqrgnkx"));
        expect(1242_u == CountRegions("flqrgnkx"));

        Printer::Print(__FILE__, "1", CountBits("ffayrhll"));
        Printer::Print(__FILE__, "2", CountRegions("ffayrhll"));
    };
};

} //namespace;
