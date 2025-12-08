#include <cstdint>
#include <vector>
#include <cassert>
#include <algorithm>
#include <queue>
#include <iostream>
#include <numeric>
#include <fstream>
#include <sstream>

struct Task
{
    uint64_t res1{}, res2{};

    Task(std::istream &&is, int K)
    {
        struct Point
        {
            int x, y, z;
        };
        std::vector<Point> points;

        auto getDist = [&](int i, int j) {
            double dx = points[i].x - points[j].x;
            double dy = points[i].y - points[j].y;
            double dz = points[i].z - points[j].z;
            return dx*dx + dy*dy + dz*dz;
        };

        std::string line;
        while (std::getline(is, line)) {
            std::istringstream iss{line};
            Point p;
            char comma;
            iss >> p.x >> comma >> p.y >> comma >> p.z;
            points.push_back(p);
        }

        struct Edge
        {
            double d;
            int i, j;
        };
        std::vector<Edge> edges;
        for (int i = 1; i < points.size(); ++i) {
            for (int j = 0; j < i; ++j) {
                edges.emplace_back(getDist(i, j), i, j);
            }
        }

        std::sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b) { return a.d < b.d; });


        int compCount = points.size();
        std::vector<int> parents(compCount);
        std::iota(parents.begin(), parents.end(), 0);
        std::vector<int> sizes(compCount, 1);

        auto djFind = [&](int i) {
            for (; parents[i] != i; i = parents[i]) {
                parents[i] = parents[parents[i]];
            }
            return i;
        };

        auto djUnion = [&](int i, int j) {
            i = djFind(i);
            j = djFind(j);
            if (i == j)
                return false;
            if (sizes[i] < sizes[j])
                std::swap(i, j);
            parents[j] = i;
            sizes[i] += sizes[j];
            --compCount;
            return true;
        };

        for (int i = 0; i < K; ++i) {
            djUnion(edges[i].i, edges[i].j);
        }

        std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
        for (int i = 0; i < points.size(); ++i) {
            if (i == parents[i]) {
                pq.push(sizes[i]);
                if (pq.size() > 3) {
                    pq.pop();
                }
            }
        }
        res1 = 1;
        while (!pq.empty()) {
            res1 *= pq.top();
            pq.pop();
        }

        for (int i = K; i < edges.size(); ++i) {
            auto p1 = edges[i].i;
            auto p2 = edges[i].j;
            if (djUnion(p1, p2) && compCount == 1) {
                res2 = uint64_t(points[p1].x) * points[p2].x;
                break;
            }
        }
    }
};

int main()
{
    const char *const TEST =
        "162,817,812\n"
        "57,618,57\n"
        "906,360,560\n"
        "592,479,940\n"
        "352,342,300\n"
        "466,668,158\n"
        "542,29,236\n"
        "431,825,988\n"
        "739,650,466\n"
        "52,470,668\n"
        "216,146,977\n"
        "819,987,18\n"
        "117,168,530\n"
        "805,96,715\n"
        "346,949,466\n"
        "970,615,88\n"
        "941,993,340\n"
        "862,61,35\n"
        "984,92,344\n"
        "425,690,689\n";
    Task test{std::istringstream{TEST}, 10};
    assert(test.res1 == 40);
    assert(test.res2 == 25272);

    Task task{std::ifstream{"08.txt"}, 1000};
    std::cout << task.res1 << std::endl;
    std::cout << task.res2 << std::endl;
    return 0;
}
