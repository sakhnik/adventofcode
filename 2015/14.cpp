#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "../test.hpp"
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

    int GetWinner2(int time)
    {
        std::vector<int> dist(_deers.size(), 0);
        std::vector<int> points(_deers.size(), 0);

        for (int t = 0; t < time; ++t)
        {
            for (size_t i = 0; i < _deers.size(); ++i)
            {
                const auto &d = _deers[i];
                if (d.IsRunning(t))
                {
                    dist[i] += d.speed;
                }
            }

            auto lead = *std::max_element(dist.begin(), dist.end());
            for (size_t i = 0; i < _deers.size(); ++i)
            {
                if (dist[i] == lead)
                    ++points[i];
            }
        }

        return *std::max_element(points.begin(), points.end());
    }

private:
    struct Deer
    {
        int speed;
        int dash;
        int rest;

        bool IsRunning(int time) const
        {
            auto phase = time % (dash + rest);
            return phase < dash;
        }

        int GetDistance(int time) const
        {
            auto d = std::div(time, dash + rest);
            auto progress = std::min(d.rem, dash);
            return (d.quot * dash + progress) * speed;
        }
    };
    std::vector<Deer> _deers;
};

using namespace boost::ut;

suite s = [] {
    "2015-14.test"_test = [] {
        const char *const TEST =
            "Comet can fly 14 km/s for 10 seconds, but then must rest for 127 seconds.\n"
            "Dancer can fly 16 km/s for 11 seconds, but then must rest for 162 seconds.\n";
        Race r(std::istringstream{TEST});
        expect(1120_i == r.GetWinner(1000));
        expect(689_i == r.GetWinner2(1000));
    };

    "2015-14.task"_test = [] {
        Race r(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", r.GetWinner(2503));
        Printer::Print(__FILE__, "2", r.GetWinner2(2503));
    };
};

} //namespace;