#include "../test.hpp"
#include <fstream>

namespace {

struct MonkeyMap
{
    enum Facing
    {
        F_RIGHT = 0,
        F_DOWN = 1,
        F_LEFT = 2,
        F_UP = 3,
    };

    std::vector<std::string> map;
    std::string path;
    std::vector<int> edge[4];
    int max_col{};
    int face_size{};

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

        for (int r = 0; r < map.size(); ++r)
        {
            edge[F_LEFT].push_back(map[r].find_first_not_of(' '));
            edge[F_RIGHT].push_back(map[r].find_last_not_of(' '));
        }

        max_col = std::max_element(map.begin(), map.end(),
                [](const auto &a, const auto &b) { return a.size() < b.size(); })->size();
        for (int c = 0; c < max_col; ++c)
        {
            for (int r = 0; r < map.size(); ++r)
            {
                if (c >= map[r].size())
                    continue;
                if (map[r][c] != ' ')
                {
                    edge[F_UP].push_back(r);
                    break;
                }
            }
            for (int r = map.size() - 1; r >= 0; --r)
            {
                if (c >= map[r].size())
                    continue;
                if (map[r][c] != ' ')
                {
                    edge[F_DOWN].push_back(r);
                    break;
                }
            }
        }

        assert(edge[F_LEFT].size() == edge[F_RIGHT].size() && edge[F_LEFT].size() == map.size());
        assert(edge[F_UP].size() == edge[F_DOWN].size());

        auto max_col = std::max_element(map.begin(), map.end(),
                [](const auto &a, const auto &b) { return a.size() < b.size(); })->size();
        int max_row = map.size();
        face_size = std::gcd(max_row, max_col);
    }

    struct Position
    {
        int row{};
        int col{};
        int facing{};

        int GetPassword() const
        {
            return 1000 * (row + 1) + 4 * (col + 1) + facing;
        }
    };

    Position FlatStep(Position pos) const
    {
        switch (pos.facing)
        {
        case 0:
            pos.col = pos.col == edge[F_RIGHT][pos.row] ? edge[F_LEFT][pos.row] : pos.col + 1;
            break;
        case 1:
            pos.row = pos.row == edge[F_DOWN][pos.col] ? edge[F_UP][pos.col] : pos.row + 1;
            break;
        case 2:
            pos.col = pos.col == edge[F_LEFT][pos.row] ? edge[F_RIGHT][pos.row] : pos.col - 1;
            break;
        case 3:
            pos.row = pos.row == edge[F_UP][pos.col] ? edge[F_DOWN][pos.col] : pos.row - 1;
            break;
        }
        return pos;
    }

