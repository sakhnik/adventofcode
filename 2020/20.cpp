#include "../test.hpp"

namespace {

class Tiles
{
public:
    Tiles(std::istream &&is)
    {
        _Tile tile;
        int row{};

        std::string line;
        while (getline(is, line))
        {
            if (line.empty())
            {
                _tiles.push_back(std::move(tile));
                tile = _Tile{};
                row = 0;
                continue;
            }
            if (1 == sscanf(line.c_str(), "Tile %ld", &tile.id))
                continue;
            if (row == 0)
            {
                for (size_t i = 0; i < N; ++i)
                    tile.e[0][i] = line[i] == '#';
            }
            else if (row == N - 1)
            {
                for (size_t i = 0; i < N; ++i)
                    tile.e[2][i] = line[N - i - 1] == '#';
            }
            tile.e[1][row] = line.back() == '#';
            tile.e[3][N - row - 1] = line.front() == '#';
            ++row;
        }
        _tiles.push_back(std::move(tile));
    }

    int64_t FindCorners()
    {
        // Find tiles with two unique edges.
        std::vector<std::vector<int64_t>> edge2id(1 << N);

        for (const auto &tile : _tiles)
        {
            for (int i = 0; i < 4; ++i)
            {
                edge2id[_CalcEdge(tile.e[i])].push_back(tile.id);
                edge2id[_CalcEdge2(tile.e[i])].push_back(tile.id);
            }
        }

        auto it = std::remove_if(begin(edge2id), end(edge2id), [](const auto &a) { return a.empty(); });
        edge2id.erase(it, end(edge2id));

        std::sort(begin(edge2id), end(edge2id), [](const auto &a, const auto &b) {
                    return a.size() < b.size();
                  });
        //for (const auto &a : edge2id)
        //{
        //    std::cout << " " << a.size();
        //}
        //std::cout << std::endl;

        std::unordered_map<int64_t, int> counts;

        for (size_t i = 0; i < edge2id.size() && edge2id[i].size() == 1; ++i)
        {
            for (auto id : edge2id[i])
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
    using _EdgeT = std::array<bool, N>;
    struct _Tile
    {
        int64_t id;
        std::array<_EdgeT, 4> e;

        _Tile Rotate(int c) const
        {
            _Tile t = *this;
            std::rotate(begin(t.e), begin(t.e) + c, end(t.e));
            return t;
        }

        _Tile Flip(int c) const
        {
            _Tile t = *this;
            std::swap(t.e[c], t.e[c+2]);
            std::reverse(t.e[c+1].begin(), t.e[c+1].end());
            std::reverse(t.e[(c+3) % 4].begin(), t.e[(c+3) % 4].end());
            return t;
        }
    };

    static int _CalcEdge(const _EdgeT &edge)
    {
        int res{};
        for (size_t i = 0; i < edge.size(); ++i)
        {
            res <<= 1;
            res |= edge[i];
        }
        return res;
    };

    static int _CalcEdge2(_EdgeT edge)
    {
        std::reverse(edge.begin(), edge.end());
        return _CalcEdge(edge);
    }

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
..#.###...
            )";
            Tiles tiles{std::istringstream{TEST}};
            expect(eq(20899048083289ll, tiles.FindCorners()));
        }

        Tiles tiles{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", tiles.FindCorners());
    };
};

} //namespace;
