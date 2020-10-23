#include <sstream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cassert>
#include <list>
#include <unordered_map>
#include <numeric>
#include "../test.hpp"

namespace {

class Battle
{
    int _round = 0;

public:
    Battle(std::istream &&is, int elf_attack = 3)
        : Battle(is, elf_attack)
    {
    }

    Battle(std::istream &is, int elf_attack = 3)
        : _elf_attack(elf_attack)
    {
        std::string line;
        while (is && getline(is, line))
        {
            for (size_t i = 0; i < line.size(); ++i)
            {
                char ch = line[i];
                if (ch == 'E' || ch == 'G')
                {
                    int attack = ch == 'E' ? elf_attack : 3;
                    _units.emplace_back(_map.size(), i, ch, attack);
                    ++_counts[ch];
                }
            }
            _map.emplace_back(std::move(line));
        }
    }

    Battle(const Battle &o) = default;

    int GetElfAttack() const
    {
        return _elf_attack;
    }

    static Battle Task2(std::istream &ifs)
    {
        for (int attack = 3; ; ++attack)
        {
            ifs.clear();
            ifs.seekg(0, std::ios::beg);
            Battle b(ifs, attack);
            int elf_count = b.GetElfCount();
            if ('E' == b.RunUntilFinish(false) && elf_count == b.GetElfCount())
                return b;
        }
    }

    int GetRound() const
    {
        return _round;
    }

    int64_t GetScore() const
    {
        auto it = _counts.find('G');
        auto it2 = _counts.find('E');
        if (it->second && it2->second)
            return 0;

        return std::accumulate(begin(_units), end(_units), 0,
                               [](int acc, const auto &u) { return acc + u.hp; });
    }

    int GetElfCount() const
    {
        auto it = _counts.find('E');
        return it->second;
    }

    int64_t GetOutcome() const
    {
        return _round * GetScore();
    }

    char GetWinner()
    {
        auto it1 = _counts.find('G');
        auto it2 = _counts.find('E');
        if (it1->second && it2->second)
            return 0;
        if (it1->second)
            return it1->first;
        return it2->first;
    }

