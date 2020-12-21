#include "../test.hpp"
#include <fstream>
#include <unordered_map>
#include <cmath>
#include <regex>

namespace {

using MapT = std::vector<std::string>;

void Rotate(MapT &m)
{
    size_t N = m.size();

    for (size_t i = 0; i < N; ++i)
    {
        for (size_t j = 0; j + i + i + 1 < N; ++j)
        {
            int i2 = i + j;
            int i3 = N - 1 - i - j;
            int i4 = N - 1 - i;

            char t = m[i][i2];
            m[i][i2] = m[i3][i];
            m[i3][i] = m[i4][i3];
            m[i4][i3] = m[i2][i4];
            m[i2][i4] = t;
        }
    }
}

void Flip(MapT &m)
{
    std::reverse(m.begin(), m.end());
}

size_t Count(const MapT &m)
{
    size_t c{};
    for (size_t r = 0; r < m.size(); ++r)
        c += std::count(m[r].begin(), m[r].end(), '#');
    return c;
}

template <int N>
struct Tile
{
    MapT rows;
    int64_t id = 0;

    std::string GetEdge(int c, bool flip = false) const
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

    bool operator==(const Tile<N> &o) const
    {
        return rows == o.rows;
    }
};

class Tiles
{
public:
    Tiles(std::istream &&is)
    {
        Tile<N> tile;

        std::string line;
        while (getline(is, line))
        {
            if (line.empty())
            {
                _tiles[tile.id] = std::move(tile);
                tile = {};
                continue;
            }
            if (1 == sscanf(line.c_str(), "Tile %ld", &tile.id))
                continue;
            tile.rows.push_back(line);
        }
        _tiles[tile.id] = std::move(tile);
    }

    void Arrange()
    {
        // Find tiles with two unique edges.
        std::unordered_map<std::string, std::vector<int64_t>> edge2id;

        // Add edges to the pool {edge -> id}.
        auto addEdges = [&](const Tile<N> &tile) {
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 2; ++j)
                {
                    edge2id[tile.GetEdge(i, j)].push_back(tile.id);
                }
            }
        };

        for (const auto &id_tile : _tiles)
            addEdges(id_tile.second);

