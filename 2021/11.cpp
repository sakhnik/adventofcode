#include "../test.hpp"

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
            _field += line + '\n';
        }
        _field_to_step[_field] = _step;
        _step_to_field[_step] = _field;
        _flashes.push_back(0);
    }

    const std::string& Evolve()
    {
        size_t new_flashes{};

        auto idx = [&](int row, int col) {
            return row * (_width + 1) + col;
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

        auto it = _field_to_step.find(_field);
        if (it != _field_to_step.end())
        {
            _loop_start = it->second;
            return it->first;
        }
        _field_to_step[_field] = ++_step;
        _step_to_field[_step] = _field;
        _flashes.push_back(new_flashes + _flashes.back());
        return _field;
    }

    void FindLoop()
    {
        while (_loop_start == -1)
            Evolve();
    }

    int LoopStart() const { return _loop_start; }
    int LoopPeriod() const { return _flashes.size() - _loop_start; }

    size_t CountFlashes(int steps) const
    {
        if (steps < _loop_start)
            return _flashes[steps];
        size_t res{};
        int count = (steps - _loop_start) / LoopPeriod();
        int rem = (steps - _loop_start) % LoopPeriod();
        res += count * (_flashes.back() - _flashes[_loop_start]);
        res += _flashes[_loop_start + rem];
        return res;
    }

private:
    int _width{};
    std::string _field;
    int _step{};
    std::unordered_map<std::string, int> _field_to_step;
    std::unordered_map<int, std::string> _step_to_field;
    int _loop_start = -1;
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
    "6594254334\n"
    "3856965822\n"
    "6375667284\n"
    "7252447257\n"
    "7468496589\n"
    "5278635756\n"
    "3287952832\n"
    "7993992245\n"
    "5957959665\n"
    "6394862637\n";

const char *TEST2 =
    "8807476555\n"
    "5089087054\n"
    "8597889608\n"
    "8485769600\n"
    "8700908800\n"
    "6600088989\n"
    "6800005943\n"
    "0000007456\n"
    "9000000876\n"
    "8700006848\n";

const char *TEST3 =
    "0050900866\n"
    "8500800575\n"
    "9900000039\n"
    "9700000041\n"
    "9935080063\n"
    "7712300000\n"
    "7911250009\n"
    "2211130000\n"
    "0421125000\n"
    "0021119000\n";

using namespace boost::ut;

suite s = [] {
    "2021-11"_test = [] {
        Octopuses test{std::istringstream{TEST_INPUT}};
        expect(eq(std::string{TEST1}, test.Evolve()));
        expect(eq(std::string{TEST2}, test.Evolve()));
        expect(eq(std::string{TEST3}, test.Evolve()));
        test.FindLoop();
        expect(0_u == test.CountFlashes(0));
        expect(0_u == test.CountFlashes(1));
        expect(35_u == test.CountFlashes(2));
        expect(204_u == test.CountFlashes(10));

        Octopuses octopuses{std::ifstream{INPUT}};
        octopuses.FindLoop();
        Printer::Print(__FILE__, "1", octopuses.CountFlashes(100));
    };
};

} //namespace;
