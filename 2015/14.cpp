#include <doctest/doctest.h>
#include <fstream>
#include <sstream>
#include <vector>

namespace {

class Race
{
public:
    Race(std::istream &&is)
    {
        std::string line;
        while (is && getline(is, line))
        {
            char skip[64];
            Deer d;
            sscanf(line.data(),
                   "%s can fly %d km/s for %d seconds, but then must rest for %d seconds.",
                   skip, &d.speed, &d.dash, &d.rest);
            _deers.push_back(d);
        }
    }

    int GetWinner(int time)
    {
        int max_dist{};
        for (const auto &d : _deers)
        {
            auto dist = d.GetDistance(time);
            if (max_dist < dist)
                max_dist = dist;
        }
        return max_dist;
    }

private:
    struct Deer
    {
        int speed;
        int dash;
        int rest;

        int GetDistance(int time) const
        {
            auto d = std::div(time, dash + rest);
            auto progress = std::min(d.rem, dash);
            return (d.quot * dash + progress) * speed;
        }
    };
    std::vector<Deer> _deers;
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
        const char *const TEST =
            "Comet can fly 14 km/s for 10 seconds, but then must rest for 127 seconds.\n"
            "Dancer can fly 16 km/s for 11 seconds, but then must rest for 162 seconds.\n";
        Race r(std::istringstream{TEST});
        REQUIRE(1120 == r.GetWinner(1000));
    }

    SUBCASE("task") {
        Race r(std::ifstream{INPUT});
        MESSAGE(r.GetWinner(2503));
    }
}
