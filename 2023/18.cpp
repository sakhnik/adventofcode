#include "../test.hpp"
#include <fstream>
#include <boost/functional/hash.hpp>

namespace {

using namespace boost::ut;

struct Lagoon
{
    std::vector<std::string> map;
    struct Pos
    {
        int x{}, y{};
        bool operator==(const Pos &o) const = default;
    };
    static constexpr const auto posHash = [](Pos p) {
        size_t seed = p.x;
        boost::hash_combine(seed, p.y);
        return seed;
    };
    std::unordered_set<Pos, decltype(posHash)> lagoon{0, posHash};

    Lagoon(std::istream &&is)
    {
        Pos pos{};
        lagoon.insert(pos);
        decltype(lagoon) rims[2] = {};

        char dir{};
        std::string scolour;
        int count{};
        while (is >> dir >> count >> scolour)
        {
            switch (dir)
            {
            case 'U':
                while (count--)
                {
                    --pos.y;
                    lagoon.insert(pos);
                    rims[0].insert({pos.x - 1, pos.y});
                    rims[1].insert({pos.x + 1, pos.y});
                }
                break;
            case 'D':
                while (count--)
                {
                    ++pos.y;
                    lagoon.insert(pos);
                    rims[0].insert({pos.x + 1, pos.y});
                    rims[1].insert({pos.x - 1, pos.y});
                }
                break;
            case 'L':
                while (count--)
                {
                    --pos.x;
                    lagoon.insert(pos);
                    rims[0].insert({pos.x, pos.y + 1});
                    rims[1].insert({pos.x, pos.y - 1});
                }
                break;
            case 'R':
                while (count--)
                {
                    ++pos.x;
                    lagoon.insert(pos);
                    rims[0].insert({pos.x, pos.y - 1});
                    rims[0].insert({pos.x, pos.y + 1});
                }
                break;
            }
        }

        // The inner part is smaller than the outer
        auto &q = rims[0].size() < rims[1].size() ? rims[0] : rims[1];
        while (!q.empty())
        {
            auto cur_pos = *q.begin();
            q.erase(q.begin());
            if (lagoon.count(cur_pos))
                continue;
            lagoon.insert(cur_pos);
            auto enqueue = [&](Pos p) {
                if (!lagoon.count(p))
                    q.insert(p);
            };
            enqueue({cur_pos.x + 1, cur_pos.y});
            enqueue({cur_pos.x - 1, cur_pos.y});
            enqueue({cur_pos.x, cur_pos.y + 1});
            enqueue({cur_pos.x, cur_pos.y - 1});
        }
    }

    int Task1() const
    {
        return lagoon.size();
    }
};

suite s = [] {
    "2023-18"_test = [] {
        const char *const TEST1 = R"(R 6 (#70c710)
D 5 (#0dc571)
L 2 (#5713f0)
D 2 (#d2c081)
R 2 (#59c680)
D 2 (#411b91)
L 5 (#8ceee2)
U 2 (#caa173)
L 1 (#1b58a2)
U 2 (#caa171)
R 2 (#7807d2)
U 3 (#a77fa3)
L 2 (#015232)
U 2 (#7a21e3)
)";
        Lagoon test1{std::istringstream{TEST1}};
        expect(62_i == test1.Task1());
        //expect(94_i == test1.Task2());

        Lagoon lagoon{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", lagoon.Task1());
        //Printer::Print(__FILE__, "2", lagoon.Task2());
    };
};

} //namespace;
