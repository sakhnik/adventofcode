#include "../test.hpp"

namespace {

using namespace boost::ut;

struct Pos
{
    int x{}, y{};
    bool operator==(const Pos &) const = default;
};

struct Task
{
    std::unordered_map<int64_t, int64_t> cache;

    int64_t SearchArrows(Pos from, Pos to, int level)
    {
        int64_t hash = level;
        hash = hash * 100 + from.x;
        hash = hash * 4 + from.y;
        hash = hash * 4 + to.x;
        hash = hash * 4 + to.y;

        auto it = cache.find(hash);
        if (it != cache.end())
            return it->second;

        int64_t res = std::numeric_limits<int64_t>::max();
        struct Head
        {
            Pos p;
            std::string keys;
        };
        std::queue<Head> q;
        q.push({from, ""});
        while (!q.empty()) {
            auto h = q.front();
            auto cur = h.p;
            q.pop();
            if (cur == to) {
                res = std::min(res, SearchRobot(h.keys + "A", level - 1));
                continue;
            }
            if (cur == Pos{0, 0})
                continue;
            else {
                if (cur.y < to.y)
                    q.push({{cur.x, cur.y + 1}, h.keys + "v"});
                else if (cur.y > to.y)
                    q.push({{cur.x, cur.y - 1}, h.keys + "^"});
                if (cur.x < to.x)
                    q.push({{cur.x + 1, cur.y}, h.keys + ">"});
                else if (cur.x > to.x)
                    q.push({{cur.x - 1, cur.y}, h.keys + "<"});
            }
        }
        return cache[hash] = res;
    }

    int64_t SearchRobot(std::string_view keys, int level)
    {
        if (level == 1) {
            return keys.size();
        }

        int64_t res = 0;
        Pos p{2, 0};

        constexpr const std::array<std::pair<char, Pos>, 5> ARROWS = {{
            {'^', {1, 0}}, {'A', {2, 0}},
            {'<', {0, 1}}, {'v', {1, 1}}, {'>', {2, 1}}
        }};

        for (char k : keys) {
            auto it = std::find_if(ARROWS.begin(), ARROWS.end(), [&](const auto &a) { return a.first == k; });
            assert(it != ARROWS.end());
            auto newP = it->second;
            res += SearchArrows(p, newP, level);
            p = newP;
        }

        return res;
    }

    int64_t Search(Pos from, Pos to, int levels)
    {
        int64_t res = std::numeric_limits<int64_t>::max();

        struct Head
        {
            Pos p;
            std::string keys;
        };
        std::queue<Head> q;
        q.push({from, ""});

        while (!q.empty()) {
            auto h = q.front();
            q.pop();
            auto cur = h.p;
            if (cur == to) {
                int64_t rec = SearchRobot(h.keys + "A", levels);
                res = std::min(res, rec);
                continue;
            }
            if (cur == Pos{0, 3})
                continue;
            else {
                if (cur.y < to.y)
                    q.push({{cur.x, cur.y + 1}, h.keys + "v"});
                else if (cur.y > to.y)
                    q.push({{cur.x, cur.y - 1}, h.keys + "^" });
                if (cur.x < to.x)
                    q.push({{cur.x + 1, cur.y}, h.keys + ">" });
                else if (cur.x > to.x)
                    q.push({ cur.x - 1, cur.y, h.keys + "<" });
            }
        }

        return res;
    }

    using CodesT = std::vector<std::string>;

    int64_t Solve(const CodesT &codes, int levels)
    {
        constexpr const std::array<Pos, 11> NUMPAD = {{
            {1, 3},
            {0, 2}, {1, 2}, {2, 2},
            {0, 1}, {1, 1}, {2, 1},
            {0, 0}, {1, 0}, {2, 0},
            {2, 3}
        }};

        int64_t res{};

        for (auto &code : codes) {
            auto p = Pos{2, 3};

            int64_t cost{};
            for (char c : code) {
                auto newP = NUMPAD[std::stoi(std::string{c}, nullptr, 16)];
                cost += Search(p, newP, levels);
                p = newP;
            }
            res += cost * std::stoi(code.substr(0, code.size() - 1));
        }

        return res;
    }
};

suite s = [] {
    "21"_test = [] {
        auto test1 = Task{}.Solve({"029A", "980A", "179A", "456A", "379A"}, 3);
        expect(126384_i == test1);

        auto task1 = Task{}.Solve({"540A", "839A", "682A", "826A", "974A"}, 3);
        Printer::Print(__FILE__, "1", task1);

        auto task2 = Task{}.Solve({"540A", "839A", "682A", "826A", "974A"}, 26);
        Printer::Print(__FILE__, "2", task2);
    };
};

} //namespace;
