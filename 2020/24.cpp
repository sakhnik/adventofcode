#include "../test.hpp"
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <boost/functional/hash.hpp>

namespace {

struct Tile
{
    int x, y;

    bool operator==(const Tile &o) const
    {
        return x == o.x && y == o.y;
    }

    Tile operator+(const Tile &o) const
    {
        return {x + o.x, y + o.y};
    }
};

struct TileHash
{
    size_t operator()(const Tile &t) const noexcept
    {
        size_t seed{};
        boost::hash_combine(seed, t.x);
        boost::hash_combine(seed, t.y);
        return seed;
    }
};

Tile TracePath(const std::string &path)
{
    Tile cur{};
    int dy = 0;
    int dx = 0;

    for (char ch : path)
    {
        switch (ch)
        {
        case 'n':
            dy = -1;
            continue;
        case 's':
            dy = 1;
            continue;
        case 'w':
            switch (dy)
            {
            case -1:
                dx = 0;
                break;
            case 0:
            case 1:
                dx = -1;
                break;
            }
            break;
        case 'e':
            switch (dy)
            {
            case -1:
            case 0:
                dx = 1;
                break;
            case 1:
                dx = 0;
                break;
            }
            break;
        }

        cur.x += dx;
        cur.y += dy;
        dx = 0;
        dy = 0;
    }
    return cur;
}

class Floor
{
public:
    Floor(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            auto tile = TracePath(line);
            auto it = _blacks.find(tile);
            if (it == _blacks.end())
                _blacks.insert(tile);
            else
                _blacks.erase(it);
        }
    }

    void NextDay()
    {
        std::unordered_map<Tile, int, TileHash> counts;

        const std::array<Tile, 6> NEIGHBOURS{
            Tile{0, -1}, Tile{1, -1}, Tile{1, 0},
            Tile{0, 1}, Tile{-1, 1}, Tile{-1, 0},
        };

        for (const auto &b : _blacks)
        {
            for (const auto &n : NEIGHBOURS)
            {
                ++counts[b + n];
            }
        }

        decltype(_blacks) new_blacks;
        for (const auto &tc : counts)
        {
            if (_blacks.contains(tc.first))
            {
                if (tc.second == 1 || tc.second == 2)
                    new_blacks.insert(tc.first);
            }
            else
            {
                if (tc.second == 2)
                    new_blacks.insert(tc.first);
            }
        }

        _blacks.swap(new_blacks);
    }

    void SkipDays(int count)
    {
        while (count-- > 0)
            NextDay();
    }

    size_t GetCount() const { return _blacks.size(); }

private:
    std::unordered_set<Tile, TileHash> _blacks;
};

using namespace boost::ut;

suite s = [] {
    "2020-24"_test = [] {
        {
            expect(Tile{0, 1} == TracePath("esew"));

            const char *const TEST =
                "sesenwnenenewseeswwswswwnenewsewsw\n"
                "neeenesenwnwwswnenewnwwsewnenwseswesw\n"
                "seswneswswsenwwnwse\n"
                "nwnwneseeswswnenewneswwnewseswneseene\n"
                "swweswneswnenwsewnwneneseenw\n"
                "eesenwseswswnenwswnwnwsewwnwsene\n"
                "sewnenenenesenwsewnenwwwse\n"
                "wenwwweseeeweswwwnwwe\n"
                "wsweesenenewnwwnwsenewsenwwsesesenwne\n"
                "neeswseenwwswnwswswnw\n"
                "nenwswwsewswnenenewsenwsenwnesesenew\n"
                "enewnwewneswsewnwswenweswnenwsenwsw\n"
                "sweneswneswneneenwnewenewwneswswnese\n"
                "swwesenesewenwneswnwwneseswwne\n"
                "enesenwswwswneneswsenwnewswseenwsese\n"
                "wnwnesenesenenwwnenwsewesewsesesew\n"
                "nenewswnwewswnenesenwnesewesw\n"
                "eneswnwswnwsenenwnwnwwseeswneewsenese\n"
                "neswnwewnwnwseenwseesewsenwsweewe\n"
                "wseweeenwnesenwwwswnew\n";
            Floor f{std::istringstream{TEST}};
            expect(10_u == f.GetCount());
            f.NextDay();
            expect(15_u == f.GetCount());
            f.NextDay();
            expect(12_u == f.GetCount());
            f.NextDay();
            expect(25_u == f.GetCount());
            f.NextDay();
            expect(14_u == f.GetCount());
        }

        Floor f{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", f.GetCount());
        f.SkipDays(100);
        Printer::Print(__FILE__, "2", f.GetCount());
    };
};

} //namespace;
