#include "../test.hpp"

namespace {

using namespace boost::ut;

struct Pos
{
    int x{}, y{};
    bool operator==(const Pos &o) const = default;
    struct Hash
    {
        size_t operator()(const Pos &r) const
        {
            return std::hash<int>()(r.x + r.y * 1000);
        }
    };
};

struct Robot : Pos
{
    int vx{}, vy{};
};

using RobotsT = std::vector<Robot>;

void Simulate(RobotsT &robots, int width, int height, int count = 1)
{
    for (auto &r : robots) {
        if (r.vx < 0)
            r.vx = width + r.vx;
        if (r.vy < 0)
            r.vy = height + r.vy;
        r.x = (r.x + r.vx * count) % width;
        r.y = (r.y + r.vy * count) % height;
    }
}

bool HaveSamePosition(const RobotsT &robots)
{
    std::unordered_map<Pos, int, Pos::Hash> pos;
    for (auto &r : robots) {
        if (++pos[r] > 1)
            return true;
    }
    return false;
}

std::pair<int, int> Task(std::istream &&is, int width, int height)
{
    RobotsT robots;

    std::string line;
    while (getline(is, line)) {
        Robot r;
        sscanf(line.c_str(), "p=%d,%d v=%d,%d", &r.x, &r.y, &r.vx, &r.vy);
        robots.push_back(r);
    }

    int task1 = 0;
    {
        RobotsT robots1{robots};
        Simulate(robots1, width, height, 100);
        std::array<int, 4> quadrants = {};
        for (Pos &r : robots1) {
            if (r.x != width / 2 && r.y != height / 2) {
                int q = 2 * (2 * r.y / height) + (2 * r.x / width);
                ++quadrants[q];
            }
        }
        task1 = std::accumulate(quadrants.begin(), quadrants.end(), 1, [](int a, int q) { return a * q; });
    }

    int task2 = 1;
    {
        // Iterate until the robots don't collide
        for ( ; ; ++task2) {
            Simulate(robots, width, height, 1);
            if (!HaveSamePosition(robots))
                break;
        }
    }
    return {task1, task2};
}

suite s = [] {
    "14"_test = [] {
        const char *const TEST1 = R"(p=0,4 v=3,-3
p=6,3 v=-1,-3
p=10,3 v=-1,2
p=2,0 v=2,-1
p=0,0 v=1,3
p=3,0 v=-2,-2
p=7,6 v=-1,-3
p=3,0 v=-1,-2
p=9,3 v=2,3
p=7,3 v=-1,2
p=2,4 v=2,-3
p=9,5 v=-3,-3)";
        auto test = Task(std::istringstream{TEST1}, 11, 7);
        expect(12_i == test.first);

        auto task = Task(std::ifstream{INPUT}, 101, 103);
        Printer::Print(__FILE__, "1", task.first);
        Printer::Print(__FILE__, "2", task.second);
    };
};

} //namespace;
