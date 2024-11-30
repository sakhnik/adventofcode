#include "../test.hpp"
#include <fstream>
#include <boost/functional/hash.hpp>

namespace {

struct Diffusion
{
    using PosT = std::array<int, 2>;

    struct PosHash
    {
        size_t operator()(const PosT &p) const
        {
            size_t seed{};
            for (auto x : p)
                boost::hash_combine(seed, x);
            return seed;
        }
    };

    std::unordered_set<PosT, PosHash> elves;

    Diffusion(std::istream &&is)
    {
        std::string line;
        int row{};
        while (std::getline(is, line))
        {
            for (int col = 0; col < line.size(); ++col)
            {
                if (line[col] == '#')
                    elves.insert({row, col});
            }
            ++row;
        }
    }

    std::pair<PosT, PosT> GetBoundingBox() const
    {
        PosT min{*elves.begin()}, max{*elves.begin()};
        for (auto p : elves)
        {
            min[0] = std::min(min[0], p[0]);
            min[1] = std::min(min[1], p[1]);
            max[0] = std::max(max[0], p[0]);
            max[1] = std::max(max[1], p[1]);
        }
        return {min, max};
    }

    void Print() const
    {
        auto [min, max] = GetBoundingBox();
        for (int r = min[0]; r <= max[0]; ++r)
        {
            for (int c = min[1]; c <= max[1]; ++c)
                std::cout << (elves.contains({r, c}) ? '#' : '.');
            std::cout << std::endl;
        }
        std::cout << "----" << std::endl;
    }

    int task1{};

    std::pair<int, int> Simulate()
    {
        int phase{};
        std::unordered_map<PosT, std::vector<PosT>, PosHash> new_positions;

        auto has_neighbour = [&](PosT p) {
            for (int dr = -1; dr <= 1; ++dr)
                for (int dc = -1; dc <= 1; ++dc)
                {
                    if ((dr || dc) && elves.contains({p[0] + dr, p[1] + dc}))
                        return true;
                }
            return false;
        };

        auto can_move = [&](PosT p, int r, int c) {
            if (!r)
            {
                for (int dr = -1; dr <= 1; ++dr)
                    if (elves.contains({p[0] + dr, p[1] + c}))
                        return false;
                return true;
            }
            for (int dc = -1; dc <= 1; ++dc)
                if (elves.contains({p[0] + r, p[1] + dc}))
                    return false;
            return true;
        };

        int round{};
        while (++round)
        {
            //Print();
            int updates{};
            for (const auto &p : elves)
            {
                if (!has_neighbour(p))
                {
                    new_positions[p].push_back(p);
                    continue;
                }

                const PosT MOVES[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
                auto find_move = [&]() {
                    for (int i = 0; i < 4; ++i)
                    {
                        auto move = (phase + i) % 4;
                        if (can_move(p, MOVES[move][0], MOVES[move][1]))
                            return move;
                    }
                    return -1;
                };

                auto m = find_move();
                if (-1 == m)
                {
                    new_positions[p].push_back(p);
                    continue;
                }

                ++updates;
                auto move = MOVES[m];
                new_positions[{p[0] + move[0], p[1] + move[1]}].push_back(p);
            }
            if (!updates)
                break;
            // move actually
            elves.clear();
            for (const auto &[next, prev] : new_positions)
            {
                if (prev.size() == 1)
                    elves.insert(next);
                else
                    elves.insert(prev.begin(), prev.end());
            }
            new_positions.clear();
            phase = (phase + 1) % 4;

            if (round == 10)
            {
                auto [min, max] = GetBoundingBox();
                int area = (max[0] - min[0] + 1) * (max[1] - min[1] + 1);
                task1 = area - elves.size();
            }
        }
        //Print();

        return {task1, round};
    }
};

const char *const TEST = 
    "....#..\n"
    "..###.#\n"
    "#...#.#\n"
    ".#...##\n"
    "#.###..\n"
    "##.#.##\n"
    ".#..#..\n";

using namespace boost::ut;

suite s = [] {
    "23"_test = [] {
        Diffusion test{std::istringstream{TEST}};
        auto test_res = test.Simulate();
        expect(110_i == test_res.first);
        expect(20_i == test_res.second);

        if (Printer::EnableAll())
        {
            Diffusion diffusion{std::ifstream{INPUT}};
            auto res = diffusion.Simulate();
            Printer::Print(__FILE__, "1", res.first);
            Printer::Print(__FILE__, "2", res.second);
        }
    };
};

} //namespace;
