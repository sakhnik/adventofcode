#include "../test.hpp"
#include <fstream>

namespace {

using MapT = std::vector<std::string>;

std::pair<int, int> FindPath(std::istream &&is)
{
    MapT map;

    std::string line;
    while (std::getline(is, line))
        map.push_back(std::move(line));

    int start_row{}, start_col{};
    int finish_row{}, finish_col{};
    for (int row = 0; row < map.size(); ++row)
    {
        for (int col = 0; col < map[row].size(); ++col)
        {
            if (map[row][col] == 'S')
            {
                start_row = row;
                start_col = col;
                map[row][col] = 'a';
            }
            else if (map[row][col] == 'E')
            {
                finish_row = row;
                finish_col = col;
                map[row][col] = 'z';
            }
        }
    }

    auto idx = [](int row, int col) {
        return (row << 16) | col;
    };

    struct Info
    {
        int distance = std::numeric_limits<int>::max();
    };
    std::unordered_map<int, Info> info_map;
    info_map[idx(finish_row, finish_col)].distance = 0;
    std::queue<std::pair<int, int>> q;
    q.push({finish_row, finish_col});

    int first_a = -1;

    while (!q.empty())
    {
        auto [row, col] = q.front();
        q.pop();
        auto elevation = map[row][col];

        const auto &info = info_map.find(idx(row, col))->second;
        auto tryMove = [&](int new_row, int new_col) {
            if (new_row < 0 || new_col < 0)
                return;
            if (new_row >= map.size() || new_col >= map[new_row].size())
                return;
            if (info_map.contains(idx(new_row, new_col)))
                return;
            auto new_elevation = map[new_row][new_col];
            if (elevation - 1 > new_elevation)
                return;
            if (new_elevation == 'a' && first_a == -1)
                first_a = info.distance + 1;
            info_map[idx(new_row, new_col)].distance = info.distance + 1;
            q.push({new_row, new_col});
        };

        tryMove(row - 1, col);
        tryMove(row, col - 1);
        tryMove(row + 1, col);
        tryMove(row, col + 1);
    }

    return {info_map[idx(start_row, start_col)].distance, first_a};
}

const char *const TEST =
    "Sabqponm\n"
    "abcryxxl\n"
    "accszExk\n"
    "acctuvwj\n"
    "abdefghi\n";

using namespace boost::ut;

suite s = [] {
    "2022-12"_test = [] {
        auto test = FindPath(std::istringstream{TEST});
        expect(31_i == test.first);
        expect(29_i == test.second);

        auto res = FindPath(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", res.first);
        Printer::Print(__FILE__, "2", res.second);
    };
};

} //namespace;
