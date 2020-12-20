#include "../test.hpp"
#include <fstream>
#include <unordered_map>

namespace {

class Tiles
{
public:
    Tiles(std::istream &&is)
    {
        _Tile tile;

        std::string line;
        while (getline(is, line))
        {
            if (line.empty())
            {
                _tiles.push_back(std::move(tile));
                tile = _Tile{};
                continue;
            }
            if (1 == sscanf(line.c_str(), "Tile %ld", &tile.id))
                continue;
            tile.rows.push_back(line);
        }
        _tiles.push_back(std::move(tile));
    }

    int64_t FindCorners()
    {
        // Find tiles with two unique edges.
        std::unordered_map<std::string, std::vector<int64_t>> edge2id;

        for (const auto &tile : _tiles)
        {
            for (int i = 0; i < 4; ++i)
            {
                edge2id[tile.GetEdge(i, false)].push_back(tile.id);
                edge2id[tile.GetEdge(i, true)].push_back(tile.id);
            }
        }

        std::unordered_map<int64_t, int> counts;

        for (const auto &edge_ids : edge2id)
        {
            const auto &ids = edge_ids.second;
            if (ids.size() != 1)
                continue;
            for (auto id : ids)
            {
                ++counts[id];
            }
        }

        int64_t res{1};
        for (const auto &c : counts)
        {
            if (c.second == 4)  // two pairs of unique edges (normal and flipped)
            {
                res *= c.first;
            }
        }
        return res;
    }


private:
    const static size_t N = 10;

    struct _Tile
    {
        int64_t id;
        std::vector<std::string> rows;

        std::string GetEdge(int c, bool flip) const
        {
            std::string edge;
            switch (c)
            {
            case 0:
                for (size_t i = 0; i < N; ++i)
                    edge.push_back(rows[0][i]);
                break;
            case 1:
                for (size_t i = 0; i < N; ++i)
                    edge.push_back(rows[i][N-1]);
                break;
            case 2:
                for (size_t i = 0; i < N; ++i)
                    edge.push_back(rows[N-1][N-i-1]);
                break;
            case 3:
                for (size_t i = 0; i < N; ++i)
                    edge.push_back(rows[N-i-1][0]);
                break;
            }
            if (flip)
                std::reverse(edge.begin(), edge.end());
            return edge;
        }
    };

    std::vector<_Tile> _tiles;
};

using namespace boost::ut;

suite s = [] {
    "2020-20"_test = [] {
        {
            const char *const TEST = R"(Tile 2311:
..##.#..#.
##..#.....
#...##..#.
####.#...#
##.##.###.
##...#.###
.#.#.#..##
..#....#..
###...#.#.
..###..###

Tile 1951:
#.##...##.
#.####...#
.....#..##
#...######
.##.#....#
.###.#####
###.##.##.
.###....#.
..#.#..#.#
#...##.#..

Tile 1171:
####...##.
#..##.#..#
##.#..#.#.
.###.####.
..###.####
.##....##.
.#...####.
#.##.####.
####..#...
.....##...

Tile 1427:
###.##.#..
.#..#.##..
.#.##.#..#
#.#.#.##.#
....#...##
...##..##.
...#.#####
.#.####.#.
..#..###.#
..##.#..#.

Tile 1489:
##.#.#....
..##...#..
.##..##...
..#...#...
#####...#.
#..#.#.#.#
...#.#.#..
##.#...##.
..##.##.##
###.##.#..

Tile 2473:
#....####.
#..#.##...
#.##..#...
######.#.#
.#...#.#.#
.#########
.###.#..#.
########.#
##...##.#.
..###.#.#.

Tile 2971:
..#.#....#
#...###...
#.#.###...
##.##..#..
.#####..##
.#..####.#
#..#.#..#.
..####.###
..#.#.###.
...#.#.#.#

Tile 2729:
...#.#.#.#
####.#....
..#.#.....
....#..#.#
.##..##.#.
.#.####...
####.#.#..
##.####...
##..#.##..
#.##...##.

Tile 3079:
#.#.#####.
.#..######
..#.......
######....
####.#..#.
.#...#.##.
#.#####.##
..#.###...
..#.......
..#.###...)";
            Tiles tiles{std::istringstream{TEST}};
            expect(eq(20899048083289ll, tiles.FindCorners()));
        }

        Tiles tiles{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", tiles.FindCorners());
    };
};

} //namespace;
