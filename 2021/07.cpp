#include "../test.hpp"

namespace {

class Crabs
{
public:
    Crabs(std::istream &&is)
    {
        std::string line;
        getline(is, line);
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream iss(line);
        int n{};
        while (iss >> n)
            _positions.push_back(n);
        std::sort(_positions.begin(), _positions.end());
    }

    int CalcAlignment(int pos) const
    {
        return std::accumulate(_positions.begin(), _positions.end(), 0,
                [pos](int a, int p) { return a + std::abs(p - pos); });
    }

    int CalcAlignment2(int pos) const
    {
        return std::accumulate(_positions.begin(), _positions.end(), 0,
                [pos](int a, int p) {
                    auto d = std::abs(p - pos);
                    return a + d * (d + 1) / 2;
                });
    }

    int FindAlignment() const
    {
        return _Find([this](int pos) { return CalcAlignment(pos); });
    }

    int FindAlignment2() const
    {
        return _Find([this](int pos) { return CalcAlignment2(pos); });
    }

private:
    std::vector<int> _positions;

    int _Find(std::function<int(int)> alignment) const
    {
        int min_fuel = std::numeric_limits<int>::max();
        for (int pos = 0; pos != _positions.back(); ++pos)
        {
            auto fuel = alignment(pos);
            if (fuel < min_fuel)
                min_fuel = fuel;
        }
        return min_fuel;
    }
};

const char *TEST_INPUT = "16,1,2,0,4,2,7,1,2,14";

using namespace boost::ut;

suite s = [] {
    "2021-07"_test = [] {
        Crabs test(std::istringstream{TEST_INPUT});
        expect(41_i == test.CalcAlignment(1));
        expect(39_i == test.CalcAlignment(3));
        expect(71_i == test.CalcAlignment(10));
        expect(37_i == test.FindAlignment());
        expect(206_i == test.CalcAlignment2(2));
        expect(168_i == test.FindAlignment2());

        Crabs crabs(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", crabs.FindAlignment());
        Printer::Print(__FILE__, "2", crabs.FindAlignment2());
    };
};

} //namespace;
