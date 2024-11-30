#include "../test.hpp"
#include <fstream>

namespace {

struct Caves
{
    using PosT = std::pair<int, int>;

    std::unordered_map<int, char> map;
    int left_edge = 500;
    int right_edge = 500;
    int depth = 0;

    int sand = 0;

    int Idx(int x, int y) const
    {
        return (x << 16) | y;
    }

    int Idx(PosT p) const
    {
        return Idx(p.first, p.second);
    }

    Caves(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line))
        {
            std::istringstream iss{line};
            std::string s;
            iss >> s;

            PosT start;
            sscanf(s.data(), "%d,%d", &start.first, &start.second);
            map[Idx(start)] = '#';

            std::string arrow;
            while (iss >> arrow >> s)
            {
                PosT finish;
                sscanf(s.data(), "%d,%d", &finish.first, &finish.second);
                if (finish.first == start.first)
                {
                    int dy = (start.second - finish.second) / std::abs(finish.second - start.second);
                    for (int y = finish.second; y != start.second; y += dy)
                        map[Idx(start.first, y)] = '#';
                }
                else if (finish.second == start.second)
                {
                    int dx = (start.first - finish.first) / std::abs(finish.first - start.first);
                    for (int x = finish.first; x != start.first; x += dx)
                        map[Idx(x, start.second)] = '#';
                }
                start.swap(finish);
            }
        }

        CalcDimensions();
    }

    void CalcDimensions()
    {
        for (const auto &p : map)
        {
            int x = p.first >> 16;
            int y = p.first & 0xffff;
            if (x < left_edge)
                left_edge = x;
            if (x > right_edge)
                right_edge = x;
            if (y > depth)
                depth = y;
        }
    }

    void Print() const
    {
        for (int y = 0; y <= depth; ++y)
        {
            for (int x = left_edge; x <= right_edge; ++x)
            {
                auto it = map.find(Idx(x, y));
                std::cout << (it == map.end() ? '.' : it->second);
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
    }

    bool AddSand(bool floor = false)
    {
        auto canFall = [&](PosT p) -> bool {
            if (floor && p.second > depth + 1)
                return false;
            auto it = map.find(Idx(p));
            return it == map.end();
        };

        PosT p{500, 0};
        if (!canFall(p))
            return false;

        while (true)
        {
            if (!floor && p.second >= depth)
                return false;

            bool found_way{false};
            for (int dx : {0, -1, 1})
            {
                PosT next{p.first + dx, p.second + 1};
                if (canFall(next))
                {
                    p.swap(next);
                    found_way = true;
                    break;
                }
            }
            if (found_way)
                continue;

            map[Idx(p)] = 'o';
            ++sand;
            return true;
        }
    }

    int Task1()
    {
        //Print();
        while (AddSand(false))
            ;
        return sand;
    }

    int Task2()
    {
        while (AddSand(true))
            ;
        return sand;
    }
};

const char *const TEST =
    "498,4 -> 498,6 -> 496,6\n"
    "503,4 -> 502,4 -> 502,9 -> 494,9";

using namespace boost::ut;

suite s = [] {
    "14"_test = [] {
        Caves test{std::istringstream{TEST}};
        expect(24_i == test.Task1());
        expect(93_i == test.Task2());

        Caves caves{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", caves.Task1());
        Printer::Print(__FILE__, "2", caves.Task2());
    };
};

} //namespace;
