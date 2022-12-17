#include "../test.hpp"
#include <fstream>
#include <boost/circular_buffer.hpp>

namespace {

struct Rocks
{
    using ShapeT = std::vector<std::pair<int, int>>;
    std::vector<ShapeT> shapes;

    std::string winds;
    boost::circular_buffer<std::string> well{1000};
    int height{0};

    Rocks(std::istream &&is)
    {
        shapes.push_back({{0, 0}, {1, 0}, {2, 0}, {3, 0}});
        shapes.push_back({{1, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 2}});
        shapes.push_back({{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}});
        shapes.push_back({{0, 0}, {0, 1}, {0, 2}, {0, 3}});
        shapes.push_back({{0, 0}, {1, 0}, {0, 1}, {1, 1}});

        std::getline(is, winds);
    }

    int Run(int count)
    {
        int rock = 0;
        int jet = 0;

        while (count--)
        {
            const auto &shape = shapes[rock++];
            if (rock >= shapes.size())
                rock = 0;

            auto empty_space = FindEmptySpaceOnTop();
            int new_lines = 7 - empty_space;
            for (int i = 0; i < new_lines; ++i)
                well.push_back("       ");
            height += new_lines;

            int shiftX = 2;
            int shiftY = 0;

            auto getX = [&](int x) {
                return x + shiftX;
            };
            auto getIdx = [&](int y) {
                return well.size() + y - shiftY - 4;
            };

            auto fits = [&] {
                for (const auto &r : shape)
                {
                    int x = getX(r.first);
                    if (x < 0 || x >= 7)
                        return false;
                    int idx = getIdx(r.second);
                    if (idx < 0)
                        return false;
                    if (well[idx][x] != ' ')
                        return false;
                }
                return true;
            };

            while (true)
            {
                int dx = winds[jet++] == '<' ? -1 : 1;
                if (jet >= winds.size())
                    jet = 0;

                shiftX += dx;
                if (!fits())
                    shiftX -= dx;

                shiftY += 1;
                if (!fits())
                {
                    shiftY -= 1;
                    for (const auto &r : shape)
                        well[getIdx(r.second)][getX(r.first)] = '#';
                    break;
                }
            }

        }

        return height - FindEmptySpaceOnTop();
    }

    int FindEmptySpaceOnTop() const
    {
        for (int i = 0; i < well.size(); ++i)
        {
            if (well[well.size() - 1 - i] != "       ")
                return i;
        }
        return well.size();
    }
};

const char *const TEST = ">>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>";

using namespace boost::ut;

suite s = [] {
    "2022-17"_test = [] {
        Rocks test{std::istringstream{TEST}};
        expect(3068_i == test.Run(2022));

        Rocks rocks{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", rocks.Run(2022));
    };
};

} //namespace;
