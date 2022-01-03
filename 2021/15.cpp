#include "../test.hpp"

namespace {

class Map
{
public:
    Map(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            if (line.empty())
                continue;
            _map.emplace_back(std::move(line));
        }
    }

    int CalcRisk(int mult) const
    {
        const int width = _map.size() * mult;

        auto getIdx = [=](int x, int y) {
            return y * width + x;
        };
        auto getX = [=](int idx) {
            return idx % width;
        };
        auto getY = [=](int idx) {
            return idx / width;
        };

        auto const INF = std::numeric_limits<int>::max();

        struct Data
        {
            int risk{INF};
            bool processed{};
        };

        std::vector<Data> data(width * width);
        data[0].risk = 0;

        //auto print = [&] {
        //    for (int y = 0; y < width; ++y)
        //    {
        //        for (int x = 0; x < width; ++x)
        //        {
        //            std::cout << "\t";
        //            auto risk = data[getIdx(x, y)].risk;
        //            if (risk != INF)
        //                std::cout << risk;
        //        }
        //        std::cout << "\n";
        //    }
        //    std::cout << "----" << std::endl;
        //};
      
        // Dijkstra algorithm
        struct Vert
        {
            int idx{};
            int risk{};

            bool operator<(const Vert &o) const { return risk > o.risk; }
        };
        std::priority_queue<Vert> q;
        q.push({0, 0});
        while (!q.empty())
        {
            //print();
            auto v = q.top();
            q.pop();
            if (data[v.idx].processed)
                continue;
            data[v.idx].processed = true;
            data[v.idx].risk = v.risk;

            auto tryGo = [&](int nx, int ny) {
                if (nx < 0 || ny < 0 || nx >= width || ny >= width)
                    return;
                auto idx = getIdx(nx, ny);
                if (data[idx].processed)
                    return;
                q.push({idx, v.risk + _GetRiskFromMap(nx, ny)});
            };

            auto x = getX(v.idx);
            auto y = getY(v.idx);
            tryGo(x - 1, y);
            tryGo(x + 1, y);
            tryGo(x, y - 1);
            tryGo(x, y + 1);
        }

        return data.back().risk;
    }

    void PrintMap(int mult) const
    {
        const int width = _map.size() * mult;
        for (int y = 0; y < width; ++y)
        {
            for (int x = 0; x < width; ++x)
                std::cout << _GetRiskFromMap(x, y);
            std::cout << "\n";
        }
    }

private:
    std::vector<std::string> _map;

    int _GetRiskFromMap(int x, int y) const
    {
        const auto width = _map.size();
        int dx = x / width;
        int dy = y / width;
        int val = _map[y % width][x % width] - '0';
        val += dx + dy;
        if (val > 9)
            return val % 9;
        return val;
    }
};

const char *TEST_INPUT = R"(
1163751742
1381373672
2136511328
3694931569
7463417111
1319128137
1359912421
3125421639
1293138521
2311944581
)";

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "2021-15"_test = [] {
        Map test{std::istringstream{TEST_INPUT}};
        expect(40_i == test.CalcRisk(1));
        expect(315_i == test.CalcRisk(5));

        Map map{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", map.CalcRisk(1));
        Printer::Print(__FILE__, "2", map.CalcRisk(5));
    };
};

} //namespace;
