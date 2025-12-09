#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cassert>

struct Task
{
    uint64_t res1{}, res2{};

    Task(std::istream &&is)
    {
        struct Point
        {
            int x, y;
        };
        std::vector<Point> points;

        std::string line;
        while (std::getline(is, line)) {
            auto comma = line.find(',');
            points.emplace_back(std::stoi(line.substr(0, comma)), std::stoi(line.substr(comma + 1)));
        }

        struct Edge
        {
            int x1, y1, x2, y2;

            Edge(const Point &p1, const Point &p2)
                : x1{std::min(p1.x, p2.x)}
                , y1{std::min(p1.y, p2.y)}
                , x2{std::max(p1.x, p2.x)}
                , y2{std::max(p1.y, p2.y)}
            {
            }
        };
        std::vector<Edge> edges;
        for (int i = 0; i < points.size(); ++i) {
            const auto &p1 = points[i];
            const auto &p2 = points[(i + 1) % points.size()];
            edges.emplace_back(p1, p2);
        }

        auto intersects = [&](int x1, int y1, int x2, int y2) -> bool {
            for (const auto &e : edges) {
                if (x1 < e.x2 && x2 > e.x1 && y1 < e.y2 && y2 > e.y1)
                    return true;
            }
            return false;
        };

        for (int i = 1; i < points.size(); ++i) {
            for (int j = 0; j < i; ++j) {
                const auto &p1 = points[i];
                const auto &p2 = points[j];

                int x1 = std::min(p1.x, p2.x);
                int y1 = std::min(p1.y, p2.y);
                int x2 = std::max(p1.x, p2.x);
                int y2 = std::max(p1.y, p2.y);

                uint64_t s1 = (1ull + x2 - x1) * (1ull + y2 - y1);
                res1 = std::max(res1, s1);

                if (!intersects(x1, y1, x2, y2)) {
                    res2 = std::max(res2, s1);
                }
            }
        }
    }
};

int main()
{
    const char *const TEST =
        "7,1\n"
        "11,1\n"
        "11,7\n"
        "9,7\n"
        "9,5\n"
        "2,5\n"
        "2,3\n"
        "7,3";
    Task test{std::istringstream{TEST}};
    assert(test.res1 == 50);
    assert(test.res2 == 24);

    Task task{std::ifstream{"09.txt"}};
    std::cout << task.res1 << std::endl;
    std::cout << task.res2 << std::endl;

    return 0;
}
