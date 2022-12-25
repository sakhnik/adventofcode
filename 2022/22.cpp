#include "../test.hpp"
#include <fstream>

namespace {

struct MonkeyMap
{
    std::vector<std::string> map;
    std::string path;

    MonkeyMap(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line))
        {
            if (line.empty())
                break;
            map.push_back(std::move(line));
        }
        std::getline(is, path);
    }

    int Task1() const
    {
        std::vector<int> left_edge;
        std::vector<int> right_edge;
        for (int r = 0; r < map.size(); ++r)
        {
            left_edge.push_back(map[r].find_first_not_of(' '));
            right_edge.push_back(map[r].find_last_not_of(' '));
        }

        std::vector<int> top_edge;
        std::vector<int> bottom_edge;
        auto max_col = std::max_element(map.begin(), map.end(),
                [](const auto &a, const auto &b) { return a.size() < b.size(); })->size();
        for (int c = 0; c < max_col; ++c)
        {
            for (int r = 0; r < map.size(); ++r)
            {
                if (c >= map[r].size())
                    continue;
                if (map[r][c] != ' ')
                {
                    top_edge.push_back(r);
                    break;
                }
            }
            for (int r = map.size() - 1; r >= 0; --r)
            {
                if (c >= map[r].size())
                    continue;
                if (map[r][c] != ' ')
                {
                    bottom_edge.push_back(r);
                    break;
                }
            }
        }

        assert(left_edge.size() == right_edge.size() && left_edge.size() == map.size());
        assert(top_edge.size() == bottom_edge.size());

        int row{0};
        int col = left_edge[0];
        int facing{0};
        int steps{};

        auto go = [&]() {
            while (steps--)
            {
                int new_row = row;
                int new_col = col;
                switch (facing)
                {
                case 0:
                    new_col = col == right_edge[row] ? left_edge[row] : col + 1;
                    break;
                case 1:
                    new_row = row == bottom_edge[col] ? top_edge[col] : row + 1;
                    break;
                case 2:
                    new_col = col == left_edge[row] ? right_edge[row] : col - 1;
                    break;
                case 3:
                    new_row = row == top_edge[col] ? bottom_edge[col] : row - 1;
                    break;
                }
                if (map[new_row][new_col] != '.')
                    break;
                row = new_row;
                col = new_col;
            }
            //std::cout << row << " " << col << std::endl;
        };

        for (char ch : path)
        {
            switch (ch)
            {
            case '0'...'9':
                steps = steps * 10 + (ch - '0');
                continue;
            case 'L':
                go();
                facing = (facing + 3) % 4;
                steps = 0;
                continue;
            case 'R':
                go();
                facing = (facing + 1) % 4;
                steps = 0;
                continue;
            }
        }
        if (steps)
            go();
        return 1000 * (row + 1) + 4 * (col + 1) + facing;
    }
};

const char *const TEST = 
    "        ...#\n"
    "        .#..\n"
    "        #...\n"
    "        ....\n"
    "...#.......#\n"
    "........#...\n"
    "..#....#....\n"
    "..........#.\n"
    "        ...#....\n"
    "        .....#..\n"
    "        .#......\n"
    "        ......#.\n"
    "\n"
    "10R5L5R10L4R5L5\n";

using namespace boost::ut;

suite s = [] {
    "2022-22"_test = [] {
        MonkeyMap test{std::istringstream{TEST}};
        expect(6032_i == test.Task1());

        MonkeyMap map{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", map.Task1());
    };
};

} //namespace;
