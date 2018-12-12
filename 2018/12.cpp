#include <doctest/doctest.h>
#include <sstream>
#include <fstream>
#include <algorithm>

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

    void Evolve()
    {
        std::string new_pots;
        int idx = _initial_num - 3;

        auto applyRules = [&](auto config) {
            auto res = _rules[config];
            if (res)
            {
                if (new_pots.empty())
                {
                    _initial_num = idx;
                }
                new_pots.push_back('#');
            }
            else
            {
                if (!new_pots.empty())
                {
                    new_pots.push_back('.');
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

        while (new_pots.back() == '.')
        {
            new_pots.pop_back();
        }

        _pots.swap(new_pots);
    }

    const std::string& GetPots() const
    {
        return _pots;
    }

    int GetInitialNum() const
    {
        return _initial_num;
    }

    int CalcPositionsSum() const
    {
        auto idx = _initial_num;
        auto sum = 0;

        for (auto ch : _pots)
        {
            if (ch == '#')
                sum += idx;
            ++idx;
        }

        return sum;
    }

private:
    int _initial_num = 0;
    std::string _pots;
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
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
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
        Plants plants(std::istringstream{TEST_INPUT});
        REQUIRE("#..#.#..##......###...###" == plants.GetPots());
        plants.Evolve();
        REQUIRE("#...#....#.....#..#..#..#" == plants.GetPots());
        plants.Evolve();
        REQUIRE("##..##...##....#..#..#..##" == plants.GetPots());
        plants.Evolve();
        REQUIRE("#.#...#..#.#....#..#..#...#" == plants.GetPots());
        for (int i = 0; i < 17; ++i)
        {
            plants.Evolve();
        }
        REQUIRE("#....##....#####...#######....#.#..##" == plants.GetPots());
        REQUIRE(-2 == plants.GetInitialNum());
        REQUIRE(325 == plants.CalcPositionsSum());
    }

    SUBCASE("task1") {
        Plants plants(std::ifstream{INPUT});
        for (int i = 0; i < 20; ++i)
        {
            plants.Evolve();
        }
        MESSAGE(plants.CalcPositionsSum());
    }
}
