#include <doctest/doctest.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <array>


namespace {

class Moons
{
public:
    const static auto DIM = 3;
    using PosT = std::array<int, DIM>;

    Moons(std::initializer_list<PosT> l)
        : _pos(l)
        , _vel(_pos.size())
    {
    }

    std::string Dump() const
    {
        std::ostringstream oss;
        for (const auto &m : _pos)
        {
            oss << "<" << m[0] << "," << m[1] << "," << m[2] << ">";
        }
        return oss.str();
    }

    void Simulate(int steps)
    {
        while (--steps >= 0)
        {
            for (int i = 0; i < _pos.size(); ++i)
            {
                for (int j = 0; j < DIM; ++j)
                {
                    int dv{};
                    for (int k = 0; k < _pos.size(); ++k)
                    {
                        if (_pos[i][j] == _pos[k][j])
                        {
                            continue;
                        }
                        dv += _pos[i][j] < _pos[k][j] ? 1 : -1;
                    }
                    _vel[i][j] += dv;
                }
            }

            for (int i = 0, in = _pos.size(); i < in; ++i)
            {
                for (int j = 0; j < DIM; ++j)
                {
                    _pos[i][j] += _vel[i][j];
                }
            }
        }
    }

    int CalcTotalEnergy() const
    {
        int ret{};

        for (int i = 0; i < _pos.size(); ++i)
        {
            int pot{};
            int kin{};
            for (int j = 0; j < DIM; ++j)
            {
                pot += std::abs(_pos[i][j]);
                kin += std::abs(_vel[i][j]);
            }
            ret += pot * kin;
        }

        return ret;
    }

private:
    using MoonsT = std::vector<PosT>;
    MoonsT _pos, _vel;
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    {
        Moons test1{{-1, 0, 2}, {2, -10, -7}, {4, -8, 8}, {3, 5, -1}};
        test1.Simulate(10);
        REQUIRE(179 == test1.CalcTotalEnergy());
    }

    Moons task1{{-16, 15, -9}, {-14, 5, 4}, {2, 0, 6}, {-3, 18, 9}};
    task1.Simulate(1000);
    MESSAGE(task1.CalcTotalEnergy());

}