    char RunUntilFinish(bool visual)
    {
        while (true)
        {
            if (visual)
            {
                ::system("clear");
                std::cout << "Round " << _round << "\n";
                std::cout << Dump() << std::endl;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            Round();
            auto winner = GetWinner();
            if (winner)
                return winner;
        }
    }

    std::string Dump()
    {
        _SortUnits();
        auto it = _units.begin();
        std::ostringstream oss;
        for (int row = 0; row < int(_map.size()); ++row)
        {
            oss << _map[row];
            oss << "   ";
            const char *comma = "";
            for (; it != _units.end() && it->row == row; ++it)
            {
                oss << comma;
                oss << it->kind << "(" << it->hp << ")";
                comma = ", ";
            }
            oss << "\n";
        }
        return oss.str();
    }

    void Round()
    {
        _SortUnits();
        for (auto it = begin(_units); it != end(_units); ++it)
        {
            _Move(*it);
            if (_Attack(*it))
            {
                return;
            }
        }
        ++_round;
    }

private:
    int _elf_attack;
    std::vector<std::string> _map;
    std::unordered_map<char, int> _counts;
    struct Unit
    {
        Unit(int row, int col, char kind, int attack)
            : row(row), col(col), kind(kind), attack(attack)
        {}
        int row, col;
        char kind;
        int attack;
        int hp = 200;
    };
    std::list<Unit> _units;

    void _SortUnits()
    {
        _units.sort(
            [](const Unit &a, const Unit &b) {
                if (a.row < b.row) return true;
                if (a.row > b.row) return false;
                return a.col < b.col;
        });
    }

    // Return distancd
    int _Move(Unit &unit)
    {
        const auto MAX_INT = std::numeric_limits<int>::max();

        int height = _map.size();
        int width = _map.front().size();
        auto idx = [=](int row, int col) {
            return width * row + col;
        };
        auto getRow = [=](int idx) {
            return idx / width;
        };
        auto getCol = [=](int idx) {
            return idx % width;
        };

        struct Way
        {
            bool is_target = false;
            int from = -1;
            int dist = MAX_INT;
        };
        Way marks[width * height];
        for (auto &w : marks)
            w = Way{};

        for (const auto &u : _units)
        {
            auto checkTarget = [&](int row, int col) {
                auto ch = _map[row][col];
                if (ch == '#' || ch == unit.kind)
                    return;
                if (ch != '.')
                    return;
                marks[idx(row, col)].is_target = true;
            };
            if (u.kind != unit.kind)
            {
                if (std::abs(u.row - unit.row) + std::abs(u.col - unit.col) == 1)
                    return 0;  // already in the target position
                checkTarget(u.row - 1, u.col);
                checkTarget(u.row, u.col - 1);
                checkTarget(u.row, u.col + 1);
                checkTarget(u.row + 1, u.col);
            }
        }

        auto idx0 = idx(unit.row, unit.col);
        marks[idx0] = Way{false, -1, 0};
        std::queue<int> q;
        q.push(idx0);

        std::vector<int> targets;
        int targetDist = std::numeric_limits<int>::max();

        while (!q.empty())
        {
            int cur_idx = q.front();
            q.pop();

            auto try_move = [&](int from_idx, int to_idx) {
                if (marks[to_idx].dist != MAX_INT)
                    return;
                marks[to_idx].dist = marks[from_idx].dist + 1;
                marks[to_idx].from = from_idx;
                if (marks[to_idx].dist > targetDist)
                    return;

                int row = getRow(to_idx);
                int col = getCol(to_idx);
                char ch = _map[row][col];
                if (ch == '.')
                {
                    q.push(to_idx);
                    if (marks[to_idx].is_target)
                    {
                        if (targets.empty())
                        {
                            targetDist = marks[to_idx].dist;
                        }
                        targets.push_back(to_idx);
                    }
                }
            };

            try_move(cur_idx, cur_idx - width);
            try_move(cur_idx, cur_idx - 1);
            try_move(cur_idx, cur_idx + 1);
            try_move(cur_idx, cur_idx + width);
        }

        if (targets.empty())
            return -1;

        int target = *std::min_element(begin(targets), end(targets));
        while (marks[target].dist != 1)
            target = marks[target].from;

        _map[unit.row][unit.col] = '.';
        unit.row = getRow(target);
        unit.col = getCol(target);
        _map[unit.row][unit.col] = unit.kind;
        return marks[target].dist;
    }

    bool _Attack(const Unit &unit)
    {
        auto order = [&](const Unit &a, const Unit &b) {
            if (a.hp < b.hp)
                return true;
            if (a.hp > b.hp)
                return false;
            if (a.row < b.row)
                return true;
            if (a.row > b.row)
                return false;
            return a.col < b.col;
        };

        // Find adjacent enemy unit with the least hitpoints.
        auto target = end(_units);
        for (auto it = begin(_units); it != end(_units); ++it)
        {
            if (it->kind == unit.kind)
                continue;
            if (std::abs(it->row - unit.row) + std::abs(it->col - unit.col) != 1)
                continue;
            if (target == end(_units) || order(*it, *target))
            {
                target = it;
                continue;
            }
        }

        if (target == end(_units))
            return false;

        target->hp -= unit.attack;
        if (target->hp <= 0)
        {
            char kind = target->kind;
            _map[target->row][target->col] = '.';
            _units.erase(target);
            if (0 == --_counts[kind])
            {
                // Congratulations!
                return true;
            }
        }
        return false;
    }
};

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2018-15"_test = [] {
        {
            Battle b{std::istringstream{
                "#######\n"
                "#.G...#\n"
                "#...EG#\n"
                "#.#.#G#\n"
                "#..G#E#\n"
                "#.....#\n"
                "#######\n"}};

            std::string exp =
                "#######   \n"
                "#.G...#   G(200)\n"
                "#...EG#   E(200), G(200)\n"
                "#.#.#G#   G(200)\n"
                "#..G#E#   G(200), E(200)\n"
                "#.....#   \n"
                "#######   \n";
            expect(eq(exp, b.Dump()));

            auto runUntil = [&](int target) {
                while (b.GetRound() < target)
                {
                    b.Round();
                }
            };

            runUntil(1);
            exp =
                "#######   \n"
                "#..G..#   G(200)\n"
                "#...EG#   E(197), G(197)\n"
                "#.#G#G#   G(200), G(197)\n"
                "#...#E#   E(197)\n"
                "#.....#   \n"
                "#######   \n";
            expect(eq(exp, b.Dump()));

            runUntil(2);
            exp =
                "#######   \n"
                "#...G.#   G(200)\n"
                "#..GEG#   G(200), E(188), G(194)\n"
                "#.#.#G#   G(194)\n"
                "#...#E#   E(194)\n"
                "#.....#   \n"
                "#######   \n";
            expect(eq(exp, b.Dump()));

            runUntil(23);
            exp =
                "#######   \n"
                "#...G.#   G(200)\n"
                "#..G.G#   G(200), G(131)\n"
                "#.#.#G#   G(131)\n"
                "#...#E#   E(131)\n"
                "#.....#   \n"
                "#######   \n";
            expect(eq(exp, b.Dump()));

            runUntil(24);
            exp =
                "#######   \n"
                "#..G..#   G(200)\n"
                "#...G.#   G(131)\n"
                "#.#G#G#   G(200), G(128)\n"
                "#...#E#   E(128)\n"
                "#.....#   \n"
                "#######   \n";
            expect(eq(exp, b.Dump()));

            runUntil(25);
            exp =
                "#######   \n"
                "#.G...#   G(200)\n"
                "#..G..#   G(131)\n"
                "#.#.#G#   G(125)\n"
                "#..G#E#   G(200), E(125)\n"
                "#.....#   \n"
                "#######   \n";
            expect(eq(exp, b.Dump()));

            runUntil(28);
            exp =
                "#######   \n"
                "#G....#   G(200)\n"
                "#.G...#   G(131)\n"
                "#.#.#G#   G(116)\n"
                "#...#E#   E(113)\n"
                "#....G#   G(200)\n"
                "#######   \n";
            expect(eq(exp, b.Dump()));

            expect(0_i == b.GetScore());
            runUntil(47);
            exp =
                "#######   \n"
                "#G....#   G(200)\n"
                "#.G...#   G(131)\n"
                "#.#.#G#   G(59)\n"
                "#...#.#   \n"
                "#....G#   G(200)\n"
                "#######   \n";
            expect(eq(exp, b.Dump()));

            expect(590_i == b.GetScore());
            expect(27730_i == b.GetOutcome());
        }

