#include <sstream>
#include <fstream>
#include <vector>
#include <regex>
#include "../test.hpp"

namespace {

class Bots
{
public:
    using IntT = int64_t;

    Bots(std::istream &&is)
    {
        std::string line;
        while (is && getline(is, line))
        {
            const static std::regex r{R"(pos=<([0-9-]+),([0-9-]+),([0-9-]+)>, r=([0-9-]+))"};
            std::smatch m;
            if (regex_match(line, m, r))
            {
                auto &b = _bots.emplace_back();
                b.pos.x = std::stoi(m[1]);
                b.pos.y = std::stoi(m[2]);
                b.pos.z = std::stoi(m[3]);
                b.radius = std::stoi(m[4]);
            }
        }
    }

    size_t CountInRange() const
    {
        auto it = std::max_element(begin(_bots), end(_bots),
                                   [](const auto &a, const auto &b) {
                                        return a.radius < b.radius;
                                   });
        return std::count_if(begin(_bots), end(_bots),
                             [it](const auto &b) {
                                return it->pos.Dist(b.pos) <= it->radius;
                             });
    }

    IntT Find() const
    {
        auto itX = std::minmax_element(begin(_bots), end(_bots),
                                       [](const auto &a, const auto &b) { return a.pos.x < b.pos.x; });
        auto itY = std::minmax_element(begin(_bots), end(_bots),
                                       [](const auto &a, const auto &b) { return a.pos.y < b.pos.y; });
        auto itZ = std::minmax_element(begin(_bots), end(_bots),
                                       [](const auto &a, const auto &b) { return a.pos.z < b.pos.z; });

        IntT minX{itX.first->pos.x};
        IntT maxX{itX.second->pos.x};
        IntT minY{itY.first->pos.y};
        IntT maxY{itY.second->pos.y};
        IntT minZ{itZ.first->pos.z};
        IntT maxZ{itZ.second->pos.z};

        auto dx = maxX - minX;
        IntT prec{1};
        while (prec < dx)
            prec *= 2;

        _Pos origin{0, 0, 0};

        while (true)
        {
            int max_count{0};
            _Pos position{};

            for (auto x{minX}; x <= maxX; x += prec)
            {
                for (auto y{minY}; y <= maxY; y += prec)
                {
                    for (auto z{minZ}; z <= maxZ; z += prec)
                    {
                        _Pos p{x, y, z};
                        auto count = count_if(begin(_bots), end(_bots),
                                              [prec, &p](const _Bot &b) {
                                                return (b.pos.Dist(p) - b.radius) / prec <= 0;
                                              });
                        if (count > max_count)
                        {
                            max_count = count;
                            position = p;
                        }
                        else if (count == max_count && position.Dist(origin) < p.Dist(origin))
                        {
                            position = p;
                        }
                    }
                }
            }

            if (prec == 1)
            {
                return position.Dist(origin);
            }

            minX = position.x - prec;
            maxX = position.x + prec;
            minY = position.y - prec;
            maxY = position.y + prec;
            minZ = position.z - prec;
            maxZ = position.z + prec;
            prec /= 2;
        }
    }

private:
    struct _Pos
    {
        IntT x, y, z;
        IntT Dist(const _Pos &o) const
        {
            return std::abs(o.x - x) + std::abs(o.y - y) + std::abs(o.z - z);
        }
    };
    struct _Bot
    {
        _Pos pos;
        IntT radius;
    };
    std::vector<_Bot> _bots;

    size_t _CountInRange(const _Pos &p) const
    {
        return std::count_if(begin(_bots), end(_bots),
                             [&](const auto &b) {
                                return p.Dist(b.pos) <= b.radius;
                             });
    }
};

using namespace boost::ut;

suite s = [] {
    "2018-23"_test = [] {
        {
            const char *const TEST =
                "pos=<0,0,0>, r=4\n"
                "pos=<1,0,0>, r=1\n"
                "pos=<4,0,0>, r=3\n"
                "pos=<0,2,0>, r=1\n"
                "pos=<0,5,0>, r=3\n"
                "pos=<0,0,3>, r=1\n"
                "pos=<1,1,1>, r=1\n"
                "pos=<1,1,2>, r=1\n"
                "pos=<1,3,1>, r=1\n";
            Bots b(std::istringstream{TEST});
            expect(7_u == b.CountInRange());
        }

        Bots b(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", b.CountInRange());
        Printer::Print(__FILE__, "2", b.Find());
    };
};

} //namespace;