#include <fstream>
#include <iterator>
#include <numeric>
#include "../test.hpp"
namespace {

using namespace boost::ut;

suite s = [] {
    "2019-01"_test = [] {
        std::ifstream ifs{INPUT};
        using IterT = std::istream_iterator<int>;

        // Calculate fuel necessary for a given mass
        auto div3sub2 = [](auto mass) {
            return mass / 3 - 2;
        };

        // Sum of all fuel for all the ships
        auto fuel = std::accumulate(IterT{ifs}, IterT{}, 0,
                                    [=](auto fuel, auto mass) {
                                        return fuel + div3sub2(mass);
                                    });
        Printer::Print(__FILE__, "1", fuel);

        // Calculate fuel necessary including the fuel itself
        auto add_fuel = [=](auto mass) {
            auto res{0};
            while (true)
            {
                auto fuel = div3sub2(mass);
                if (fuel <= 0)
                {
                    break;
                }
                res += fuel;
                mass = fuel;
            }
            return res;
        };

        expect(966_i == add_fuel(1969));
        expect(50346_i == add_fuel(100756));

        std::ifstream ifs2(INPUT);
        auto total_fuel = std::accumulate(IterT{ifs2}, IterT{}, 0,
                                          [=](auto fuel, auto mass) {
                                              return fuel + add_fuel(mass);
                                          });
        Printer::Print(__FILE__, "2", total_fuel);
    };
};

} //namespace;