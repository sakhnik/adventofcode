#include "../test.hpp"
#include <algorithm>

namespace {

using namespace boost::ut;

struct Range
{
    int a, b;
    int c, d;

    bool Contains(int x) const
    {
        return (a <= x && x <= b) || (c <= x && x <= d);
    }
};

class Tickets
{
public:
    Tickets(std::istream &&is)
    {
        std::string line;
        while (getline(is, line) && !line.empty())
        {
            Range r;
            char field[64];
            int n = sscanf(line.c_str(), "%[^:]: %d-%d or %d-%d",
                           field, &r.a, &r.b, &r.c, &r.d);
            expect(n == 5);
            _ranges.push_back(r);
        }

        bool check_nearby = false;
        while (getline(is, line))
        {
            if (line.empty() || line.starts_with("your ticket:"))
                continue;

            if (line.starts_with("nearby tickets:"))
            {
                check_nearby = true;
                continue;
            }

            std::istringstream iss{line};
            for (size_t i = 0; i < _ranges.size(); ++i)
            {
                int v{};
                char comma{};
                iss >> v >> comma;
                if (check_nearby)
                {
                    auto it = std::find_if(_ranges.begin(), _ranges.end(),
                                           [v](const Range &r) {
                                           return r.Contains(v);
                                           });
                    if (it == _ranges.end())
                    {
                        _error += v;
                    }
                }
            }
        }
    }

    int GetError() const { return _error; }

private:
    std::vector<Range> _ranges;
    int _error = 0;
};

suite s = [] {
    "2020-16"_test = [] {
        {
            const char *const TEST =
                R"(class: 1-3 or 5-7
row: 6-11 or 33-44
seat: 13-40 or 45-50

your ticket:
7,1,14

nearby tickets:
7,3,47
40,4,50
55,2,20
38,6,12)";
            Tickets t{std::istringstream{TEST}};
            expect(71_i == t.GetError());
        }

        Tickets tickets{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", tickets.GetError());
    };
};

} //namespace;
