#include "../test.hpp"

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

    int Count() const
    {
        int count{};

        for (size_t row{}, col{}; row < _map.size(); ++row, col += 3)
        {
            const auto &line = _map[row];
            count += (line[col % line.size()] == '#');
        }

        return count;
    }

private:
    std::vector<std::string> _map;
};

using namespace boost::ut;

suite s = [] {
    "2020-03"_test = [] {
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
            expect(7_i == m.Count());
        }

        Map m{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", m.Count());
    };
};

} //namespace;