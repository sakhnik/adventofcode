#include <doctest/doctest.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <regex>

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
};

} //namespace;

TEST_CASE(TEST_NAME)
{
    SUBCASE("test") {
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
        REQUIRE(7 == b.CountInRange());
    }

    SUBCASE("task") {
        Bots b(std::ifstream{INPUT});
        MESSAGE(b.CountInRange());
    }
}
