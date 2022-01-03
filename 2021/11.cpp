#include "../test.hpp"
#include <unordered_map>
#include <fstream>
#include <functional>

namespace {

class Octopuses
{
public:
    Octopuses(std::istream &&is)
    {
        std::string line;
        while (is >> line)
        {
            _width = line.size();
            _field += line;
        }
        _field_to_step[_field] = _step;
        _step_to_field[_step] = _field;
        _flashes.push_back(0);
    }

    const std::string& Evolve()
    {
        size_t new_flashes{};

        auto idx = [&](int row, int col) {
            return row * _width + col;
        };

        std::function<void(int, int)> flash;

        auto light = [&](int row, int col) {
            if (row >= 0 && row < _width && col >= 0 && col < _width)
            {
                if (_field[idx(row, col)] != '0')
                {
                    ++_field[idx(row, col)];
                    flash(row, col);
                }
            }
        };

        flash = [&](int row, int col) {
            if (_field[idx(row, col)] > '9')
            {
                ++new_flashes;
                _field[idx(row, col)] = '0';
                for (int dx = -1; dx <= 1; ++dx)
                {
                    for (int dy = -1; dy <= 1; ++dy)
                    {
                        if (dx || dy)
                            light(row + dy, col + dx);
                    }
                }
            }
        };

        for (int row = 0; row < _width; ++row)
        {
            for (int col = 0; col < _width; ++col)
            {
                ++_field[idx(row, col)];
            }
        }

        for (int row = 0; row < _width; ++row)
        {
            for (int col = 0; col < _width; ++col)
            {
                flash(row, col);
            }
        }

        _field_to_step[_field] = ++_step;
        _step_to_field[_step] = _field;
        _flashes.push_back(new_flashes + _flashes.back());
        return _field;
    }

    const std::string& Evolve(int count)
    {
        while (count-- > 0)
            Evolve();
        return _step_to_field.at(_step);
    }

    size_t CountFlashes(int steps) const
    {
        return _flashes[steps];
    }

    int GetStep() const { return _step; }
    const std::string& GetField(int step) const { return _step_to_field.at(step); }

    bool IsAllFlash() const
    {
        const auto &f = _step_to_field.at(_step);
        return f.find_first_not_of("0") == f.npos;
    }

private:
    int _width{};
    std::string _field;
    int _step{};
    std::unordered_map<std::string, int> _field_to_step;
    std::unordered_map<int, std::string> _step_to_field;
    std::vector<size_t> _flashes;
};

const char *TEST_INPUT = R"(
5483143223
2745854711
5264556173
6141336146
6357385478
4167524645
2176841721
6882881134
4846848554
5283751526
)";

const char *TEST1 =
    "6594254334"
    "3856965822"
    "6375667284"
    "7252447257"
    "7468496589"
    "5278635756"
    "3287952832"
    "7993992245"
    "5957959665"
    "6394862637";

const char *TEST2 =
    "8807476555"
    "5089087054"
    "8597889608"
    "8485769600"
    "8700908800"
    "6600088989"
    "6800005943"
    "0000007456"
    "9000000876"
    "8700006848";

const char *TEST3 =
    "0050900866"
    "8500800575"
    "9900000039"
    "9700000041"
    "9935080063"
    "7712300000"
    "7911250009"
    "2211130000"
    "0421125000"
    "0021119000";

const char *TEST193 =
    "5877777777"
    "8877777777"
    "7777777777"
    "7777777777"
    "7777777777"
    "7777777777"
    "7777777777"
    "7777777777"
    "7777777777"
    "7777777777";

const char *TEST194 =
    "6988888888"
    "9988888888"
    "8888888888"
    "8888888888"
    "8888888888"
    "8888888888"
    "8888888888"
    "8888888888"
    "8888888888"
    "8888888888";

using namespace boost::ut;

suite s = [] {
    "2021-11"_test = [] {
        Octopuses test{std::istringstream{TEST_INPUT}};
        expect(eq(std::string{TEST1}, test.Evolve()));
        expect(eq(std::string{TEST2}, test.Evolve()));
        expect(eq(std::string{TEST3}, test.Evolve()));
        test.Evolve(100);
        expect(0_u == test.CountFlashes(0));
        expect(0_u == test.CountFlashes(1));
        expect(35_u == test.CountFlashes(2));
        expect(204_u == test.CountFlashes(10));
        expect(1656_u == test.CountFlashes(100));
        while (!test.IsAllFlash())
            test.Evolve();
        expect(195_i == test.GetStep());
        expect(eq(std::string{TEST193}, test.GetField(193)));
        expect(eq(std::string{TEST194}, test.GetField(194)));

        Octopuses octopuses{std::ifstream{INPUT}};
        octopuses.Evolve(100);
        Printer::Print(__FILE__, "1", octopuses.CountFlashes(100));
        while (!octopuses.IsAllFlash())
            octopuses.Evolve();
        Printer::Print(__FILE__, "2", octopuses.GetStep());
    };
};

} //namespace;
