#include "../test.hpp"
#include <fstream>
#include <boost/circular_buffer.hpp>

namespace {

struct Rocks
{
    using ShapeT = std::vector<std::pair<int, int>>;
    std::vector<ShapeT> shapes;

    std::string winds;
    boost::circular_buffer<std::string> well{2048};
    size_t height{0};
    std::vector<size_t> heights = {0};
    size_t offset{};

    Rocks(std::istream &&is)
    {
        shapes.push_back({{0, 0}, {1, 0}, {2, 0}, {3, 0}});
        shapes.push_back({{1, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 2}});
        shapes.push_back({{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}});
        shapes.push_back({{0, 0}, {0, 1}, {0, 2}, {0, 3}});
        shapes.push_back({{0, 0}, {1, 0}, {0, 1}, {1, 1}});

        std::getline(is, winds);

        Run();
    }

    void Run()
    {
        int rock = 0;
        int jet = 0;

        auto get_signature = [&]() -> std::string {
            std::ostringstream oss;
            oss << rock << "\n";
            oss << jet << "\n";
            for (int i = 0; i < 7; ++i)
            {
                auto it = std::find_if(well.rbegin(), well.rend(),
                        [&](const auto &row) { return row[i] == '#'; });
                oss << " " << (it == well.rend() ? -1 : it - well.rbegin());
            }
            oss << "\n";
            return oss.str();
        };

        std::unordered_map<std::string, size_t> signatures;

        while (true)
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

            int dh = FindEmptySpaceOnTop();
            heights.push_back(height - dh);
            auto signature = get_signature();
            auto it = signatures.find(signature);
            if (it != signatures.end())
            {
                offset = it->second;
                signatures.clear();
                return;
            }
            signatures[signature] = heights.size();
        }
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

    size_t CalcHeight(size_t count)
    {
        if (count < offset)
            return heights[count];
        size_t period = heights.size() - offset;
        count -= offset;
        size_t periods = count / period;
        size_t rest = count % period;
        return (heights.back() - heights[offset - 1]) * periods + heights[rest + offset];
    }
};

const char *const TEST = ">>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>";

using namespace boost::ut;

suite s = [] {
    "17"_test = [] {
        Rocks test{std::istringstream{TEST}};
        expect(3068_u == test.CalcHeight(2022));
        expect(eq(1514285714288ull, test.CalcHeight(1000000000000ull)));

        Rocks rocks{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", rocks.CalcHeight(2022));
        Printer::Print(__FILE__, "2", rocks.CalcHeight(1000000000000ull));
    };
};

} //namespace;
