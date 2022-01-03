#include "../test.hpp"
#include <vector>
#include <unordered_map>
#include <limits>

namespace {

class Game
{
public:
    Game(std::vector<int> start)
    {
        for (size_t turn = 0; turn != start.size(); ++turn)
        {
            _history[start[turn]] = _Info{turn, 0};
        }
        _turn = start.size();
        _recent = start.back();
    }

    int Run(size_t target)
    {
        while (_turn < target)
        {
            const auto &lookup = _history[_recent];
            _recent = lookup.interval;

            auto &new_info = _history[_recent];
            new_info.interval = new_info.prev_turn != _Info::NO_PREV ? _turn - new_info.prev_turn : 0;
            new_info.prev_turn = _turn;
            ++_turn;
        }
        return _recent;
    }

private:
    int _recent;
    size_t _turn;
    struct _Info
    {
        const static size_t NO_PREV = std::numeric_limits<size_t>::max();
        size_t prev_turn = NO_PREV;  // Most recent turn the number appeared
        int interval = 0;            // Interval to the previous recent turn
    };
    // number -> number info
    std::unordered_map<int, _Info> _history;
};

using namespace boost::ut;

suite s = [] {
    "2020-15"_test = [] {
        expect(436_i == Game{{0,3,6}}.Run(2020));
        expect(1_i == Game{{1,3,2}}.Run(2020));
        expect(10_i == Game{{2,1,3}}.Run(2020));
        expect(27_i == Game{{1,2,3}}.Run(2020));
        expect(78_i == Game{{2,3,1}}.Run(2020));
        expect(438_i == Game{{3,2,1}}.Run(2020));
        expect(1836_i == Game{{3,1,2}}.Run(2020));

        Printer::Print(__FILE__, "1", Game{{9,12,1,4,17,0,18}}.Run(2020));

        const int TARGET2 = 30000000;
        //expect(175594_i == Game{{0,3,6}}.Run(TARGET2));
        //expect(2578_i == Game{{1,3,2}}.Run(TARGET2));
        //expect(3544142_i == Game{{2,1,3}}.Run(TARGET2));
        //expect(261214_i == Game{{1,2,3}}.Run(TARGET2));
        //expect(6895259_i == Game{{2,3,1}}.Run(TARGET2));
        //expect(18_i == Game{{3,2,1}}.Run(TARGET2));
        //expect(362_i == Game{{3,1,2}}.Run(TARGET2));

        Printer::Print(__FILE__, "2", Game{{9,12,1,4,17,0,18}}.Run(TARGET2));
    };
};

} //namespace;
