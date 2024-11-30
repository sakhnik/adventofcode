#include "../test.hpp"
#include <fstream>

namespace {

class Map
{
public:
    Map(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            _map.emplace_back(std::move(line));
        }
    }

    uint64_t Count(int right = 3, int down = 1) const
    {
        uint64_t count{};

        for (size_t row{}, col{}; row < _map.size(); row += down, col += right)
        {
            const auto &line = _map[row];
            count += (line[col % line.size()] == '#');
        }

        return count;
    }

    uint64_t Count2() const
    {
        return Count(1, 1) * Count(3, 1) * Count(5, 1) * Count(7, 1) * Count(1, 2);
    }

private:
    std::vector<std::string> _map;
};

using namespace boost::ut;

suite s = [] {
    "03"_test = [] {
        {
            const char *const TEST =
                "..##.......\n"
                "#...#...#..\n"
                ".#....#..#.\n"
                "..#.#...#.#\n"
                ".#...##..#.\n"
                "..#.##.....\n"
                ".#.#.#....#\n"
                ".#........#\n"
                "#.##...#...\n"
                "#...##....#\n"
                ".#..#...#.#\n";
            Map m{std::istringstream{TEST}};
            expect(7_u == m.Count());
            expect(336_u == m.Count2());
        }

        Map m{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", m.Count());
        Printer::Print(__FILE__, "2", m.Count2());
    };
};

} //namespace;
