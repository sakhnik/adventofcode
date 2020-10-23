#include <vector>
#include <sstream>
#include <array>
#include <cmath>
#include <numeric>
#include "../test.hpp"

namespace {

const auto SIZE = 4;
const auto DIM = 3;

class Moons
{
public:
    using PosT = std::array<int64_t, DIM>;
    using MoonsT = std::vector<PosT>;

    Moons(const MoonsT &moons)
        : _pos(moons)
        , _vel(_pos.size())
    {
    }

    std::string Dump() const
    {
        std::ostringstream oss;
        for (const auto &m : _pos)
        {
            oss << m[0] << "," << m[1] << "," << m[2];
        }
        return oss.str();
    }

    void Simulate(int steps)
    {
        for (int dim = 0; dim < DIM; ++dim)
        {
            for (int i = 0; i < steps; ++i)
            {
                _Simulate1(dim);
            }
        }
    }

    int CalcTotalEnergy() const
    {
        int ret{};

        for (int i = 0; i < SIZE; ++i)
        {
            int pot{};
            int kin{};
            for (int dim = 0; dim < DIM; ++dim)
            {
                pot += std::abs(_pos[i][dim]);
                kin += std::abs(_vel[i][dim]);
            }
            ret += pot * kin;
        }

        return ret;
    }

    uint64_t FindPeriod()
    {
        std::array<int, DIM> periods{0};
        for (int dim = 0; dim < DIM; ++dim)
        {
            auto pos0 = _pos;
            auto vel0 = _vel;

            auto returned = [dim](const auto &a, const auto &b) {
                for (int i = 0; i != SIZE; ++i)
                {
                    if (a[i][dim] != b[i][dim])
                        return false;
                }
                return true;
            };

            do {
                _Simulate1(dim);
                ++periods[dim];
            } while (!returned(_pos, pos0) || !returned(_vel, vel0));
        }

        return std::accumulate(begin(periods), end(periods), 1ull,
                               [](auto a, auto b) { return std::lcm(a, b); });
    }

private:
    MoonsT _pos, _vel;

    void _Simulate1(int dim)
    {
        for (int i{0}, in{SIZE-1}; i < in; ++i)
        {
            for (int j{i+1}; j < SIZE; ++j)
            {
                auto d = _pos[i][dim] - _pos[j][dim];
                if (!d)
                {
                    continue;
                }
                auto dv = d > 0 ? -1 : 1;
                _vel[i][dim] += dv;
                _vel[j][dim] -= dv;
            }
        }

        for (int i = 0, in = SIZE; i < in; ++i)
        {
            _pos[i][dim] += _vel[i][dim];
        }
    }
};

using namespace boost::ut;

suite s = [] {
    "2019-12"_test = [] {
        {
            Moons test1({{-1, 0, 2}, {2, -10, -7}, {4, -8, 8}, {3, 5, -1}});
            test1.Simulate(10);
            expect(179_i == test1.CalcTotalEnergy());
        }

        Moons::MoonsT init{{-16, 15, -9}, {-14, 5, 4}, {2, 0, 6}, {-3, 18, 9}};

        Moons task{init};
        task.Simulate(1000);
        Printer::Print(__FILE__, "1", task.CalcTotalEnergy());
        Printer::Print(__FILE__, "2", task.FindPeriod());
    };
};

} //namespace;