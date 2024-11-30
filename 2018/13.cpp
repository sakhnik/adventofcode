#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <functional>
#include <list>
#include <cassert>
#include "../test.hpp"

namespace {

class Map
{
public:
    Map(std::istream &&is)
    {
        int row = 0;
        std::string line;
        while (is && getline(is, line))
        {
            if (_map.empty())
            {
                _width = line.length();
            }
            else
            {
                assert(_width == line.length());
            }

            for (int col = 0, colN = line.size(); col < colN; ++col)
            {
                switch (char ch = line[col])
                {
                case ' ':
                case '\\':
                case '/':
                case '+':
                    _map.push_back(ch);
                    break;
                case '-':
                case '|':
                    _map.push_back(ch);
                    break;
                case '^':
                    _map.push_back('|');
                    _carts.push_back({row, col, ch});
                    break;
                case '>':
                    _map.push_back('-');
                    _carts.push_back({row, col, ch});
                    break;
                case 'v':
                    _map.push_back('|');
                    _carts.push_back({row, col, ch});
                    break;
                case '<':
                    _map.push_back('-');
                    _carts.push_back({row, col, ch});
                    break;
                default:
                    boost::ut::expect(false) << "Incorrect char: " << ch;
                }
            }
            ++row;
        }
    }

    size_t GetCartCount() const
    {
        return _carts.size();
    }

    struct Position
    {
        int row, col;

        std::string Dump() const
        {
            std::ostringstream oss;
            oss << col << "," << row;
            return oss.str();
        }
    };

    Position RunUntilFirstCollision()
    {
        Position ret;
        auto capture = [&ret](int row, int col) {
            ret = Position{row, col};
            return true;
        };
        _Simulate(capture);
        return ret;
    }

    Position RunUntilLastCollision()
    {
        auto ignore = [](int, int) {
            return false;
        };
        _Simulate(ignore);
        boost::ut::expect(size(_carts) > 0);
        const auto cart = _carts.front();
        return {cart.row, cart.col};
    }

private:
    size_t _width = 0;
    std::vector<char> _map;

    struct _Cart
    {
        int row;
        int col;
        char dir;
        int phase = -1;

        bool operator<(const _Cart &o) const
        {
            if (row < o.row)
                return true;
            if (row > o.row)
                return false;
            return col < o.col;
        }

        void Advance(char place)
        {
            switch (dir)
            {
            case '^':
                assert(place != '-');
                assert(place != ' ');
                --row;
                break;
            case '>':
                assert(place != '|');
                assert(place != ' ');
                ++col;
                break;
            case 'v':
                assert(place != '-');
                assert(place != ' ');
                ++row;
                break;
            case '<':
                assert(place != '|');
                assert(place != ' ');
                --col;
                break;
            default:
                assert(!"Incorrect direction");
            }
        }

        void CheckForTurn(char place)
        {
            auto turn = [](char d, std::string_view cur, std::string_view next) {
                return next[cur.find(d)];
            };

            switch (place)
            {
            case '/':
                // follow the curve
                dir = turn(dir, "^<v>", ">v<^");
                break;
            case '\\':
                // follow the curve
                dir = turn(dir, "v>^<", ">v<^");
                break;
            case '+':
                phase = (phase + 1) % 3;
                switch (phase)
                {
                case 0:
                    // turn left
                    dir = turn(dir, "v<^>", ">v<^");
                    break;
                case 1:
                    // straight
                    break;
                case 2:
                    // turn right
                    dir = turn(dir, "^>v<", ">v<^");
                    break;
                }
            }
        }
    };

    std::list<_Cart> _carts;

    size_t _Idx(int row, int col) const
    {
        assert(col < static_cast<int>(_width));
        return row * _width + col;
    }

    void _Simulate(std::function<bool(int, int)> on_collision)
    {
        std::vector<decltype(_carts.begin())> cart_flags(_map.size(), _carts.end());
        for (auto it = begin(_carts); it != end(_carts); ++it)
        {
            cart_flags[_Idx(it->row, it->col)] = it;
        }

        while (_carts.size() > 1)
        {
            _carts.sort();
            for (auto it = begin(_carts); it != end(_carts); ++it)
            {
                auto idx = _Idx(it->row, it->col);
                cart_flags[idx] = end(_carts);
                it->Advance(_map[idx]);
                idx = _Idx(it->row, it->col);
                it->CheckForTurn(_map[idx]);
                auto it2 = cart_flags[idx];
                if (it2 != end(_carts))
                {
                    if (on_collision(it->row, it->col))
                    {
                        return;
                    }
                    cart_flags[_Idx(it2->row, it2->col)] = end(_carts);
                    _carts.erase(it2);
                    it = _carts.erase(it);
                    --it;
                }
                else
                {
                    cart_flags[idx] = it;
                }
            }
        }
    }
};

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "13"_test = [] {
        {
            Map m(std::istringstream{
                "|\n"
                "v\n"
                "|\n"
                "|\n"
                "|\n"
                "^\n"
                "|\n"});
            expect(2_u == m.GetCartCount());
            expect(eq("0,3"s, m.RunUntilFirstCollision().Dump()));
        }

        {
            Map m(std::istringstream{
                R"(/->-\        )"
                "\n"
                R"(|   |  /----\)"
                "\n"
                R"(| /-+--+-\  |)"
                "\n"
                R"(| | |  | v  |)"
                "\n"
                R"(\-+-/  \-+--/)"
                "\n"
                R"(  \------/   )"
                "\n"});
            expect(2_u == m.GetCartCount());
            expect(eq("7,3"s, m.RunUntilFirstCollision().Dump()));
        }

        {
            Map m(std::ifstream(INPUT));
            Printer::Print(__FILE__, "1", m.RunUntilFirstCollision().Dump());
        }

        {
            Map m(std::istringstream{
                R"(/>-<\  )"
                "\n"
                R"(|   |  )"
                "\n"
                R"(| /<+-\)"
                "\n"
                R"(| | | v)"
                "\n"
                R"(\>+</ |)"
                "\n"
                R"(  |   ^)"
                "\n"
                R"(  \<->/)"
                "\n"});
            expect(eq("6,4"s, m.RunUntilLastCollision().Dump()));
        }

        {
            Map m(std::ifstream(INPUT));
            Printer::Print(__FILE__, "2", m.RunUntilLastCollision().Dump());
        }
    };
};

} //namespace;
