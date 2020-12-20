#include "../test.hpp"
#include <fstream>
#include <unordered_map>

namespace {

class Tiles
{
public:
    Tiles(std::istream &&is)
    {
        int64_t id{};
        _Tile tile;

        std::string line;
        while (getline(is, line))
        {
            if (line.empty())
            {
                _tiles[id] = std::move(tile);
                tile = _Tile{};
                continue;
            }
            if (1 == sscanf(line.c_str(), "Tile %ld", &id))
                continue;
            tile.rows.push_back(line);
        }
        _tiles[id] = std::move(tile);
    }

    void Arrange()
    {
        // Find tiles with two unique edges.
        std::unordered_map<std::string, std::vector<int64_t>> edge2id;

        for (const auto &id_tile : _tiles)
        {
            int64_t id = id_tile.first;
            const auto &tile = id_tile.second;

            for (int i = 0; i < 4; ++i)
            {
                edge2id[tile.GetEdge(i, false)].push_back(id);
                edge2id[tile.GetEdge(i, true)].push_back(id);
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

        int64_t first_id{};
        for (const auto &c : counts)
        {
            if (c.second == 4)  // two pairs of unique edges (normal and flipped)
            {
                first_id = c.first;
                _corner_product *= c.first;
            }
        }


        // TODO: orient the chosen corner tile and start picking
        // the adjacent tiles.
    }

    int64_t GetCornerProduct() const { return _corner_product; }

private:
    int64_t _corner_product = 1;

    const static size_t N = 10;

    struct _Tile
    {
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

    std::unordered_map<int64_t, _Tile> _tiles;
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
            tiles.Arrange();
            expect(eq(20899048083289ll, tiles.GetCornerProduct()));
        }

        Tiles tiles{std::ifstream{INPUT}};
        tiles.Arrange();
        Printer::Print(__FILE__, "1", tiles.GetCornerProduct());
    };
};

} //namespace;
