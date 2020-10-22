#include <vector>
#include <sstream>
#include <fstream>
#include <limits>
#include <sstream>

#include "../test.hpp"

// The idea is to search for the configuration with the smallest
// bounding box. This may fail if there is a rogue light, not participating
// in the signage. But let's try.

namespace {

struct Vec
{
    int x, y;
};

struct Light
{
    Vec position;
    Vec velocity;
};

struct Box
{
    Vec topleft;
    Vec bottomright;

    int Area() const
    {
        return (bottomright.x - topleft.x) * (bottomright.y - topleft.y);
    }

    int GetWidth() const
    {
        return bottomright.x - topleft.x + 1;
    }

    int GetHeight() const
    {
        return bottomright.y - topleft.y + 1;
    }
};

class Sky
{
public:
    Sky(std::istream &&is)
    {
        std::string line;
        while (is && getline(is, line))
        {
            Light l;
            sscanf(line.c_str(), "position=<%d,%d> velocity=<%d,%d>",
                   &l.position.x, &l.position.y, &l.velocity.x, &l.velocity.y);
            _lights.push_back(l);
        }
    }

    Box GetBoundingBox() const
    {
        auto MAX = std::numeric_limits<int>::max();
        auto MIN = std::numeric_limits<int>::min();
        Box box = {
            .topleft = { MAX, MAX },
            .bottomright = { MIN, MIN },
        };
        for (auto l : _lights)
        {
            if (l.position.x < box.topleft.x) box.topleft.x = l.position.x;
            if (l.position.y < box.topleft.y) box.topleft.y = l.position.y;
            if (l.position.x > box.bottomright.x) box.bottomright.x = l.position.x;
            if (l.position.y > box.bottomright.y) box.bottomright.y = l.position.y;
        }
        return box;
    }

    void Evolve(int dt)
    {
        for (auto &l : _lights)
        {
            l.position.x += l.velocity.x * dt;
            l.position.y += l.velocity.y * dt;
        }
    }

    void Print(std::ostream &os) const
    {
        auto box = GetBoundingBox();
        auto width = box.GetWidth();
        auto height = box.GetHeight();
        std::vector<char> display(width * height, '.');

        auto idx = [width](auto x, auto y) {
            return y * width + x;
        };
        auto set = [&](int x, int y) {
            display.at(idx(x - box.topleft.x, y - box.topleft.y)) = '#';
        };

        for (auto l : _lights)
        {
            set(l.position.x, l.position.y);
        }

        for (int h = 0; h < height; ++h)
        {
            for (int w = 0; w < width; ++w)
            {
                os << display[idx(w, h)];
            }
            os << "\n";
        }
    }

    int SearchMin()
    {
        auto prev_area = GetBoundingBox().Area();

        for (int i = 0; ; ++i)
        {
            Evolve(1);
            auto area = GetBoundingBox().Area();
            if (area < 80*25 && area > prev_area)
            {
                Evolve(-1);
                return i;
            }
            prev_area = area;
        }
    }

private:
    std::vector<Light> _lights;
};

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2018-10"_test = [] {
        const auto TEST = R"(position=< 9,  1> velocity=< 0,  2>
position=< 7,  0> velocity=<-1,  0>
position=< 3, -2> velocity=<-1,  1>
position=< 6, 10> velocity=<-2, -1>
position=< 2, -4> velocity=< 2,  2>
position=<-6, 10> velocity=< 2, -2>
position=< 1,  8> velocity=< 1, -1>
position=< 1,  7> velocity=< 1,  0>
position=<-3, 11> velocity=< 1, -2>
position=< 7,  6> velocity=<-1, -1>
position=<-2,  3> velocity=< 1,  0>
position=<-4,  3> velocity=< 2,  0>
position=<10, -3> velocity=<-1,  1>
position=< 5, 11> velocity=< 1, -2>
position=< 4,  7> velocity=< 0, -1>
position=< 8, -2> velocity=< 0,  1>
position=<15,  0> velocity=<-2,  0>
position=< 1,  6> velocity=< 1,  0>
position=< 8,  9> velocity=< 0, -1>
position=< 3,  3> velocity=<-1,  1>
position=< 0,  5> velocity=< 0, -1>
position=<-2,  2> velocity=< 2,  0>
position=< 5, -2> velocity=< 1,  2>
position=< 1,  4> velocity=< 2,  1>
position=<-2,  7> velocity=< 2, -2>
position=< 3,  6> velocity=<-1, -1>
position=< 5,  0> velocity=< 1,  0>
position=<-6,  0> velocity=< 2,  0>
position=< 5,  9> velocity=< 1, -2>
position=<14,  7> velocity=<-2,  0>
position=<-3,  6> velocity=< 2, -1>)";
        Sky test(std::istringstream{TEST});

        std::ostringstream oss;
        expect(3_i == test.SearchMin());
        test.Print(oss);
        expect(eq(oss.str(), R"(#...#..###
#...#...#.
#...#...#.
#####...#.
#...#...#.
#...#...#.
#...#...#.
#...#..###
)"s));

        Sky input(std::ifstream{INPUT});
        Printer::Print(__FILE__, "1", input.SearchMin());
        std::ostringstream oss2;
        input.Print(oss2);
        Printer::Print(__FILE__, "2", oss2.str());
    };
};

} //namespace;