#include <fstream>
#include <sstream>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include "../test.hpp"
namespace {

class Table
{
public:
    Table(std::istream &&is)
    {
        std::string line;
        while (is && getline(is, line))
        {
            char a[64], b[64], effect[64];
            int amount{};

            sscanf(line.c_str(), "%s would %s %d happiness units by sitting next to %s.", a, effect, &amount, b);
            int i = _GetNameIdx(a);
            int j = _GetNameIdx(b);
            if (std::string_view{effect} == "lose")
                amount *= -1;
            _points[i][j] = amount;
        }

        for (size_t i = 0; i < _names.size(); ++i)
            _points[i][i] = 0;
    }

    void AddMyself()
    {
        _GetNameIdx("Me");
        for (size_t i = 0; i < _names.size(); ++i)
        {
            _points[i][_names.size() - 1] = 0;
            _points[_names.size() - 1][i] = 0;
        }
    }

    void Print() const
    {
        for (size_t i = 0; i < _names.size(); ++i)
        {
            for (size_t j = 0; j < _names.size(); ++j)
            {
                printf("%4d", _points[i][j]);
            }
            printf("\n");
        }
    }

    int Arrange() const
    {
        int idx[_names.size()];
        std::iota(idx, idx + _names.size(), 0);

        auto count = [this](int idx[]) {
            int res{};
            for (size_t i = 0; i < _names.size(); ++i)
            {
                size_t j = (i + _names.size() + 1) % _names.size();
                res += _points[idx[i]][idx[j]];
                res += _points[idx[j]][idx[i]];
            }
            return res;
        };

        int max_points = std::numeric_limits<int>::min();
        while (std::next_permutation(idx, idx + _names.size()))
        {
            auto c = count(idx);
            if (c > max_points)
            {
                max_points = c;
            }
        }

        return max_points;
    }

private:
    std::unordered_map<std::string, int> _names;
    int _points[10][10];

    int _GetNameIdx(std::string name)
    {
        while (name.back() == '.')
            name.pop_back();

        auto it = _names.find(name);
        if (it != _names.end())
        {
            return it->second;
        }
        return _names[name] = _names.size();
    }
};

using namespace boost::ut;

suite s = [] {
    "13.test"_test = [] {
        const char *const TEST =
            "Alice would gain 54 happiness units by sitting next to Bob.\n"
            "Alice would lose 79 happiness units by sitting next to Carol.\n"
            "Alice would lose 2 happiness units by sitting next to David.\n"
            "Bob would gain 83 happiness units by sitting next to Alice.\n"
            "Bob would lose 7 happiness units by sitting next to Carol.\n"
            "Bob would lose 63 happiness units by sitting next to David.\n"
            "Carol would lose 62 happiness units by sitting next to Alice.\n"
            "Carol would gain 60 happiness units by sitting next to Bob.\n"
            "Carol would gain 55 happiness units by sitting next to David.\n"
            "David would gain 46 happiness units by sitting next to Alice.\n"
            "David would lose 7 happiness units by sitting next to Bob.\n"
            "David would gain 41 happiness units by sitting next to Carol.\n";
        Table t(std::istringstream{TEST});
        expect(330_i == t.Arrange());
    };

    "13.task"_test = [] {
        Table t(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", t.Arrange());
        t.AddMyself();
        Printer::Print(__FILE__, "2", t.Arrange());
    };
};

} //namespace;
