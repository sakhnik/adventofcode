#include "../test.hpp"

namespace {

class Origami
{
public:
    Origami(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            if (line.empty())
                break;
            _PosT pos{};
            sscanf(line.c_str(), "%d,%d", &pos.second, &pos.first);
            _dots.insert(pos);
        }

        while (getline(is, line))
        {
            char axis{};
            int val{};
            if (2 == sscanf(line.c_str(), "fold along %c=%d", &axis, &val))
            {
                for (auto it = _dots.begin(); it != _dots.end(); )
                {
                    if (axis == 'y')
                    {
                        if (it->first > val)
                        {
                            _dots.insert({2 * val - it->first, it->second});
                            it = _dots.erase(it);
                            continue;
                        }
                    }
                    else if (axis == 'x')
                    {
                        if (it->second > val)
                        {
                            _dots.insert({it->first, 2 * val - it->second});
                            it = _dots.erase(it);
                            continue;
                        }
                    }
                    ++it;
                }

                if (!_count1)
                    _count1 = _dots.size();
            }
        }
    }

    size_t GetCount1() const { return _count1; }

    std::string Print() const
    {
        std::ostringstream oss;

        int max_col{0};
        for (const auto &pos : _dots)
        {
            if (pos.second > max_col)
                max_col = pos.second;
        }

        for (int row = 0; row <= _dots.rbegin()->first; ++row)
        {
            for (int col = 0; col <= max_col; ++col)
                oss << (_dots.contains({row, col}) ? '#' : '.');
            oss << "\n";
        }
        return oss.str();
    }

private:
    using _PosT = std::pair<int, int>;
    std::set<_PosT> _dots;
    size_t _count1 = 0;
};

const char *TEST_INPUT = R"(6,10
0,14
9,10
0,3
10,4
4,11
6,0
6,12
4,1
0,13
10,12
3,4
3,0
8,4
1,10
2,14
8,10
9,0

fold along y=7
fold along x=5
)";

using namespace boost::ut;

suite s = [] {
    "2021-13"_test = [] {
        Origami test{std::istringstream{TEST_INPUT}};
        expect(17_u == test.GetCount1());

        Origami origami{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", origami.GetCount1());
        Printer::Print(__FILE__, "2", "\n" + origami.Print());
    };
};

} //namespace;
