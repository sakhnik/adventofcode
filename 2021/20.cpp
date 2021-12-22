#include "../test.hpp"
#include <boost/functional/hash.hpp>

namespace {

class TrenchMap
{
public:
    std::string algorithm;
    using PosT = std::pair<int, int>;
    using MapT = std::unordered_set<PosT, boost::hash<PosT>>;
    MapT map;
    bool def_pixel = false;
    bool flips = false;

    TrenchMap(std::istream &&is)
    {
        getline(is, algorithm);

        std::string line;
        while (getline(is, line))
        {
            if (line.empty())
                break;
            algorithm += line;
        }
        using namespace boost::ut;
        expect(512_u == algorithm.size());

        PosT pos{};
        while (getline(is, line))
        {
            for (char p : line)
            {
                if (p == '#')
                    map.insert(pos);
                ++pos.second;
            }
            ++pos.first;
            pos.second = 0;
        }

        for (char &p : algorithm)
            p = p == '#';
        flips = algorithm[0];
        if (flips)
            expect(!algorithm.back());
    }

    bool Lookup(PosT pos) const
    {
        return map.find(pos) != map.end();
    }

    char GetSubst(PosT pos) const
    {
        size_t idx{};
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                idx = (idx << 1) | (def_pixel != Lookup({pos.first + i, pos.second + j}));
            }
        }
        return algorithm[idx];
    }

    void Enhance()
    {
        MapT new_map;

        MapT todo;
        for (PosT pos : map)
        {
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    todo.insert({pos.first + i, pos.second + j});
                }
            }
        }

        if (!flips)
        {
            for (PosT pos : todo)
            {
                if (GetSubst(pos))
                    new_map.insert(pos);
            }
        }
        else
        {
            for (PosT pos : todo)
            {
                if (GetSubst(pos) == def_pixel)
                    new_map.insert(pos);
            }
            def_pixel = !def_pixel;
        }

        map.swap(new_map);
    }

    void Enhance(int count)
    {
        while (count--)
        {
            //std::cout << " " << count << std::flush;
            Enhance();
        }
        //std::cout << std::endl;
    }

    size_t GetCount() const { return map.size(); }

    void Print() const
    {
        PosT min_pos{std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
        PosT max_pos{std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};

        for (PosT pos : map)
        {
            min_pos.first = std::min(pos.first, min_pos.first);
            min_pos.second = std::min(pos.second, min_pos.second);
            max_pos.first = std::max(pos.first, max_pos.first);
            max_pos.second = std::max(pos.second, max_pos.second);
        }

        for (int row = min_pos.first; row <= max_pos.first; ++row)
        {
            for (int col = min_pos.second; col <= max_pos.second; ++col)
            {
                std::cout << (map.find({row, col}) == map.end() ? '.' : '#');
            }
            std::cout << std::endl;
        }
    }
};

const char *TEST_INPUT = R"(
..#.#..#####.#.#.#.###.##.....###.##.#..###.####..#####..#....#..#..##..##
#..######.###...####..#..#####..##..#.#####...##.#.#..#.##..#.#......#.###
.######.###.####...#.##.##..#..#..#####.....#.#....###..#.##......#.....#.
.#..#..##..#...##.######.####.####.#.#...#.......#..#.#.#...####.##.#.....
.#..#...##.#.##..#...##.#.##..###.#......#.#.......#.#.#.####.###.##...#..
...####.#..#..#.##.#....##..#.####....##...##..#...#......#.#.......#.....
..##..####..#...#.#.#...##..#.#..###..#####........#..####......#..#

#..#.
#....
##..#
..#..
..###
)";

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-20"_test = [] {
        TrenchMap test{std::istringstream{TEST_INPUT}};
        expect(10_u == test.GetCount());
        test.Enhance();
        test.Enhance();
        expect(35_u == test.GetCount());
        test.Enhance(50 - 2);
        expect(3351_u == test.GetCount());

        TrenchMap image{std::ifstream{INPUT}};
        image.Enhance();
        image.Enhance();
        Printer::Print(__FILE__, "1", image.GetCount());
        image.Enhance(50 - 2);
        Printer::Print(__FILE__, "2", image.GetCount());
    };
};

} //namespace;
