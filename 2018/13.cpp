#include <doctest/doctest.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>

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
                REQUIRE(_width == line.length());
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
                    INFO(ch);
                    REQUIRE(!"Incorrect char");
                }
            }
            ++row;
        }
    }

    size_t GetCartCount() const
    {
        return _carts.size();
    }

    struct Collision
    {
        int row, col;

        std::string Dump() const
        {
            std::ostringstream oss;
            oss << col << "," << row;
            return oss.str();
        }
    };

    Collision Simulate()
    {
        std::vector<bool> cart_flags(_map.size(), false);
        for (auto &cart : _carts)
        {
            cart_flags[_Idx(cart.row, cart.col)] = true;
        }

        while (true)
        {
            std::sort(begin(_carts), end(_carts));
            for (auto &cart : _carts)
            {
                cart_flags[_Idx(cart.row, cart.col)] = false;
                cart.Advance();
                auto idx = _Idx(cart.row, cart.col);
                if (cart_flags[idx])
                {
                    return {cart.row, cart.col};
                }
                else
                {
                    cart_flags[idx] = true;
                }
                cart.CheckForTurn(_map[idx]);
            }
        }
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

        void Advance()
        {
            switch (dir)
            {
            case '^': --row; break;
            case '>': ++col; break;
            case 'v': ++row; break;
            case '<': --col; break;
            }
        }

        void CheckForTurn(char place)
        {
            switch (place)
            {
            case '/':
                // follow the curve
                dir = ">v<^"[std::string{"^<v>"}.find(dir)];
                break;
            case '\\':
                // follow the curve
                dir = ">v<^"[std::string{"v>^<"}.find(dir)];
                break;
            case '+':
                phase = (phase + 1) % 3;
                switch (phase)
                {
                case 0:
                    // turn left
                    dir = ">v<^"[std::string{"v<^>"}.find(dir)];
                    break;
                case 1:
                    // straight
                    break;
                case 2:
                    dir = ">v<^"[std::string{"^>v<"}.find(dir)];
                    // turn right
                    break;
                }
            }
        }
    };
    std::vector<_Cart> _carts;

    size_t _Idx(int row, int col) const
    {
        return row * _width + col;
    }

    void _AdvanceCart(_Cart &cart)
    {
        auto idx = _Idx(cart.row, cart.col);
        switch (_map[idx])
        {
        case '.': _Straight(cart); break;
        }
    }

    void _Straight(_Cart &cart)
    {
    }
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test1") {
        Map m(std::istringstream{"|\nv\n|\n|\n|\n^\n|"});
        REQUIRE(2 == m.GetCartCount());
        REQUIRE("0,3" == m.Simulate().Dump());
    }

    SUBCASE("test2") {
        Map m(std::istringstream{"/->-\\        \n|   |  /----\\\n| /-+--+-\\  |\n| | |  | v  |\n\\-+-/  \\-+--/\n  \\------/   \n"});
        REQUIRE(2 == m.GetCartCount());
        REQUIRE("7,3" == m.Simulate().Dump());
    }

    SUBCASE("task") {
        Map m(std::ifstream(INPUT));
        MESSAGE(m.Simulate().Dump());
    }
}
