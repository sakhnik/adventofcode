#include <sstream>
#include <fstream>
#include <algorithm>
#include "../test.hpp"

namespace {

class Plants
{
public:
    Plants(std::istream &&is)
    {
        std::string initial, state;
        is >> initial >> state >> _pots;

        std::string config, arrow, result;
        while (is && (is >> config >> arrow >> result))
        {
            _rules[_Parse(config.c_str())] = _GetVal(result.front());
        }
    }

    void Evolve(int64_t steps)
    {
        for (int64_t i = 0; i < steps; ++i)
        {
            _DoEvolve();

            if (_pots == _prev_pots)
            {
                // Reached a stable pattern. No need to cycle anymore.
                int64_t dn = _initial_num - _prev_initial_num;
                _initial_num += dn * (steps - i - 1);
                _prev_initial_num = _initial_num - dn;
                return;
            }
        }
    }

    const std::string& GetPots() const
    {
        return _pots;
    }

    int64_t GetInitialNum() const
    {
        return _initial_num;
    }

    int64_t CalcPositionsSum() const
    {
        auto idx = _initial_num;
        int64_t sum = 0;

        for (auto ch : _pots)
        {
            if (ch == '#')
                sum += idx;
            ++idx;
        }

        return sum;
    }

private:
    int64_t _initial_num = 0;
    int64_t _prev_initial_num = 0;
    std::string _pots;
    std::string _prev_pots;
    bool _rules[1 << 5] = { 0 };

    uint8_t _Parse(const char *s)
    {
        uint8_t ret{0};
        for (int i = 0; i < 5; ++i, ++s)
        {
            ret <<= 1;
            ret |= _GetVal(*s);
        }
        return ret;
    }

    uint8_t _GetVal(char pot)
    {
        return pot == '#' ? 1 : 0;
    }

    void _DoEvolve()
    {
        _prev_pots.clear();
        int64_t idx = _initial_num - 3;

        auto applyRules = [&](auto config) {
            auto res = _rules[config];
            if (res)
            {
                if (_prev_pots.empty())
                {
                    _prev_initial_num = _initial_num;
                    _initial_num = idx;
                }
                _prev_pots.push_back('#');
            }
            else
            {
                if (!_prev_pots.empty())
                {
                    _prev_pots.push_back('.');
                }
            }
            ++idx;
        };

        // Shift to the the next 5 pots
        auto shift = [](auto config, uint8_t new_bit) {
            return ((config << 1) & 0x1f) | new_bit;
        };

        uint8_t cur_config{0};
        applyRules(cur_config);

        for (char ch : _pots)
        {
            cur_config = shift(cur_config, _GetVal(ch));
            applyRules(cur_config);
        }

        while (cur_config)
        {
            cur_config = shift(cur_config, 0);
            applyRules(cur_config);
        }

        while (_prev_pots.back() == '.')
        {
            _prev_pots.pop_back();
        }

        _pots.swap(_prev_pots);
    }
};

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "12"_test = [] {
        const auto TEST_INPUT = R"(initial state: #..#.#..##......###...###

...## => #
..#.. => #
.#... => #
.#.#. => #
.#.## => #
.##.. => #
.#### => #
#.#.# => #
#.### => #
##.#. => #
##.## => #
###.. => #
###.# => #
####. => #)";
        {
            Plants plants(std::istringstream{TEST_INPUT});
            expect(eq("#..#.#..##......###...###"s, plants.GetPots()));
            plants.Evolve(1);
            expect(eq("#...#....#.....#..#..#..#"s, plants.GetPots()));
            plants.Evolve(1);
            expect(eq("##..##...##....#..#..#..##"s, plants.GetPots()));
            plants.Evolve(1);
            expect(eq("#.#...#..#.#....#..#..#...#"s, plants.GetPots()));
            plants.Evolve(17);
            expect(eq("#....##....#####...#######....#.#..##"s, plants.GetPots()));
            expect(-2_i == plants.GetInitialNum());
            expect(325_i == plants.CalcPositionsSum());
        }

        Plants plants(std::ifstream{INPUT});
        plants.Evolve(20);
        Printer::Print(__FILE__, "1", plants.CalcPositionsSum());
        plants.Evolve(50000000000 - 20);
        Printer::Print(__FILE__, "2", plants.CalcPositionsSum());
    };
};

} //namespace;