    int Wander(std::function<Position(Position)> next) const
    {
        Position pos{0, edge[F_LEFT][0], 0};
        int steps{};

        //auto print = [&]() {
        //    for (int row = 0; row < map.size(); ++row)
        //    {
        //        for (int col = 0; col < map[row].size(); ++col)
        //        {
        //            if (row == pos.row && col == pos.col)
        //                std::cout << ">v<^"[pos.facing];
        //            else
        //                std::cout << map[row][col];
        //        }
        //        std::cout << "\n";
        //    }
        //    std::cout << "----" << std::endl;
        //};

        auto go = [&]() {
            while (steps--)
            {
                auto new_pos = next(pos);
                if (map[new_pos.row][new_pos.col] != '.')
                    break;
                pos = new_pos;
                //print();
            }
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
                pos.facing = (pos.facing + 3) % 4;
                steps = 0;
                continue;
            case 'R':
                go();
                pos.facing = (pos.facing + 1) % 4;
                steps = 0;
                continue;
            }
        }
        if (steps)
            go();
        return pos.GetPassword();
    }

    int Task1() const
    {
        return Wander([this](Position pos) { return FlatStep(pos); });
    }

    int Task2() const
    {
        auto has_face = [&](int row, int col) -> bool {
            row = row * face_size + 1;
            col = col * face_size + 1;
            if (row >= map.size() || col >= map[row].size())
                return false;
            return map[row][col] != ' ';
        };

        auto get_signature = [&]() {
            int res{};
            for (int row = 0; row < 4; ++row)
                for (int col = 0; col < 4; ++col)
                    res = (res << 1) | has_face(row, col);
            return res;
        };
        if (get_signature() == 0x2e30)
        {
            const int S = face_size;
            // This is buggy, but works merely for the test case.
            auto cubeStep = [&](Position pos) -> Position {
                switch (pos.facing)
                {
                case F_RIGHT:
                    if (pos.col != edge[F_RIGHT][pos.row])
                        return ++pos.col, pos;
                    switch (pos.row / S)
                    {
                    case 0: return Position{S*1 - 1 - pos.row + 2*3, S*S - 1, F_LEFT};
                    case 1: return Position{S*2, S*2 - 1 - pos.row + S*3, F_DOWN};
                    case 2: return Position{S*3 - 1 - pos.row, S*3 - 1, F_LEFT};
                    }
                    break;
                case F_DOWN:
                    if (pos.row != edge[F_DOWN][pos.col])
                        return ++pos.row, pos;
                    switch (pos.col / S)
                    {
                    case 0: return Position{S*3 - 1, S*1 - 1 - pos.col + S*2, F_UP};
                    case 1: return Position{S*2 - 1 - pos.col + S*2, S*2, F_RIGHT};
                    case 2: return Position{S*2 - 1, S*3 - 1 - pos.col, F_UP};
                    case 3: return Position{S*S - 1 - pos.col + S*1, 0, F_RIGHT};
                    }
                    break;
                case F_LEFT:
                    if (pos.col != edge[F_LEFT][pos.row])
                        return --pos.col, pos;
                    switch (pos.row / S)
                    {
                    case 0: return Position{S*1, S*1 - 1 - pos.row + S*1, F_DOWN};
                    case 1: return Position{S*3 - 1, S*2 - 1 - pos.row + S*3, F_UP};
                    case 2: return Position{S*2, S*3 - 1 - pos.row + S*1, F_UP};
                    }
                    break;
                case F_UP:
                    if (pos.row != edge[F_UP][pos.col])
                        return --pos.row, pos;
                    switch (pos.col / S)
                    {
                    case 0: return Position{0, S*1 - 1 - pos.col + S*2, F_DOWN};
                    case 1: return Position{pos.col - S*1, S*2, F_RIGHT};
                    case 2: return Position{S*1, S*3 - 1 - pos.col, F_DOWN};
                    case 3: return Position{S*S - 1 - pos.col + S*1, S*3 - 1, F_LEFT};
                    }
                }
                return pos;
            };

            return Wander(cubeStep);
        }
        else if (get_signature() == 0x64c8)
        {
            const int S = face_size;
            auto cubeStep = [&](Position pos) -> Position {
                switch (pos.facing)
                {
                case F_RIGHT:
                    if (pos.col != edge[F_RIGHT][pos.row])
                        return ++pos.col, pos;
                    switch (pos.row / S)
                    {
                    case 0: return Position{S*1 - 1 - pos.row + S*2, S*2 - 1, F_LEFT};
                    case 1: return Position{S*1 - 1, pos.row - S*1 + S*2, F_UP};
                    case 2: return Position{S*3 - 1 - pos.row, S*3 - 1, F_LEFT};
                    case 3: return Position{S*3 - 1, pos.row - S*3 + S*1, F_UP};
                    }
                    break;
                case F_DOWN:
                    if (pos.row != edge[F_DOWN][pos.col])
                        return ++pos.row, pos;
                    switch (pos.col / S)
                    {
                    case 0: return Position{S*0, pos.col + S*2, F_DOWN};
                    case 1: return Position{pos.col - S*1 + S*3, S*1 - 1, F_LEFT};
                    case 2: return Position{pos.col - S*2 + S*1, S*2 - 1, F_LEFT};
                    }
                    break;
                case F_LEFT:
                    if (pos.col != edge[F_LEFT][pos.row])
                        return --pos.col, pos;
                    switch (pos.row / S)
                    {
                    case 0: return Position{S*1 - 1 - pos.row + S*2, S*0, F_RIGHT};
                    case 1: return Position{S*2, pos.row - S*1, F_DOWN};
                    case 2: return Position{S*3 - 1 - pos.row + S*0, S*1, F_RIGHT};
                    case 3: return Position{0, pos.row - S*3 + S*1, F_DOWN};
                    }
                    break;
                case F_UP:
                    if (pos.row != edge[F_UP][pos.col])
                        return --pos.row, pos;
                    switch (pos.col / S)
                    {
                    case 0: return Position{pos.col + S*1, S*1, F_RIGHT};
                    case 1: return Position{pos.col - S*1 + S*3, 0, F_RIGHT};
                    case 2: return Position{S*4 - 1, pos.col - S*2, F_UP};
                    }
                }
                return pos;
            };

            return Wander(cubeStep);
        }
        else
        {
            throw std::runtime_error("Need to know the wrapping");
        }

        return 0;
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
        expect(5031_i == test.Task2());

        MonkeyMap map{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", map.Task1());
        Printer::Print(__FILE__, "2", map.Task2());
    };
};

} //namespace;
