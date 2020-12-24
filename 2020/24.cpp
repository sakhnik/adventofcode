#include "../test.hpp"
#include <unordered_set>
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

size_t CountTiles(std::istream &&is)
{
    std::unordered_set<Tile, TileHash> blacks;

    std::string line;
    while (getline(is, line))
    {
        auto tile = TracePath(line);
        auto it = blacks.find(tile);
        if (it == blacks.end())
            blacks.insert(tile);
        else
            blacks.erase(it);
    }

    return blacks.size();
}

using namespace boost::ut;

suite s = [] {
    "2020-24"_test = [] {
        {
            expect(Tile{0, 1} == TracePath("esew"));
            expect(1_u == CountTiles(std::istringstream{"esew"}));
            expect(1_u == CountTiles(std::istringstream{"nwwswee"}));
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
            expect(10_u == CountTiles(std::istringstream{TEST}));
        }

        Printer::Print(__FILE__, "1", CountTiles(std::ifstream{INPUT}));
    };
};

} //namespace;
