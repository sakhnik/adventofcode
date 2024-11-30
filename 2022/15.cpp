#include "../test.hpp"
#include <fstream>

namespace {

struct Ranges
{
    std::map<int, int> ranges;

    void Add(int left, int right)
    {
        ++ranges[left];
        --ranges[right];
    }

    int Count() const
    {
        int res{};
        int prev{};
        int count{0};
        for (auto [p, c] : ranges)
        {
            if (!count)
            {
                prev = p;
                count = c;
                continue;
            }
            count += c;
            if (!count)
            {
                res += p - prev;
                count = 0;
            }
        }
        return res;
    }
};

struct Sensors
{
    using PosT = std::pair<int, int>;
    std::vector<PosT> sensors;
    std::vector<PosT> beacons;

    Sensors(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line))
        {
            PosT sensor{}, beacon{};
            sscanf(line.c_str(), "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d",
                    &sensor.first, &sensor.second, &beacon.first, &beacon.second);
            sensors.push_back(sensor);
            beacons.push_back(beacon);
        }
    }

    int CountOccupied(int y)
    {
        Ranges ranges;

        for (int i = 0; i != sensors.size(); ++i)
        {
            const auto &sensor = sensors[i];
            const auto &beacon = beacons[i];

            int dist = std::abs(sensor.first - beacon.first) + std::abs(sensor.second - beacon.second);
            int dist2 = std::abs(sensor.second - y);
            if (dist2 > dist)
                continue;
            int d = dist - dist2;

            ranges.Add(sensor.first - d, sensor.first + d);
        }

        return ranges.Count();
    }
};

const char *const TEST = R"(Sensor at x=2, y=18: closest beacon is at x=-2, y=15
Sensor at x=9, y=16: closest beacon is at x=10, y=16
Sensor at x=13, y=2: closest beacon is at x=15, y=3
Sensor at x=12, y=14: closest beacon is at x=10, y=16
Sensor at x=10, y=20: closest beacon is at x=10, y=16
Sensor at x=14, y=17: closest beacon is at x=10, y=16
Sensor at x=8, y=7: closest beacon is at x=2, y=10
Sensor at x=2, y=0: closest beacon is at x=2, y=10
Sensor at x=0, y=11: closest beacon is at x=2, y=10
Sensor at x=20, y=14: closest beacon is at x=25, y=17
Sensor at x=17, y=20: closest beacon is at x=21, y=22
Sensor at x=16, y=7: closest beacon is at x=15, y=3
Sensor at x=14, y=3: closest beacon is at x=15, y=3
Sensor at x=20, y=1: closest beacon is at x=15, y=3
)";

using namespace boost::ut;

suite s = [] {
    "15"_test = [] {
        Ranges ranges;
        expect(0_i == ranges.Count());
        ranges.Add(0, 5);
        expect(5_i == ranges.Count());
        ranges.Add(6, 10);
        expect(9_i == ranges.Count());
        ranges.Add(5, 6);
        expect(10_i == ranges.Count());
        ranges.Add(3, 9);
        expect(10_i == ranges.Count());

        Sensors test{std::istringstream{TEST}};
        expect(26_i == test.CountOccupied(10));

        Sensors sensors{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", sensors.CountOccupied(2000000));
    };
};

} //namespace;