        {
            Battle b(std::istringstream{
                "#######\n"
                "#G..#E#\n"
                "#E#E.E#\n"
                "#G.##.#\n"
                "#...#E#\n"
                "#...E.#\n"
                "#######\n"});

            expect(eq('E', b.RunUntilFinish(false)));
            expect(37_i == b.GetRound());
            expect(982_i == b.GetScore());
            expect(36334_i == b.GetOutcome());

            std::string exp =
                "#######   \n"
                "#...#E#   E(200)\n"
                "#E#...#   E(197)\n"
                "#.E##.#   E(185)\n"
                "#E..#E#   E(200), E(200)\n"
                "#.....#   \n"
                "#######   \n";
            expect(eq(exp, b.Dump()));
        }

        {
            Battle b(std::istringstream{
                "#######\n"
                "#E..EG#\n"
                "#.#G.E#\n"
                "#E.##E#\n"
                "#G..#.#\n"
                "#..E#.#\n"
                "#######\n"});

            expect(eq('E', b.RunUntilFinish(false)));
            expect(46_i == b.GetRound());
            expect(859_i == b.GetScore());
            expect(39514_i == b.GetOutcome());
        }

        {
            Battle b(std::istringstream{
                "#######\n"
                "#E.G#.#\n"
                "#.#G..#\n"
                "#G.#.G#\n"
                "#G..#.#\n"
                "#...E.#\n"
                "#######\n"});

            expect(eq('G', b.RunUntilFinish(false)));
            expect(35_i == b.GetRound());
            expect(793_i == b.GetScore());
            expect(27755_i == b.GetOutcome());

            std::string exp =
                "#######   \n"
                "#G.G#.#   G(200), G(98)\n"
                "#.#G..#   G(200)\n"
                "#..#..#   \n"
                "#...#G#   G(95)\n"
                "#...G.#   G(200)\n"
                "#######   \n";
            expect(eq(exp, b.Dump()));
        }

        {
            Battle b(std::istringstream{
                "#######\n"
                "#.E...#\n"
                "#.#..G#\n"
                "#.###.#\n"
                "#E#G#G#\n"
                "#...#G#\n"
                "#######\n"});

            expect(eq('G', b.RunUntilFinish(false)));
            expect(54_i == b.GetRound());
            expect(536_i == b.GetScore());
            expect(28944_i == b.GetOutcome());

            std::string exp =
                "#######   \n"
                "#.....#   \n"
                "#.#G..#   G(200)\n"
                "#.###.#   \n"
                "#.#.#.#   \n"
                "#G.G#G#   G(98), G(38), G(200)\n"
                "#######   \n";
            expect(eq(exp, b.Dump()));
        }

        {
            Battle b(std::istringstream{
                "#########\n"
                "#G......#\n"
                "#.E.#...#\n"
                "#..##..G#\n"
                "#...##..#\n"
                "#...#...#\n"
                "#.G...G.#\n"
                "#.....G.#\n"
                "#########\n"});

            expect(eq('G', b.RunUntilFinish(false)));
            expect(20_i == b.GetRound());
            expect(937_i == b.GetScore());
            expect(18740_i == b.GetOutcome());

            std::string exp =
                "#########   \n"
                "#.G.....#   G(137)\n"
                "#G.G#...#   G(200), G(200)\n"
                "#.G##...#   G(200)\n"
                "#...##..#   \n"
                "#.G.#...#   G(200)\n"
                "#.......#   \n"
                "#.......#   \n"
                "#########   \n";
            expect(eq(exp, b.Dump()));
        }

        {
            Battle b(std::ifstream(INPUT));
            std::ostringstream oss;
            oss << b.RunUntilFinish(false) << " " << b.GetOutcome();
            Printer::Print(__FILE__, "1", oss.str());
        }

        {
            std::istringstream iss{
                "#######\n"
                "#.G...#\n"
                "#...EG#\n"
                "#.#.#G#\n"
                "#..G#E#\n"
                "#.....#\n"
                "#######\n"};

            auto b = Battle::Task2(iss);
            expect(15_i == b.GetElfAttack());
            expect(29_i == b.GetRound());
            expect(172_i == b.GetScore());
        }

        {
            std::ifstream ifs{INPUT};
            auto battle = Battle::Task2(ifs);
            Printer::Print(__FILE__, "2", battle.GetOutcome());
        }
    };
};

} //namespace;