#include "../test.hpp"

namespace {

class Map
{
public:
    Map(std::istream &&is)
    {
        _map.assign(std::istreambuf_iterator<char>{is}, std::istreambuf_iterator<char>{});
        _width = _map.find("\n") + 1;
    }

    int CalcRisk() const
    {
        auto getIdx = [this](int x, int y) {
            return y * _width + x;
        };
        auto getX = [this](int idx) {
            return idx % _width;
        };
        auto getY = [this](int idx) {
            return idx / _width;
        };

        auto const INF = std::numeric_limits<int>::max();

        struct Data
        {
            int risk{INF};
            bool processed{};
        };

        std::vector<Data> data(_width * _width);
        data[0].risk = 0;

        //auto print = [&] {
        //    for (int y = 0; y < _width - 1; ++y)
        //    {
        //        for (int x = 0; x < _width - 1; ++x)
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
            size_t idx{};
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
                if (nx < 0 || ny < 0 || nx >= _width - 1 || ny >= _width - 1)
                    return;
                auto idx = getIdx(nx, ny);
                if (data[idx].processed)
                    return;
                q.push({idx, v.risk + (_map[idx] - '0')});
            };

            auto x = getX(v.idx);
            auto y = getY(v.idx);
            tryGo(x - 1, y);
            tryGo(x + 1, y);
            tryGo(x, y - 1);
            tryGo(x, y + 1);
        }

        return data[getIdx(_width - 2, _width - 2)].risk;
    }

private:
    std::string _map;
    size_t _width{};
};

const char *TEST_INPUT = R"(1163751742
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
        expect(40_i == test.CalcRisk());

        Map map{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", map.CalcRisk());
    };
};

} //namespace;