        // Remove unused edges from the {edge -> id} mapping when found a good placement.
        auto removeEdges = [&](const Tile<N> &tile) {
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 2; ++j)
                {
                    std::string edge = tile.GetEdge(i, j);
                    auto it = edge2id.find(edge);
                    it->second.erase(std::find(it->second.begin(), it->second.end(), tile.id));
                    if (it->second.empty())
                        edge2id.erase(it);
                }
            }
        };

        std::unordered_map<int64_t, std::vector<std::string>> id2unique;

        for (const auto &edge_ids : edge2id)
        {
            const auto &ids = edge_ids.second;
            if (ids.size() != 1)
                continue;
            for (auto id : ids)
            {
                id2unique[id].push_back(edge_ids.first);
            }
        }

        int64_t first_id{};
        for (const auto &c : id2unique)
        {
            if (c.second.size() == 4)  // two pairs of unique edges (normal and flipped)
            {
                first_id = c.first;
                _corner_product *= c.first;
            }
        }

        // Orient the chosen corner tile and start picking
        // the adjacent tiles.

        size_t dim = std::sqrt(_tiles.size());
        std::vector<Tile<N>> arranged(_tiles.size());

        auto get_arranged = [&arranged, dim](size_t row, size_t col) -> Tile<N>& {
            return arranged[row * dim + col];
        };

        auto isCorner = [&](const std::string &edge) {
            auto it = edge2id.find(edge);
            return it->second.size() == 1;
        };

        auto fits = [&](const std::string &e1, const std::string &e2) {
            if (e1.empty())
                return isCorner(e2);
            return std::equal(e1.begin(), e1.end(), e2.rbegin());
        };

        auto findRotation = [](Tile<N> &tile, auto left, auto top) {
            for (int j = 0; j < 2; ++j)
            {
                for (int i = 0; i < 4; ++i)
                {
                    if (left(tile.GetEdge(3)) && top(tile.GetEdge(0)))
                        return true;
                    Rotate(tile.rows);
                }
                Flip(tile.rows);
            }
            return false;
        };

        for (size_t row = 0; row < dim; ++row)
        {
            for (size_t col = 0; col < dim; ++col)
            {
                if (!row && !col)
                {
                    auto &t = _tiles[first_id];
                    if (!findRotation(t, isCorner, isCorner))
                        throw "(_|_)";
                    get_arranged(row, col) = t;
                    removeEdges(t);
                    continue;
                }

                std::string left_edge = col > 0 ? get_arranged(row, col - 1).GetEdge(1) : "";
                std::string top_edge = row > 0 ? get_arranged(row - 1, col).GetEdge(2) : "";

                decltype(&edge2id.begin()->second) ids{};

                if (!left_edge.empty())
                {
                    ids = &edge2id[left_edge];
                }
                else if (!top_edge.empty())
                {
                    ids = &edge2id[top_edge];
                }

                if (ids->size() != 1)
                    throw "(_|_)";
                auto &t = _tiles[ids->front()];
                if (!findRotation(t,
                                  [&](const std::string &e) { return fits(left_edge, e); },
                                  [&](const std::string &e) { return fits(top_edge, e); }))
                {
                    throw "(_|_)";
                }
                get_arranged(row, col) = t;
                removeEdges(t);
            }
        }

        for (size_t row = 0; row < dim; ++row)
        {
            for (size_t l = 1; l < N - 1; ++l)
            {
                std::string line;
                for (size_t col = 0; col < dim; ++col)
                {
                    const auto &tile = get_arranged(row, col);
                    line += tile.rows[l].substr(1, N - 2);
                }
                _map.push_back(std::move(line));
            }
        }
    }

    int64_t GetCornerProduct() const { return _corner_product; }

    size_t GetRoughness()
    {
        const MapT MONSTER_MAP = {{
            "..................#.",
            "#....##....##....###",
            ".#..#..#..#..#..#...",
        }};
        const size_t MONSTER_HEIGHT = MONSTER_MAP.size();
        const size_t MONSTER_WIDTH = MONSTER_MAP[0].size();

        std::vector<std::regex> MONSTER;
        for (const auto &s : MONSTER_MAP)
            MONSTER.emplace_back(s);

        auto has_monster = [&](size_t row, size_t col) {
            for (size_t i = 0; i < MONSTER_HEIGHT; ++i)
            {
                auto it = _map[row + i].data() + col;
                if (!std::regex_match(it, it + MONSTER_WIDTH, MONSTER[i]))
                    return false;
            }
            return true;
        };

        size_t monster_count{};
        for (int side = 0; side < 2; ++side)
        {
            for (int rotation = 0; rotation < 4; ++rotation)
            {
                for (size_t row = 0; row < _map.size() - MONSTER_HEIGHT; ++row)
                {
                    for (size_t col = 0; col < _map[0].size() - MONSTER_WIDTH; ++col)
                    {
                        monster_count += has_monster(row, col);
                    }
                }
                Rotate(_map);
            }
            Flip(_map);
        }

        return Count(_map) - monster_count * Count(MONSTER_MAP);
    }

private:
    int64_t _corner_product = 1;


    const static size_t N = 10;
    std::unordered_map<int64_t, Tile<N>> _tiles;

    MapT _map;
};

using namespace boost::ut;

suite s = [] {
    "2020-20"_test = [] {
        {
            MapT t{{
                "###",
                ".##",
                "..#",
            }};
            Rotate(t);
            MapT t2{{
                "..#",
                ".##",
                "###",
            }};

            expect(t2 == t);
        }

        {
            MapT t{{
                "####",
                "#.##",
                "##.#",
                ".###",
            }};
            Rotate(t);
            MapT t2{{
                ".###",
                "##.#",
                "#.##",
                "####",
            }};
            expect(t2 == t);
        }

        {
            MapT t{{
                "####...##.",
                "#..##.#..#",
                "##.#..#.#.",
                ".###.####.",
                "..###.####",
                ".##....##.",
                ".#...####.",
                "#.##.####.",
                "####..#...",
                ".....##...",
            }};
            Rotate(t);
            MapT t2{{
                ".##....###",
                ".#.##.##.#",
                ".##.###..#",
                ".##..#####",
                ".....#..#.",
                "#.##..#...",
                "####.####.",
                "..#####..#",
                "..######.#",
                ".....#..#.",
            }};
            expect(t2 == t);
        }

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
            expect(273_u == tiles.GetRoughness());
        }

        Tiles tiles{std::ifstream{INPUT}};
        tiles.Arrange();
        Printer::Print(__FILE__, "1", tiles.GetCornerProduct());
        Printer::Print(__FILE__, "2", tiles.GetRoughness());
    };
};

} //namespace;
