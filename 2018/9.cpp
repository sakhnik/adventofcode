#include <doctest/doctest.h>
#include <vector>
#include <algorithm>
#include <list>
#include <sstream>

namespace {

class Circle
{
public:
    Circle(int num_players, int num_marbles)
        : _players(num_players)
        , _num_marbles(num_marbles)
    {
    }

    uint64_t Play()
    {
        for (int i = 0; i < _num_marbles; ++i)
        {
            Step();
        }
        return *std::max_element(begin(_players), end(_players));
    }

    void Step()
    {
        ++_marble;
        if (_marble % 23 == 0)
        {
            _players[_player] += _marble;
            for (int i = 0; i < 7; ++i)
                _cur = _AdvancePrev();
            _players[_player] += *_cur;
            _cur = _marbles.erase(_cur);
        }
        else
        {
            _cur = _AdvanceNext();
            _cur = _marbles.insert(++_cur, _marble);
        }
        ++_player;
        _player %= _players.size();
    }

    std::string DumpMarbles() const
    {
        std::ostringstream oss;
        for (auto it = begin(_marbles); it != end(_marbles); ++it)
        {
            oss << " ";
            if (it == _cur)
                oss << "(";
            oss << *it;
            if (it == _cur)
                oss << ")";
        }
        return oss.str();
    }

private:
    int _player = 0;
    std::vector<uint64_t> _players;
    int _num_marbles;
    int _marble = 0;
    using _MarblesT = std::list<int>;
    _MarblesT _marbles{ 0 };
    _MarblesT::iterator _cur = begin(_marbles);

    _MarblesT::iterator _AdvanceNext()
    {
        auto it = ++_cur;
        if (it == end(_marbles))
            it = begin(_marbles);
        return it;
    }

    _MarblesT::iterator _AdvancePrev()
    {
        auto it = _cur;
        if (it == begin(_marbles))
            it = end(_marbles);
        return --it;
    }
};

uint64_t Play(int num_players, int num_marbles)
{
    Circle c(num_players, num_marbles);
    return c.Play();
}

} //namespace;

TEST_CASE(TEST_NAME)
{

    SUBCASE("mov") {
        Circle c(9, 25);
        REQUIRE(" (0)" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 (1)" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 (2) 1" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 2 1 (3)" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 (4) 2 1 3" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 4 2 (5) 1 3" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 4 2 5 1 (6) 3" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 4 2 5 1 6 3 (7)" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 (8) 4 2 5 1 6 3 7" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 8 4 (9) 2 5 1 6 3 7" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 8 4 9 2 (10) 5 1 6 3 7" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 8 4 9 2 10 5 (11) 1 6 3 7" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 8 4 9 2 10 5 11 1 (12) 6 3 7" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 8 4 9 2 10 5 11 1 12 6 (13) 3 7" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 8 4 9 2 10 5 11 1 12 6 13 3 (14) 7" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 8 4 9 2 10 5 11 1 12 6 13 3 14 7 (15)" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 (16) 8 4 9 2 10 5 11 1 12 6 13 3 14 7 15" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 16 8 (17) 4 9 2 10 5 11 1 12 6 13 3 14 7 15" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 16 8 17 4 (18) 9 2 10 5 11 1 12 6 13 3 14 7 15" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 16 8 17 4 18 9 (19) 2 10 5 11 1 12 6 13 3 14 7 15" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 16 8 17 4 18 9 19 2 (20) 10 5 11 1 12 6 13 3 14 7 15" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 16 8 17 4 18 9 19 2 20 10 (21) 5 11 1 12 6 13 3 14 7 15" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 16 8 17 4 18 9 19 2 20 10 21 5 (22) 11 1 12 6 13 3 14 7 15" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 16 8 17 4 18 (19) 2 20 10 21 5 22 11 1 12 6 13 3 14 7 15" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 16 8 17 4 18 19 2 (24) 20 10 21 5 22 11 1 12 6 13 3 14 7 15" == c.DumpMarbles());
        c.Step();
        REQUIRE(" 0 16 8 17 4 18 19 2 24 20 (25) 10 21 5 22 11 1 12 6 13 3 14 7 15" == c.DumpMarbles());
    }

    SUBCASE("play") {
        REQUIRE(32 == Play(9, 25));
        REQUIRE(8317 == Play(10, 1618));
        REQUIRE(146373 == Play(13, 7999));
        REQUIRE(2764 == Play(17, 1104));
        REQUIRE(54718 == Play(21, 6111));
        REQUIRE(37305 == Play(30, 5807));
    }

    SUBCASE("task1") {
        MESSAGE(Play(459, 71320));
    }

    SUBCASE("task2") {
        MESSAGE(Play(459, 71320 * 100));
    }
}
