#include "../test.hpp"
#include <fstream>
#include <boost/functional/hash.hpp>

namespace {

struct Basin
{
    struct Blizzard
    {
        int start{};
        int direction{};
    };
    std::vector<std::vector<Blizzard>> rows, cols;

    Basin(std::istream &&is)
    {
        std::string line;
        while (std::getline(is, line))
        {
            rows.push_back({});
            if (cols.empty())
            {
                cols.resize(line.size());
                continue;
            }

            for (int col = 0; col < line.size(); ++col)
            {
                switch (line[col])
                {
                case '<': rows.back().push_back({col - 1, -1}); break;
                case '>': rows.back().push_back({col - 1, 1}); break;
                case '^': cols[col].push_back({static_cast<int>(rows.size() - 2), -1}); break;
                case 'v': cols[col].push_back({static_cast<int>(rows.size() - 2), 1}); break;
                }
            }
        }
    }

    bool IsFree(int row, int col, int time) const
    {
        if (row < 0 || row >= rows.size())
            return false;
        if (row == 0)
            return col == 1;
        if (row == rows.size() - 1)
            return col == cols.size() - 2;
        if (col == 0 || col == cols.size() - 1)
            return false;
        const int cols_period = cols.size() - 2;
        const int rows_period = rows.size() - 2;
        for (auto b : rows[row])
        {
            int pos = (b.start + b.direction * time) % cols_period;
            if (pos < 0)
                pos += cols_period;
            if (pos + 1 == col)
                return false;
        }
        for (auto b : cols[col])
        {
            int pos = (b.start + b.direction * time) % rows_period;
            if (pos < 0)
                pos += rows_period;
            if (pos + 1 == row)
                return false;
        }
        return true;
    }

    struct Pos
    {
        int row{};
        int col{};
        bool operator==(const Pos &o) const = default;
    };

    int FindRoute(const Pos &from, const Pos &to, int start_time) const
    {
        struct State : Pos
        {
            int time{};
            bool operator==(const State &o) const = default;
        };

        struct StateHash
        {
            size_t operator()(const State &s) const
            {
                size_t seed{};
                boost::hash_combine(seed, s.row);
                boost::hash_combine(seed, s.col);
                boost::hash_combine(seed, s.time);
                return seed;
            }
        };
        std::unordered_set<State, StateHash> visited;

        auto distFromTarget = [&](const Pos &p) {
            return std::abs(to.row - p.row) + std::abs(to.col - p.col);
        };

        auto priority = [&](const State &a, const State &b) -> bool {
            if (a.time == b.time)
            {
                int da = distFromTarget(a);
                int db = distFromTarget(b);
                return da > db;
            }
            return a.time > b.time;
        };

        std::priority_queue<State, std::vector<State>, decltype(priority)> pq(priority);
        pq.push({from, start_time});
        visited.insert({from, start_time});

        while (!pq.empty())
        {
            auto state = pq.top();
            pq.pop();

            if (state.row == to.row && state.col == to.col)
                return state.time;

            auto tryGo = [&](int row, int col, int time) {
                if (visited.contains({{row, col}, time}))
                    return;
                if (!IsFree(row, col, time))
                    return;
                visited.insert({{row, col}, time});
                pq.push({{row, col}, time});
            };
            tryGo(state.row - 1, state.col, state.time + 1);
            tryGo(state.row + 1, state.col, state.time + 1);
            tryGo(state.row, state.col - 1, state.time + 1);
            tryGo(state.row, state.col + 1, state.time + 1);
            tryGo(state.row, state.col, state.time + 1);
        }
        return -1;
    }

    std::vector<int> Tasks() const
    {
        const Pos START{0, 1};
        const Pos FINISH{static_cast<int>(rows.size() - 1), static_cast<int>(cols.size() - 2)};

        std::vector<int> times;
        times.push_back(FindRoute(START, FINISH, 0));
        times.push_back(FindRoute(FINISH, START, times.back()));
        times.push_back(FindRoute(START, FINISH, times.back()));
        return times;
    }
};

const char *const TEST =
    "#.######\n"
    "#>>.<^<#\n"
    "#.<..<<#\n"
    "#>v.><>#\n"
    "#<^v^^>#\n"
    "######.#\n";

using namespace boost::ut;

suite s = [] {
    "2022-24"_test = [] {
        Basin test{std::istringstream{TEST}};
        expect(test.IsFree(0, 1, 10));
        expect(!test.IsFree(1, 1, 0));
        expect(test.IsFree(1, 1, 1));
        expect(!test.IsFree(2, 1, 1));
        expect(test.IsFree(2, 1, 2));
        expect(!test.IsFree(1, 1, 3));
        expect(test.IsFree(1, 1, 4));
        expect(test.IsFree(1, 2, 5));
        expect(test.IsFree(1, 3, 6));
        expect(test.IsFree(2, 3, 7));
        expect(test.IsFree(2, 2, 8));
        expect(test.IsFree(1, 2, 9));
        expect(!test.IsFree(1, 2, 10));
        expect(test.IsFree(1, 3, 10));

        auto test_res = test.Tasks();
        expect(18_i == test_res[0]);
        expect(54_i == test_res.back());

        Basin basin{std::ifstream{INPUT}};
        auto res = basin.Tasks();
        Printer::Print(__FILE__, "1", res[0]);
        Printer::Print(__FILE__, "2", res.back());
    };
};

} //namespace;
