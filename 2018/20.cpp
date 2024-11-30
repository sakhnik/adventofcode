#include <sstream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include "../test.hpp"

namespace {

class Map
{
public:
    Map(std::istream &&is)
    {
        std::string line;
        getline(is, line);

        _map[{0,0}] = {0};
        const char *s = line.data();
        _Search({0,0}, s);
    }

    int GetFartherst() const
    {
        auto it = std::max_element(begin(_map), end(_map),
                                   [](const auto &a, const auto &b) {
                                        return a.second.dist < b.second.dist;
                                   });
        return it->second.dist;
    }

    int CountRooms1000DoorsAway() const
    {
        return std::count_if(begin(_map), end(_map),
                             [](const auto &en) {
                                return en.second.dist >= 1000;
                             });
    }

private:
    struct _Data
    {
        int dist;
    };
    struct _Pos
    {
        int x, y;
        bool operator==(const _Pos &o) const { return x == o.x && y == o.y; }
    };
    static constexpr auto _posHash = [](const _Pos &p) {
        return std::hash<int>()((p.x << 16) | p.y);
    };
    std::unordered_map<_Pos, _Data, decltype(_posHash)> _map{ 0, _posHash };

    void _Search(_Pos cur, const char *&path)
    {
        _Pos start = cur;  // remember for taking alternatives

        auto track = [&](int dx, int dy) {
            auto it = _map.find(cur);
            assert(it != end(_map));
            cur.x += dx;
            cur.y += dy;
            auto it2 = _map.find(cur);
            if (it2 == end(_map))
            {
                _map.insert({cur, {it->second.dist + 1}});
            }
        };

        while (char ch = *path++)
        {
            switch (ch)
            {
            case 'E': track(1, 0); break;
            case 'S': track(0, 1); break;
            case 'W': track(-1, 0); break;
            case 'N': track(0, -1); break;
            case '(':
                _Search(cur, path);
                break;
            case '|':
                cur = start;
                break;
            case ')':
            case '$':
                return;
            }
        }
    }
};

using namespace boost::ut;

suite s = [] {
    "20"_test = [] {
        {
            Map m(std::istringstream{"^ESSWWN(E|NNENN(EESS(WNSE|)SSS|WWWSSSSE(SW|NNNE)))$"});
            expect(23_i == m.GetFartherst());
        }

        {
            Map m(std::ifstream{INPUT});
            Printer::Print(__FILE__, "1", m.GetFartherst());
            Printer::Print(__FILE__, "2", m.CountRooms1000DoorsAway());
        }
    };
};

} //namespace;
