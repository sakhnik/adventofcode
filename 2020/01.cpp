#include "../test.hpp"
#include <unordered_set>
#include <fstream>

namespace {

class Ledger
{
    std::unordered_set<int> _nums;

public:
    Ledger(std::istream &&is)
    {
        while (is)
        {
            int n{};
            is >> n;
            if (n)
                _nums.insert(n);
        }
    }

    unsigned Find1() const
    {
        for (auto x : _nums)
        {
            unsigned y = 2020 - x;
            if (_nums.find(y) != _nums.end())
            {
                return x * y;
            }
        }
        return 0;
    }

    unsigned Find2() const
    {
        for (auto x : _nums)
        {
            for (auto y : _nums)
            {
                if (x == y)
                    continue;
                auto z = 2020 - x - y;
                if (_nums.find(z) != _nums.end())
                {
                    return x * y * z;
                }
            }
        }
        return 0;
    }
};

using namespace boost::ut;

suite s = [] {
    "2020-01"_test = [] {
        {
            Ledger l{std::istringstream{"1721\n979\n366\n299\n675\n1456"}};
            expect(514579_u == l.Find1());
            expect(241861950_u == l.Find2());
        }

        {
            Ledger l{std::ifstream{INPUT}};
            Printer::Print(__FILE__, "1", l.Find1());
            Printer::Print(__FILE__, "2", l.Find2());
        }
    };
};

} //namespace;
